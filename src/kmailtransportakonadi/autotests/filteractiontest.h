/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <QString>

#include <collection.h>
#include <item.h>

class FilterActionTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testMassModifyItem();
    void testMassModifyItems();
    void testMassModifyCollection();

private:
    Akonadi::Collection createCollection(const QString &name);
    Akonadi::Item createItem(const Akonadi::Collection &col, bool accept);
};

