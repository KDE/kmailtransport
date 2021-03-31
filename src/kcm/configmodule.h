/*
    SPDX-FileCopyrightText: 2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KCModule>

namespace MailTransport
{
/**
  KCModule for transport management.
*/
class ConfigModule : public KCModule
{
    Q_OBJECT
public:
    explicit ConfigModule(QWidget *parent = nullptr, const QVariantList &args = QVariantList());
};
} // namespace MailTransport

