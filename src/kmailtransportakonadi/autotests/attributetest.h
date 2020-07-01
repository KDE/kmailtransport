/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ATTRIBUTETEST_H
#define ATTRIBUTETEST_H

#include <QObject>

/**
  This is a test of the various attributes.
*/
class AttributeTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testRegistrar();
    void testSerialization();
};

#endif
