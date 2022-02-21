/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "sendqueued.h"

#include <QApplication>

#include <Akonadi/Collection>
#include <Akonadi/Control>
#include <Akonadi/SpecialMailCollections>
#include <Akonadi/SpecialMailCollectionsRequestJob>
#include <filteractionjob_p.h>
#include <kmailtransportakonadi/dispatcherinterface.h>

using namespace Akonadi;
using namespace MailTransport;

Runner::Runner()
{
    Control::start();

    auto rjob = new SpecialMailCollectionsRequestJob(this);
    rjob->requestDefaultCollection(SpecialMailCollections::Outbox);
    connect(rjob, &SpecialMailCollectionsRequestJob::result, this, &Runner::checkFolders);
    rjob->start();
}

void Runner::checkFolders()
{
    DispatcherInterface().dispatchManually();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("sendqueued"));

    new Runner();
    return app.exec();
}
