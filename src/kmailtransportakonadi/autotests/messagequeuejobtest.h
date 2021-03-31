/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

/**
  This tests the ability to queue messages (MessageQueueJob class).
  Note that the actual sending of messages is the MDA's job, and is not tested
  here.
 */
class MessageQueueJobTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testValidMessages();
    void testInvalidMessages();

private:
    void verifyOutboxContents(qlonglong count);
};

