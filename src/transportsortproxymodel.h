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
 * \author Laurent Montel <montel@kde.org>
 *
 * \brief The TransportSortProxyModel class
 */
class MAILTRANSPORT_EXPORT TransportSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    /*!
     */
    explicit TransportSortProxyModel(QObject *parent = nullptr);
    /*!
     */
    ~TransportSortProxyModel() override;

    /*!
     */
    [[nodiscard]] TransportActivitiesAbstract *transportActivitiesAbstract() const;
    /*!
     */
    void setTransportActivitiesAbstract(TransportActivitiesAbstract *newTransportActivitiesAbstract);

    /*!
     */
    [[nodiscard]] bool enablePlasmaActivities() const;
    /*!
     */
    void setEnablePlasmaActivities(bool newEnablePlasmaActivities);

protected:
    /*!
     */
    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    MAILTRANSPORT_NO_EXPORT void slotInvalidateFilter();
    TransportActivitiesAbstract *mTransportActivitiesAbstract = nullptr;
    bool mEnablePlasmaActivities = false;
};
}
