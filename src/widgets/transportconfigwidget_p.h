/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

class KConfigDialogManager;

namespace MailTransport
{
class Transport;

/**
  @internal
*/
class TransportConfigWidgetPrivate
{
public:
    Transport *transport = nullptr;
    KConfigDialogManager *manager = nullptr;

    virtual ~TransportConfigWidgetPrivate() = default;
};
} // namespace MailTransport
