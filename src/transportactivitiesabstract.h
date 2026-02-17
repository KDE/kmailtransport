/*
    SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"
#include <QObject>
namespace MailTransport
{
/*!
 * \class MailTransport::TransportActivitiesAbstract
 * \inmodule KMailTransport
 * \inheaderfile MailTransport/TransportActivitiesAbstract
 * \brief The TransportActivitiesAbstract class
 *
 * \author Laurent Montel <montel@kde.org>
 */
class MAILTRANSPORT_EXPORT TransportActivitiesAbstract : public QObject
{
    Q_OBJECT
public:
    /*!
      Creates a new TransportActivitiesAbstract with the specified parent object.
    */
    explicit TransportActivitiesAbstract(QObject *parent = nullptr);
    /*!
      Destroys the TransportActivitiesAbstract.
    */
    ~TransportActivitiesAbstract() override;

    /*!
      Returns whether the given activities are accepted by the filter.
    */
    [[nodiscard]] virtual bool filterAcceptsRow(const QStringList &activities) const = 0;

    /*!
      Returns whether activity support is available.
    */
    [[nodiscard]] virtual bool hasActivitySupport() const = 0;

    /*!
      Returns the current activity.
    */
    [[nodiscard]] virtual QString currentActivity() const = 0;
Q_SIGNALS:
    /*!
      Emitted when the activities have changed.
    */
    void activitiesChanged();
};
}
