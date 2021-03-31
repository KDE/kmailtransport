/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <mailtransportakonadi_export.h>

#include <QDateTime>

#include <attribute.h>

namespace MailTransport
{
/**
  Attribute determining how and when a message from the outbox should be
  dispatched.  Messages can be sent immediately, sent only when the user
  explicitly requests it, or sent automatically at a certain date and time.

  @author Constantin Berzan <exit3219@gmail.com>
  @since 4.4
*/
class MAILTRANSPORTAKONADI_EXPORT DispatchModeAttribute : public Akonadi::Attribute
{
public:
    /**
      Determines how the message is sent.
    */
    enum DispatchMode {
        Automatic, ///< Send message as soon as possible, but no earlier than
        ///  specified by setSendAfter()
        Manual ///< Send message only when the user requests so.
    };

    /**
      Creates a new DispatchModeAttribute.
    */
    explicit DispatchModeAttribute(DispatchMode mode = Automatic);

    /**
      Destroys the DispatchModeAttribute.
    */
    ~DispatchModeAttribute() override;

    /* reimpl */
    DispatchModeAttribute *clone() const override;
    Q_REQUIRED_RESULT QByteArray type() const override;
    Q_REQUIRED_RESULT QByteArray serialized() const override;
    void deserialize(const QByteArray &data) override;

    /**
      Returns the dispatch mode for the message.
      @see DispatchMode.
    */
    Q_REQUIRED_RESULT DispatchMode dispatchMode() const;

    /**
      Sets the dispatch mode for the message.
      @param mode the dispatch mode to set
      @see DispatchMode.
    */
    void setDispatchMode(DispatchMode mode);

    /**
      Returns the date and time when the message should be sent.
      Only valid if dispatchMode() is Automatic.
    */
    Q_REQUIRED_RESULT QDateTime sendAfter() const;

    /**
      Sets the date and time when the message should be sent.
      @param date the date and time to set
      @see setDispatchMode.
    */
    void setSendAfter(const QDateTime &date);

private:
    class Private;
    Private *const d;
};
} // namespace MailTransport

