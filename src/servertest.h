/*
  SPDX-FileCopyrightText: 2007 KovoKs <info@kovoks.nl>
  SPDX-FileCopyrightText: 2008 Thomas McGuire <thomas.mcguire@gmx.net>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"
#include "transport.h"

#include <QList>
#include <QObject>

#include <memory>

class QProgressBar;

namespace MailTransport
{
class ServerTestPrivate;

/*!
 * \class MailTransport::ServerTest
 * \inmodule KMailTransport
 * \inheaderfile MailTransport/ServerTest
 * This class can be used to test certain server to see if they support stuff.
 * \author Tom Albers <tomalbers@kde.nl>
 */
class MAILTRANSPORT_EXPORT ServerTest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString server READ server WRITE setServer)
    Q_PROPERTY(QString protocol READ protocol WRITE setProtocol)
    Q_PROPERTY(QProgressBar *progressBar READ progressBar WRITE setProgressBar)

public:
    /*!
     * This enumeration has the special capabilities a server might
     * support. This covers only capabilities not related to authentication.
     * \since 4.1
     * \value Pipelining POP3 only. The server supports pipeplining of commands
     * \value Top POP3 only. The server supports fetching only the headers
     * \value UIDL POP3 only. The server has support for unique identifiers
     */
    enum Capability {
        Pipelining, ///< POP3 only. The server supports pipeplining of commands
        Top, ///< POP3 only. The server supports fetching only the headers
        UIDL ///< POP3 only. The server has support for unique identifiers
    };

    /*!
     * Creates a new server test.
     *
     * \a parent The parent widget.
     */
    explicit ServerTest(QObject *parent = nullptr);

    /*!
     * Destroys the server test.
     */
    ~ServerTest() override;

    /*!
     * Sets the server to test.
     */
    void setServer(const QString &server);

    /*!
     * Returns the server to test.
     */
    [[nodiscard]] QString server() const;

    /*!
     * Set a custom port to use.
     *
     * Each encryption mode (no encryption or SSL) has a different port.
     * TLS uses the same port as no encryption, because TLS is invoked during
     * a normal session.
     *
     * If this function is never called, the default port is used, which is:
     * (normal first, then SSL)
     * SMTP: 587 (falls back to 25), 465
     * POP: 110, 995
     * IMAP: 143, 993
     * NNTP: 119, 563
     *
     * \a encryptionMode the port will only be used in this encryption mode.
     *                       Valid values for this are only 'None' and 'SSL'.
     * \a port the port to use
     *
     * \since 4.1
     */
    void setPort(Transport::EnumEncryption encryptionMode, uint port);

    /*!
     * \a encryptionMode the port of this encryption mode is returned.
     *                       Can only be 'None' and 'SSL'
     *
     * Returns the port set by \ setPort or -1 if \ setPort() was never
     *         called for this encryption mode.
     *
     * \since 4.1
     */
    [[nodiscard]] int port(Transport::EnumEncryption encryptionMode) const;

    /*!
     * Sets a fake hostname for the test. This is currently only used when
     * testing a SMTP server; there, the command for testing the capabilities
     * (called EHLO) needs to have the hostname of the client included.
     * The user can however choose to send a fake hostname instead of the
     * local hostname to work around various problems. This fake hostname needs
     * to be set here.
     *
     * \a fakeHostname the fake hostname to send
     */
    void setFakeHostname(const QString &fakeHostname);

    /*!
     * Returns the fake hostname, as set before with \ setFakeHostname
     */
    [[nodiscard]] QString fakeHostname() const;

    /*!
     * Makes \a pb the progressbar to use. This class will call show()
     * and hide() and will count down. It does not take ownership of
     * the progressbar.
     */
    void setProgressBar(QProgressBar *pb);

    /*!
     * Returns the used progress bar.
     */
    QProgressBar *progressBar() const;

    /*!
     * Sets \a protocol the protocol to test, currently supported are
     * "smtp", "pop", "imap", and "nntp".
     */
    void setProtocol(const QString &protocol);

    /*!
     * Returns the protocol.
     */
    [[nodiscard]] QString protocol() const;

    /*!
     * Starts the test. Will emit finished() when done.
     */
    void start();

    /*!
     * Get the protocols for the normal connections.. Call this only
     * after the finished() signals has been sent.
     * Returns an enum of the type Transport::EnumAuthenticationType
     */
    [[nodiscard]] QList<int> normalProtocols() const;

    /*!
     * tells you if the normal server is available
     * \since 4.5
     */
    [[nodiscard]] bool isNormalPossible() const;

    /*!
     * Get the protocols for the TLS connections. Call this only
     * after the finished() signals has been sent.
     * Returns an enum of the type Transport::EnumAuthenticationType
     * \since 4.1
     */
    [[nodiscard]] QList<int> tlsProtocols() const;

    /*!
     * Get the protocols for the SSL connections. Call this only
     * after the finished() signals has been sent.
     * Returns an enum of the type Transport::EnumAuthenticationType
     */
    [[nodiscard]] QList<int> secureProtocols() const;

    /*!
     * tells you if the ssl server is available
     * \since 4.5
     */
    [[nodiscard]] bool isSecurePossible() const;

    /*!
     * Get the special capabilities of the server.
     * Call this only after the finished() signals has been sent.
     *
     * Returns the list of special capabilities of the server.
     * \since 4.1
     */
    [[nodiscard]] QList<Capability> capabilities() const;

Q_SIGNALS:
    /*!
     * This will be emitted when the test is done. It will contain
     * the values from the enum EnumEncryption which are possible.
     */
    void finished(const QList<int> &);

private:
    Q_DECLARE_PRIVATE(ServerTest)
    std::unique_ptr<ServerTestPrivate> const d;
};
} // namespace MailTransport
