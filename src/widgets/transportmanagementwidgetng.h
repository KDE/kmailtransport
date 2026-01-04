/*
  SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"

#include <QWidget>

#include <memory>

namespace MailTransport
{
class TransportManagementWidgetNgPrivate;
class TransportActivitiesAbstract;

/**
  A widget to manage mail transports.
*/
class MAILTRANSPORT_EXPORT TransportManagementWidgetNg : public QWidget
{
    Q_OBJECT

public:
    /*!
      Creates a new TransportManagementWidgetNg.
      \param parent The parent widget.
    */
    explicit TransportManagementWidgetNg(QWidget *parent = nullptr);

    /*!
      Destroys the widget.
    */
    ~TransportManagementWidgetNg() override;

    /*!
     */
    [[nodiscard]] bool enablePlasmaActivities() const;
    /*!
     */
    void setEnablePlasmaActivities(bool newEnablePlasmaActivities);

    /*!
     */
    void setTransportActivitiesAbstract(TransportActivitiesAbstract *activitiesAbstract);

private:
    std::unique_ptr<TransportManagementWidgetNgPrivate> const d;
};
} // namespace MailTransport
