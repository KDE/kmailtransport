/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "kmailtransport_private_export.h"
#include <QTreeView>

namespace MailTransport
{
class TransportSortProxyModel;
class TransportActivitiesAbstract;
class KMAILTRANSPORT_TESTS_EXPORT TransportListView : public QTreeView
{
    Q_OBJECT
public:
    explicit TransportListView(QWidget *parent = nullptr);
    ~TransportListView() override;

    [[nodiscard]] TransportActivitiesAbstract *transportActivitiesAbstract() const;
    void setTransportActivitiesAbstract(TransportActivitiesAbstract *activitiesAbstract);

private:
    TransportSortProxyModel *const mTransportSortProxyModel;
};
}
