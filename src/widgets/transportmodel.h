/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"
#include <QAbstractListModel>
namespace MailTransport
{
class MAILTRANSPORT_EXPORT TransportModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum TransportRoles {
        NameRole,
        TransportNameRole,
        TransportIdentifierRole,
        DefaultRole,
        LastColumn = DefaultRole,
    };

    explicit TransportModel(QObject *parent = nullptr);
    ~TransportModel() override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};
}
