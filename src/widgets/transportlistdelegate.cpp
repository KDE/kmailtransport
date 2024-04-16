/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "transportlistdelegate.h"
using namespace MailTransport;
TransportListDelegate::TransportListDelegate(QObject *parent)
    : QItemDelegate{parent}
{
}

TransportListDelegate::~TransportListDelegate() = default;
