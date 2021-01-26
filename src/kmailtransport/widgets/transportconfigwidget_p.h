/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MAILTRANSPORT_TRANSPORTCONFIGWIDGET_P_H
#define MAILTRANSPORT_TRANSPORTCONFIGWIDGET_P_H

#include "transport.h"

#include <KConfigDialogManager>

namespace MailTransport
{
/**
  @internal
*/
class TransportConfigWidgetPrivate
{
public:
    Transport *transport = nullptr;
    KConfigDialogManager *manager = nullptr;

    virtual ~TransportConfigWidgetPrivate()
    {
    }
};
} // namespace MailTransport

#endif // MAILTRANSPORT_TRANSPORTCONFIGWIDGET_P_H
