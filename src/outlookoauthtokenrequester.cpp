/*
    SPDX-FileCopyrightText: 2024 Daniel Vrátil <dvratil@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "outlookoauthtokenrequester.h"
#include "mailtransport_debug.h"

#include <QCryptographicHash>
#include <QDesktopServices>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRandomGenerator64>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUrl>
#include <QUrlQuery>

using namespace MailTransport;

TokenResult::TokenResult(ErrorCode errorCode, const QString &errorText)
    : mErrorCode(errorCode)
    , mErrorText(errorText)
{
}

TokenResult::TokenResult(const QString &accessToken, const QString &refreshToken)
    : mAccessToken(accessToken)
    , mRefreshToken(refreshToken)
{
}

QString TokenResult::accessToken() const
{
    return mAccessToken;
}

QString TokenResult::refreshToken() const
{
    return mRefreshToken;
}

bool TokenResult::hasError() const
{
    return mErrorCode != 0;
}

TokenResult::ErrorCode TokenResult::errorCode() const
{
    return mErrorCode;
}

QString TokenResult::errorText() const
{
    return mErrorText;
}

Q_DECLARE_METATYPE(TokenResult);

/*********************************************************************/

namespace MailTransport
{

/// Helper class to generate PKCE verifier and challenge (RFC 7636)
class PKCE
{
public:
    explicit PKCE()
    {
        mVerifier = generateRandomString(128);
        mChallenge = generateChallenge(mVerifier);
    }

    QString challenge() const
    {
        return mChallenge;
    }

