/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportmodel.h"
#include "mailtransport_debug.h"
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
    if (role == Qt::FontRole) {
        if (static_cast<TransportRoles>(index.column()) == NameRole) {
            if (TransportManager::self()->defaultTransportId() == transport->id()) {
                QFont f;
                f.setBold(true);
                return f;
            }
        }
    }
    if (role != Qt::DisplayRole) {
        return {};
    }
    switch (static_cast<TransportRoles>(index.column())) {
    case NameRole:
        return generateTransportName(transport);
    case TransportNameRole:
        return transport->transportType().name();
    case TransportIdentifierRole:
        return transport->id();
    case DefaultRole:
        return TransportManager::self()->defaultTransportId();
    }

    return {};
}

QString TransportModel::generateTransportName(Transport *t) const
{
    QString str = t->name();
    if (mShowDefault && TransportManager::self()->defaultTransportId() == t->id()) {
        str += QLatin1Char(' ') + i18nc("Default transport", " (default)");
    }
    return str;
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

int TransportModel::transportId(int index) const
{
    return mTransportIds.at(index);
}

int TransportModel::indexOf(int transportId) const
{
    return mTransportIds.indexOf(transportId);
}

void TransportModel::updateComboboxList()
{
    beginResetModel();
    mTransportIds = mTransportManager->transportIds();
    endResetModel();
}

Qt::ItemFlags TransportModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (static_cast<TransportRoles>(index.column()) == NameRole) {
        return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
    }
    return QAbstractItemModel::flags(index);
}

bool TransportModel::setData(const QModelIndex &modelIndex, const QVariant &value, int role)
{
    if (!modelIndex.isValid()) {
        qCWarning(MAILTRANSPORT_LOG) << "ERROR: invalid index";
        return false;
    }
    if (role != Qt::EditRole) {
        return false;
    }
    const int idx = modelIndex.row();
    const auto transport = mTransportManager->transportById(mTransportIds[idx]);
    switch (static_cast<TransportRoles>(modelIndex.column())) {
    case NameRole: {
        const QModelIndex newIndex = index(modelIndex.row(), NameRole);
        Q_EMIT dataChanged(newIndex, newIndex);
        transport->setName(value.toString());
        transport->forceUniqueName();
        transport->save();
        return true;
    }
    default:
        break;
    }
    return false;
}

void TransportModel::setShowDefault(bool show)
{
    mShowDefault = show;
}

#include "moc_transportmodel.cpp"
