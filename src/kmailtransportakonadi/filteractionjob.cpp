/*
  Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>

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

#include "filteractionjob_p.h"

#include <collection.h>
#include <itemfetchjob.h>
#include <itemfetchscope.h>

#include "mailtransportakonadi_debug.h"

using namespace Akonadi;

class Q_DECL_HIDDEN Akonadi::FilterActionJob::Private
{
public:
    Private(FilterActionJob *qq)
        : q(qq)
        , functor(nullptr)
    {
    }

    ~Private()
    {
        delete functor;
    }

    FilterActionJob *q;
    Collection collection;
    Item::List items;
    FilterAction *functor = nullptr;
    ItemFetchScope fetchScope;

    // Q_SLOTS:
    void fetchResult(KJob *job);

    void traverseItems();
};

void FilterActionJob::Private::fetchResult(KJob *job)
{
    if (job->error()) {
        // KCompositeJob takes care of errors.
        return;
    }

    ItemFetchJob *fjob = dynamic_cast<ItemFetchJob *>(job);
    Q_ASSERT(fjob);
    Q_ASSERT(items.isEmpty());
    items = fjob->items();
    traverseItems();
}

void FilterActionJob::Private::traverseItems()
{
    Q_ASSERT(functor);
    qCDebug(MAILTRANSPORTAKONADI_LOG) << "Traversing" << items.count() << "items.";
    for (const Item &item : qAsConst(items)) {
        if (functor->itemAccepted(item)) {
            functor->itemAction(item, q);
            qCDebug(MAILTRANSPORTAKONADI_LOG) << "Added subjob for item" << item.id();
        }
    }
    if (q->subjobs().isEmpty()) {
        qCDebug(MAILTRANSPORTAKONADI_LOG) << "No subjobs; I am done";
    } else {
        qCDebug(MAILTRANSPORTAKONADI_LOG) << "Have subjobs; Done when last of them is";
    }
    q->commit();
}

FilterAction::~FilterAction()
{
}

FilterActionJob::FilterActionJob(const Item &item, FilterAction *functor, QObject *parent)
    : TransactionSequence(parent)
    , d(new Private(this))
{
    d->functor = functor;
    d->items << item;
}

FilterActionJob::FilterActionJob(const Item::List &items, FilterAction *functor, QObject *parent)
    : TransactionSequence(parent)
    , d(new Private(this))
{
    d->functor = functor;
    d->items = items;
}

FilterActionJob::FilterActionJob(const Collection &collection, FilterAction *functor, QObject *parent)
    : TransactionSequence(parent)
    , d(new Private(this))
{
    d->functor = functor;
    Q_ASSERT(collection.isValid());
    d->collection = collection;
}

FilterActionJob::~FilterActionJob()
{
    delete d;
}

void FilterActionJob::doStart()
{
    if (d->collection.isValid()) {
        qCDebug(MAILTRANSPORTAKONADI_LOG) << "Fetching collection" << d->collection.id();
        ItemFetchJob *fjob = new ItemFetchJob(d->collection, this);
        Q_ASSERT(d->functor);
        d->fetchScope = d->functor->fetchScope();
        fjob->setFetchScope(d->fetchScope);
        connect(fjob, &ItemFetchJob::result, this, [this](KJob *job) { d->fetchResult(job); });
    } else {
        d->traverseItems();
    }
}

#include "moc_filteractionjob_p.cpp"
