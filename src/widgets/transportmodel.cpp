/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportmodel.h"
#include "transportmanager.h"

using namespace MailTransport;
TransportModel::TransportModel(QObject *parent)
    : QAbstractListModel{parent}
    , mTransportManager(TransportManager::self())
{
}

TransportModel::~TransportModel() = default;

QVariant TransportModel::data(const QModelIndex &index, int role) const
{
    return {};
}

int TransportModel::rowCount(const QModelIndex &parent) const
{
    return {};
}
int TransportModel::columnCount(const QModelIndex &parent) const
{
    return {};
}

QVariant TransportModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return {};
}

#include "moc_transportmodel.cpp"
