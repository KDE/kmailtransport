/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <mailtransportakonadi_export.h>

#include <agentinstance.h>

// krazy:excludeall=dpointer

namespace MailTransport
{
/**
  @short An interface for applications to interact with the dispatcher agent.

  This class provides methods such as send queued messages (@see
  dispatchManually) and retry sending (@see retryDispatching).

  This class also takes care of registering the attributes that the mail
  dispatcher agent and MailTransport use.

  @author Constantin Berzan <exit3219@gmail.com>
  @since 4.4
*/
class MAILTRANSPORTAKONADI_EXPORT DispatcherInterface
{
public:
    /**
      Creates a new dispatcher interface.
    */
    DispatcherInterface();

    /**
      Returns the current instance of the mail dispatcher agent. May return an invalid
      AgentInstance in case it cannot find the mail dispatcher agent.
    */
    Q_REQUIRED_RESULT Akonadi::AgentInstance dispatcherInstance() const;

    /**
      Looks for messages in the outbox with DispatchMode::Manual and marks them
      DispatchMode::Automatic for sending.
    */
    void dispatchManually();

    /**
      Looks for messages in the outbox with ErrorAttribute, and clears them and
      queues them again for sending.
    */
    void retryDispatching();

    /**
      Looks for messages in the outbox with DispatchMode::Manual and changes the
      Transport for them to the one with id @p transportId.

      @param transportId the transport to dispatch "manual dispatch" messages with
      @since 4.5
    */
    void dispatchManualTransport(int transportId);
};
} // namespace MailTransport

