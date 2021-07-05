/*
  SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>
  SPDX-FileCopyrightText: 2007 KovoKs <info@kovoks.nl>
  SPDX-FileCopyrightText: 2008 Thomas McGuire <thomas.mcguire@gmx.net>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "servertest.h"
#include "socket.h"

#include <mailtransport_defs.h>
#include <transportbase.h>

// Qt
#include <QHash>
#include <QHostInfo>
#include <QProgressBar>
#include <QRegularExpression>
#include <QSet>
#include <QTimer>

// KDE
#include "mailtransport_debug.h"

using namespace MailTransport;

namespace MailTransport
{
class ServerTestPrivate
{
public:
    ServerTestPrivate(ServerTest *test);

    ServerTest *const q;
    QString server;
    QString fakeHostname;
    QString testProtocol;

    MailTransport::Socket *normalSocket = nullptr;
    MailTransport::Socket *secureSocket = nullptr;

    QSet<int> connectionResults;
    QHash<int, QVector<int>> authenticationResults;
    QSet<ServerTest::Capability> capabilityResults;
    QHash<int, uint> customPorts;
    QTimer *normalSocketTimer = nullptr;
    QTimer *secureSocketTimer = nullptr;
    QTimer *progressTimer = nullptr;

    QProgressBar *testProgress = nullptr;

    bool secureSocketFinished = false;
    bool normalSocketFinished = false;
    bool tlsFinished = false;
    bool popSupportsTLS;
    int normalStage;
    int secureStage;
    int encryptionMode;

    bool normalPossible = true;
    bool securePossible = true;

    void finalResult();
    void handleSMTPIMAPResponse(int type, const QString &text);
    void sendInitialCapabilityQuery(MailTransport::Socket *socket);
    bool handlePopConversation(MailTransport::Socket *socket, int type, int stage, const QString &response, bool *shouldStartTLS);
    bool handleNntpConversation(MailTransport::Socket *socket, int type, int *stage, const QString &response, bool *shouldStartTLS);
    QVector<int> parseAuthenticationList(const QStringList &authentications);

    inline bool isGmail(const QString &server) const
    {
        return server.endsWith(QLatin1String("gmail.com")) || server.endsWith(QLatin1String("googlemail.com"));
    }

    // slots
    void slotNormalPossible();
    void slotNormalNotPossible();
    void slotSslPossible();
    void slotSslNotPossible();
    void slotTlsDone();
    void slotReadNormal(const QString &text);
    void slotReadSecure(const QString &text);
    void slotUpdateProgress();
};
}

ServerTestPrivate::ServerTestPrivate(ServerTest *test)
    : q(test)
{
}

void ServerTestPrivate::finalResult()
{
    if (!secureSocketFinished || !normalSocketFinished || !tlsFinished) {
        return;
    }

    qCDebug(MAILTRANSPORT_LOG) << "Modes:" << connectionResults;
    qCDebug(MAILTRANSPORT_LOG) << "Capabilities:" << capabilityResults;
    qCDebug(MAILTRANSPORT_LOG) << "Normal:" << q->normalProtocols();
    qCDebug(MAILTRANSPORT_LOG) << "SSL:" << q->secureProtocols();
    qCDebug(MAILTRANSPORT_LOG) << "TLS:" << q->tlsProtocols();

    if (testProgress) {
        testProgress->hide();
    }
    progressTimer->stop();
    secureSocketFinished = false;
    normalSocketFinished = false;
    tlsFinished = false;

    QVector<int> resultsAsVector;
    resultsAsVector.reserve(connectionResults.size());
    for (int res : std::as_const(connectionResults)) {
        resultsAsVector.append(res);
    }

    Q_EMIT q->finished(resultsAsVector);
}

QVector<int> ServerTestPrivate::parseAuthenticationList(const QStringList &authentications)
{
    QVector<int> result;
    for (QStringList::ConstIterator it = authentications.begin(); it != authentications.end(); ++it) {
        QString current = (*it).toUpper();
        if (current == QLatin1String("LOGIN")) {
            result << Transport::EnumAuthenticationType::LOGIN;
        } else if (current == QLatin1String("PLAIN")) {
            result << Transport::EnumAuthenticationType::PLAIN;
        } else if (current == QLatin1String("CRAM-MD5")) {
            result << Transport::EnumAuthenticationType::CRAM_MD5;
        } else if (current == QLatin1String("DIGEST-MD5")) {
            result << Transport::EnumAuthenticationType::DIGEST_MD5;
        } else if (current == QLatin1String("NTLM")) {
            result << Transport::EnumAuthenticationType::NTLM;
        } else if (current == QLatin1String("GSSAPI")) {
            result << Transport::EnumAuthenticationType::GSSAPI;
        } else if (current == QLatin1String("ANONYMOUS")) {
            result << Transport::EnumAuthenticationType::ANONYMOUS;
        } else if (current == QLatin1String("XOAUTH2")) {
            if (isGmail(server)) {
                result << Transport::EnumAuthenticationType::XOAUTH2;
            }
        }
        // APOP is handled by handlePopConversation()
    }
    qCDebug(MAILTRANSPORT_LOG) << authentications << result;

    // LOGIN doesn't offer anything over PLAIN, requires more server
    // roundtrips and is not an official SASL mechanism, but a MS-ism,
    // so only enable it if PLAIN isn't available:
    if (result.contains(Transport::EnumAuthenticationType::PLAIN)) {
        result.removeAll(Transport::EnumAuthenticationType::LOGIN);
    }

    return result;
}

void ServerTestPrivate::handleSMTPIMAPResponse(int type, const QString &text)
{
    if (!text.contains(QLatin1String("AUTH"), Qt::CaseInsensitive)) {
        qCDebug(MAILTRANSPORT_LOG) << "No authentication possible";
        return;
    }

    QStringList protocols;
    if (isGmail(server)) {
        protocols << QStringLiteral("XOAUTH2");
    }

    protocols << QStringLiteral("LOGIN") << QStringLiteral("PLAIN") << QStringLiteral("CRAM-MD5") << QStringLiteral("DIGEST-MD5") << QStringLiteral("NTLM")
              << QStringLiteral("GSSAPI") << QStringLiteral("ANONYMOUS");

    QStringList results;
    for (int i = 0; i < protocols.count(); ++i) {
        if (text.contains(protocols.at(i), Qt::CaseInsensitive)) {
            results.append(protocols.at(i));
        }
    }

    authenticationResults[type] = parseAuthenticationList(results);

    // if we couldn't parse any authentication modes, default to clear-text
    if (authenticationResults[type].isEmpty()) {
        authenticationResults[type] << Transport::EnumAuthenticationType::CLEAR;
    }

    qCDebug(MAILTRANSPORT_LOG) << "For type" << type << ", we have:" << authenticationResults[type];
}

void ServerTestPrivate::slotNormalPossible()
{
    normalSocketTimer->stop();
    connectionResults << Transport::EnumEncryption::None;
}

void ServerTestPrivate::sendInitialCapabilityQuery(MailTransport::Socket *socket)
{
    if (testProtocol == IMAP_PROTOCOL) {
        socket->write(QStringLiteral("1 CAPABILITY"));
    } else if (testProtocol == SMTP_PROTOCOL) {
        // Detect the hostname which we send with the EHLO command.
        // If there is a fake one set, use that, otherwise use the
        // local host name (and make sure it contains a domain, so the
        // server thinks it is valid).
        QString hostname;
        if (!fakeHostname.isNull()) {
            hostname = fakeHostname;
        } else {
            hostname = QHostInfo::localHostName();
            if (hostname.isEmpty()) {
                hostname = QStringLiteral("localhost.invalid");
            } else if (!hostname.contains(QChar::fromLatin1('.'))) {
                hostname += QLatin1String(".localnet");
            }
        }
        qCDebug(MAILTRANSPORT_LOG) << "Hostname for EHLO is" << hostname;

        socket->write(QLatin1String("EHLO ") + hostname);
    }
}

void ServerTestPrivate::slotTlsDone()
{
    // The server will not send a response after starting TLS. Therefore, we have to manually
    // call slotReadNormal(), because this is not triggered by a data received signal this time.
    slotReadNormal(QString());
}

bool ServerTestPrivate::handlePopConversation(MailTransport::Socket *socket, int type, int stage, const QString &response, bool *shouldStartTLS)
{
    Q_ASSERT(shouldStartTLS != nullptr);

    // Initial Greeting
    if (stage == 0) {
        // Regexp taken from POP3 ioslave
        const QString responseWithoutCRLF = response.chopped(2);
        const QRegularExpression re(QStringLiteral("<[A-Za-z0-9\\.\\-_]+@[A-Za-z0-9\\.\\-_]+>$"), QRegularExpression::CaseInsensitiveOption);
        if (responseWithoutCRLF.indexOf(re) != -1) {
            authenticationResults[type] << Transport::EnumAuthenticationType::APOP;
        }

        // Each server is supposed to support clear text login
        authenticationResults[type] << Transport::EnumAuthenticationType::CLEAR;

        // If we are in TLS stage, the server does not send the initial greeting.
        // Assume that the APOP availability is the same as with an unsecured connection.
        if (type == Transport::EnumEncryption::TLS
            && authenticationResults[Transport::EnumEncryption::None].contains(Transport::EnumAuthenticationType::APOP)) {
            authenticationResults[Transport::EnumEncryption::TLS] << Transport::EnumAuthenticationType::APOP;
        }

        socket->write(QStringLiteral("CAPA"));
        return true;
    }
    // CAPA result
    else if (stage == 1) {
        //     Example:
        //     CAPA
        //     +OK
        //     TOP
        //     USER
        //     SASL LOGIN CRAM-MD5
        //     UIDL
        //     RESP-CODES
        //     .
        if (response.contains(QLatin1String("TOP"))) {
            capabilityResults += ServerTest::Top;
        }
        if (response.contains(QLatin1String("PIPELINING"))) {
            capabilityResults += ServerTest::Pipelining;
        }
        if (response.contains(QLatin1String("UIDL"))) {
            capabilityResults += ServerTest::UIDL;
        }
        if (response.contains(QLatin1String("STLS"))) {
            connectionResults << Transport::EnumEncryption::TLS;
            popSupportsTLS = true;
        }
        socket->write(QStringLiteral("AUTH"));
        return true;
    }
    // AUTH response
    else if (stage == 2) {
        //     Example:
        //     C: AUTH
        //     S: +OK List of supported authentication methods follows
        //     S: LOGIN
        //     S: CRAM-MD5
        //     S:.
        QString formattedReply = response;

        // Get rid of trailing ".CRLF"
        formattedReply.chop(3);

        // Get rid of the first +OK line
        formattedReply = formattedReply.right(formattedReply.size() - formattedReply.indexOf(QLatin1Char('\n')) - 1);
        formattedReply = formattedReply.replace(QLatin1Char(' '), QLatin1Char('-')).replace(QLatin1String("\r\n"), QLatin1String(" "));

        authenticationResults[type] += parseAuthenticationList(formattedReply.split(QLatin1Char(' ')));
    }

    *shouldStartTLS = popSupportsTLS;
    return false;
}

bool ServerTestPrivate::handleNntpConversation(MailTransport::Socket *socket, int type, int *stage, const QString &response, bool *shouldStartTLS)
{
    Q_ASSERT(shouldStartTLS != nullptr);
    Q_ASSERT(stage != nullptr);

    // Initial Greeting
    if (*stage == 0) {
        if (response.startsWith(QLatin1String("382 "))) {
            return true;
        }
        if (!response.isEmpty() && !response.startsWith(QLatin1String("200 "))) {
            return false;
        }

        socket->write(QStringLiteral("CAPABILITIES"));
        return true;
    }
    // CAPABILITIES result
    else if (*stage == 1) {
        // Check whether we got "500 command 'CAPABILITIES' not recognized"
        if (response.startsWith(QLatin1String("500 "))) {
            return false;
        }

        //     Example:
        //     101 Capability list:
        //     VERSION 2
        //     IMPLEMENTATION INN 2.5.4
        //     AUTHINFO USER SASL
        //     HDR
        //     LIST ACTIVE [etc]
        //     OVER
        //     POST
        //     READER
        //     SASL DIGEST-MD5 CRAM-MD5 NTLM PLAIN LOGIN
        //     STARTTLS
        //     .
        const QVector<QStringRef> lines = response.splitRef(QStringLiteral("\r\n"), Qt::SkipEmptyParts);
        for (const QStringRef &line : lines) {
            if (line.compare(QLatin1String("STARTTLS"), Qt::CaseInsensitive) == 0) {
                *shouldStartTLS = true;
            } else if (line.startsWith(QLatin1String("AUTHINFO "), Qt::CaseInsensitive)) {
                const QVector<QStringRef> authinfos = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
                const QString s(QStringLiteral("USER"));
                const QStringRef ref(&s);
                if (authinfos.contains(ref)) {
                    authenticationResults[type].append(Transport::EnumAuthenticationType::CLEAR); // XXX
                }
            } else if (line.startsWith(QLatin1String("SASL "), Qt::CaseInsensitive)) {
                const QStringList auths = line.mid(5).toString().split(QLatin1Char(' '), Qt::SkipEmptyParts);
                authenticationResults[type] += parseAuthenticationList(auths);
            } else if (line == QLatin1Char('.')) {
                return false;
            }
        }
        // We have not hit the end of the capabilities list yet,
        // so avoid the stage counter to rise without reason.
        --(*stage);
        return true;
    }

    return false;
}

// slotReadNormal() handles normal (no) encryption and TLS encryption.
// At first, the communication is not encrypted, but if the server supports
// the STARTTLS/STLS keyword, the same authentication query is done again
// with TLS.
void ServerTestPrivate::slotReadNormal(const QString &text)
{
    Q_ASSERT(encryptionMode != Transport::EnumEncryption::SSL);
    static const int tlsHandshakeStage = 42;

    qCDebug(MAILTRANSPORT_LOG) << "Stage" << normalStage + 1 << ", Mode" << encryptionMode;

    // If we are in stage 42, we just do the handshake for TLS encryption and
    // then reset the stage to -1, so that all authentication modes and
    // capabilities are queried again for TLS encryption (some servers have
    // different authentication  methods in normal and in TLS mode).
    if (normalStage == tlsHandshakeStage) {
        Q_ASSERT(encryptionMode == Transport::EnumEncryption::TLS);
        normalStage = -1;
        normalSocket->startTLS();
        return;
    }

    bool shouldStartTLS = false;
    normalStage++;

    // Handle the whole POP and NNTP conversations separately, as
    // they are very different from IMAP and SMTP
    if (testProtocol == POP_PROTOCOL) {
        if (handlePopConversation(normalSocket, encryptionMode, normalStage, text, &shouldStartTLS)) {
            return;
        }
    } else if (testProtocol == NNTP_PROTOCOL) {
        if (handleNntpConversation(normalSocket, encryptionMode, &normalStage, text, &shouldStartTLS)) {
            return;
        }
    } else {
        // Handle the SMTP/IMAP conversation here. We just send the EHLO command in
        // sendInitialCapabilityQuery.
        if (normalStage == 0) {
            sendInitialCapabilityQuery(normalSocket);
            return;
        }

        if (text.contains(QLatin1String("STARTTLS"), Qt::CaseInsensitive)) {
            connectionResults << Transport::EnumEncryption::TLS;
            shouldStartTLS = true;
        }
        handleSMTPIMAPResponse(encryptionMode, text);
    }

    // If we reach here, the normal authentication/capabilities query is completed.
    // Now do the same for TLS.
    normalSocketFinished = true;

    // If the server announced that STARTTLS/STLS is available, we'll add TLS to the
    // connection result, do the command and set the stage to 42 to start the handshake.
    if (shouldStartTLS && encryptionMode == Transport::EnumEncryption::None) {
        qCDebug(MAILTRANSPORT_LOG) << "Trying TLS...";
        connectionResults << Transport::EnumEncryption::TLS;
        if (testProtocol == POP_PROTOCOL) {
            normalSocket->write(QStringLiteral("STLS"));
        } else if (testProtocol == IMAP_PROTOCOL) {
            normalSocket->write(QStringLiteral("2 STARTTLS"));
        } else {
            normalSocket->write(QStringLiteral("STARTTLS"));
        }
        encryptionMode = Transport::EnumEncryption::TLS;
        normalStage = tlsHandshakeStage;
        return;
    }

    // If we reach here, either the TLS authentication/capabilities query is finished
    // or the server does not support the STARTTLS/STLS command.
    tlsFinished = true;
    finalResult();
}

void ServerTestPrivate::slotReadSecure(const QString &text)
{
    secureStage++;
    if (testProtocol == POP_PROTOCOL) {
        bool dummy;
        if (handlePopConversation(secureSocket, Transport::EnumEncryption::SSL, secureStage, text, &dummy)) {
            return;
        }
    } else if (testProtocol == NNTP_PROTOCOL) {
        bool dummy;
        if (handleNntpConversation(secureSocket, Transport::EnumEncryption::SSL, &secureStage, text, &dummy)) {
            return;
        }
    } else {
        if (secureStage == 0) {
            sendInitialCapabilityQuery(secureSocket);
            return;
        }
        handleSMTPIMAPResponse(Transport::EnumEncryption::SSL, text);
    }
    secureSocketFinished = true;
    finalResult();
}

void ServerTestPrivate::slotNormalNotPossible()
{
    if (testProtocol == SMTP_PROTOCOL && normalSocket->port() == SMTP_PORT) {
        // For SMTP, fallback to port 25
        normalSocket->setPort(SMTP_OLD_PORT);
        normalSocket->reconnect();
        normalSocketTimer->start(10000);
        return;
    }

    normalSocketTimer->stop();
    normalPossible = false;
    normalSocketFinished = true;
    tlsFinished = true;
    finalResult();
}

void ServerTestPrivate::slotSslPossible()
{
    secureSocketTimer->stop();
    connectionResults << Transport::EnumEncryption::SSL;
}

void ServerTestPrivate::slotSslNotPossible()
{
    secureSocketTimer->stop();
    securePossible = false;
    secureSocketFinished = true;
    finalResult();
}

void ServerTestPrivate::slotUpdateProgress()
{
    if (testProgress) {
        testProgress->setValue(testProgress->value() + 1);
    }
}

//---------------------- end private class -----------------------//

ServerTest::ServerTest(QObject *parent)
    : QObject(parent)
    , d(new ServerTestPrivate(this))
{
    d->normalSocketTimer = new QTimer(this);
    d->normalSocketTimer->setSingleShot(true);
    connect(d->normalSocketTimer, SIGNAL(timeout()), SLOT(slotNormalNotPossible()));

    d->secureSocketTimer = new QTimer(this);
    d->secureSocketTimer->setSingleShot(true);
    connect(d->secureSocketTimer, SIGNAL(timeout()), SLOT(slotSslNotPossible()));

    d->progressTimer = new QTimer(this);
    connect(d->progressTimer, SIGNAL(timeout()), SLOT(slotUpdateProgress()));
}

ServerTest::~ServerTest()
{
    delete d;
}

void ServerTest::start()
{
    qCDebug(MAILTRANSPORT_LOG) << d;

    d->connectionResults.clear();
    d->authenticationResults.clear();
    d->capabilityResults.clear();
    d->popSupportsTLS = false;
    d->normalStage = -1;
    d->secureStage = -1;
    d->encryptionMode = Transport::EnumEncryption::None;
    d->normalPossible = true;
    d->securePossible = true;

    if (d->testProgress) {
        d->testProgress->setMaximum(20);
        d->testProgress->setValue(0);
        d->testProgress->setTextVisible(true);
        d->testProgress->show();
        d->progressTimer->start(1000);
    }

    d->normalSocket = new MailTransport::Socket(this);
    d->secureSocket = new MailTransport::Socket(this);
    d->normalSocket->setObjectName(QStringLiteral("normal"));
    d->normalSocket->setServer(d->server);
    d->normalSocket->setProtocol(d->testProtocol);
    if (d->testProtocol == IMAP_PROTOCOL) {
        d->normalSocket->setPort(IMAP_PORT);
        d->secureSocket->setPort(IMAPS_PORT);
    } else if (d->testProtocol == SMTP_PROTOCOL) {
        d->normalSocket->setPort(SMTP_PORT);
        d->secureSocket->setPort(SMTPS_PORT);
    } else if (d->testProtocol == POP_PROTOCOL) {
        d->normalSocket->setPort(POP_PORT);
        d->secureSocket->setPort(POPS_PORT);
    } else if (d->testProtocol == NNTP_PROTOCOL) {
        d->normalSocket->setPort(NNTP_PORT);
        d->secureSocket->setPort(NNTPS_PORT);
    }

    if (d->customPorts.contains(Transport::EnumEncryption::None)) {
        d->normalSocket->setPort(d->customPorts.value(Transport::EnumEncryption::None));
    }
    if (d->customPorts.contains(Transport::EnumEncryption::SSL)) {
        d->secureSocket->setPort(d->customPorts.value(Transport::EnumEncryption::SSL));
    }

    connect(d->normalSocket, SIGNAL(connected()), SLOT(slotNormalPossible()));
    connect(d->normalSocket, SIGNAL(failed()), SLOT(slotNormalNotPossible()));
    connect(d->normalSocket, SIGNAL(data(QString)), SLOT(slotReadNormal(QString)));
    connect(d->normalSocket, SIGNAL(tlsDone()), SLOT(slotTlsDone()));
    d->normalSocket->reconnect();
    d->normalSocketTimer->start(10000);

    if (d->secureSocket->port() > 0) {
        d->secureSocket->setObjectName(QStringLiteral("secure"));
        d->secureSocket->setServer(d->server);
        d->secureSocket->setProtocol(d->testProtocol + QLatin1Char('s'));
        d->secureSocket->setSecure(true);
        connect(d->secureSocket, SIGNAL(connected()), SLOT(slotSslPossible()));
        connect(d->secureSocket, SIGNAL(failed()), SLOT(slotSslNotPossible()));
        connect(d->secureSocket, SIGNAL(data(QString)), SLOT(slotReadSecure(QString)));
        d->secureSocket->reconnect();
        d->secureSocketTimer->start(10000);
    } else {
        d->slotSslNotPossible();
    }
}

void ServerTest::setFakeHostname(const QString &fakeHostname)
{
    d->fakeHostname = fakeHostname;
}

QString ServerTest::fakeHostname() const
{
    return d->fakeHostname;
}

void ServerTest::setServer(const QString &server)
{
    d->server = server;
}

void ServerTest::setPort(Transport::EnumEncryption::type encryptionMode, uint port)
{
    Q_ASSERT(encryptionMode == Transport::EnumEncryption::None || encryptionMode == Transport::EnumEncryption::SSL);
    d->customPorts.insert(encryptionMode, port);
}

void ServerTest::setProgressBar(QProgressBar *pb)
{
    d->testProgress = pb;
}

void ServerTest::setProtocol(const QString &protocol)
{
    d->testProtocol = protocol;
    d->customPorts.clear();
}

QString ServerTest::protocol() const
{
    return d->testProtocol;
}

QString ServerTest::server() const
{
    return d->server;
}

int ServerTest::port(TransportBase::EnumEncryption::type encryptionMode) const
{
    Q_ASSERT(encryptionMode == Transport::EnumEncryption::None || encryptionMode == Transport::EnumEncryption::SSL);
    if (d->customPorts.contains(encryptionMode)) {
        return d->customPorts.value(static_cast<int>(encryptionMode));
    } else {
        return -1;
    }
}

QProgressBar *ServerTest::progressBar() const
{
    return d->testProgress;
}

QVector<int> ServerTest::normalProtocols() const
{
    return d->authenticationResults[TransportBase::EnumEncryption::None];
}

bool ServerTest::isNormalPossible() const
{
    return d->normalPossible;
}

QVector<int> ServerTest::tlsProtocols() const
{
    return d->authenticationResults[TransportBase::EnumEncryption::TLS];
}

QVector<int> ServerTest::secureProtocols() const
{
    return d->authenticationResults[Transport::EnumEncryption::SSL];
}

bool ServerTest::isSecurePossible() const
{
    return d->securePossible;
}

QList<ServerTest::Capability> ServerTest::capabilities() const
{
    return d->capabilityResults.values();
}

#include "moc_servertest.cpp"
