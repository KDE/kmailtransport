/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "transportattribute.h"
#include <filteractionjob_p.h>
#include <mailtransportakonadi_export.h>

#include <itemfetchscope.h>
#include <job.h>

namespace MailTransport
{
/**
  FilterAction that finds all messages with a DispatchMode of Manual
  and assigns them a DispatchMode of Immediately.

  This is used to send "queued" messages on demand.

  @see FilterActionJob

  @author Constantin Berzan <exit3219@gmail.com>
  @since 4.4
*/
class SendQueuedAction : public Akonadi::FilterAction
{
public:
    /** Creates a SendQueuedAction. */
    SendQueuedAction();

    /** Destroys this object. */
    ~SendQueuedAction() override;

    Akonadi::ItemFetchScope fetchScope() const override;

    bool itemAccepted(const Akonadi::Item &item) const override;

    Akonadi::Job *itemAction(const Akonadi::Item &item, Akonadi::FilterActionJob *parent) const override;
};

/**
  FilterAction that finds all messages with an ErrorAttribute,
  removes the attribute, and sets the "$QUEUED" flag.

  This is used to retry sending messages that failed.

  @see FilterActionJob

  @author Constantin Berzan <exit3219@gmail.com>
  @since 4.4
*/
class ClearErrorAction : public Akonadi::FilterAction
{
public:
    /** Creates a ClearErrorAction. */
    ClearErrorAction();

    /** Destroys this object. */
    ~ClearErrorAction() override;

    Akonadi::ItemFetchScope fetchScope() const override;

    bool itemAccepted(const Akonadi::Item &item) const override;

    Akonadi::Job *itemAction(const Akonadi::Item &item, Akonadi::FilterActionJob *parent) const override;
};

/**
  FilterAction that changes the transport for all messages and
  sets the "$QUEUED" flag.

  This is used to send queued messages using an alternative transport.

  @see FilterActionJob

  @author Torgny Nyblom <kde@nyblom.org>
  @since 4.5
*/
class DispatchManualTransportAction : public Akonadi::FilterAction
{
public:
    DispatchManualTransportAction(int transportId);

    ~DispatchManualTransportAction() override;

    Akonadi::ItemFetchScope fetchScope() const override;

    bool itemAccepted(const Akonadi::Item &item) const override;

    Akonadi::Job *itemAction(const Akonadi::Item &item, Akonadi::FilterActionJob *parent) const override;

private:
    int mTransportId;
};
} // namespace MailTransport

