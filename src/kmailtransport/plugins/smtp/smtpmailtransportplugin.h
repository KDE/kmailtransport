/*
    SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <MailTransport/TransportAbstractPlugin>
#include <QVariant>

class SMTPMailTransportPlugin : public MailTransport::TransportAbstractPlugin
{
    Q_OBJECT
public:
    explicit SMTPMailTransportPlugin(QObject *parent = nullptr, const QList<QVariant> & = {});
    ~SMTPMailTransportPlugin() override;

    Q_REQUIRED_RESULT QVector<MailTransport::TransportAbstractPluginInfo> names() const override;
    Q_REQUIRED_RESULT bool configureTransport(const QString &identifier, MailTransport::Transport *transport, QWidget *parent) override;
    Q_REQUIRED_RESULT MailTransport::TransportJob *createTransportJob(MailTransport::Transport *t, const QString &identifier) override;
};

