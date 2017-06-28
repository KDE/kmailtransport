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

#include "transportpluginmanager.h"
#include <kpluginmetadata.h>
#include <KPluginLoader>
#include <KPluginFactory>

#include <MailTransport/TransportAbstractPlugin>

using namespace MailTransport;

class TransportPluginManagerInstancePrivate
{
public:
    TransportPluginManagerInstancePrivate()
        : transportPluginManager(new TransportPluginManager)
    {
    }

    ~TransportPluginManagerInstancePrivate()
    {
        delete transportPluginManager;
    }

    TransportPluginManager *transportPluginManager;
};

Q_GLOBAL_STATIC(TransportPluginManagerInstancePrivate, sInstance)

class MailTransportPluginInfo
{
public:
    MailTransportPluginInfo()
        : plugin(nullptr),
          isEnabled(true)
    {

    }

    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    MailTransport::TransportAbstractPlugin *plugin;
    bool isEnabled;
};

namespace
{
QString pluginVersion()
{
    return QStringLiteral("1.0");
}
}

class MailTransport::TransportPluginManagerPrivate
{
public:
    TransportPluginManagerPrivate(TransportPluginManager *qq)
        : q(qq)
    {

    }
    void loadPlugin(MailTransportPluginInfo *item);
    QVector<MailTransport::TransportAbstractPlugin *> pluginsList() const;
    QVector<MailTransportPluginInfo> mPluginList;
private:
    TransportPluginManager *q;
};

QVector<MailTransport::TransportAbstractPlugin *> TransportPluginManagerPrivate::pluginsList() const
{
    QVector<MailTransport::TransportAbstractPlugin *> lst;
    QVector<MailTransportPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<MailTransportPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

TransportPluginManager::TransportPluginManager(QObject *parent)
    : QObject(parent),
      d(new TransportPluginManagerPrivate(this))
{

}

TransportPluginManager::~TransportPluginManager()
{
    delete d;
}

TransportPluginManager *TransportPluginManager::self()
{
    return sInstance->transportPluginManager;
}

QVector<MailTransport::TransportAbstractPlugin *> TransportPluginManager::pluginsList() const
{
    return d->pluginsList();
}

