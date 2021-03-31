/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

/**
  This class uses the SendQueuedAction to mark all queued messages in the
  outbox for immediate sending.
*/
class Runner : public QObject
{
    Q_OBJECT

public:
    Runner();

private Q_SLOTS:
    void checkFolders();
};

