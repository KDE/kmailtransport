/*
  SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "mailtransport_export.h"
#include <QStringList>
#include <QWidget>
namespace MailTransport
{
/*!
 * \class MailTransport::TransportActivitiesAbstractPlugin
 * \inmodule KMailTransport
 * \inheaderfile MailTransport/TransportActivitiesAbstractPlugin
 * \brief The TransportActivitiesAbstractPlugin class.
 *
 * \author Laurent Montel <montel@kde.org>
 */
class MAILTRANSPORT_EXPORT TransportActivitiesAbstractPlugin : public QWidget
{
    Q_OBJECT
public:
    struct ActivitySettings {
        QStringList activities;
        bool enabled = false;
    };

    /*!
      Creates a new TransportActivitiesAbstractPlugin with the specified \a parent widget.
    */
    explicit TransportActivitiesAbstractPlugin(QWidget *parent = nullptr);
    /*!
      Destroys the plugin.
    */
    ~TransportActivitiesAbstractPlugin() override;

    /*!
      Returns the current activity settings.
    */
    [[nodiscard]] virtual TransportActivitiesAbstractPlugin::ActivitySettings activitiesSettings() const = 0;
    /*!
      Sets the activity settings to \a activitySettings.
    */
    virtual void setActivitiesSettings(const TransportActivitiesAbstractPlugin::ActivitySettings &activitySettings) = 0;
};
}
