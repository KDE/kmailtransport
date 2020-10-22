/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transporttype.h"
#include "transporttype_p.h"

using namespace MailTransport;

TransportType::TransportType()
    : d(new Private)
{
}

TransportType::TransportType(const TransportType &other)
    : d(other.d)
{
}

TransportType::~TransportType()
{
}

TransportType &TransportType::operator=(const TransportType &other)
{
    if (this != &other) {
        d = other.d;
    }
    return *this;
}

bool TransportType::operator==(const TransportType &other) const
{
    return d->mIdentifier == other.d->mIdentifier;
}

bool TransportType::isValid() const
{
    return !d->mIdentifier.isEmpty();
}

QString TransportType::name() const
{
    return d->mName;
}

QString TransportType::description() const
{
    return d->mDescription;
}

QString TransportType::identifier() const
{
    return d->mIdentifier;
}

bool TransportType::isAkonadiResource() const
{
    return d->mIsAkonadiResource;
}
