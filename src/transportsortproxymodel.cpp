/*
    SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportsortproxymodel.h"
#include "transportactivitiesabstract.h"
#include "transportmodel.h"

using namespace MailTransport;
TransportSortProxyModel::TransportSortProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
}

TransportSortProxyModel::~TransportSortProxyModel() = default;

bool TransportSortProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (mTransportActivitiesAbstract && mEnablePlasmaActivities) {
        const bool enableActivities = sourceModel()->index(source_row, TransportModel::EnabledActivitiesRole).data().toBool();
        if (enableActivities) {
            const auto activities = sourceModel()->index(source_row, TransportModel::ActivitiesRole).data().toStringList();
            const bool result = mTransportActivitiesAbstract->filterAcceptsRow(activities);
            // qDebug() << " result " << result << " identity name : " << sourceModel()->index(source_row,
            // IdentityTreeModel::IdentityNameRole).data().toString();
            return result;
        }
    }
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool TransportSortProxyModel::enablePlasmaActivities() const
{
    return mEnablePlasmaActivities;
}

void TransportSortProxyModel::setEnablePlasmaActivities(bool newEnablePlasmaActivities)
{
    if (mEnablePlasmaActivities != newEnablePlasmaActivities) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
        beginFilterChange();
#endif
        mEnablePlasmaActivities = newEnablePlasmaActivities;
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
        endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
        invalidateFilter();
#endif
    }
}

TransportActivitiesAbstract *TransportSortProxyModel::transportActivitiesAbstract() const
{
    return mTransportActivitiesAbstract;
}

void TransportSortProxyModel::setTransportActivitiesAbstract(TransportActivitiesAbstract *newTransportActivitiesAbstract)
{
    if (mTransportActivitiesAbstract != newTransportActivitiesAbstract) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
        beginFilterChange();
#endif
        mTransportActivitiesAbstract = newTransportActivitiesAbstract;
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
        endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
        invalidateFilter();
#endif
        connect(mTransportActivitiesAbstract, &TransportActivitiesAbstract::activitiesChanged, this, &TransportSortProxyModel::slotInvalidateFilter);
    }
}

void TransportSortProxyModel::slotInvalidateFilter()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    beginFilterChange();
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
    invalidateFilter();
#endif
}

#include "moc_transportsortproxymodel.cpp"
