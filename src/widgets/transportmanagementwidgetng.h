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

/*!
  \class MailTransport::TransportManagementWidgetNg
  \inmodule KMailTransport
  \inheaderfile MailTransport/TransportManagementWidgetNg
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
      Returns whether plasma activities filtering is enabled.
    */
    [[nodiscard]] bool enablePlasmaActivities() const;
    /*!
      Enables or disables plasma activities filtering.
    */
    void setEnablePlasmaActivities(bool newEnablePlasmaActivities);

    /*!
      Sets the TransportActivitiesAbstract object used for activities filtering.
    */
    void setTransportActivitiesAbstract(TransportActivitiesAbstract *activitiesAbstract);

private:
    std::unique_ptr<TransportManagementWidgetNgPrivate> const d;
};
} // namespace MailTransport
