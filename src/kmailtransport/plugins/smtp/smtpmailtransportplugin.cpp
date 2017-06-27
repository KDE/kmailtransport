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

#include "smtpmailtransportplugin.h"
#include <kpluginfactory.h>
#include <KLocalizedString>

K_PLUGIN_FACTORY_WITH_JSON(SMTPMailTransportPluginFactory, "smtpmailtransport.json", registerPlugin<SMTPMailTransportPlugin>();
                           )

SMTPMailTransportPlugin::SMTPMailTransportPlugin(QObject *parent, const QList<QVariant> &)
    : MailTransport::TransportAbstractPlugin(parent)
{

}

SMTPMailTransportPlugin::~SMTPMailTransportPlugin()
{

}

QVector<MailTransport::TransportAbstractPluginInfo> SMTPMailTransportPlugin::names() const
{
    MailTransport::TransportAbstractPluginInfo info;
    info.name = i18n("SMTP");
    info.identifier = QStringLiteral("smtp");
    return QVector<MailTransport::TransportAbstractPluginInfo>() << info;
}

bool SMTPMailTransportPlugin::configureTransport(const QString &identifier, MailTransport::Transport *transport, QWidget *parent)
{
    //TODO FIXME
    return false;
}

void SMTPMailTransportPlugin::cleanUp(const QString &identifier)
{
    Q_UNUSED(identifier);
    //TODO FIXME
}

#include "smtpmailtransportplugin.moc"


