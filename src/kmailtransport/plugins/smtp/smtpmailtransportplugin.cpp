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

#include "smtpmailtransportplugin.h"
#include "smtpconfigdialog.h"
#include "smtpjob.h"
#include <kpluginfactory.h>
#include <KLocalizedString>

K_PLUGIN_CLASS_WITH_JSON(SMTPMailTransportPlugin, "smtpmailtransport.json")

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

    info.name = i18nc("@option SMTP transport", "SMTP");
    info.description = i18n("An SMTP server on the Internet");
    info.identifier = QStringLiteral("SMTP");
    info.isAkonadi = false;
    return QVector<MailTransport::TransportAbstractPluginInfo>() << info;
}

bool SMTPMailTransportPlugin::configureTransport(const QString &identifier, MailTransport::Transport *transport, QWidget *parent)
{
    Q_UNUSED(identifier);
    QPointer<MailTransport::SmtpConfigDialog> transportConfigDialog
        = new MailTransport::SmtpConfigDialog(transport, parent);
    transportConfigDialog->setWindowTitle(i18n("Configure account"));
    bool okClicked = (transportConfigDialog->exec() == QDialog::Accepted);
    delete transportConfigDialog;
    return okClicked;
}

MailTransport::TransportJob *SMTPMailTransportPlugin::createTransportJob(MailTransport::Transport *t, const QString &identifier)
{
    Q_UNUSED(identifier);
    return new MailTransport::SmtpJob(t, this);
}

#include "smtpmailtransportplugin.moc"
