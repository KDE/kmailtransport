/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "clearerror.h"

#include <QApplication>

#include <collection.h>
#include <control.h>
#include <filteractionjob_p.h>
#include <kmailtransportakonadi/dispatcherinterface.h>
#include <specialmailcollections.h>
#include <specialmailcollectionsrequestjob.h>

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
    DispatcherInterface().retryDispatching();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("clearerror"));

    new Runner();
    return app.exec();
}
