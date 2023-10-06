/*
  SPDX-FileCopyrightText: 2017 Daniel Vrátil <dvratil@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KIO/SslUi>
#include <KSMTP/SessionUiProxy>

class SmtpSessionUiProxy : public KSmtp::SessionUiProxy
{
public:
    [[nodiscard]] bool ignoreSslError(const KSslErrorUiData &errorData) override
    {
        return KIO::SslUi::askIgnoreSslErrors(errorData, KIO::SslUi::RecallAndStoreRules);
    }
};
