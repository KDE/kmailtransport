/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "transportactivitiesabstract.h"
using namespace MailTransport;
TransportActivitiesAbstract::TransportActivitiesAbstract(QObject *parent)
    : QObject{parent}
{
}

TransportActivitiesAbstract::~TransportActivitiesAbstract() = default;
