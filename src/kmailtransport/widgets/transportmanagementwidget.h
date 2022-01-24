/*
  SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

  Based on KMail code by:
  SPDX-FileCopyrightText: 2001-2003 Marc Mutz <mutz@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"

#include <QWidget>

#include <memory>

namespace MailTransport
{
class TransportManagementWidgetPrivate;

/**
  A widget to manage mail transports.
*/
class MAILTRANSPORT_EXPORT TransportManagementWidget : public QWidget
{
    Q_OBJECT

public:
    /**
      Creates a new TransportManagementWidget.
      @param parent The parent widget.
    */
    explicit TransportManagementWidget(QWidget *parent = nullptr);

    /**
      Destroys the widget.
    */
    ~TransportManagementWidget() override;

private:
    std::unique_ptr<TransportManagementWidgetPrivate> const d;
};
} // namespace MailTransport

