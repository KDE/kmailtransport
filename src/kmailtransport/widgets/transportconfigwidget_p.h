/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

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

