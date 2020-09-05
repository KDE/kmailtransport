/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "sentbehaviourattribute.h"

using namespace Akonadi;
using namespace MailTransport;

class Q_DECL_HIDDEN SentBehaviourAttribute::Private
{
public:
    Private()
    {
    }

    SentBehaviourAttribute::SentBehaviour mBehaviour = SentBehaviourAttribute::MoveToDefaultSentCollection;
    Akonadi::Collection mMoveToCollection;
    bool mSilent = false;
};

SentBehaviourAttribute::SentBehaviourAttribute(SentBehaviour beh, const Collection &moveToCollection, bool sendSilently)
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
    return new SentBehaviourAttribute(d->mBehaviour, d->mMoveToCollection, d->mSilent);
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
        out = QByteArrayLiteral("delete");
        break;
    case MoveToCollection:
        out = QByteArrayLiteral("moveTo") + QByteArray::number(d->mMoveToCollection.id());
        break;
    case MoveToDefaultSentCollection:
        out = QByteArrayLiteral("moveToDefault");
        break;
    default:
        Q_ASSERT(false);
        return QByteArray();
    }

    if (d->mSilent) {
        out += QByteArrayLiteral(",silent");
    }

    return out;
}

void SentBehaviourAttribute::deserialize(const QByteArray &data)
{
    const QByteArrayList in = data.split(',');
    Q_ASSERT(!in.isEmpty());

    const QByteArray attr0 = in[0];
    d->mMoveToCollection = Akonadi::Collection(-1);
    if (attr0 == "delete") {
        d->mBehaviour = Delete;
    } else if (attr0 == "moveToDefault") {
        d->mBehaviour = MoveToDefaultSentCollection;
    } else if (attr0.startsWith(QByteArrayLiteral("moveTo"))) {
        d->mBehaviour = MoveToCollection;
        d->mMoveToCollection = Akonadi::Collection(attr0.mid(6).toLongLong());
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
