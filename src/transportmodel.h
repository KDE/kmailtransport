/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"
#include <QAbstractListModel>
namespace MailTransport
{
class TransportManager;
class Transport;
class MAILTRANSPORT_EXPORT TransportModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum TransportRoles {
        NameRole,
        TransportNameRole,
        TransportIdentifierRole,
        DefaultRole,
        ActivitiesRole,
        EnabledActivitiesRole,
        LastColumn = EnabledActivitiesRole,
    };

    explicit TransportModel(QObject *parent = nullptr);
    ~TransportModel() override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] int transportId(int index) const;
    [[nodiscard]] int indexOf(int transportId) const;

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    [[nodiscard]] bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    void setShowDefault(bool show);

private:
    MAILTRANSPORT_NO_EXPORT void updateComboboxList();
    [[nodiscard]] MAILTRANSPORT_NO_EXPORT QString generateTransportName(Transport *t) const;
    QList<int> mTransportIds;
    TransportManager *const mTransportManager;
    bool mShowDefault = false;
};
}
