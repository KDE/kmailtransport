/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dispatcherinterface.h"
#include "dispatcherinterface_p.h"

#include "kmailtransportakonadi/outboxactions_p.h"

#include "mailtransportakonadi_debug.h"

#include "kmailtransportakonadi/transportattribute.h"
#include <agentmanager.h>
#include <collection.h>
#include <specialmailcollections.h>

using namespace Akonadi;
using namespace MailTransport;

Q_GLOBAL_STATIC(DispatcherInterfacePrivate, sInstance)

void DispatcherInterfacePrivate::massModifyResult(KJob *job)
{
    // Nothing to do here, really.  If the job fails, the user can retry it.
    if (job->error()) {
        qCDebug(MAILTRANSPORTAKONADI_LOG) << "failed" << job->errorString();
    } else {
        qCDebug(MAILTRANSPORTAKONADI_LOG) << "succeeded.";
    }
}

DispatcherInterface::DispatcherInterface()
{
}

AgentInstance DispatcherInterface::dispatcherInstance() const
{
    AgentInstance a = AgentManager::self()->instance(QStringLiteral("akonadi_maildispatcher_agent"));
    if (!a.isValid()) {
        qCWarning(MAILTRANSPORTAKONADI_LOG) << "Could not get MDA instance.";
    }
    return a;
}

void DispatcherInterface::dispatchManually()
{
    Collection outbox = SpecialMailCollections::self()->defaultCollection(SpecialMailCollections::Outbox);
    if (!outbox.isValid()) {
        //    qCritical() << "Could not access Outbox.";
        return;
    }

    auto mjob = new FilterActionJob(outbox, new SendQueuedAction, sInstance);
    QObject::connect(mjob, &KJob::result, sInstance(), &DispatcherInterfacePrivate::massModifyResult);
}

void DispatcherInterface::retryDispatching()
{
    Collection outbox = SpecialMailCollections::self()->defaultCollection(SpecialMailCollections::Outbox);
    if (!outbox.isValid()) {
        //    qCritical() << "Could not access Outbox.";
        return;
    }

    auto mjob = new FilterActionJob(outbox, new ClearErrorAction, sInstance);
    QObject::connect(mjob, &KJob::result, sInstance(), &DispatcherInterfacePrivate::massModifyResult);
}

void DispatcherInterface::dispatchManualTransport(int transportId)
{
    Collection outbox = SpecialMailCollections::self()->defaultCollection(SpecialMailCollections::Outbox);
    if (!outbox.isValid()) {
        //    qCritical() << "Could not access Outbox.";
        return;
    }

    auto mjob = new FilterActionJob(outbox, new DispatchManualTransportAction(transportId), sInstance);
    QObject::connect(mjob, &KJob::result, sInstance(), &DispatcherInterfacePrivate::massModifyResult);
}

#include "moc_dispatcherinterface_p.cpp"
