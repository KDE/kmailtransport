/*
  SPDX-FileCopyrightText: 2006-2007 KovoKs <info@kovoks.nl>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"

#include <QSslSocket>

#include <memory>

namespace MailTransport
{
class SocketPrivate;

/**
 * @class Socket
 * Responsible for communicating with the server, it's designed to work
 * with the ServerTest class.
 * @author Tom Albers <tomalbers@kde.nl>
 */
class Socket : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor, it will not auto connect. Call reconnect() to connect to
     * the parameters given.
     * @param parent the parent
     */
    explicit Socket(QObject *parent);

    /**
     * Destructor
     */
    ~Socket() override;

    /**
     * Existing connection will be closed and a new connection will be
     * made
     */
    virtual void reconnect();

    /**
     * Write @p text to the socket
     */
    virtual void write(const QString &text);

    /**
     * @return true when the connection is live and kicking
     */
    virtual bool available();

    /**
     * set the protocol to use
     */
    void setProtocol(const QString &proto);

    /**
     * set the server to use
     */
    void setServer(const QString &server);

    /**
     * set the port to use. If not specified, it will use the default
     * belonging to the protocol.
     */
    void setPort(int port);

    /**
     * returns the used port.
     */
    int port() const;

    /**
     * this will be a secure connection
     */
    void setSecure(bool what);

    /**
     * If you want to start TLS encryption, call this. For example after the starttls command.
     */
    void startTLS();

private:
    Q_DECLARE_PRIVATE(Socket)
    std::unique_ptr<SocketPrivate> const d;

    Q_PRIVATE_SLOT(d, void slotConnected())
    Q_PRIVATE_SLOT(d, void slotStateChanged(QAbstractSocket::SocketState state))
    Q_PRIVATE_SLOT(d, void slotModeChanged(QSslSocket::SslMode state))
    Q_PRIVATE_SLOT(d, void slotSocketRead())
    Q_PRIVATE_SLOT(d, void slotSslErrors(const QList<QSslError> &errors))

Q_SIGNALS:
    /**
     * emits the incoming data
     */
    void data(const QString &);

    /**
     * emitted when there is a connection (ready to send something).
     */
    void connected();

    /**
     * emitted when not connected.
     */
    void failed();

    /**
     * emitted when startShake() is completed.
     */
    void tlsDone();
};
} // namespace MailTransport
