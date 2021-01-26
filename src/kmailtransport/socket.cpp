/*
  SPDX-FileCopyrightText: 2006-2007 KovoKs <info@kovoks.nl>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Uncomment the next line for full comm debug
// #define comm_debug

// Own
#include "socket.h"

// Qt
#include <QByteArray>
#include <QNetworkProxy>
// KDE
#include "mailtransport_debug.h"

using namespace MailTransport;

namespace MailTransport
{
class SocketPrivate
{
public:
    SocketPrivate(Socket *s);
    Socket *const q;
    QSslSocket *socket = nullptr;
    QString server;
    QString protocol;
    int port = 0;
    bool secure = false;

    // slots
    void slotConnected();
    void slotStateChanged(QAbstractSocket::SocketState state);
    void slotModeChanged(QSslSocket::SslMode state);
    void slotSocketRead();
    void slotSslErrors(const QList<QSslError> &errors);

private:
    QString m_msg;
};
}

SocketPrivate::SocketPrivate(Socket *s)
    : q(s)
{
}

void SocketPrivate::slotConnected()
{
    qCDebug(MAILTRANSPORT_LOG);

    if (!secure) {
        qCDebug(MAILTRANSPORT_LOG) << "normal connect";
        Q_EMIT q->connected();
    } else {
        qCDebug(MAILTRANSPORT_LOG) << "encrypted connect";
        socket->startClientEncryption();
    }
}

void SocketPrivate::slotStateChanged(QAbstractSocket::SocketState state)
{
#ifdef comm_debug
    qCDebug(MAILTRANSPORT_LOG) << "State is now:" << (int)state;
#endif
    if (state == QAbstractSocket::UnconnectedState) {
        Q_EMIT q->failed();
    }
}

void SocketPrivate::slotModeChanged(QSslSocket::SslMode state)
{
#ifdef comm_debug
    qCDebug(MAILTRANSPORT_LOG) << "Mode is now:" << state;
#endif
    if (state == QSslSocket::SslClientMode) {
        Q_EMIT q->tlsDone();
    }
}

void SocketPrivate::slotSocketRead()
{
    qCDebug(MAILTRANSPORT_LOG);

    if (!socket) {
        return;
    }

    m_msg += QLatin1String(socket->readAll());

    if (!m_msg.endsWith(QLatin1Char('\n'))) {
        return;
    }

#ifdef comm_debug
    qCDebug(MAILTRANSPORT_LOG) << socket->isEncrypted() << m_msg.trimmed();
#endif

    Q_EMIT q->data(m_msg);
    m_msg.clear();
}

void SocketPrivate::slotSslErrors(const QList<QSslError> &)
{
    qCDebug(MAILTRANSPORT_LOG);
    /* We can safely ignore the errors, we are only interested in the
    capabilities. We're not sending auth info. */
    socket->ignoreSslErrors();
    Q_EMIT q->connected();
}

// ------------------ end private ---------------------------//

Socket::Socket(QObject *parent)
    : QObject(parent)
    , d(new SocketPrivate(this))
{
    qCDebug(MAILTRANSPORT_LOG);
}

Socket::~Socket()
{
    qCDebug(MAILTRANSPORT_LOG);
    delete d;
}

void Socket::reconnect()
{
    qCDebug(MAILTRANSPORT_LOG) << "Connecting to:" << d->server << ":" << d->port;

#ifdef comm_debug
    // qCDebug(MAILTRANSPORT_LOG) << d->protocol;
#endif

    if (d->socket) {
        return;
    }

    d->socket = new QSslSocket(this);
    d->socket->setProxy(QNetworkProxy::DefaultProxy);
    d->socket->connectToHost(d->server, d->port);

    d->socket->setProtocol(QSsl::AnyProtocol);

    connect(d->socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(slotStateChanged(QAbstractSocket::SocketState)));
    connect(d->socket, SIGNAL(modeChanged(QSslSocket::SslMode)), SLOT(slotModeChanged(QSslSocket::SslMode)));
    connect(d->socket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(d->socket, SIGNAL(readyRead()), SLOT(slotSocketRead()));
    connect(d->socket, &QSslSocket::encrypted, this, &Socket::connected);
    connect(d->socket, SIGNAL(sslErrors(QList<QSslError>)), SLOT(slotSslErrors(QList<QSslError>)));
}

void Socket::write(const QString &text)
{
    // qCDebug(MAILTRANSPORT_LOG);
    // Eat things in the queue when there is no connection. We need
    // to get a connection first don't we...
    if (!d->socket || !available()) {
        return;
    }

    QByteArray cs = (text + QLatin1String("\r\n")).toLatin1();

#ifdef comm_debug
    qCDebug(MAILTRANSPORT_LOG) << "C   :" << cs;
#endif

    d->socket->write(cs.data(), cs.size());
}

bool Socket::available()
{
    // qCDebug(MAILTRANSPORT_LOG);
    bool ok = d->socket && d->socket->state() == QAbstractSocket::ConnectedState;
    return ok;
}

void Socket::startTLS()
{
    qCDebug(MAILTRANSPORT_LOG) << objectName();
    d->socket->setProtocol(QSsl::TlsV1_0OrLater);
    d->socket->startClientEncryption();
}

void Socket::setProtocol(const QString &proto)
{
    d->protocol = proto;
}

void Socket::setServer(const QString &server)
{
    d->server = server;
}

void Socket::setPort(int port)
{
    d->port = port;
}

int Socket::port() const
{
    return d->port;
}

void Socket::setSecure(bool what)
{
    d->secure = what;
}

#include "moc_socket.cpp"
