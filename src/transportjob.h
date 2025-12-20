/*
  SPDX-FileCopyrightText: 2007 Volker Krause <vkrause@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"

#include <QStringList>

#include <KCompositeJob>

#include <memory>

class QBuffer;

namespace MailTransport
{
class Transport;
class TransportJobPrivate;

/*!
  Abstract base class for all mail transport jobs.
  This is a job that is supposed to send exactly one mail.

  \deprecated Use MessageQueueJob for sending e-mail.
*/
class MAILTRANSPORT_DEPRECATED_EXPORT TransportJob : public KCompositeJob
{
    Q_OBJECT
    friend class TransportManager;

public:
    /*!
      Deletes this transport job.
    */
    ~TransportJob() override;

    /*!
      Sets the sender of the mail.
      \a sender must be the plain email address, not including display name.
    */
    void setSender(const QString &sender);

    /*!
      Sets the "To" receiver(s) of the mail.
      \a to must be the plain email address(es), not including display name.
    */
    void setTo(const QStringList &to);

    /*!
      Sets the "Cc" receiver(s) of the mail.
      \a cc must be the plain email address(es), not including display name.
    */
    void setCc(const QStringList &cc);

    /*!
      Sets the "Bcc" receiver(s) of the mail.
      \a bcc must be the plain email address(es), not including display name.
    */
    void setBcc(const QStringList &bcc);

    /*!
      Sets the content of the mail.
    */
    void setData(const QByteArray &data);

    /*!
      Starts this job. It is recommended to not call this method directly but use
      TransportManager::schedule() to execute the job instead.

      \sa TransportManager::schedule()
    */
    void start() override;

    /*!
      Returns the Transport object containing the mail transport settings.
    */
    Transport *transport() const;

    /*!
       Sets the content of the mail.
    */
    void setDeliveryStatusNotification(bool enabled);

protected:
    /*!
      Creates a new mail transport job.
      \a transport The transport configuration. This must be a deep copy of
      a Transport object, the job takes the ownership of this object.
      \a parent The parent object.
      \sa TransportManager::createTransportJob()
    */
    explicit TransportJob(Transport *transport, QObject *parent = nullptr);

    /*!
      Returns the sender of the mail.
    */
    [[nodiscard]] QString sender() const;

    /*!
      Returns the "To" receiver(s) of the mail.
    */
    [[nodiscard]] QStringList to() const;

    /*!
      Returns the "Cc" receiver(s) of the mail.
    */
    [[nodiscard]] QStringList cc() const;

    /*!
      Returns the "Bcc" receiver(s) of the mail.
    */
    [[nodiscard]] QStringList bcc() const;

    /*!
      Returns the data of the mail.
    */
    [[nodiscard]] QByteArray data() const;

    /*!
      Returns a QBuffer opened on the message data. This is useful for
      processing the data in smaller chunks.
    */
    QBuffer *buffer();

    /*!
      Do the actual work, implement in your subclass.
    */
    virtual void doStart() = 0;

    /*!
      Returns true if DSN is enabled.
    */
    [[nodiscard]] bool deliveryStatusNotification() const;

private:
    std::unique_ptr<TransportJobPrivate> const d;
};
} // namespace MailTransport
