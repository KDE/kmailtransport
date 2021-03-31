/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "kmailtransport_private_export.h"
#include <QDialog>

namespace MailTransport
{
/**
  @internal

  A dialog for creating a new transport.  It asks the user for the transport
  type and name, and then proceeds to configure the new transport.

  To create a new transport from applications, use
  TransportManager::showNewTransportDialog().

  @author Constantin Berzan <exit3219@gmail.com>
  @since 4.4
*/
class KMAILTRANSPORT_TESTS_EXPORT AddTransportDialogNG : public QDialog
{
    Q_OBJECT

public:
    /**
      Creates a new AddTransportDialogNG.
    */
    explicit AddTransportDialogNG(QWidget *parent = nullptr);

    /**
      Destroys the AddTransportDialogNG.
    */
    ~AddTransportDialogNG() override;

    /* reimpl */
    void accept() override;

private:
    class Private;
    Private *const d;
};
} // namespace MailTransport

