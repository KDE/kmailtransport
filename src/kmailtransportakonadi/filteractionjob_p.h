/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <mailtransportakonadi_export.h>

#include <item.h>
#include <transactionsequence.h>

namespace Akonadi
{
class Collection;
class ItemFetchScope;
class Job;

class FilterActionJob;

/**
 * @short Base class for a filter/action for FilterActionJob.
 *
 * Abstract class defining an interface for a filter and an action for
 * FilterActionJob.  The virtual methods must be implemented in subclasses.
 *
 * @code
 * class ClearErrorAction : public Akonadi::FilterAction
 * {
 *   public:
 *     // reimpl
 *     virtual Akonadi::ItemFetchScope fetchScope() const
 *     {
 *       ItemFetchScope scope;
 *       scope.fetchFullPayload( false );
 *       scope.fetchAttribute<ErrorAttribute>();
 *       return scope;
 *     }
 *
 *     virtual bool itemAccepted( const Akonadi::Item &item ) const
 *     {
 *       return item.hasAttribute<ErrorAttribute>();
 *     }
 *
 *     virtual Akonadi::Job *itemAction( const Akonadi::Item &item,
 *                                       Akonadi::FilterActionJob *parent ) const
 *     {
 *       Item cp = item;
 *       cp.removeAttribute<ErrorAttribute>();
 *       return new ItemModifyJob( cp, parent );
 *     }
 * };
 * @endcode
 *
 * @see FilterActionJob
 *
 * @author Constantin Berzan <exit3219@gmail.com>
 * @since 4.4
 */
class MAILTRANSPORTAKONADI_EXPORT FilterAction
{
public:
    /**
     * Destroys this filter action.
     *
     * A FilterActionJob will delete its FilterAction automatically.
     */
    virtual ~FilterAction();

    /**
     * Returns an ItemFetchScope to use if the FilterActionJob needs
     * to fetch the items from a collection.
     *
     * @note The items are not fetched unless FilterActionJob is
     *       constructed with a Collection parameter.
     */
    virtual Akonadi::ItemFetchScope fetchScope() const = 0;

    /**
     * Returns @c true if the @p item is accepted by the filter and should be
     * acted upon by the FilterActionJob.
     */
    virtual bool itemAccepted(const Akonadi::Item &item) const = 0;

    /**
     * Returns a job to act on the @p item.
     * The FilterActionJob will finish when all such jobs are finished.
     * @param item the item to work on
     * @param parent the parent job
     */
    virtual Akonadi::Job *itemAction(const Akonadi::Item &item, Akonadi::FilterActionJob *parent) const = 0;
};

/**
 * @short Job to filter and apply an action on a set of items.
 *
 * This jobs filters through a set of items, and applies an action to the
 * items which are accepted by the filter.  The filter and action
 * are provided by a functor class derived from FilterAction.
 *
 * For example, a MarkAsRead action/filter may be used to mark all messages
 * in a folder as read.
 *
 * @code
 * FilterActionJob *mjob = new FilterActionJob( LocalFolders::self()->outbox(),
 *                                              new ClearErrorAction, this );
 * connect( mjob, SIGNAL( result( KJob* ) ), this, SLOT( massModifyResult( KJob* ) ) );
 * @endcode
 *
 * @see FilterAction
 *
 * @author Constantin Berzan <exit3219@gmail.com>
 * @since 4.4
 */
class MAILTRANSPORTAKONADI_EXPORT FilterActionJob : public TransactionSequence
{
    Q_OBJECT

public:
    /**
     * Creates a filter action job to act on a single item.
     *
     * @param item The item to act on. The item is not re-fetched.
     * @param functor The FilterAction to use.
     * @param parent The parent object.
     */
    FilterActionJob(const Item &item, FilterAction *functor, QObject *parent = nullptr);

    /**
     * Creates a filter action job to act on a set of items.
     *
     * @param items The items to act on. The items are not re-fetched.
     * @param functor The FilterAction to use.
     * @param parent The parent object.
     */
    FilterActionJob(const Item::List &items, FilterAction *functor, QObject *parent = nullptr);

    /**
     * Creates a filter action job to act on items in a collection.
     *
     * @param collection The collection to act on.
     *                   The items of the collection are fetched using functor->fetchScope().
     * @param functor The FilterAction to use.
     * @param parent The parent object.
     */
    FilterActionJob(const Collection &collection, FilterAction *functor, QObject *parent = nullptr);

    /**
     * Destroys the filter action job.
     */
    ~FilterActionJob() override;

protected:
    void doStart() override;

private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond
};
} // namespace Akonadi

