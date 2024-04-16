/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transporttreeviewtest.h"
#include "widgets/transporttreeview.h"
#include <QHeaderView>
#include <QTest>
QTEST_MAIN(TransportTreeViewTest)
TransportTreeViewTest::TransportTreeViewTest(QObject *parent)
    : QObject{parent}
{
}

void TransportTreeViewTest::shouldHaveDefaultValues()
{
    MailTransport::TransportTreeView w;
    QVERIFY(w.alternatingRowColors());
    QCOMPARE(w.selectionMode(), QAbstractItemView::SingleSelection);
    QCOMPARE(w.selectionBehavior(), QAbstractItemView::SelectRows);
    QVERIFY(!w.rootIsDecorated());
    QVERIFY(w.isSortingEnabled());
    QVERIFY(w.allColumnsShowFocus());
    QCOMPARE(w.contextMenuPolicy(), Qt::CustomContextMenu);
    QVERIFY(!w.header()->sectionsMovable());
}

#include "moc_transporttreeviewtest.cpp"
