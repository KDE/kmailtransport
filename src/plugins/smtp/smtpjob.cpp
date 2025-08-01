/*
  SPDX-FileCopyrightText: 2007 Volker Krause <vkrause@kde.org>

  Based on KMail code by:
  SPDX-FileCopyrightText: 1996-1998 Stefan Taferner <taferner@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "smtpjob.h"
using namespace Qt::Literals::StringLiterals;

#include "mailtransport_defs.h"
#include "mailtransportplugin_smtp_debug.h"
#include "precommandjob.h"
#include "sessionuiproxy.h"
#include "transport.h"
#include "xoauthpasswordrequester.h"
#include <KAuthorized>
#include <QHash>
#include <QPointer>

#include "mailtransport_debug.h"
#include <KLocalizedString>
#include <KPasswordDialog>

#include <KSMTP/LoginJob>
#include <KSMTP/SendJob>

#include <KGAPI/Account>
#include <KGAPI/AccountManager>
#include <KGAPI/AuthJob>

using namespace MailTransport;

class SessionPool
{
public:
    int ref = 0;
    QHash<int, KSmtp::Session *> sessions;

    void removeSession(KSmtp::Session *session)
    {
        qCDebug(MAILTRANSPORT_SMTP_LOG) << "Removing session" << session << "from the pool";
        int key = sessions.key(session);
        if (key > 0) {
            QObject::connect(session, &KSmtp::Session::stateChanged, session, [session](KSmtp::Session::State state) {
                if (state == KSmtp::Session::Disconnected) {
                    session->deleteLater();
                }
            });
            session->quit();
            sessions.remove(key);
        }
    }
};

Q_GLOBAL_STATIC(SessionPool, s_sessionPool)

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
class SmtpJobPrivate
{
public:
    explicit SmtpJobPrivate(SmtpJob *parent)
        : q(parent)
    {
    }

    void doLogin();

    SmtpJob *const q;
    KSmtp::Session *session = nullptr;
    KSmtp::SessionUiProxy::Ptr uiProxy;
    enum State {
        Idle,
        Precommand,
        Smtp
    } currentState;
    bool finished;
};

SmtpJob::SmtpJob(Transport *transport, QObject *parent)
    : TransportJob(transport, parent)
    , d(new SmtpJobPrivate(this))
{
    d->currentState = SmtpJobPrivate::Idle;
    d->session = nullptr;
    d->finished = false;
    d->uiProxy = KSmtp::SessionUiProxy::Ptr(new SmtpSessionUiProxy);
    if (!s_sessionPool.isDestroyed()) {
        s_sessionPool->ref++;
    }
}

SmtpJob::~SmtpJob()
{
    if (!s_sessionPool.isDestroyed()) {
        s_sessionPool->ref--;
        if (s_sessionPool->ref == 0) {
            qCDebug(MAILTRANSPORT_SMTP_LOG) << "clearing SMTP session pool" << s_sessionPool->sessions.count();
            while (!s_sessionPool->sessions.isEmpty()) {
                s_sessionPool->removeSession(*(s_sessionPool->sessions.begin()));
            }
        }
    }
}

void SmtpJob::doStart()
{
    if (s_sessionPool.isDestroyed()) {
        return;
    }

    if ((!s_sessionPool->sessions.isEmpty() && s_sessionPool->sessions.contains(transport()->id())) || transport()->precommand().isEmpty()) {
        d->currentState = SmtpJobPrivate::Smtp;
        startSmtpJob();
    } else {
        d->currentState = SmtpJobPrivate::Precommand;
        auto job = new PrecommandJob(transport()->precommand(), this);
        addSubjob(job);
        job->start();
    }
}

void SmtpJob::startSmtpJob()
{
    if (s_sessionPool.isDestroyed()) {
        return;
    }

    d->session = s_sessionPool->sessions.value(transport()->id());
    if (!d->session) {
        d->session = new KSmtp::Session(transport()->host(), transport()->port());
        d->session->setUseNetworkProxy(transport()->useProxy());
        d->session->setUiProxy(d->uiProxy);
        switch (transport()->encryption()) {
        case Transport::EnumEncryption::None:
            d->session->setEncryptionMode(KSmtp::Session::Unencrypted);
            break;
        case Transport::EnumEncryption::TLS:
            d->session->setEncryptionMode(KSmtp::Session::STARTTLS);
            break;
        case Transport::EnumEncryption::SSL:
            d->session->setEncryptionMode(KSmtp::Session::TLS);
            break;
        default:
            qCWarning(MAILTRANSPORT_SMTP_LOG) << "Unknown encryption mode" << transport()->encryption();
            break;
        }
        if (transport()->specifyHostname()) {
            d->session->setCustomHostname(transport()->localHostname());
        }
        s_sessionPool->sessions.insert(transport()->id(), d->session);
    }

    connect(d->session, &KSmtp::Session::stateChanged, this, &SmtpJob::sessionStateChanged, Qt::UniqueConnection);
    connect(d->session, &KSmtp::Session::connectionError, this, [this](const QString &err) {
        setError(KJob::UserDefinedError);
        setErrorText(err);
        s_sessionPool->removeSession(d->session);
        emitResult();
    });

    if (d->session->state() == KSmtp::Session::Disconnected) {
        d->session->open();
    } else {
        if (d->session->state() != KSmtp::Session::Authenticated) {
            startPasswordRetrieval();
        }

        startSendJob();
    }
}

void SmtpJob::sessionStateChanged(KSmtp::Session::State state)
{
    if (state == KSmtp::Session::Ready) {
        startPasswordRetrieval();
    } else if (state == KSmtp::Session::Authenticated) {
        startSendJob();
    }
}

void SmtpJob::startPasswordRetrieval(bool forceRefresh)
{
    if (!transport()->requiresAuthentication() && !forceRefresh) {
        startSendJob();
        return;
    }

    auto xoauthRequester = createXOAuthPasswordRequester(transport(), this);
    if (xoauthRequester != nullptr) {
        connect(xoauthRequester,
                &XOAuthPasswordRequester::done,
                this,
                [this, xoauthRequester](XOAuthPasswordRequester::Result result, const QString &password) {
                    xoauthRequester->deleteLater();
                    if (result == XOAuthPasswordRequester::Error) {
                        setError(KJob::UserDefinedError);
                        emitResult();
                        return;
                    }

                    transport()->setPassword(password);
                    startLoginJob();
                });
        xoauthRequester->requestPassword(forceRefresh);
    } else {
        startLoginJob();
    }
}

void SmtpJob::startLoginJob()
{
    if (!transport()->requiresAuthentication()) {
        startSendJob();
        return;
    }

    auto user = transport()->userName();
    auto passwd = transport()->password();
    if ((user.isEmpty() || passwd.isEmpty()) && transport()->authenticationType() != Transport::EnumAuthenticationType::GSSAPI) {
        QPointer<KPasswordDialog> dlg = new KPasswordDialog(nullptr, KPasswordDialog::ShowUsernameLine | KPasswordDialog::ShowKeepPassword);
        dlg->setAttribute(Qt::WA_DeleteOnClose, true);
        dlg->setPrompt(
            i18n("You need to supply a username and a password "
                 "to use this SMTP server."));
        dlg->setKeepPassword(transport()->storePassword());
        dlg->addCommentLine(QString(), transport()->name());
        dlg->setUsername(user);
        dlg->setPassword(passwd);
        dlg->setRevealPasswordMode(KAuthorized::authorize(u"lineedit_reveal_password"_s) ? KPassword::RevealMode::OnlyNew : KPassword::RevealMode::Never);

        connect(this, &KJob::result, dlg, &QDialog::reject);

        connect(dlg, &QDialog::finished, this, [this, dlg](const int result) {
            if (result == QDialog::Rejected) {
                setError(KilledJobError);
                emitResult();
                return;
            }

            transport()->setUserName(dlg->username());
            transport()->setPassword(dlg->password());
            transport()->setStorePassword(dlg->keepPassword());
            transport()->save();

            d->doLogin();
        });
        dlg->open();

        return;
    }

    d->doLogin();
}

void SmtpJobPrivate::doLogin()
{
    QString passwd = q->transport()->password();
    if (q->transport()->authenticationType() == Transport::EnumAuthenticationType::XOAUTH2) {
        passwd = passwd.left(passwd.indexOf(u'\001'));
    }

    auto login = new KSmtp::LoginJob(session);
    login->setUserName(q->transport()->userName());
    login->setPassword(passwd);
    switch (q->transport()->authenticationType()) {
    case TransportBase::EnumAuthenticationType::PLAIN:
        login->setPreferedAuthMode(KSmtp::LoginJob::Plain);
        break;
    case TransportBase::EnumAuthenticationType::LOGIN:
        login->setPreferedAuthMode(KSmtp::LoginJob::Login);
        break;
    case TransportBase::EnumAuthenticationType::CRAM_MD5:
        login->setPreferedAuthMode(KSmtp::LoginJob::CramMD5);
        break;
    case TransportBase::EnumAuthenticationType::XOAUTH2:
        login->setPreferedAuthMode(KSmtp::LoginJob::XOAuth2);
        break;
    case TransportBase::EnumAuthenticationType::DIGEST_MD5:
        login->setPreferedAuthMode(KSmtp::LoginJob::DigestMD5);
        break;
    case TransportBase::EnumAuthenticationType::NTLM:
        login->setPreferedAuthMode(KSmtp::LoginJob::NTLM);
        break;
    case TransportBase::EnumAuthenticationType::GSSAPI:
        login->setPreferedAuthMode(KSmtp::LoginJob::GSSAPI);
        break;
    default:
        qCWarning(MAILTRANSPORT_SMTP_LOG) << "Unknown authentication mode" << q->transport()->authenticationTypeString();
        break;
    }

    q->addSubjob(login);
    login->start();
    qCDebug(MAILTRANSPORT_SMTP_LOG) << "Login started";
}

void SmtpJob::startSendJob()
{
    auto send = new KSmtp::SendJob(d->session);
    send->setFrom(sender());
    send->setTo(to());
    send->setCc(cc());
    send->setBcc(bcc());
    send->setData(data());
    send->setDeliveryStatusNotification(deliveryStatusNotification());

    addSubjob(send);
    send->start();

    qCDebug(MAILTRANSPORT_SMTP_LOG) << "Send started";
}

bool SmtpJob::doKill()
{
    if (s_sessionPool.isDestroyed()) {
        return false;
    }

    if (!hasSubjobs()) {
        return true;
    }
    if (d->currentState == SmtpJobPrivate::Precommand) {
        return subjobs().first()->kill();
    } else if (d->currentState == SmtpJobPrivate::Smtp) {
        clearSubjobs();
        s_sessionPool->removeSession(d->session);
        return true;
    }
    return false;
}

void SmtpJob::slotResult(KJob *job)
{
    if (s_sessionPool.isDestroyed()) {
        removeSubjob(job);
        return;
    }
    if (qobject_cast<KSmtp::LoginJob *>(job)) {
        if (job->error() == KSmtp::LoginJob::TokenExpired) {
            removeSubjob(job);
            startPasswordRetrieval(/*force refresh */ true);
            return;
        }
    }

    // The job has finished, so we don't care about any further errors. Set
    // d->finished to true, so slaveError() knows about this and doesn't call
    // emitResult() anymore.
    // Sometimes, the SMTP slave emits more than one error
    //
    // The first error causes slotResult() to be called, but not slaveError(), since
    // the scheduler doesn't emit errors for connected slaves.
    //
    // The second error then causes slaveError() to be called (as the slave is no
    // longer connected), which does emitResult() a second time, which is invalid
    // (and triggers an assert in KMail).
    d->finished = true;

    // Normally, calling TransportJob::slotResult() would set the proper error code
    // for error() via KComposite::slotResult(). However, we can't call that here,
    // since that also emits the result signal.
    // In KMail, when there are multiple mails in the outbox, KMail tries to send
    // the next mail when it gets the result signal, which then would reuse the
    // old broken slave from the slave pool if there was an error.
    // To prevent that, we call TransportJob::slotResult() only after removing the
    // slave from the pool and calculate the error code ourselves.
    int errorCode = error();
    if (!errorCode) {
        errorCode = job->error();
    }

    if (errorCode && d->currentState == SmtpJobPrivate::Smtp) {
        s_sessionPool->removeSession(d->session);
        TransportJob::slotResult(job);
        return;
    }

    TransportJob::slotResult(job);
    if (!error() && d->currentState == SmtpJobPrivate::Precommand) {
        d->currentState = SmtpJobPrivate::Smtp;
        startSmtpJob();
        return;
    }
    if (!error() && !hasSubjobs()) {
        emitResult();
    }
}

#include "moc_smtpjob.cpp"
