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

#include "akonadimailtransportplugin.h"
#include "resourcesendjob_p.h"
#include <kpluginfactory.h>
#include <AkonadiCore/AgentManager>
#include <MailTransport/TransportAbstractPlugin>
#include <QDebug>

using namespace Akonadi;
K_PLUGIN_FACTORY_WITH_JSON(AkonadiMailTransportPluginFactory, "akonadimailtransport.json", registerPlugin<AkonadiMailTransportPlugin>();
                           )

AkonadiMailTransportPlugin::AkonadiMailTransportPlugin(QObject *parent, const QList<QVariant> &)
    : MailTransport::TransportAbstractPlugin(parent)
{
}

AkonadiMailTransportPlugin::~AkonadiMailTransportPlugin()
{
}

void AkonadiMailTransportPlugin::cleanUp(const QString &identifier)
{
    Q_UNUSED(identifier);
    //TODO FIXME
}

QVector<MailTransport::TransportAbstractPluginInfo> AkonadiMailTransportPlugin::names() const
{
    QVector<MailTransport::TransportAbstractPluginInfo> lst;

    for (const AgentType &atype : AgentManager::self()->types()) {
        // TODO probably the string "MailTransport" should be #defined somewhere
        // and used like that in the resources (?)
        if (atype.capabilities().contains(QStringLiteral("MailTransport"))) {
            MailTransport::TransportAbstractPluginInfo info;
            info.name = atype.name();
            info.description = atype.description();
            info.identifier = atype.identifier();
            lst << info;
        }
    }
    return lst;
}

bool AkonadiMailTransportPlugin::configureTransport(const QString &identifier, MailTransport::Transport *transport, QWidget *parent)
{

    //TODO
    return false;
}

MailTransport::TransportJob *AkonadiMailTransportPlugin::createTransportJob(MailTransport::Transport *t, const QString &identifier)
{
    return new MailTransport::ResourceSendJob(t, this);
}

#include "akonadimailtransportplugin.moc"
