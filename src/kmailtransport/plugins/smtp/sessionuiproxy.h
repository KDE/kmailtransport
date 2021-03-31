/*
  SPDX-FileCopyrightText: 2017 Daniel Vrátil <dvratil@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KSMTP/SessionUiProxy>
#include <kio/sslui.h>

class SmtpSessionUiProxy : public KSmtp::SessionUiProxy
{
public:
    Q_REQUIRED_RESULT bool ignoreSslError(const KSslErrorUiData &errorData) override
    {
        return KIO::SslUi::askIgnoreSslErrors(errorData, KIO::SslUi::RecallAndStoreRules);
    }
};

