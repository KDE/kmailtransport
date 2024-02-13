/*
    SPDX-FileCopyrightText: 2024 g10 Code GmbH
    SPDX-FileContributor: Daniel Vrátil <dvratil@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "xoauthpasswordrequester.h"
#include <accountmanager.h>

namespace KGAPI2
{
class AccountPromise;
}

class GmailPasswordRequester : public MailTransport::XOAuthPasswordRequester
{
    Q_OBJECT
public:
    explicit GmailPasswordRequester(MailTransport::Transport *transport, QObject *parent = nullptr);
    ~GmailPasswordRequester() override;

    void requestPassword(bool forceRefresh) override;

private Q_SLOTS:
    void onTokenRequestFinished(KGAPI2::AccountPromise *promise);
};