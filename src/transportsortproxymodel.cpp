/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportsortproxymodel.h"
#include "transportactivitiesabstract.h"

using namespace MailTransport;
TransportSortProxyModel::TransportSortProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
}

TransportSortProxyModel::~TransportSortProxyModel() = default;

bool TransportSortProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (mTransportActivitiesAbstract) {
        return mTransportActivitiesAbstract->filterAcceptsRow(source_row, source_parent);
    }
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

TransportActivitiesAbstract *TransportSortProxyModel::transportActivitiesAbstract() const
{
    return mTransportActivitiesAbstract;
}

void TransportSortProxyModel::setTransportActivitiesAbstract(TransportActivitiesAbstract *newTransportActivitiesAbstract)
{
    if (mTransportActivitiesAbstract != newTransportActivitiesAbstract) {
        mTransportActivitiesAbstract = newTransportActivitiesAbstract;
        connect(mTransportActivitiesAbstract, &TransportActivitiesAbstract::activitiesChanged, this, &TransportSortProxyModel::invalidateFilter);
        invalidateFilter();
    }
}

#include "moc_transportsortproxymodel.cpp"
