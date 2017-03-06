/*
    Copyright (c) 2006 - 2007 Volker Krause <vkrause@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#ifndef MAILTRANSPORT_MAILTRANSPORT_DEFS_H
#define MAILTRANSPORT_MAILTRANSPORT_DEFS_H

/**
  @file mailtransport_defs.h
  Internal file containing constant definitions etc.
*/

#define WALLET_FOLDER QStringLiteral("mailtransports")
#define KMAIL_WALLET_FOLDER QStringLiteral("kmail")

#define DBUS_SERVICE_NAME QStringLiteral("org.kde.pim.TransportManager")
#define DBUS_INTERFACE_NAME QStringLiteral("org.kde.pim.TransportManager")
#define DBUS_OBJECT_PATH QStringLiteral("/TransportManager")
#define DBUS_CHANGE_SIGNAL QStringLiteral("changesCommitted")

#define SMTP_PROTOCOL QStringLiteral("smtp")
#define SMTPS_PROTOCOL QStringLiteral("smtps")

#define SMTP_PORT 25
#define SMTPS_PORT 465

// Because ServerTest is also capable of testing IMAP,
// some additional defines:

#define IMAP_PROTOCOL QStringLiteral("imap")
#define IMAPS_PROTOCOL QStringLiteral("imaps")

#define POP_PROTOCOL QStringLiteral("pop")
#define POPS_PROTOCOL QStringLiteral("pops")

#define IMAP_PORT 143
#define IMAPS_PORT 993

#define POP_PORT 110
#define POPS_PORT 995

#endif
