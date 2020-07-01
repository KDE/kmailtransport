/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "abort.h"

#include <QTimer>

#include <QApplication>
#include <QDebug>

#include <control.h>

#include <kmailtransportakonadi/dispatcherinterface.h>

using namespace Akonadi;
using namespace MailTransport;

Runner::Runner()
{
    Control::start();

    QTimer::singleShot(0, this, &Runner::sendAbort);
}

void Runner::sendAbort()
{
    const AgentInstance mda = DispatcherInterface().dispatcherInstance();
    if (!mda.isValid()) {
        qDebug() << "Invalid instance; waiting.";
        QTimer::singleShot(1000, this, &Runner::sendAbort);
        return;
    }

    mda.abortCurrentTask();
    qDebug() << "Told the MDA to abort.";
    QApplication::exit(0);
}

int main(int argc, char **argv)
{
    QApplication::setApplicationName(QStringLiteral("Abort"));
    QApplication app(argc, argv);

    new Runner();
    return app.exec();
}
