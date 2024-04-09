/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportmodel.h"
#include "transportmanager.h"
#include <KLocalizedString>

using namespace MailTransport;
TransportModel::TransportModel(QObject *parent)
    : QAbstractListModel{parent}
    , mTransportManager(TransportManager::self())
{
    updateComboboxList();
    connect(mTransportManager, &TransportManager::transportsChanged, this, &TransportModel::updateComboboxList);
}

TransportModel::~TransportModel() = default;

QVariant TransportModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    const auto transport = mTransportManager->transportById(mTransportIds[index.row()]);

#if 0
    const auto &identity = mIdentityManager->modifyIdentityForUoid(mTransportIds[index.row()]);
    if (role == Qt::ToolTipRole) {
        return identity.primaryEmailAddress();
    }
    if (role == Qt::FontRole) {
        if (static_cast<IdentityRoles>(index.column()) == IdentityNameRole) {
            if (identity.isDefault()) {
                QFont f;
                f.setBold(true);
                return f;
            }
        }
    }
    if (role != Qt::DisplayRole) {
        return {};
    }
    switch (static_cast<IdentityRoles>(index.column())) {
    case FullEmailRole:
        return identity.fullEmailAddr();
    case EmailRole:
        return identity.primaryEmailAddress();
    case UoidRole:
        return identity.uoid();
    case IdentityNameRole:
        return generateIdentityName(identity);
    case DefaultRole:
        return identity.isDefault();
    }
#endif

    return {};
}

int TransportModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mTransportIds.count();
}
int TransportModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    constexpr int nbCol = static_cast<int>(TransportRoles::LastColumn) + 1;
    return nbCol;
}

QVariant TransportModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (static_cast<TransportRoles>(section)) {
        case NameRole:
            return i18nc("@title:column email transport name", "Name");
        case TransportNameRole:
            return i18nc("@title:column email transport type", "Type");
        case TransportIdentifierRole:
        case DefaultRole:
            return {};
        }
    }
    return {};
}

void TransportModel::updateComboboxList()
{
    beginResetModel();
    mTransportIds = mTransportManager->transportIds();
    endResetModel();
}

#include "moc_transportmodel.cpp"
