// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <MailTransport/Transport>
#include <MailTransport/TransportManager>
#include <MailTransport/TransportModel>
#include <MailTransport/TransportType>
#include <QQmlEngine>
#include <transportbase.h>

#include <QObject>
#include <qqmlintegration.h>

class TransportModelForeign : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(TransportModel)
    QML_FOREIGN(MailTransport::TransportModel)
    QML_UNCREATABLE("Created by KMailTransport")
};

class TransportForeign : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Transport)
    QML_FOREIGN(MailTransport::Transport)
    QML_UNCREATABLE("Created by MailTransport")
};

/*!
 * \qmltype TransportBase
 * \inqmlmodule org.kde.mailtransport
 * \nativetype MailTransport::TransportBase
 * \brief Base settings shared by configured mail transports.
 */
class TransportBaseForeign : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(TransportBase)
    QML_FOREIGN(MailTransport::TransportBase)
    QML_UNCREATABLE("Created by MailTransport")
};

class TransportManagerForeign : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(TransportManager)
    QML_FOREIGN(MailTransport::TransportManager)
    QML_SINGLETON

public:
    static MailTransport::TransportManager *create(QQmlEngine *engine, QJSEngine *scriptEngine)
    {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);
        return MailTransport::TransportManager::self();
    }
};

struct TransportTypeForeign {
    Q_GADGET
    QML_FOREIGN(MailTransport::TransportType)
    QML_VALUE_TYPE(transportType)
};