    QString verifier() const
    {
        return mVerifier;
    }

private:
    QString generateRandomString(std::size_t length)
    {
        static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-._`~";

        auto generator = QRandomGenerator::securelySeeded();
        QString result;
        result.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            const int idx = generator.bounded(static_cast<int>(sizeof(charset) - 1));
            result.append(QChar::fromLatin1(charset[idx]));
        }
        return result;
    }

    QString generateChallenge(const QString &verifier)
    {
        const auto sha256 = QCryptographicHash::hash(verifier.toUtf8(), QCryptographicHash::Sha256);
        return QString::fromLatin1(sha256.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals));
    }

private:
    QString mVerifier;
    QString mChallenge;
};

} // namespace

/*********************************************************************/

OutlookOAuthTokenRequester::OutlookOAuthTokenRequester(const QString &clientId, const QString &tenantId, const QStringList &scopes, QObject *parent)
    : QObject(parent)
    , mClientId(clientId)
    , mTenantId(tenantId)
    , mScopes(scopes)
    , mPkce(std::make_unique<PKCE>())
{
}

OutlookOAuthTokenRequester::~OutlookOAuthTokenRequester() = default;

void OutlookOAuthTokenRequester::requestToken(const QString &usernameHint)
{
    qCDebug(MAILTRANSPORT_LOG) << "Requesting new Outlook OAuth2 access token";

    auto redirectUri = startLocalHttpServer();
    if (!redirectUri.has_value()) {
        Q_EMIT finished({TokenResult::InternalError, QStringLiteral("Failed to start local HTTP server to receive Outlook OAuth2 authorization code")});
    }
    mRedirectUri = *redirectUri;

    QUrl url(QStringLiteral("https://login.microsoftonline.com/%1/oauth2/v2.0/authorize").arg(mTenantId));
    QUrlQuery query{{QStringLiteral("client_id"), mClientId},
                    {QStringLiteral("redirect_uri"), mRedirectUri.toString()},
                    {QStringLiteral("response_type"), QStringLiteral("code")},
                    {QStringLiteral("response_mode"), QStringLiteral("query")},
                    {QStringLiteral("scope"), mScopes.join(u' ')},
                    {QStringLiteral("code_challenge"), mPkce->challenge()},
                    {QStringLiteral("code_challenge_method"), QStringLiteral("S256")}};
    if (!usernameHint.isEmpty()) {
        query.addQueryItem(QStringLiteral("login_hint"), usernameHint);
    } else {
        query.addQueryItem(QStringLiteral("prompt"), QStringLiteral("select_account"));
    }
    url.setQuery(query);

    qCDebug(MAILTRANSPORT_LOG) << "Browser opened, waiting for Outlook OAuth2 authorization code...";
    QDesktopServices::openUrl(url);
}

void OutlookOAuthTokenRequester::refreshToken(const QString &refreshToken)
{
    qCDebug(MAILTRANSPORT_LOG) << "Refreshing Outlook OAuth2 access token";

    QUrl url(QStringLiteral("https://login.microsoftonline.com/%1/oauth2/v2.0/token").arg(mTenantId));

    QNetworkRequest request{url};
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));
    mNam = std::make_unique<QNetworkAccessManager>();
    auto *reply = mNam->post(request,
                             QUrlQuery{{QStringLiteral("client_id"), mClientId},
                                       {QStringLiteral("grant_type"), QStringLiteral("refresh_token")},
                                       {QStringLiteral("scope"), mScopes.join(u' ')},
                                       {QStringLiteral("refresh_token"), refreshToken}}
                                 .toString(QUrl::FullyEncoded)
                                 .toUtf8());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleTokenResponse(reply, true);
    });
}

std::optional<QUrl> OutlookOAuthTokenRequester::startLocalHttpServer()
{
    mHttpServer = std::make_unique<QTcpServer>();
    connect(mHttpServer.get(), &QTcpServer::newConnection, this, &OutlookOAuthTokenRequester::handleNewConnection);
    if (!mHttpServer->listen(QHostAddress::LocalHost)) {
        return {};
    }
    qCDebug(MAILTRANSPORT_LOG) << "Local Outlook OAuth2 server listening on port" << mHttpServer->serverPort();

    return QUrl(QStringLiteral("http://localhost:%1").arg(mHttpServer->serverPort()));
}

void OutlookOAuthTokenRequester::handleNewConnection()
{
    qCDebug(MAILTRANSPORT_LOG) << "New incoming connection from Outlook OAuth2";
    mSocket = std::unique_ptr<QTcpSocket>(mHttpServer->nextPendingConnection());
    connect(mSocket.get(), &QTcpSocket::readyRead, this, &OutlookOAuthTokenRequester::handleSocketReadyRead);
}

void OutlookOAuthTokenRequester::handleSocketReadyRead()
{
    auto request = mSocket->readLine();
    mSocket->readAll(); // read the rest of data and discard it

    sendResponseToBrowserAndCloseSocket();

    if (!request.startsWith("GET /?") && !request.endsWith("HTTP/1.1")) {
        Q_EMIT finished({TokenResult::InvalidAuthorizationResponse, QStringLiteral("Invalid authorization response from server")});
        return;
    }

    // Remove verb and protocol from the request line
    request.remove(0, sizeof("GET ") - 1);
    request.truncate(request.size() - sizeof(" HTTP/1.1") - 1);
    // Prefix it with protocol and domain so that it's a full URL that we can parse
    request.prepend("http://localhost");

    // Try to parse the URL
    QUrl url(QString::fromUtf8(request));
    if (!url.isValid()) {
        qCWarning(MAILTRANSPORT_LOG) << "Failed to extract valid URL from initial HTTP request line from Outlook OAuth2:" << request;
        Q_EMIT finished({TokenResult::InvalidAuthorizationResponse, QStringLiteral("Invalid authorization response from server")});
        return;
    }

    // Extract code
    const QUrlQuery query(url);
    if (query.hasQueryItem(QStringLiteral("error"))) {
        const auto error = query.queryItemValue(QStringLiteral("error"));
        const auto errorDescription = query.queryItemValue(QStringLiteral("error_description"));
        qCWarning(MAILTRANSPORT_LOG) << "Authorization server returned error:" << error << errorDescription;
        Q_EMIT finished({TokenResult::AuthorizationFailed, errorDescription});
        return;
    }

    const auto code = query.queryItemValue(QStringLiteral("code"));
    if (code.isEmpty()) {
        qCWarning(MAILTRANSPORT_LOG) << "Failed to extract authorization code from Outlook OAuth2 response:" << request;
        Q_EMIT finished({TokenResult::InvalidAuthorizationResponse, QStringLiteral("Invalid authorization response from server")});
        return;
    }

    qCDebug(MAILTRANSPORT_LOG) << "Extracted Outlook OAuth2 autorization token from response, requesting access token...";
    requestIdToken(code);
}

void OutlookOAuthTokenRequester::requestIdToken(const QString &code)
{
    QUrl url(QStringLiteral("https://login.microsoftonline.com/%1/oauth2/v2.0/token").arg(mTenantId));

    QNetworkRequest request{url};
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));
    mNam = std::make_unique<QNetworkAccessManager>();
    auto *reply = mNam->post(request,
                             QUrlQuery{{QStringLiteral("client_id"), mClientId},
                                       {QStringLiteral("scope"), mScopes.join(u' ')},
                                       {QStringLiteral("code"), code},
                                       {QStringLiteral("redirect_uri"), mRedirectUri.toString()},
                                       {QStringLiteral("grant_type"), QStringLiteral("authorization_code")},
                                       {QStringLiteral("code_verifier"), mPkce->verifier()}}
                                 .toString(QUrl::FullyEncoded)
                                 .toUtf8());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleTokenResponse(reply);
    });
    qCDebug(MAILTRANSPORT_LOG) << "Requested Outlook OAuth2 access token, waiting for response...";
}

void OutlookOAuthTokenRequester::handleTokenResponse(QNetworkReply *reply, bool isTokenRefresh)
{
    const auto responseData = reply->readAll();
    reply->deleteLater();

    const auto response = QJsonDocument::fromJson(responseData);
    if (!response.isObject()) {
        qCWarning(MAILTRANSPORT_LOG) << "Failed to parse token response:" << responseData;
        Q_EMIT finished({TokenResult::InvalidAuthorizationResponse, QStringLiteral("Failed to parse token response")});
        return;
    }

    if (response[QStringView{u"error"}].isString()) {
        const auto error = response[QStringView{u"error"}].toString();
        const auto errorDescription = response[QStringView{u"error_description"}].toString();
        qCWarning(MAILTRANSPORT_LOG) << "Outlook OAuth2 authorization server returned error:" << error << errorDescription;

        if (isTokenRefresh && error == u"invalid_grant") {
            qCDebug(MAILTRANSPORT_LOG) << "Outlook OAuth2 refresh token is invalid, requesting new token...";
            requestToken();
        } else {
            Q_EMIT finished({TokenResult::AuthorizationFailed, errorDescription});
        }
        return;
    }

    const auto accessToken = response[QStringView{u"access_token"}].toString();
    const auto refreshToken = response[QStringView{u"refresh_token"}].toString();

    qCDebug(MAILTRANSPORT_LOG) << "Received Outlook OAuth2 access and refresh tokens";

    Q_EMIT finished({accessToken, refreshToken});
}

void OutlookOAuthTokenRequester::sendResponseToBrowserAndCloseSocket()
{
    const auto response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html>"
        "<head><title>KDE PIM Authorization</title></head>"
        "<body>"
        "<h1>You can close the browser window now and return to the application.</h1>"
        "</body>"
        "</html>\r\n\r\n";

    mSocket->write(response);
    mSocket->flush();
    mSocket->close();

    mSocket.release()->deleteLater();

    mHttpServer->close();
    mHttpServer.release()->deleteLater();

    qCDebug(MAILTRANSPORT_LOG) << "Sent HTTP OK response to browser and closed our local HTTP server.";
}
