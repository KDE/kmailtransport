/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MAILTRANSPORT_TRANSPORTLISTVIEW_H
#define MAILTRANSPORT_TRANSPORTLISTVIEW_H

#include <QTreeWidget>

namespace MailTransport
{
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

protected Q_SLOTS:
    void commitData(QWidget *editor) override;

private Q_SLOTS:
    void fillTransportList();
};
} // namespace MailTransport

#endif // MAILTRANSPORT_TRANSPORTLISTVIEW_H
