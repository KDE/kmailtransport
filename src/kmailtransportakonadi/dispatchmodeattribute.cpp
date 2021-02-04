/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dispatchmodeattribute.h"

#include "mailtransportakonadi_debug.h"

#include <attributefactory.h>

using namespace Akonadi;
using namespace MailTransport;

class Q_DECL_HIDDEN DispatchModeAttribute::Private
{
public:
    DispatchMode mMode;
    QDateTime mDueDate;
};

DispatchModeAttribute::DispatchModeAttribute(DispatchMode mode)
    : d(new Private)
{
    d->mMode = mode;
}

DispatchModeAttribute::~DispatchModeAttribute()
{
    delete d;
}

DispatchModeAttribute *DispatchModeAttribute::clone() const
{
    auto const cloned = new DispatchModeAttribute(d->mMode);
    cloned->setSendAfter(d->mDueDate);
    return cloned;
}

QByteArray DispatchModeAttribute::type() const
{
    static const QByteArray sType("DispatchModeAttribute");
    return sType;
}

QByteArray DispatchModeAttribute::serialized() const
{
    switch (d->mMode) {
    case Automatic:
        if (!d->mDueDate.isValid()) {
            return "immediately";
        } else {
            return "after" + d->mDueDate.toString(Qt::ISODate).toLatin1();
        }
    case Manual:
        return "never";
    }

    Q_ASSERT(false);
    return QByteArray(); // suppress control-reaches-end-of-non-void-function warning
}

void DispatchModeAttribute::deserialize(const QByteArray &data)
{
    d->mDueDate = QDateTime();
    if (data == "immediately") {
        d->mMode = Automatic;
    } else if (data == "never") {
        d->mMode = Manual;
    } else if (data.startsWith(QByteArrayLiteral("after"))) {
        d->mMode = Automatic;
        d->mDueDate = QDateTime::fromString(QString::fromLatin1(data.mid(5)), Qt::ISODate);
        // NOTE: 5 is the strlen of "after".
    } else {
        qCWarning(MAILTRANSPORTAKONADI_LOG) << "Failed to deserialize data [" << data << "]";
    }
}

DispatchModeAttribute::DispatchMode DispatchModeAttribute::dispatchMode() const
{
    return d->mMode;
}

void DispatchModeAttribute::setDispatchMode(DispatchMode mode)
{
    d->mMode = mode;
}

QDateTime DispatchModeAttribute::sendAfter() const
{
    return d->mDueDate;
}

void DispatchModeAttribute::setSendAfter(const QDateTime &date)
{
    d->mDueDate = date;
}
