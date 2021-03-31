/*
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

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

#define SMTP_PORT 587
#define SMTP_OLD_PORT 25
#define SMTPS_PORT 465

// Because ServerTest is also capable of testing IMAP,
// some additional defines:

#define IMAP_PROTOCOL QStringLiteral("imap")
#define IMAPS_PROTOCOL QStringLiteral("imaps")

#define POP_PROTOCOL QStringLiteral("pop")
#define POPS_PROTOCOL QStringLiteral("pops")

#define NNTP_PROTOCOL QStringLiteral("nntp")
#define NNTPS_PROTOCOL QStringLiteral("nntps")

#define IMAP_PORT 143
#define IMAPS_PORT 993

#define POP_PORT 110
#define POPS_PORT 995

#define NNTP_PORT 119
#define NNTPS_PORT 563

