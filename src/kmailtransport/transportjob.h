/*
  SPDX-FileCopyrightText: 2007 Volker Krause <vkrause@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <mailtransport_export.h>

#include <QStringList>

#include <KCompositeJob>

class QBuffer;

namespace MailTransport
{
class Transport;

/**
  Abstract base class for all mail transport jobs.
  This is a job that is supposed to send exactly one mail.

  @deprecated Use MessageQueueJob for sending e-mail.
*/
class MAILTRANSPORT_DEPRECATED_EXPORT TransportJob : public KCompositeJob
{
    Q_OBJECT
    friend class TransportManager;

public:
    /**
      Deletes this transport job.
    */
    ~TransportJob() override;

    /**
      Sets the sender of the mail.
      @p sender must be the plain email address, not including display name.
    */
    void setSender(const QString &sender);

    /**
      Sets the "To" receiver(s) of the mail.
      @p to must be the plain email address(es), not including display name.
    */
    void setTo(const QStringList &to);

    /**
      Sets the "Cc" receiver(s) of the mail.
      @p cc must be the plain email address(es), not including display name.
    */
    void setCc(const QStringList &cc);

    /**
      Sets the "Bcc" receiver(s) of the mail.
      @p bcc must be the plain email address(es), not including display name.
    */
    void setBcc(const QStringList &bcc);

    /**
      Sets the content of the mail.
    */
    void setData(const QByteArray &data);

    /**
      Starts this job. It is recommended to not call this method directly but use
      TransportManager::schedule() to execute the job instead.

      @see TransportManager::schedule()
    */
    void start() override;

    /**
      Returns the Transport object containing the mail transport settings.
    */
    Transport *transport() const;

protected:
    /**
      Creates a new mail transport job.
      @param transport The transport configuration. This must be a deep copy of
      a Transport object, the job takes the ownership of this object.
      @param parent The parent object.
      @see TransportManager::createTransportJob()
    */
    explicit TransportJob(Transport *transport, QObject *parent = nullptr);

    /**
      Returns the sender of the mail.
    */
    Q_REQUIRED_RESULT QString sender() const;

    /**
      Returns the "To" receiver(s) of the mail.
    */
    Q_REQUIRED_RESULT QStringList to() const;

    /**
      Returns the "Cc" receiver(s) of the mail.
    */
    Q_REQUIRED_RESULT QStringList cc() const;

    /**
      Returns the "Bcc" receiver(s) of the mail.
    */
    Q_REQUIRED_RESULT QStringList bcc() const;

    /**
      Returns the data of the mail.
    */
    Q_REQUIRED_RESULT QByteArray data() const;

    /**
      Returns a QBuffer opened on the message data. This is useful for
      processing the data in smaller chunks.
    */
    QBuffer *buffer();

    /**
      Do the actual work, implement in your subclass.
    */
    virtual void doStart() = 0;

private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond
};
} // namespace MailTransport

