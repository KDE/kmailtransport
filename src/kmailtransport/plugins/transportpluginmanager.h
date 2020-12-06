/*
    SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TRANSPORTPLUGINMANAGER_H
#define TRANSPORTPLUGINMANAGER_H

#include <QObject>
#include "kmailtransport_private_export.h"
namespace MailTransport {
class TransportAbstractPlugin;
class TransportPluginManagerPrivate;
class KMAILTRANSPORT_TESTS_EXPORT TransportPluginManager : public QObject
{
    Q_OBJECT
public:
    explicit TransportPluginManager(QObject *parent = nullptr);
    ~TransportPluginManager() override;

    static TransportPluginManager *self();

    Q_REQUIRED_RESULT MailTransport::TransportAbstractPlugin *plugin(const QString &identifier);
    Q_REQUIRED_RESULT QVector<MailTransport::TransportAbstractPlugin *> pluginsList() const;

Q_SIGNALS:
    void updatePluginList();

private:
    TransportPluginManagerPrivate *const d;
};
}

#endif // TRANSPORTPLUGINMANAGER_H
