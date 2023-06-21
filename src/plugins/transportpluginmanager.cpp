/*
    SPDX-FileCopyrightText: 2017-2023 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportpluginmanager.h"
#include "mailtransport_debug.h"
#include <KPluginFactory>
#include <KPluginMetaData>

#include <MailTransport/TransportAbstractPlugin>

#include <QFileInfo>

using namespace MailTransport;

class MailTransportPluginInfo
{
public:
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    KPluginMetaData data;
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
    Q_REQUIRED_RESULT QList<MailTransport::TransportAbstractPlugin *> pluginsList() const;
    QList<MailTransportPluginInfo> mPluginList;
    void initializePlugins();

private:
    TransportPluginManager *q;
};

void TransportPluginManagerPrivate::initializePlugins()
{
    if (!mPluginList.isEmpty()) {
        return;
    }
    const QList<KPluginMetaData> plugins = KPluginMetaData::findPlugins(QStringLiteral("pim6/mailtransport"));

    QListIterator<KPluginMetaData> i(plugins);
    i.toBack();
    while (i.hasPrevious()) {
        MailTransportPluginInfo info;
        const KPluginMetaData data = i.previous();

        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();
        info.data = data;
        if (pluginVersion() == data.version()) {
            info.plugin = nullptr;
            mPluginList.push_back(info);
        } else {
            qCWarning(MAILTRANSPORT_LOG) << "Plugin " << data.name() << " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    const QList<MailTransportPluginInfo>::iterator end(mPluginList.end());
    for (QList<MailTransportPluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

void TransportPluginManagerPrivate::loadPlugin(MailTransportPluginInfo *item)
{
    if (auto plugin =
            KPluginFactory::instantiatePlugin<MailTransport::TransportAbstractPlugin>(item->data, q, QVariantList() << item->metaDataFileNameBaseName).plugin) {
        item->plugin = plugin;
        QObject::connect(plugin, &TransportAbstractPlugin::updatePluginList, q, &TransportPluginManager::updatePluginList);
    }
}

QList<MailTransport::TransportAbstractPlugin *> TransportPluginManagerPrivate::pluginsList() const
{
    QList<MailTransport::TransportAbstractPlugin *> lst;
    const QList<MailTransportPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<MailTransportPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
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

TransportPluginManager::~TransportPluginManager() = default;

TransportPluginManager *TransportPluginManager::self()
{
    static TransportPluginManager s_self;
    return &s_self;
}

QList<MailTransport::TransportAbstractPlugin *> TransportPluginManager::pluginsList() const
{
    return d->pluginsList();
}

MailTransport::TransportAbstractPlugin *TransportPluginManager::plugin(const QString &identifier)
{
    const QList<MailTransport::TransportAbstractPlugin *> lstPlugins = pluginsList();
    for (MailTransport::TransportAbstractPlugin *p : lstPlugins) {
        const QList<TransportAbstractPluginInfo> lstPluginInfo = p->names();
        for (const MailTransport::TransportAbstractPluginInfo &info : lstPluginInfo) {
            if (info.identifier == identifier) {
                return p;
            }
        }
    }
    return {};
}

#include "moc_transportpluginmanager.cpp"
