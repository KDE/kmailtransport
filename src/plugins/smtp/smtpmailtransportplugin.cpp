/*
    SPDX-FileCopyrightText: 2017-2025 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "smtpmailtransportplugin.h"
using namespace Qt::Literals::StringLiterals;

#include "smtpconfigdialog.h"
#include "smtpjob.h"
#include <KLocalizedString>
#include <KPluginFactory>
#include <QPointer>

K_PLUGIN_CLASS_WITH_JSON(SMTPMailTransportPlugin, "smtpmailtransport.json")

SMTPMailTransportPlugin::SMTPMailTransportPlugin(QObject *parent, const QList<QVariant> &)
    : MailTransport::TransportAbstractPlugin(parent)
{
}

SMTPMailTransportPlugin::~SMTPMailTransportPlugin() = default;

QList<MailTransport::TransportAbstractPluginInfo> SMTPMailTransportPlugin::names() const
{
    MailTransport::TransportAbstractPluginInfo info;

    info.name = i18nc("@option SMTP transport", "SMTP");
    info.description = i18n("An SMTP server on the Internet");
    info.identifier = u"SMTP"_s;
    info.isAkonadi = false;
    return QList<MailTransport::TransportAbstractPluginInfo>() << info;
}

bool SMTPMailTransportPlugin::configureTransport(const QString &identifier, MailTransport::Transport *transport, QWidget *parent)
{
    Q_UNUSED(identifier)
    QPointer<MailTransport::SmtpConfigDialog> transportConfigDialog = new MailTransport::SmtpConfigDialog(transport, parent);
    transportConfigDialog->setWindowTitle(i18nc("@title:window", "Configure account"));
    bool okClicked = (transportConfigDialog->exec() == QDialog::Accepted);
    delete transportConfigDialog;
    return okClicked;
}

MailTransport::TransportJob *SMTPMailTransportPlugin::createTransportJob(MailTransport::Transport *t, const QString &identifier)
{
    Q_UNUSED(identifier)
    return new MailTransport::SmtpJob(t, this);
}

#include "smtpmailtransportplugin.moc"

#include "moc_smtpmailtransportplugin.cpp"
