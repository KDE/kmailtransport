/*
    SPDX-FileCopyrightText: 2024 Daniel Vrátil <dvratil@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"

#include <QObject>
#include <QString>
#include <QUrl>

#include <memory>
#include <optional>

class QTcpSocket;
class QTcpServer;
class QNetworkReply;
class QNetworkAccessManager;

namespace MailTransport
{
/*!
 * \class MailTransport::TokenResult
 * \inmodule KMailTransport
 * \inheaderfile MailTransport/OutlookOAuthTokenRequester
 * \brief The TokenResult class
 */
class MAILTRANSPORT_EXPORT TokenResult
{
public:
    enum ErrorCode {
        OK = 0, // success
        InternalError,
        InvalidAuthorizationResponse,
        AuthorizationFailed,
    };

    /*!
      Creates a TokenResult with an error code and error text.
    */
    TokenResult(ErrorCode errorCode, const QString &errorText);
    /*!
      Creates a TokenResult with an access token and refresh token.
    */
    TokenResult(const QString &accessToken, const QString &refreshToken);

    /*!
      Returns the access token.
    */
    [[nodiscard]] QString accessToken() const;
    /*!
      Returns the refresh token.
    */
    [[nodiscard]] QString refreshToken() const;

    /*!
      Returns whether this result contains an error.
    */
    [[nodiscard]] bool hasError() const;
    /*!
      Returns the error code.
    */
    [[nodiscard]] ErrorCode errorCode() const;
    /*!
      Returns the error text.
    */
    [[nodiscard]] QString errorText() const;

private:
    ErrorCode mErrorCode = ErrorCode::OK;
    QString mErrorText;
    QString mAccessToken;
    QString mRefreshToken;
};

class PKCE;
/*!
 * \class MailTransport::OutlookOAuthTokenRequester
 * \inmodule KMailTransport
 * \inheaderfile MailTransport/OutlookOAuthTokenRequester
 * \brief OAuth token requester for Outlook
 */
class MAILTRANSPORT_EXPORT OutlookOAuthTokenRequester : public QObject
{
    Q_OBJECT
public:
    /*!
      Creates a new OutlookOAuthTokenRequester with the specified credentials and parent object.
      \a clientId The OAuth client ID.
      \a tenantId The Azure tenant ID.
      \a scopes The OAuth scopes to request.
      \a parent The parent object.
    */
    explicit OutlookOAuthTokenRequester(const QString &clientId, const QString &tenantId, const QStringList &scopes, QObject *parent = nullptr);
    /*!
      Destroys the OutlookOAuthTokenRequester.
    */
    ~OutlookOAuthTokenRequester() override;

    /*!
      Requests a new OAuth token with an optional username hint.
    */
    void requestToken(const QString &usernameHint = {});
    /*!
      Refreshes an existing OAuth token.
    */
    void refreshToken(const QString &refreshToken);

Q_SIGNALS:
    /*!
      Emitted when the token request or refresh is finished.
    */
    void finished(const MailTransport::TokenResult &result);

private:
    [[nodiscard]] MAILTRANSPORT_NO_EXPORT std::optional<QUrl> startLocalHttpServer();

    MAILTRANSPORT_NO_EXPORT void handleNewConnection();
    MAILTRANSPORT_NO_EXPORT void handleSocketReadyRead();
    MAILTRANSPORT_NO_EXPORT void requestIdToken(const QString &code);
    MAILTRANSPORT_NO_EXPORT void handleTokenResponse(QNetworkReply *reply, bool isTokenRefresh = false);
    MAILTRANSPORT_NO_EXPORT void sendResponseToBrowserAndCloseSocket();

private:
    QString mClientId;
    QString mTenantId;
    QStringList mScopes;
    QUrl mRedirectUri;
    std::unique_ptr<PKCE> mPkce;
    std::unique_ptr<QTcpServer> mHttpServer;
    std::unique_ptr<QTcpSocket> mSocket;
    std::unique_ptr<QNetworkAccessManager> mNam;
};

} // namespace MailTransport
