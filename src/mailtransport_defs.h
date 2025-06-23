/*
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
using namespace Qt::Literals::StringLiterals;
/**
  @file mailtransport_defs.h
  Internal file containing constant definitions etc.
*/

#define WALLET_FOLDER u"mailtransports"_s
#define KMAIL_WALLET_FOLDER u"kmail"_s

#define DBUS_SERVICE_NAME u"org.kde.pim.TransportManager"_s
#define DBUS_INTERFACE_NAME u"org.kde.pim.TransportManager"_s
#define DBUS_OBJECT_PATH u"/TransportManager"_s
#define DBUS_CHANGE_SIGNAL u"changesCommitted"_s

#define SMTP_PROTOCOL u"smtp"_s
#define SMTPS_PROTOCOL u"smtps"_s

#define SMTP_PORT 587
#define SMTP_OLD_PORT 25
#define SMTPS_PORT 465

// Because ServerTest is also capable of testing IMAP,
// some additional defines:

#define IMAP_PROTOCOL u"imap"_s
#define IMAPS_PROTOCOL u"imaps"_s

#define POP_PROTOCOL u"pop"_s
#define POPS_PROTOCOL u"pops"_s

#define NNTP_PROTOCOL u"nntp"_s
#define NNTPS_PROTOCOL u"nntps"_s

#define IMAP_PORT 143
#define IMAPS_PORT 993

#define POP_PORT 110
#define POPS_PORT 995

#define NNTP_PORT 119
#define NNTPS_PORT 563
