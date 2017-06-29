/*
    Copyright (c) 2017 Laurent Montel <montel@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "transportmanagertest.h"
#include "kmailtransport/plugins/transportpluginmanager.h"
#include <QCoreApplication>
#include <MailTransport/TransportAbstractPlugin>
#include <QDebug>

TransportManagerTest::TransportManagerTest(QObject *parent)
    : QObject(parent)
{
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    const QVector<MailTransport::TransportAbstractPlugin *> lst = MailTransport::TransportPluginManager::self()->pluginsList();
    for (MailTransport::TransportAbstractPlugin *plugin : lst) {
        const QVector<MailTransport::TransportAbstractPluginInfo> lstPluginInfo = plugin->names();
        for (const MailTransport::TransportAbstractPluginInfo &info : lstPluginInfo) {
            qDebug() << "Plugin name " << info.name;
        }
    }
    return 0;
}
