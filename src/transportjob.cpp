/*
    SPDX-FileCopyrightText: 2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportjob.h"
#include "transport.h"

#include <QBuffer>

#include <KLocalizedString>

using namespace MailTransport;

class MailTransport::TransportJobPrivate
{
public:
    QString sender;
    QStringList to;
    QStringList cc;
    QStringList bcc;
    QByteArray data;
    bool dsn = false;
    Transport *transport = nullptr;
    QBuffer *buffer = nullptr;
};

TransportJob::TransportJob(Transport *transport, QObject *parent)
    : KCompositeJob(parent)
    , d(new TransportJobPrivate)
{
    d->transport = transport;
    d->buffer = nullptr;
}

TransportJob::~TransportJob()
{
    delete d->transport;
}

void TransportJob::setSender(const QString &sender)
{
    d->sender = sender;
}

void TransportJob::setTo(const QStringList &to)
{
    d->to = to;
}

void TransportJob::setCc(const QStringList &cc)
{
    d->cc = cc;
}

void TransportJob::setBcc(const QStringList &bcc)
{
    d->bcc = bcc;
}

void TransportJob::setData(const QByteArray &data)
{
    d->data = data;
}

Transport *TransportJob::transport() const
{
    return d->transport;
}

void TransportJob::setDeliveryStatusNotification(bool enabled)
{
    d->dsn = enabled;
}

QString TransportJob::sender() const
{
    return d->sender;
}

QStringList TransportJob::to() const
{
    return d->to;
}

QStringList TransportJob::cc() const
{
    return d->cc;
}

QStringList TransportJob::bcc() const
{
    return d->bcc;
}

QByteArray TransportJob::data() const
{
    return d->data;
}

QBuffer *TransportJob::buffer()
{
    if (!d->buffer) {
        d->buffer = new QBuffer(this);
        d->buffer->setData(d->data);
        d->buffer->open(QIODevice::ReadOnly);
        Q_ASSERT(d->buffer->isOpen());
    }
    return d->buffer;
}

bool TransportJob::deliveryStatusNotification() const
{
    return d->dsn;
}

void TransportJob::start()
{
    if (!transport()->isValid()) {
        setError(UserDefinedError);
        setErrorText(i18n("The outgoing account \"%1\" is not correctly configured.", transport()->name()));
        emitResult();
        return;
    }
    doStart();
}

#include "moc_transportjob.cpp"
