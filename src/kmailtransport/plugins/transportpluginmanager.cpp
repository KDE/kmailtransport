/*
    SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportpluginmanager.h"
#include "mailtransport_debug.h"
#include <KPluginFactory>
#include <KPluginLoader>
#include <kpluginmetadata.h>

#include <MailTransport/TransportAbstractPlugin>

#include <QFileInfo>

using namespace MailTransport;

class MailTransportPluginInfo
{
public:
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    MailTransport::TransportAbstractPlugin *plugin = nullptr;
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
        initializePlugins();
    }

    void loadPlugin(MailTransportPluginInfo *item);
    QVector<MailTransport::TransportAbstractPlugin *> pluginsList() const;
    QVector<MailTransportPluginInfo> mPluginList;
    bool initializePlugins();

private:
    TransportPluginManager *q;
};

bool TransportPluginManagerPrivate::initializePlugins()
{
    if (!mPluginList.isEmpty()) {
        return true;
    }
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("mailtransport"));

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    while (i.hasPrevious()) {
        MailTransportPluginInfo info;
        const KPluginMetaData data = i.previous();

        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();
        if (pluginVersion() == data.version()) {
            info.plugin = nullptr;
            mPluginList.push_back(info);
        } else {
            qCWarning(MAILTRANSPORT_LOG) << "Plugin " << data.name() << " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    const QVector<MailTransportPluginInfo>::iterator end(mPluginList.end());
    for (QVector<MailTransportPluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
    return true;
}

void TransportPluginManagerPrivate::loadPlugin(MailTransportPluginInfo *item)
{
    KPluginLoader pluginLoader(item->metaDataFileName);
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<MailTransport::TransportAbstractPlugin>(q, QVariantList() << item->metaDataFileNameBaseName);
        if (item->plugin) {
            QObject::connect(item->plugin, &TransportAbstractPlugin::updatePluginList, q, &TransportPluginManager::updatePluginList);
        }
    }
}

QVector<MailTransport::TransportAbstractPlugin *> TransportPluginManagerPrivate::pluginsList() const
{
    QVector<MailTransport::TransportAbstractPlugin *> lst;
    const QVector<MailTransportPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<MailTransportPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

TransportPluginManager::TransportPluginManager(QObject *parent)
    : QObject(parent)
    , d(new TransportPluginManagerPrivate(this))
{
}

TransportPluginManager::~TransportPluginManager()
{
    delete d;
}

TransportPluginManager *TransportPluginManager::self()
{
    static TransportPluginManager s_self;
    return &s_self;
}

QVector<MailTransport::TransportAbstractPlugin *> TransportPluginManager::pluginsList() const
{
    return d->pluginsList();
}

MailTransport::TransportAbstractPlugin *TransportPluginManager::plugin(const QString &identifier)
{
    const QVector<MailTransport::TransportAbstractPlugin *> lstPlugins = pluginsList();
    for (MailTransport::TransportAbstractPlugin *p : lstPlugins) {
        const QVector<TransportAbstractPluginInfo> lstPluginInfo = p->names();
        for (const MailTransport::TransportAbstractPluginInfo &info : lstPluginInfo) {
            if (info.identifier == identifier) {
                return p;
            }
        }
    }
    return {};
}
