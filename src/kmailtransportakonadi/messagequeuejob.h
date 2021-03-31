/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <mailtransportakonadi_export.h>

#include <MailTransportAkonadi/DispatchModeAttribute>
#include <MailTransportAkonadi/SentActionAttribute>
#include <MailTransportAkonadi/SentBehaviourAttribute>
#include <MailTransportAkonadi/TransportAttribute>

#include <QString>
#include <QStringList>

#include <KCompositeJob>

#include <Akonadi/KMime/AddressAttribute>
#include <collection.h>

#include <kmime/kmime_message.h>

namespace MailTransport
{
/**
  @short Provides an interface for sending email.

  This class takes a KMime::Message and some related info such as sender and
  recipient addresses, and places the message in the outbox.  The mail
  dispatcher agent will then take it from there and send it.

  This is the preferred way for applications to send email.

  This job requires some options to be set before being started.  Modify the
  attributes of this job to change these options.

  You need to set the transport of the transport attribute, the from address of
  the address attribute and one of the to, cc or bcc addresses of the address
  attribute. Also, you need to call setMessage().
  Optionally, you can change the dispatch mode attribute or the sent behaviour
  attribute.

  Example:
  @code

  MessageQueueJob *job = new MessageQueueJob( this );
  job->setMessage( msg ); // msg is a Message::Ptr
  job->transportAttribute().setTransportId( TransportManager::self()->defaultTransportId() );
  // Use the default dispatch mode.
  // Use the default sent-behaviour.
  job->addressAttribute().setFrom( from ); // from is a QString
  job->addressAttribute().setTo( to ); // to is a QStringList
  connect( job, SIGNAL(result(KJob*)), this, SLOT(jobResult(KJob*)) );
  job->start();

  @endcode

  @see DispatchModeAttribute
  @see SentActionAttribute
  @see SentBehaviourAttribute
  @see TransportAttribute
  @see AddressAttribute

  @author Constantin Berzan <exit3219@gmail.com>
  @since 4.4
*/
class MAILTRANSPORTAKONADI_EXPORT MessageQueueJob : public KCompositeJob
{
    Q_OBJECT

public:
    /**
      Creates a new MessageQueueJob.
      @param parent the QObject parent
      This is not an autostarting job; you need to call start() yourself.
    */
    explicit MessageQueueJob(QObject *parent = nullptr);

    /**
      Destroys the MessageQueueJob.
      This job deletes itself after finishing.
    */
    ~MessageQueueJob() override;

    /**
      Returns the message to be sent.
    */
    Q_REQUIRED_RESULT KMime::Message::Ptr message() const;

    /**
      Returns a reference to the dispatch mode attribute for this message.
      Modify the returned attribute to change the dispatch mode.
    */
    DispatchModeAttribute &dispatchModeAttribute();

    /**
      Returns a reference to the address attribute for this message.
      Modify the returned attribute to change the receivers or the from
      address.
    */
    Akonadi::AddressAttribute &addressAttribute();

    /**
      Returns a reference to the transport attribute for this message.
      Modify the returned attribute to change the transport used for
      sending the mail.
    */
    TransportAttribute &transportAttribute();

    /**
      Returns a reference to the sent behaviour attribute for this message.
      Modify the returned attribute to change the sent behaviour.
    */
    SentBehaviourAttribute &sentBehaviourAttribute();

    /**
      Returns a reference to the sent action attribute for this message.
      Modify the returned attribute to change the sent actions.
    */
    SentActionAttribute &sentActionAttribute();

    /**
      Sets the message to be sent.
    */
    void setMessage(const KMime::Message::Ptr &message);

    /**
      Creates the item and places it in the outbox.
      It is now queued for sending by the mail dispatcher agent.
    */
    void start() override;

protected Q_SLOTS:
    /**
      Called when the ItemCreateJob subjob finishes.

      (reimplemented from KCompositeJob)
    */
    void slotResult(KJob *) override;

private:
    class Private;
    friend class Private;
    Private *const d;
};
} // namespace MailTransport

