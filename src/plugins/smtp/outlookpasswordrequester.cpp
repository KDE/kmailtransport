/*
    SPDX-FileCopyrightText: 2024 g10 Code GmbH
    SPDX-FileContributor: Daniel Vrátil <dvratil@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "outlookpasswordrequester.h"
using namespace Qt::Literals::StringLiterals;

#include "mailtransportplugin_smtp_debug.h"
#include "transport.h"

#include <QDataStream>
#include <QVariantMap>

#include <memory>
#include <qt6keychain/keychain.h>

using namespace MailTransport;

static const QString clientId = u"18da2bc3-146a-4581-8c92-27dc7b9954a0"_s;
static const QString tenantId = u"common"_s;
static const QStringList scopes = {u"https://outlook.office.com/SMTP.Send"_s, u"offline_access"_s};

namespace
{

TokenResult extractTokens(QKeychain::Job *job)
{
    auto readJob = static_cast<QKeychain::ReadPasswordJob *>(job);
    QDataStream stream(readJob->binaryData());
    QVariantMap map;
    stream >> map;

    const auto accessToken = map.value(u"accessToken"_s).toString();
    const auto refreshToken = map.value(u"refreshToken"_s).toString();
    return {accessToken, refreshToken};
}

QByteArray serializeTokens(const TokenResult &result)
{
    QVariantMap map = {{u"accessToken"_s, result.accessToken()}, {u"refreshToken"_s, result.refreshToken()}};
    QByteArray data;
    QDataStream stream(&data, QDataStream::WriteOnly);
    stream << map;
    return data;
}

} // namespace

OutlookPasswordRequester::OutlookPasswordRequester(Transport *transport, QObject *parent)
    : XOAuthPasswordRequester(transport, parent)
{
}

OutlookPasswordRequester::~OutlookPasswordRequester() = default;

void OutlookPasswordRequester::requestPassword(bool forceRefresh)
{
    auto job = new QKeychain::ReadPasswordJob(u"mailtransports"_s);
    job->setKey(QString::number(transport()->id()));
    connect(job, &QKeychain::ReadPasswordJob::finished, this, [this, forceRefresh](QKeychain::Job *job) {
        if (job->error() == QKeychain::Error::EntryNotFound) {
            qCDebug(MAILTRANSPORT_SMTP_LOG) << "No Outlook OAuth2 token found in keychain, requesting new token...";
            mTokenRequester = std::make_unique<OutlookOAuthTokenRequester>(clientId, tenantId, scopes);
            connect(mTokenRequester.get(), &OutlookOAuthTokenRequester::finished, this, &OutlookPasswordRequester::onTokenRequestFinished);
            mTokenRequester->requestToken(transport()->userName());
            return;
        }

        if (job->error() != QKeychain::Error::NoError) {
            qCWarning(MAILTRANSPORT_SMTP_LOG) << "Failed to read Outlook OAuth2 token from keychain:" << job->errorString();
            Q_EMIT done(Error, {});
            return;
        }

        mTokenRequester = std::make_unique<OutlookOAuthTokenRequester>(clientId, tenantId, scopes);
        connect(mTokenRequester.get(), &OutlookOAuthTokenRequester::finished, this, &OutlookPasswordRequester::onTokenRequestFinished);

        const auto tokens = extractTokens(job);
        if (tokens.accessToken().isEmpty()) {
            qCDebug(MAILTRANSPORT_SMTP_LOG) << "No Outlook OAuth2 access token found in keychain, requesting new token...";
            mTokenRequester->requestToken(transport()->userName());
        } else if (forceRefresh) {
            if (!tokens.refreshToken().isEmpty()) {
                qCDebug(MAILTRANSPORT_SMTP_LOG) << "Found an Outlook OAuth2 refresh token in keychain, refreshing access token...";
                mTokenRequester->refreshToken(tokens.refreshToken());
            } else {
                qCDebug(MAILTRANSPORT_SMTP_LOG) << "No Outlook OAuth2 refresh token found in keychain, requesting new token...";
                mTokenRequester->requestToken(transport()->userName());
            }
        } else {
            qCDebug(MAILTRANSPORT_SMTP_LOG) << "Found an Outlook OAuth2 access token in KWallet, using it...";
            Q_EMIT done(PasswordRetrieved, tokens.accessToken());
        }
    });
    job->start();
}

void OutlookPasswordRequester::onTokenRequestFinished(const TokenResult &result)
{
    if (result.hasError()) {
        qCWarning(MAILTRANSPORT_SMTP_LOG) << "Error obtaining Outlook OAuth2 token:" << result.errorText();
        Q_EMIT done(Error, {});
        return;
    }

    auto job = new QKeychain::WritePasswordJob(u"mailtransports"_s);
    job->setKey(QString::number(transport()->id()));
    job->setBinaryData(serializeTokens(result));
    connect(job, &QKeychain::WritePasswordJob::finished, this, [result](QKeychain::Job *job) {
        if (job->error() != QKeychain::Error::NoError) {
            qCWarning(MAILTRANSPORT_SMTP_LOG) << "Failed to store Outlook OAuth2 token to keychain:" << job->errorString();
        }
    });
    job->start();

    const QString tokens = u"%1\001%2"_s.arg(result.accessToken(), result.refreshToken());
    Q_EMIT done(PasswordRetrieved, tokens);
}

#include "moc_outlookpasswordrequester.cpp"
