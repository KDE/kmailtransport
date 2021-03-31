/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    Based on MailTransport code by:
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

    Based on KMail code by:
    SPDX-FileCopyrightText: 2001-2002 Michael Haeckel <haeckel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>
#include <mailtransport_export.h>

class KConfigDialogManager;

namespace MailTransport
{
class Transport;
class TransportConfigWidgetPrivate;

/**
  @internal

  Abstract configuration widget for a mail transport.  It makes sure that
  the configured transport has a unique name, and takes care of writing its
  settings to the config file.  If it is a new transport, the caller must
  still call TransportManager::addTransport() to register the transport.

  Concrete configuration is done in subclasses SMTPConfigWidget.

  To configure a transport from applications, use
  TransportManager::configureTransport().  You still need to call
  TransportManager::addTransport() if this is a new transport, not registered
  with TransportManager.

  @author Constantin Berzan <exit3219@gmail.com>
  @since 4.4
*/
class MAILTRANSPORT_EXPORT TransportConfigWidget : public QWidget
{
    Q_OBJECT

public:
    /**
      Creates a new mail transport configuration widget for the given
      Transport object.
      @param transport The Transport object to configure.
      @param parent The parent widget.
    */
    explicit TransportConfigWidget(Transport *transport, QWidget *parent = nullptr);

    /**
      Destroys the widget.
    */
    ~TransportConfigWidget() override;

    /**
      @internal
      Get the KConfigDialogManager for this widget.
    */
    KConfigDialogManager *configManager() const;

public Q_SLOTS:
    /**
      Saves the transport's settings.

      The base implementation writes the settings to the config file and makes
      sure the transport has a unique name.  Reimplement in derived classes to
      save your custom settings, and call the base implementation.
    */
    // TODO: do we also want to check for invalid settings?
    virtual void apply();

protected:
    TransportConfigWidgetPrivate *const d_ptr;
    TransportConfigWidget(TransportConfigWidgetPrivate &dd, Transport *transport, QWidget *parent);

private:
    Q_DECLARE_PRIVATE(TransportConfigWidget)

    void init(Transport *transport);
};
} // namespace MailTransport

