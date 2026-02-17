/*
    SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"
#include <QAbstractListModel>
namespace MailTransport
{
class TransportManager;
class Transport;
/*!
 * \class MailTransport::TransportModel
 * \inmodule KMailTransport
 * \inheaderfile MailTransport/TransportModel
 * \author Laurent Montel <montel@kde.org>
 *
 * \brief The TransportModel class
 */
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

    /*!
      Creates a new TransportModel with the specified parent object.
    */
    explicit TransportModel(QObject *parent = nullptr);
    /*!
      Destroys the TransportModel.
    */
    ~TransportModel() override;

    /*!
      Returns the data for the item at the given index with the specified role.
    */
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /*!
      Returns the number of rows under the given parent.
    */
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /*!
      Returns the number of columns under the given parent.
    */
    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
    /*!
      Returns the header data for the given section.
    */
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    /*!
      Returns the transport ID at the given index.
    */
    [[nodiscard]] int transportId(int index) const;
    /*!
      Returns the index of the transport with the given ID.
    */
    [[nodiscard]] int indexOf(int transportId) const;

    /*!
      Returns the item flags for the given index.
    */
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

    /*!
      Sets the data for the item at the given index with the specified value and role.
    */
    [[nodiscard]] bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    /*!
      Shows or hides the default transport.
    */
    void setShowDefault(bool show);

private:
    MAILTRANSPORT_NO_EXPORT void updateComboboxList();
    [[nodiscard]] MAILTRANSPORT_NO_EXPORT QString generateTransportName(Transport *t) const;
    QList<int> mTransportIds;
    TransportManager *const mTransportManager;
    bool mShowDefault = false;
};
}
