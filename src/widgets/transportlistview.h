/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QTreeWidget>

namespace MailTransport
{
class TransportActivitiesAbstract;
/**
  @internal
  A QTreeWidget for transports.
*/
class TransportListView : public QTreeWidget
{
    Q_OBJECT

public:
    explicit TransportListView(QWidget *parent = nullptr);

    // overloaded from QTreeWidget
    void editItem(QTreeWidgetItem *item, int column = 0);

    /// @since 6.1
    [[nodiscard]] MailTransport::TransportActivitiesAbstract *transportActivitiesAbstract() const;
    /// @since 6.1
    void setTransportActivitiesAbstract(MailTransport::TransportActivitiesAbstract *activitiesAbstract);

protected Q_SLOTS:
    void commitData(QWidget *editor) override;

private Q_SLOTS:
    void fillTransportList();
};
} // namespace MailTransport
