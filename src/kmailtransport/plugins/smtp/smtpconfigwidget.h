/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    Based on MailTransport code by:
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

    Based on KMail code by:
    SPDX-FileCopyrightText: 2001-2002 Michael Haeckel <haeckel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "widgets/transportconfigwidget.h"
class QAbstractButton;
namespace MailTransport
{
class Transport;

/**
  @internal
*/
class SMTPConfigWidgetPrivate;

/**
  @internal
  Configuration widget for a SMTP transport.
*/
class SMTPConfigWidget : public TransportConfigWidget
{
    Q_OBJECT

public:
    explicit SMTPConfigWidget(Transport *transport, QWidget *parent = nullptr);

public Q_SLOTS:
    /** reimpl */
    void apply() override;

private Q_SLOTS:
    void checkSmtpCapabilities();
    void passwordsLoaded();
    void slotFinished(const QVector<int> &results);
    void hostNameChanged(const QString &text);
    void encryptionChanged(int enc);
    void ensureValidAuthSelection();

private:
    void encryptionAbstractButtonChanged(QAbstractButton *button);
    Q_DECLARE_PRIVATE(SMTPConfigWidget)

    void init();
    void enablePasswordLine();
};
} // namespace MailTransport

