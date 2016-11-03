/*
    Copyright 2009 Constantin Berzan <exit3219@gmail.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "sentbehaviourattribute.h"

using namespace Akonadi;
using namespace MailTransport;

class SentBehaviourAttribute::Private
{
public:
    SentBehaviourAttribute::SentBehaviour mBehaviour;
    Akonadi::Collection mMoveToCollection;
    bool mSilent;
};

SentBehaviourAttribute::SentBehaviourAttribute(SentBehaviour beh, const Collection &moveToCollection,
                                               bool sendSilently)
    : d(new Private)
{
    d->mBehaviour = beh;
    d->mMoveToCollection = moveToCollection;
    d->mSilent = sendSilently;
}

SentBehaviourAttribute::~SentBehaviourAttribute()
{
    delete d;
}

SentBehaviourAttribute *SentBehaviourAttribute::clone() const
{
    return new SentBehaviourAttribute(d->mBehaviour, d->mMoveToCollection);
}

QByteArray SentBehaviourAttribute::type() const
{
    static const QByteArray sType("SentBehaviourAttribute");
    return sType;
}

QByteArray SentBehaviourAttribute::serialized() const
{
    QByteArray out;

    switch (d->mBehaviour) {
    case Delete:
        out = "delete";
        break;
    case MoveToCollection: 
        out = "moveTo" + QByteArray::number(d->mMoveToCollection.id());
        break;
    case MoveToDefaultSentCollection:
        out = "moveToDefault";
        break;
    }

    if (d->mSilent) {
        out += ",silent";
    }

    Q_ASSERT(false);
    return QByteArray();
}

void SentBehaviourAttribute::deserialize(const QByteArray &data)
{
    const QByteArrayList in = data.split(',');
    Q_ASSERT(in.size() > 0);

    d->mMoveToCollection = Akonadi::Collection(-1);
    if (in[0] == "delete") {
        d->mBehaviour = Delete;
    } else if (in[0] == "moveToDefault") {
        d->mBehaviour = MoveToDefaultSentCollection;
    } else if (in[0].startsWith(QByteArray("moveTo"))) {
        d->mBehaviour = MoveToCollection;
        d->mMoveToCollection = Akonadi::Collection(data.mid(6).toLongLong());
        // NOTE: 6 is the strlen of "moveTo".
    } else {
        Q_ASSERT(false);
    }

    if (in.size() == 2 && in[1] == "silent") {
        d->mSilent = true;
    }
}

SentBehaviourAttribute::SentBehaviour SentBehaviourAttribute::sentBehaviour() const
{
    return d->mBehaviour;
}

void SentBehaviourAttribute::setSentBehaviour(SentBehaviour beh)
{
    d->mBehaviour = beh;
}

Collection SentBehaviourAttribute::moveToCollection() const
{
    return d->mMoveToCollection;
}

void SentBehaviourAttribute::setMoveToCollection(const Collection &moveToCollection)
{
    d->mMoveToCollection = moveToCollection;
}

bool SentBehaviourAttribute::sendSilently() const
{
    return d->mSilent;
}

void SentBehaviourAttribute::setSendSilently(bool sendSilently)
{
    d->mSilent = sendSilently;
}

