/*
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <mailtransport_export.h>
#include <transportbase.h>
#include <transporttype.h>

class TransportPrivate;
namespace QKeychain
{
class Job;
}
namespace MailTransport
{
class TransportType;

/**
  Represents the settings of a specific mail transport.

  To create a new empty Transport object, use TransportManager::createTransport().

  Initialize an empty Transport object by calling the set...() methods defined in
  kcfg-generated TransportBase, and in this class.
*/
class MAILTRANSPORT_EXPORT Transport : public TransportBase
{
    Q_OBJECT
    friend class TransportManager;
    friend class TransportManagerPrivate;

public:
    /**
      Destructor
    */
    ~Transport() override;

    using List = QList<Transport *>;

    /**
      Returns true if this transport is valid, ie. has all necessary data set.
    */
    Q_REQUIRED_RESULT bool isValid() const;

    /**
      Returns the password of this transport.
    */
    Q_REQUIRED_RESULT QString password();

    /**
      Sets the password of this transport.
      @param passwd The new password.
    */
    void setPassword(const QString &passwd);

    /**
      Makes sure the transport has a unique name.  Adds #1, #2, #3 etc. if
      necessary.
      @since 4.4
    */
    void forceUniqueName();

    /**
      This function synchronizes the password of this transport with the
      password of the transport with the same ID that is managed by the
      transport manager. This is only useful for cloned transports, since
      their passwords don't automatically get updated when calling
      TransportManager::loadPasswordsAsync() or TransportManager::loadPasswords().

      @sa clone()
      @since 4.2
    */
    void updatePasswordState();

    /**
      Returns true if all settings have been loaded.
      This is the way to find out if the password has already been loaded
      from the wallet.
    */
    Q_REQUIRED_RESULT bool isComplete() const;

    /**
      Returns a string representation of the authentication type.
    */
    Q_REQUIRED_RESULT QString authenticationTypeString() const;

    /**
      Returns a string representation of the authentication type.
      Convenience function when there isn't a Transport object
      instantiated.

      @since 4.5
    */
    static Q_REQUIRED_RESULT QString authenticationTypeString(int type);

    /**
      Returns a deep copy of this Transport object which will no longer be
      automatically updated. Use this if you need to store a Transport object
      over a longer time. However it is recommended to store transport identifiers
      instead if possible.

      @sa updatePasswordState()
    */
    Transport *clone() const;

    /**
      Returns the type of this transport.
      @see TransportType.
      @since 4.4
    */
    Q_REQUIRED_RESULT TransportType transportType() const;

protected:
    /**
      Creates a Transport object. Should only be used by TransportManager.
      @param cfgGroup The KConfig group to read its data from.
    */
    Transport(const QString &cfgGroup);

    void usrRead() override;
    bool usrSave() override;

    /**
      Returns true if the password was not stored in the wallet.
    */
    Q_REQUIRED_RESULT bool needsWalletMigration() const;

    /**
      Try to migrate the password from the config file to the wallet.
    */
    void migrateToWallet();

private Q_SLOTS:

    // Used by our friend, TransportManager
    void readPassword();

private:
    void readTransportPasswordFinished(QKeychain::Job *baseJob);
    void loadPassword();
    TransportPrivate *const d;
};
} // namespace MailTransport

