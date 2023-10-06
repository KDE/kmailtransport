/*
    SPDX-FileCopyrightText: 2017-2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "kmailtransport_private_export.h"
#include <QObject>

#include <memory>

namespace MailTransport
{
class TransportAbstractPlugin;
class TransportPluginManagerPrivate;
class KMAILTRANSPORT_TESTS_EXPORT TransportPluginManager : public QObject
{
    Q_OBJECT
public:
    explicit TransportPluginManager(QObject *parent = nullptr);
    ~TransportPluginManager() override;

    static TransportPluginManager *self();

    [[nodiscard]] MailTransport::TransportAbstractPlugin *plugin(const QString &identifier);
    [[nodiscard]] QList<MailTransport::TransportAbstractPlugin *> pluginsList() const;

Q_SIGNALS:
    void updatePluginList();

private:
    std::unique_ptr<TransportPluginManagerPrivate> const d;
};
}
