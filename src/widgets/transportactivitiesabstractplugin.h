/*
  SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "mailtransport_export.h"
#include <QWidget>
namespace MailTransport
{
/*!
 * \brief The TransportActivitiesAbstractPlugin class
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
     */
    explicit TransportActivitiesAbstractPlugin(QWidget *parent = nullptr);
    /*!
     */
    ~TransportActivitiesAbstractPlugin() override;

    /*!
     */
    [[nodiscard]] virtual TransportActivitiesAbstractPlugin::ActivitySettings activitiesSettings() const = 0;
    /*!
     */
    virtual void setActivitiesSettings(const TransportActivitiesAbstractPlugin::ActivitySettings &activitySettings) = 0;
};
}
