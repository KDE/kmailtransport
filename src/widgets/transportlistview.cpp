/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  Based on KMail code by:
  SPDX-FileCopyrightText: 2002 Marc Mutz <mutz@kde.org>
  SPDX-FileCopyrightText: 2007 Mathias Soeken <msoeken@tzi.de>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportlistview.h"
#include "transport.h"
#include "transportmanager.h"
#include "transporttype.h"

#include <QHeaderView>
#include <QLineEdit>

#include "mailtransport_debug.h"
#include <KLocalizedString>

using namespace MailTransport;

TransportListView::TransportListView(QWidget *parent)
    : QTreeWidget(parent)
{
    setHeaderLabels(QStringList() << i18nc("@title:column email transport name", "Name") << i18nc("@title:column email transport type", "Type"));
    setRootIsDecorated(false);
    header()->setSectionsMovable(false);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    setAllColumnsShowFocus(true);
    setAlternatingRowColors(true);
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    setSelectionMode(SingleSelection);

    fillTransportList();
    connect(TransportManager::self(), &TransportManager::transportsChanged, this, &TransportListView::fillTransportList);
}

void TransportListView::editItem(QTreeWidgetItem *item, int column)
{
    // TODO: is there a nicer way to make only the 'name' column editable?
    if (column == 0 && item) {
        Qt::ItemFlags oldFlags = item->flags();
        item->setFlags(oldFlags | Qt::ItemIsEditable);
        QTreeWidget::editItem(item, 0);
        item->setFlags(oldFlags);
        const int id = item->data(0, Qt::UserRole).toInt();
        Transport *t = TransportManager::self()->transportById(id);
        if (!t) {
            qCWarning(MAILTRANSPORT_LOG) << "Transport" << id << "not known by manager.";
            return;
        }
        if (TransportManager::self()->defaultTransportId() == t->id()) {
            item->setText(0, t->name());
        }
    }
}

void TransportListView::commitData(QWidget *editor)
{
    if (selectedItems().isEmpty()) {
        // transport was deleted by someone else???
        qCDebug(MAILTRANSPORT_LOG) << "No selected item.";
        return;
    }
    QTreeWidgetItem *item = selectedItems().first();
    auto edit = dynamic_cast<QLineEdit *>(editor); // krazy:exclude=qclasses
    Q_ASSERT(edit); // original code had if

    const int id = item->data(0, Qt::UserRole).toInt();
    Transport *t = TransportManager::self()->transportById(id);
    if (!t) {
        qCWarning(MAILTRANSPORT_LOG) << "Transport" << id << "not known by manager.";
        return;
    }
    qCDebug(MAILTRANSPORT_LOG) << "Renaming transport" << id << "to" << edit->text();
    t->setName(edit->text());
    t->forceUniqueName();
    t->save();
}

void TransportListView::fillTransportList()
{
    // try to preserve the selection
    int selected = -1;
    if (currentItem()) {
        selected = currentItem()->data(0, Qt::UserRole).toInt();
    }

    clear();
    const auto lstTransports = TransportManager::self()->transports();
    for (Transport *t : lstTransports) {
        auto item = new QTreeWidgetItem(this);
        item->setData(0, Qt::UserRole, t->id());
        QString name = t->name();
        if (TransportManager::self()->defaultTransportId() == t->id()) {
            name += i18nc("@label the default mail transport", " (Default)");
            QFont font(item->font(0));
            font.setBold(true);
            item->setFont(0, font);
        }
        item->setText(0, name);
        item->setText(1, t->transportType().name());
        if (t->id() == selected) {
            setCurrentItem(item);
        }
    }
}
