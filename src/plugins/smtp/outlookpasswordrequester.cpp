/*
    SPDX-FileCopyrightText: 2024 g10 Code GmbH
    SPDX-FileContributor: Daniel Vrátil <dvratil@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "outlookpasswordrequester.h"
#include "mailtransportplugin_smtp_debug.h"
#include "transport.h"

#include <KWallet>

using namespace MailTransport;

static const QString clientId = QStringLiteral("18da2bc3-146a-4581-8c92-27dc7b9954a0");
static const QString tenantId = QStringLiteral("common");
static const QStringList scopes = {QStringLiteral("https://outlook.office.com/SMTP.Send"), QStringLiteral("offline_access")};

static const QString kwalletFolder = QStringLiteral("mailtransports");

namespace
{

QString loadTokenFromKWallet(KWallet::Wallet *wallet, const QString &name, const QString &tokenType)
{
    if (!wallet->hasFolder(kwalletFolder)) {
        return {};
    }

    wallet->setFolder(kwalletFolder);
    QMap<QString, QString> result;
    wallet->readMap(name, result);
    auto token = result.constFind(tokenType);
    if (token == result.cend()) {
        return {};
    }

    return *token;
}

void storeResultToWallet(KWallet::Wallet *wallet, const QString &name, const TokenResult &result)
{
    qCDebug(MAILTRANSPORT_SMTP_LOG).nospace().noquote() << "Storing Outlook OAuth2 token to KWallet (" << kwalletFolder << "/" << name << ")";
    if (!wallet->hasFolder(kwalletFolder)) {
        wallet->createFolder(kwalletFolder);
    }
    wallet->setFolder(kwalletFolder);
    const int ok = wallet->writeMap(name, {{QStringLiteral("accessToken"), result.accessToken()}, {QStringLiteral("refreshToken"), result.refreshToken()}});
    if (ok != 0) {
        qCWarning(MAILTRANSPORT_SMTP_LOG) << "Failed to store Outlook OAuth2 token to KWallet:" << ok;
    }
}

QString keyForTransport(const Transport *transport)
{
    return QString::number(transport->id());
}

} // namespace

OutlookPasswordRequester::OutlookPasswordRequester(Transport *transport, QObject *parent)
    : XOAuthPasswordRequester(transport, parent)
{
}

OutlookPasswordRequester::~OutlookPasswordRequester() = default;

void OutlookPasswordRequester::requestPassword(bool forceRefresh)
{
    auto *wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(), 0, KWallet::Wallet::Asynchronous);
    connect(wallet, &KWallet::Wallet::walletOpened, this, [this, wallet, forceRefresh](bool success) {
        if (!success) {
            Q_EMIT done(Error, {});
            return;
        }

        mTokenRequester = std::make_unique<OutlookOAuthTokenRequester>(clientId, tenantId, scopes);
        connect(mTokenRequester.get(), &OutlookOAuthTokenRequester::finished, this, [this, wallet](const auto &result) {
            onTokenRequestFinished(wallet, result);
        });

        if (forceRefresh) {
            const auto refreshToken = loadTokenFromKWallet(wallet, keyForTransport(transport()), QStringLiteral("refreshToken"));
            if (!refreshToken.isEmpty()) {
                qCDebug(MAILTRANSPORT_SMTP_LOG) << "Found an Outlook OAuth2 refresh token in KWallet, refreshing access token...";
                mTokenRequester->refreshToken(refreshToken);
            } else {
                qCDebug(MAILTRANSPORT_SMTP_LOG) << "No Outlook OAuth2 refresh token found in KWallet, requesting new token...";
                mTokenRequester->requestToken();
            }
        } else {
            const auto accessToken = loadTokenFromKWallet(wallet, keyForTransport(transport()), QStringLiteral("accessToken"));
            if (accessToken.isEmpty()) {
                qCDebug(MAILTRANSPORT_SMTP_LOG) << "No Outlook OAuth2 access token found in KWallet, requesting new token...";
                mTokenRequester->requestToken();
            } else {
                qCDebug(MAILTRANSPORT_SMTP_LOG) << "Found an Outlook OAuth2 access token in KWallet, using it...";
                Q_EMIT done(PasswordRetrieved, accessToken);
            }
        }
    });
}

void OutlookPasswordRequester::onTokenRequestFinished(KWallet::Wallet *wallet, const TokenResult &result)
{
    if (result.hasError()) {
        qCWarning(MAILTRANSPORT_SMTP_LOG) << "Error obtaining Outlook OAuth2 token:" << result.errorText();
        Q_EMIT done(Error, {});
        return;
    }

    storeResultToWallet(wallet, keyForTransport(transport()), result);

    const QString tokens = QStringLiteral("%1\001%2").arg(result.accessToken(), result.refreshToken());
    Q_EMIT done(PasswordRetrieved, tokens);
}
