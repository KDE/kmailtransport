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
/*
    static const QString s_serviceTypeName = serviceTypeName;
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(pluginName, [](const KPluginMetaData & md) {
        return md.serviceTypes().contains(s_serviceTypeName);
    });

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());
    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    while (i.hasPrevious()) {
        GenericPluginInfo info;
        const KPluginMetaData data = i.previous();

        //1) get plugin data => name/description etc.
        info.pluginData = PimCommon::PluginUtil::createPluginMetaData(data);
        //2) look at if plugin is activated
        const bool isPluginActivated = PimCommon::PluginUtil::isPluginActivated(pair.first, pair.second, info.pluginData.mEnableByDefault, info.pluginData.mIdentifier);
        info.isEnabled = isPluginActivated;
        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();

        if (pluginVersion() == data.version()) {
            // only load plugins once, even if found multiple times!
            if (unique.contains(info.metaDataFileNameBaseName)) {
                continue;
            }
            info.plugin = nullptr;
            mPluginList.push_back(info);
            unique.insert(info.metaDataFileNameBaseName);
        } else {
            qCWarning(PIMCOMMON_LOG) << "Plugin " << data.name() << " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    QVector<GenericPluginInfo>::iterator end(mPluginList.end());
    for (QVector<GenericPluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
    */
    return true;
}

void TransportPluginManagerPrivate::loadPlugin(MailTransportPluginInfo *item)
{
    /*
    KPluginLoader pluginLoader(item->metaDataFileName);
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<PluginEditorInit>(q, QVariantList() << item->metaDataFileNameBaseName);
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = item->plugin->hasConfigureDialog();
        mPluginDataList.append(item->pluginData);
    }
    */
}


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

