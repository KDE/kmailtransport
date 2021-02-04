/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messagequeuejob.h"

#include "kmailtransportakonadi/transportattribute.h"
#include "transport.h"
#include "transportmanager.h"

#include "mailtransportakonadi_debug.h"
#include <KLocalizedString>

#include <addressattribute.h>
#include <item.h>
#include <itemcreatejob.h>
#include <messageflags.h>
#include <specialmailcollections.h>
#include <specialmailcollectionsrequestjob.h>

using namespace Akonadi;
using namespace KMime;
using namespace MailTransport;

/**
  @internal
*/
class Q_DECL_HIDDEN MailTransport::MessageQueueJob::Private
{
public:
    Private(MessageQueueJob *qq)
        : q(qq)
    {
    }

    MessageQueueJob *const q;

    Message::Ptr message;
    TransportAttribute transportAttribute;
    DispatchModeAttribute dispatchModeAttribute;
    SentBehaviourAttribute sentBehaviourAttribute;
    SentActionAttribute sentActionAttribute;
    AddressAttribute addressAttribute;
    bool started = false;

    /**
      Returns true if this message has everything it needs and is ready to be
      sent.
    */
    bool validate();

    // slot
    void outboxRequestResult(KJob *job);
};

bool MessageQueueJob::Private::validate()
{
    if (!message) {
        q->setError(UserDefinedError);
        q->setErrorText(i18n("Empty message."));
        q->emitResult();
        return false;
    }

    if ((addressAttribute.to().count() + addressAttribute.cc().count() + addressAttribute.bcc().count()) == 0) {
        q->setError(UserDefinedError);
        q->setErrorText(i18n("Message has no recipients."));
        q->emitResult();
        return false;
    }

    const int transport = transportAttribute.transportId();
    if (TransportManager::self()->transportById(transport, false) == nullptr) {
        q->setError(UserDefinedError);
        q->setErrorText(i18n("Message has invalid transport."));
        q->emitResult();
        return false;
    }

    if (sentBehaviourAttribute.sentBehaviour() == SentBehaviourAttribute::MoveToCollection && !(sentBehaviourAttribute.moveToCollection().isValid())) {
        q->setError(UserDefinedError);
        q->setErrorText(i18n("Message has invalid sent-mail folder."));
        q->emitResult();
        return false;
    } else if (sentBehaviourAttribute.sentBehaviour() == SentBehaviourAttribute::MoveToDefaultSentCollection) {
        // TODO require SpecialMailCollections::SentMail here?
    }

    return true; // all ok
}

void MessageQueueJob::Private::outboxRequestResult(KJob *job)
{
    Q_ASSERT(!started);
    started = true;

    if (job->error()) {
        qCritical() << "Failed to get the Outbox folder:" << job->error() << job->errorString();
        q->setError(job->error());
        q->emitResult();
        return;
    }

    if (!validate()) {
        // The error has been set; the result has been emitted.
        return;
    }

    auto requestJob = qobject_cast<SpecialMailCollectionsRequestJob *>(job);
    if (!requestJob) {
        return;
    }

    // Create item.
    Item item;
    item.setMimeType(QStringLiteral("message/rfc822"));
    item.setPayload<Message::Ptr>(message);

    // Set attributes.
    item.addAttribute(addressAttribute.clone());
    item.addAttribute(dispatchModeAttribute.clone());
    item.addAttribute(sentBehaviourAttribute.clone());
    item.addAttribute(sentActionAttribute.clone());
    item.addAttribute(transportAttribute.clone());

    Akonadi::MessageFlags::copyMessageFlags(*message, item);
    // Set flags.
    item.setFlag(Akonadi::MessageFlags::Queued);

    // Store the item in the outbox.
    const Collection collection = requestJob->collection();
    Q_ASSERT(collection.isValid());
    auto cjob = new ItemCreateJob(item, collection); // job autostarts
    q->addSubjob(cjob);
}

MessageQueueJob::MessageQueueJob(QObject *parent)
    : KCompositeJob(parent)
    , d(new Private(this))
{
}

MessageQueueJob::~MessageQueueJob()
{
    delete d;
}

Message::Ptr MessageQueueJob::message() const
{
    return d->message;
}

DispatchModeAttribute &MessageQueueJob::dispatchModeAttribute()
{
    return d->dispatchModeAttribute;
}

AddressAttribute &MessageQueueJob::addressAttribute()
{
    return d->addressAttribute;
}

TransportAttribute &MessageQueueJob::transportAttribute()
{
    return d->transportAttribute;
}

SentBehaviourAttribute &MessageQueueJob::sentBehaviourAttribute()
{
    return d->sentBehaviourAttribute;
}

SentActionAttribute &MessageQueueJob::sentActionAttribute()
{
    return d->sentActionAttribute;
}

void MessageQueueJob::setMessage(const Message::Ptr &message)
{
    d->message = message;
}

void MessageQueueJob::start()
{
    auto rjob = new SpecialMailCollectionsRequestJob(this);
    rjob->requestDefaultCollection(SpecialMailCollections::Outbox);
    connect(rjob, &SpecialMailCollectionsRequestJob::result, this, [this](KJob *job) {
        d->outboxRequestResult(job);
    });
    rjob->start();
}

void MessageQueueJob::slotResult(KJob *job)
{
    // error handling
    KCompositeJob::slotResult(job);

    if (!error()) {
        emitResult();
    }
}

#include "moc_messagequeuejob.cpp"
