/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QTreeWidget>

namespace MailTransport
{
/**
  @internal
  A QTreeWidget for transports.
*/
class TransportListWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit TransportListWidget(QWidget *parent = nullptr);

    // overloaded from QTreeWidget
    void editItem(QTreeWidgetItem *item, int column = 0);

protected Q_SLOTS:
    void commitData(QWidget *editor) override;

private:
    void fillTransportList();
};
} // namespace MailTransport
