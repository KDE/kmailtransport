/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportmodel.h"

using namespace MailTransport;
TransportModel::TransportModel(QObject *parent)
    : QAbstractListModel{parent}
{
}

TransportModel::~TransportModel() = default;
