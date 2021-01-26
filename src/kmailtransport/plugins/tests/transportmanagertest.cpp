/*
    SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportmanagertest.h"
#include "kmailtransport/plugins/transportpluginmanager.h"
#include <MailTransport/TransportAbstractPlugin>
#include <QCoreApplication>
#include <QDebug>

TransportManagerTest::TransportManagerTest(QObject *parent)
    : QObject(parent)
{
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    const QVector<MailTransport::TransportAbstractPlugin *> lst = MailTransport::TransportPluginManager::self()->pluginsList();
    if (lst.isEmpty()) {
        qDebug() << "Any plugin found. Please verify your installation";
        return 0;
    }
    for (MailTransport::TransportAbstractPlugin *plugin : lst) {
        const QVector<MailTransport::TransportAbstractPluginInfo> lstPluginInfo = plugin->names();
        for (const MailTransport::TransportAbstractPluginInfo &info : lstPluginInfo) {
            qDebug() << "Plugin name " << info.name;
        }
    }
    return 0;
}
