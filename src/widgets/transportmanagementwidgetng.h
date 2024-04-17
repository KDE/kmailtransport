/*
  SPDX-FileCopyrightText: Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"

#include <QWidget>

#include <memory>

namespace MailTransport
{
class TransportManagementWidgetNgPrivate;

/**
  A widget to manage mail transports.
*/
class MAILTRANSPORT_EXPORT TransportManagementWidgetNg : public QWidget
{
    Q_OBJECT

public:
    /**
      Creates a new TransportManagementWidgetNg.
      @param parent The parent widget.
    */
    explicit TransportManagementWidgetNg(QWidget *parent = nullptr);

    /**
      Destroys the widget.
    */
    ~TransportManagementWidgetNg() override;

private:
    std::unique_ptr<TransportManagementWidgetNgPrivate> const d;
};
} // namespace MailTransport
