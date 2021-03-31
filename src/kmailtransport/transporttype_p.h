/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QSharedData>
#include <QString>

namespace MailTransport
{
/**
  @internal
*/
class TransportType::Private : public QSharedData
{
public:
    Private()
    {
    }

    Private(const Private &other)
        : QSharedData(other)
    {
        mName = other.mName;
        mDescription = other.mDescription;
        mIdentifier = other.mIdentifier;
        mIsAkonadiResource = other.mIsAkonadiResource;
    }

    QString mName;
    QString mDescription;
    QString mIdentifier;
    bool mIsAkonadiResource = false;
};
} // namespace MailTransport

