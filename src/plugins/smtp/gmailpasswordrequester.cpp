/*
    SPDX-FileCopyrightText: 2024 g10 Code GmbH
    SPDX-FileContributor: Daniel Vrátil <dvratil@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "gmailpasswordrequester.h"
#include "mailtransportplugin_smtp_debug.h"
#include "plugins/smtp/xoauthpasswordrequester.h"
#include "transport.h"

#include <KGAPI/Account>
#include <KGAPI/AccountManager>

using namespace MailTransport;

static const QString apiKey = QStringLiteral("554041944266.apps.googleusercontent.com");
static const QString apiSecret = QStringLiteral("mdT1DjzohxN3npUUzkENT0gO");

GmailPasswordRequester::GmailPasswordRequester(Transport *transport, QObject *parent)
    : XOAuthPasswordRequester(transport, parent)
{
}

GmailPasswordRequester::~GmailPasswordRequester() = default;

void GmailPasswordRequester::requestPassword(bool forceRefresh)
{
    auto promise = KGAPI2::AccountManager::instance()->findAccount(apiKey, transport()->userName(), {KGAPI2::Account::mailScopeUrl()});
    connect(promise, &KGAPI2::AccountPromise::finished, this, [forceRefresh, this](KGAPI2::AccountPromise *promise) {
        if (promise->account()) {
            if (forceRefresh) {
                promise = KGAPI2::AccountManager::instance()->refreshTokens(apiKey, apiSecret, transport()->userName());
            } else {
                onTokenRequestFinished(promise);
                return;
            }
        } else {
            promise = KGAPI2::AccountManager::instance()->getAccount(apiKey, apiSecret, transport()->userName(), {KGAPI2::Account::mailScopeUrl()});
        }
        connect(promise, &KGAPI2::AccountPromise::finished, this, &GmailPasswordRequester::onTokenRequestFinished);
    });
}

void GmailPasswordRequester::onTokenRequestFinished(KGAPI2::AccountPromise *promise)
{
    if (promise->hasError()) {
        qCWarning(MAILTRANSPORT_SMTP_LOG) << "Error obtaining XOAUTH2 Gmail token:" << promise->errorText();
        Q_EMIT done(Error, {});
        return;
    }

    const auto account = promise->account();
    const QString tokens = QStringLiteral("%1\001%2").arg(account->accessToken(), account->refreshToken());

    Q_EMIT done(PasswordRetrieved, tokens);
}
