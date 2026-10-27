/*
    SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"
#include <QList>
#include <QObject>
namespace MailTransport
{
class TransportJob;
class Transport;
/*!
 * \struct MailTransport::TransportAbstractPluginInfo
 * \inmodule KMailTransport
 * \inheaderfile MailTransport/TransportAbstractPlugin
 * \brief The TransportAbstractPluginInfo struct.
 *
 * \author Laurent Montel <montel@kde.org>
 */
struct MAILTRANSPORT_EXPORT TransportAbstractPluginInfo {
    QString name;
    QString identifier;
    QString description;
    bool isAkonadi = false;
};

/*!
 * \class MailTransport::TransportAbstractPlugin
 * \inmodule KMailTransport
 * \inheaderfile MailTransport/TransportAbstractPlugin
 * \brief The TransportAbstractPlugin class.
 *
 * \author Laurent Montel <montel@kde.org>
 */
class MAILTRANSPORT_EXPORT TransportAbstractPlugin : public QObject
{
    Q_OBJECT
public:
    /*!
      Creates a new TransportAbstractPlugin with the specified \a parent object.
    */
    explicit TransportAbstractPlugin(QObject *parent = nullptr);
    /*!
      Destroys the plugin.
    */
    ~TransportAbstractPlugin() override;

    /*!
      Creates a transport job for the given transport \a t and \a identifier.
    */
    [[nodiscard]] virtual TransportJob *createTransportJob(MailTransport::Transport *t, const QString &identifier) = 0;
    /*!
      Returns the list of transport names provided by this plugin.
    */
    [[nodiscard]] virtual QList<TransportAbstractPluginInfo> names() const = 0;
    /*!
      Configures the \a transport with the given \a identifier, using \a parent as parent widget.
      Returns \c true if the transport was configured.
    */
    [[nodiscard]] virtual bool configureTransport(const QString &identifier, Transport *transport, QWidget *parent) = 0;
    /*!
      Cleans up the resources of the transport \a t.
    */
    virtual void cleanUp(MailTransport::Transport *t);
    /*!
      Initializes the transport \a t with the given \a identifier.
    */
    virtual void initializeTransport(MailTransport::Transport *t, const QString &identifier);

Q_SIGNALS:
    /*!
      Emitted when the plugin list should be updated.
    */
    void updatePluginList();
};
}
Q_DECLARE_TYPEINFO(MailTransport::TransportAbstractPluginInfo, Q_RELOCATABLE_TYPE);
