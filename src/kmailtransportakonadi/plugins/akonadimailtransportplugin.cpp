/*
    Copyright (C) 2017-2018 Laurent Montel <montel@kde.org>

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

#include "akonadimailtransportplugin.h"
#include "resourcesendjob_p.h"
#include "mailtransportplugin_akonadi_debug.h"
#include <kpluginfactory.h>
#include <AkonadiCore/AgentManager>
#include <MailTransport/TransportAbstractPlugin>
#include <AkonadiCore/AgentInstanceCreateJob>
#include <MailTransport/Transport>

using namespace Akonadi;
#include <kcoreaddons_version.h>
#if KCOREADDONS_VERSION < QT_VERSION_CHECK(5, 44, 0)
#define K_PLUGIN_CLASS_WITH_JSON(classname, json) K_PLUGIN_FACTORY_WITH_JSON(classname ## Factory, json, registerPlugin<classname >();)
#endif

K_PLUGIN_CLASS_WITH_JSON(AkonadiMailTransportPlugin, "akonadimailtransport.json")

AkonadiMailTransportPlugin::AkonadiMailTransportPlugin(QObject *parent, const QList<QVariant> &)
    : MailTransport::TransportAbstractPlugin(parent)
{
    // Watch for appearing and disappearing types.
    connect(AgentManager::self(), &AgentManager::typeAdded, this, &AkonadiMailTransportPlugin::slotUpdatePluginList);
    connect(AgentManager::self(), &AgentManager::typeRemoved, this, &AkonadiMailTransportPlugin::slotUpdatePluginList);
}

AkonadiMailTransportPlugin::~AkonadiMailTransportPlugin()
{
}

void AkonadiMailTransportPlugin::slotUpdatePluginList(const Akonadi::AgentType &type)
{
    if (type.capabilities().contains(QLatin1String("MailTransport"))) {
        Q_EMIT updatePluginList();
    }
}

void AkonadiMailTransportPlugin::cleanUp(MailTransport::Transport *t)
{
    const AgentInstance instance = AgentManager::self()->instance(t->host());
    if (!instance.isValid()) {
        qCWarning(MAILTRANSPORT_AKONADI_LOG) << "Could not find resource instance for name:" << t->host();
    }
    AgentManager::self()->removeInstance(instance);
}

QVector<MailTransport::TransportAbstractPluginInfo> AkonadiMailTransportPlugin::names() const
{
    QVector<MailTransport::TransportAbstractPluginInfo> lst;

    for (const AgentType &atype : AgentManager::self()->types()) {
        // TODO probably the string "MailTransport" should be #defined somewhere
        // and used like that in the resources (?)
        if (atype.capabilities().contains(QLatin1String("MailTransport"))) {
            MailTransport::TransportAbstractPluginInfo info;
            info.name = atype.name();
            info.description = atype.description();
            info.identifier = atype.identifier();
            info.isAkonadi = true;
            lst << info;
        }
    }
    return lst;
}

bool AkonadiMailTransportPlugin::configureTransport(const QString &identifier, MailTransport::Transport *transport, QWidget *parent)
{
    Q_UNUSED(identifier);
    AgentInstance instance = AgentManager::self()->instance(transport->host());
    if (!instance.isValid()) {
        qCWarning(MAILTRANSPORT_AKONADI_LOG) << "Invalid resource instance" << transport->host();
    }
    instance.configure(parent);   // Async...
    transport->save();
    return true; // No way to know here if the user cancelled or not.
}

MailTransport::TransportJob *AkonadiMailTransportPlugin::createTransportJob(MailTransport::Transport *t, const QString &identifier)
{
    Q_UNUSED(identifier);
    return new MailTransport::ResourceSendJob(t, this);
}

void AkonadiMailTransportPlugin::initializeTransport(MailTransport::Transport *t, const QString &identifier)
{
    AgentInstanceCreateJob *cjob = new AgentInstanceCreateJob(identifier);
    if (!cjob->exec()) {
        qCWarning(MAILTRANSPORT_AKONADI_LOG) << "Failed to create agent instance of type" << identifier;
        return;
    }
    t->setHost(cjob->instance().identifier());
}

#include "akonadimailtransportplugin.moc"
