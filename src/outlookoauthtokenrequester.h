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
     */
    TokenResult(ErrorCode errorCode, const QString &errorText);
    /*!
     */
    TokenResult(const QString &accessToken, const QString &refreshToken);

    /*!
     */
    [[nodiscard]] QString accessToken() const;
    /*!
     */
    [[nodiscard]] QString refreshToken() const;

    /*!
     */
    [[nodiscard]] bool hasError() const;
    /*!
     */
    [[nodiscard]] ErrorCode errorCode() const;
    /*!
     */
    [[nodiscard]] QString errorText() const;

private:
    ErrorCode mErrorCode = ErrorCode::OK;
    QString mErrorText;
    QString mAccessToken;
    QString mRefreshToken;
};

class PKCE;
class MAILTRANSPORT_EXPORT OutlookOAuthTokenRequester : public QObject
{
    Q_OBJECT
public:
    explicit OutlookOAuthTokenRequester(const QString &clientId, const QString &tenantId, const QStringList &scopes, QObject *parent = nullptr);
    ~OutlookOAuthTokenRequester() override;

    void requestToken(const QString &usernameHint = {});
    void refreshToken(const QString &refreshToken);

Q_SIGNALS:
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
