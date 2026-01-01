/*
    SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

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

    [[nodiscard]] QList<MailTransport::TransportAbstractPluginInfo> names() const override;
    [[nodiscard]] bool configureTransport(const QString &identifier, MailTransport::Transport *transport, QWidget *parent) override;
    [[nodiscard]] MailTransport::TransportJob *createTransportJob(MailTransport::Transport *t, const QString &identifier) override;
};
