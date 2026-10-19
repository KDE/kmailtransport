/*
    SPDX-FileCopyrightText: 2024 g10 Code GmbH
    SPDX-FileContributor: Daniel Vrátil <dvratil@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "xoauthpasswordrequester.h"

#include <memory>

namespace MailTransport
{
class OutlookOAuthTokenRequester;
class TokenResult;
}

class OutlookPasswordRequester : public MailTransport::XOAuthPasswordRequester
{
    Q_OBJECT
public:
    explicit OutlookPasswordRequester(MailTransport::Transport *transport, QObject *parent = nullptr);
    ~OutlookPasswordRequester() override;

    void requestPassword(bool forceRefresh) override;

private Q_SLOTS:
    void onTokenRequestFinished(const MailTransport::TokenResult &result);

private:
    std::unique_ptr<MailTransport::OutlookOAuthTokenRequester> mTokenRequester;
};
