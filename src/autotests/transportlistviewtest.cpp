/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportlistviewtest.h"
#include "widgets/transportlistview.h"
#include <QHeaderView>
#include <QTest>
QTEST_MAIN(TransportListViewTest)
TransportListViewTest::TransportListViewTest(QObject *parent)
    : QObject{parent}
{
}

void TransportListViewTest::shouldHaveDefaultValues()
{
    MailTransport::TransportListView w;
    QVERIFY(w.alternatingRowColors());
    QCOMPARE(w.selectionMode(), QAbstractItemView::SingleSelection);
    QCOMPARE(w.selectionBehavior(), QAbstractItemView::SelectRows);
    QVERIFY(!w.rootIsDecorated());
    QVERIFY(w.isSortingEnabled());
    QVERIFY(w.allColumnsShowFocus());
    QCOMPARE(w.contextMenuPolicy(), Qt::CustomContextMenu);
    QVERIFY(!w.header()->sectionsMovable());
}

#include "moc_transportlistviewtest.cpp"
