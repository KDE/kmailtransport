/*
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "transporttype.h"

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
class TransportPrivate
{
public:
    MailTransport::TransportType transportType;
    QString password;
    QString oldName;
    bool passwordLoaded;
    bool passwordDirty;
    bool storePasswordInFile;
    bool needsWalletMigration;
};

