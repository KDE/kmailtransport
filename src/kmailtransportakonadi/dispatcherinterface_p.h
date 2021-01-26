/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef MAILTRANSPORT_DISPATCHERINTERFACE_P_H
#define MAILTRANSPORT_DISPATCHERINTERFACE_P_H

#include <QObject>

class KJob;

namespace MailTransport
{
/**
  @internal
*/
class DispatcherInterfacePrivate : public QObject
{
    Q_OBJECT

public:
public Q_SLOTS:
    void massModifyResult(KJob *job);
};
}

#endif
