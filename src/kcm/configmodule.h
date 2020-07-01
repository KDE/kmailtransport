/*
    SPDX-FileCopyrightText: 2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MAILTRANSPORT_CONFIGMODULE_H
#define MAILTRANSPORT_CONFIGMODULE_H

#include <KCModule>

namespace MailTransport {
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

#endif // MAILTRANSPORT_CONFIGMODULE_H
