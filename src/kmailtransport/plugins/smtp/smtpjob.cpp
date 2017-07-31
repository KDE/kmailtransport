/*
  Copyright (c) 2007 Volker Krause <vkrause@kde.org>

  Based on KMail code by:
  Copyright (c) 1996-1998 Stefan Taferner <taferner@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#include "smtpjob.h"
#include "transport.h"
#include "mailtransport_defs.h"
#include "precommandjob.h"
#include "sessionuiproxy.h"
#include "mailtransportplugin_smtp_debug.h"

#include <QBuffer>
#include <QHash>
#include <QPointer>

#include <KLocalizedString>
#include <QUrl>
#include <QUrlQuery>
#include "mailtransport_debug.h"
#include <KPasswordDialog>

#include <KSMTP/Session>
#include <KSMTP/LoginJob>
#include <KSMTP/SendJob>

using namespace MailTransport;

class SessionPool
{
public:
    SessionPool() : ref(0)
    {
    }

    int ref;
    QHash<int, KSmtp::Session*> sessions;

    void removeSession(KSmtp::Session *session)
    {
        qCDebug(MAILTRANSPORT_SMTP_LOG) << "Removing session" << session << "from the pool";
        int key = sessions.key(session);
        if (key > 0) {
            QObject::connect(session, &KSmtp::Session::stateChanged,
                            [session](KSmtp::Session::State state) {
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
    SmtpJobPrivate(SmtpJob *parent) : q(parent)
    {
    }

    SmtpJob *q;
    KSmtp::Session *session;
    KSmtp::SessionUiProxy::Ptr uiProxy;
    enum State {
        Idle, Precommand, Smtp
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
    delete d;
}

void SmtpJob::doStart()
{
    if (s_sessionPool.isDestroyed()) {
        return;
    }

    if ((!s_sessionPool->sessions.isEmpty()
         && s_sessionPool->sessions.contains(transport()->id()))
        || transport()->precommand().isEmpty()) {
        d->currentState = SmtpJobPrivate::Smtp;
        startSmtpJob();
    } else {
        d->currentState = SmtpJobPrivate::Precommand;
        PrecommandJob *job = new PrecommandJob(transport()->precommand(), this);
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
        d->session->setUiProxy(d->uiProxy);
        if (transport()->specifyHostname()) {
            d->session->setCustomHostname(transport()->localHostname());
        }
        s_sessionPool->sessions.insert(transport()->id(), d->session);
    }

    connect(d->session, &KSmtp::Session::stateChanged,
            this, &SmtpJob::sessionStateChanged, Qt::UniqueConnection);
    connect(d->session, &KSmtp::Session::connectionError,
            this, [this](const QString &err) {
                setError(KJob::UserDefinedError);
                setErrorText(err);
                s_sessionPool->removeSession(d->session);
                emitResult();
            });

    if (d->session->state() == KSmtp::Session::Disconnected) {
        d->session->open();
    } else {
        if (d->session->state() != KSmtp::Session::Authenticated) {
            startLoginJob();
        }

        startSendJob();
    }
}

void SmtpJob::sessionStateChanged(KSmtp::Session::State state)
{
    if (state == KSmtp::Session::Ready) {
        startLoginJob();
    } else if (state == KSmtp::Session::Authenticated) {
        startSendJob();
    }
}

void SmtpJob::startLoginJob()
{
    if (!transport()->requiresAuthentication()) {
        startSendJob();
        return;
    }

    auto login = new KSmtp::LoginJob(d->session);
    auto user = transport()->userName();
    auto passwd = transport()->password();
    if ((user.isEmpty() || passwd.isEmpty())
        && transport()->authenticationType() != Transport::EnumAuthenticationType::GSSAPI) {
        QPointer<KPasswordDialog> dlg
            = new KPasswordDialog(
            nullptr,
            KPasswordDialog::ShowUsernameLine
            |KPasswordDialog::ShowKeepPassword);
        dlg->setPrompt(i18n("You need to supply a username and a password "
                            "to use this SMTP server."));
        dlg->setKeepPassword(transport()->storePassword());
        dlg->addCommentLine(QString(), transport()->name());
        dlg->setUsername(user);
        dlg->setPassword(passwd);

        bool gotIt = false;
        if (dlg->exec()) {
            transport()->setUserName(dlg->username());
            transport()->setPassword(dlg->password());
            transport()->setStorePassword(dlg->keepPassword());
            transport()->save();
            gotIt = true;
        }
        delete dlg;

        if (!gotIt) {
            setError(KilledJobError);
            emitResult();
            return;
        }
    }

    login->setUserName(transport()->userName());
    login->setPassword(transport()->password());
    switch (transport()->authenticationType()) {
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
        login->setPreferedAuthMode(KSmtp::LoginJob::XOAuth);
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
        qCWarning(MAILTRANSPORT_SMTP_LOG) << "Unknown authentication mode" << transport()->authenticationTypeString();
        break;
    }

    switch (transport()->encryption()) {
    case Transport::EnumEncryption::None:
        login->setEncryptionMode(KSmtp::LoginJob::Unencrypted);
        break;
    case Transport::EnumEncryption::TLS:
        login->setEncryptionMode(KSmtp::LoginJob::TlsV1);
        break;
    case Transport::EnumEncryption::SSL:
        login->setEncryptionMode(KSmtp::LoginJob::AnySslVersion);
        break;
    default:
        qCWarning(MAILTRANSPORT_SMTP_LOG) << "Unknown encryption mode" << transport()->encryption();
        break;

    }

    connect(login, &KJob::result, this, &SmtpJob::slotResult);
    addSubjob(login);
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

    connect(send, &KJob::result, this, &SmtpJob::slotResult);
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
        return;
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

    // Normally, calling TransportJob::slotResult() whould set the proper error code
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
