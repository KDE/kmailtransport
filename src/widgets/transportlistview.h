/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QTreeView>
namespace MailTransport
{
class TransportSortProxyModel;
class TransportListView : public QTreeView
{
    Q_OBJECT
public:
    explicit TransportListView(QWidget *parent = nullptr);
    ~TransportListView() override;

private:
    TransportSortProxyModel *const mTransportSortProxyModel;
};
}
