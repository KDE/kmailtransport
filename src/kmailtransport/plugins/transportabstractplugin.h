/*
    SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"
#include <QObject>
#include <QVector>
namespace MailTransport
{
class TransportJob;
class Transport;
/**
 * @brief The TransportAbstractPluginInfo struct
 * @author Laurent Montel <montel@kde.org>
 */
struct MAILTRANSPORT_EXPORT TransportAbstractPluginInfo {
    QString name;
    QString identifier;
    QString description;
    bool isAkonadi = false;
};

/**
 * @brief The TransportAbstractPlugin class
 * @author Laurent Montel <montel@kde.org>
 */
class MAILTRANSPORT_EXPORT TransportAbstractPlugin : public QObject
{
    Q_OBJECT
public:
    explicit TransportAbstractPlugin(QObject *parent = nullptr);
    ~TransportAbstractPlugin() override;

    Q_REQUIRED_RESULT virtual TransportJob *createTransportJob(MailTransport::Transport *t, const QString &identifier) = 0;
    Q_REQUIRED_RESULT virtual QVector<TransportAbstractPluginInfo> names() const = 0;
    Q_REQUIRED_RESULT virtual bool configureTransport(const QString &identifier, Transport *transport, QWidget *parent) = 0;
    virtual void cleanUp(MailTransport::Transport *t);
    virtual void initializeTransport(MailTransport::Transport *t, const QString &identifier);

Q_SIGNALS:
    void updatePluginList();
};
}
Q_DECLARE_TYPEINFO(MailTransport::TransportAbstractPluginInfo, Q_MOVABLE_TYPE);

