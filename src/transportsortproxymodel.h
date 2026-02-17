/*
    SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "mailtransport_export.h"
#include <QSortFilterProxyModel>
namespace MailTransport
{
class TransportActivitiesAbstract;
/*!
 * \class MailTransport::TransportSortProxyModel
 * \inmodule KMailTransport
 * \inheaderfile MailTransport/TransportSortProxyModel
 * \author Laurent Montel <montel@kde.org>
 *
 * \brief The TransportSortProxyModel class
 */
class MAILTRANSPORT_EXPORT TransportSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    /*!
      Creates a new TransportSortProxyModel with the specified parent object.
    */
    explicit TransportSortProxyModel(QObject *parent = nullptr);
    /*!
      Destroys the TransportSortProxyModel.
    */
    ~TransportSortProxyModel() override;

    /*!
      Returns the TransportActivitiesAbstract object used for filtering.
    */
    [[nodiscard]] TransportActivitiesAbstract *transportActivitiesAbstract() const;
    /*!
      Sets the TransportActivitiesAbstract object used for filtering.
    */
    void setTransportActivitiesAbstract(TransportActivitiesAbstract *newTransportActivitiesAbstract);

    /*!
      Returns whether plasma activities filtering is enabled.
    */
    [[nodiscard]] bool enablePlasmaActivities() const;
    /*!
      Enables or disables plasma activities filtering.
    */
    void setEnablePlasmaActivities(bool newEnablePlasmaActivities);

protected:
    /*!
      Filters rows based on transport activities. Reimplemented from QSortFilterProxyModel.
    */
    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    MAILTRANSPORT_NO_EXPORT void slotInvalidateFilter();
    TransportActivitiesAbstract *mTransportActivitiesAbstract = nullptr;
    bool mEnablePlasmaActivities = false;
};
}
