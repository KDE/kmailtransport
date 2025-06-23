/*
    SPDX-FileCopyrightText: 2024 Daniel Vrátil <dvratil@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "outlookoauthtokenrequester.h"
using namespace Qt::Literals::StringLiterals;

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
        Q_EMIT finished({TokenResult::InternalError, u"Failed to start local HTTP server to receive Outlook OAuth2 authorization code"_s});
    }
    mRedirectUri = *redirectUri;

    QUrl url(u"https://login.microsoftonline.com/%1/oauth2/v2.0/authorize"_s.arg(mTenantId));
    QUrlQuery query{{u"client_id"_s, mClientId},
                    {u"redirect_uri"_s, mRedirectUri.toString()},
                    {u"response_type"_s, u"code"_s},
                    {u"response_mode"_s, u"query"_s},
                    {u"scope"_s, mScopes.join(u' ')},
                    {u"code_challenge"_s, mPkce->challenge()},
                    {u"code_challenge_method"_s, u"S256"_s}};
    if (!usernameHint.isEmpty()) {
        query.addQueryItem(u"login_hint"_s, usernameHint);
    } else {
        query.addQueryItem(u"prompt"_s, u"select_account"_s);
    }
    url.setQuery(query);

    qCDebug(MAILTRANSPORT_LOG) << "Browser opened, waiting for Outlook OAuth2 authorization code...";
    QDesktopServices::openUrl(url);
}

void OutlookOAuthTokenRequester::refreshToken(const QString &refreshToken)
{
    qCDebug(MAILTRANSPORT_LOG) << "Refreshing Outlook OAuth2 access token";

    QUrl url(u"https://login.microsoftonline.com/%1/oauth2/v2.0/token"_s.arg(mTenantId));

    QNetworkRequest request{url};
    request.setHeader(QNetworkRequest::ContentTypeHeader, u"application/x-www-form-urlencoded"_s);
    mNam = std::make_unique<QNetworkAccessManager>();
    auto *reply = mNam->post(
        request,
        QUrlQuery{{u"client_id"_s, mClientId}, {u"grant_type"_s, u"refresh_token"_s}, {u"scope"_s, mScopes.join(u' ')}, {u"refresh_token"_s, refreshToken}}
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

    return QUrl(u"http://localhost:%1"_s.arg(mHttpServer->serverPort()));
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
        Q_EMIT finished({TokenResult::InvalidAuthorizationResponse, u"Invalid authorization response from server"_s});
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
        Q_EMIT finished({TokenResult::InvalidAuthorizationResponse, u"Invalid authorization response from server"_s});
        return;
    }

    // Extract code
    const QUrlQuery query(url);
    if (query.hasQueryItem(u"error"_s)) {
        const auto error = query.queryItemValue(u"error"_s);
        const auto errorDescription = query.queryItemValue(u"error_description"_s);
        qCWarning(MAILTRANSPORT_LOG) << "Authorization server returned error:" << error << errorDescription;
        Q_EMIT finished({TokenResult::AuthorizationFailed, errorDescription});
        return;
    }

    const auto code = query.queryItemValue(u"code"_s);
    if (code.isEmpty()) {
        qCWarning(MAILTRANSPORT_LOG) << "Failed to extract authorization code from Outlook OAuth2 response:" << request;
        Q_EMIT finished({TokenResult::InvalidAuthorizationResponse, u"Invalid authorization response from server"_s});
        return;
    }

    qCDebug(MAILTRANSPORT_LOG) << "Extracted Outlook OAuth2 authorization token from response, requesting access token...";
    requestIdToken(code);
}

void OutlookOAuthTokenRequester::requestIdToken(const QString &code)
{
    QUrl url(u"https://login.microsoftonline.com/%1/oauth2/v2.0/token"_s.arg(mTenantId));

    QNetworkRequest request{url};
    request.setHeader(QNetworkRequest::ContentTypeHeader, u"application/x-www-form-urlencoded"_s);
    mNam = std::make_unique<QNetworkAccessManager>();
    auto *reply = mNam->post(request,
                             QUrlQuery{{u"client_id"_s, mClientId},
                                       {u"scope"_s, mScopes.join(u' ')},
                                       {u"code"_s, code},
                                       {u"redirect_uri"_s, mRedirectUri.toString()},
                                       {u"grant_type"_s, u"authorization_code"_s},
                                       {u"code_verifier"_s, mPkce->verifier()}}
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
        Q_EMIT finished({TokenResult::InvalidAuthorizationResponse, u"Failed to parse token response"_s});
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

#include "moc_outlookoauthtokenrequester.cpp"
