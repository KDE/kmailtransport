/*
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"
#include "transportbase.h"
#include "transporttype.h"

#include <memory>

class TransportPrivate;
namespace QKeychain
{
class Job;
}
namespace MailTransport
{
class TransportType;

/*!
  Represents the settings of a specific mail transport.

  To create a new empty Transport object, use TransportManager::createTransport().

  Initialize an empty Transport object by calling the set...() methods defined in
  kcfg-generated TransportBase, and in this class.
*/
class MAILTRANSPORT_EXPORT Transport : public TransportBase
{
    Q_OBJECT

    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(TransportType transportType READ transportType NOTIFY transportTypeChanged)

    friend class TransportManager;
    friend class TransportManagerPrivate;

public:
    /*!
      Destructor
    */
    ~Transport() override;

    using List = QList<Transport *>;
    using Id = int;

    /*!
      Returns true if this transport is valid, ie. has all necessary data set.
    */
    [[nodiscard]] Q_INVOKABLE bool isValid() const;

    /*!
      Returns the password of this transport.
    */
    [[nodiscard]] QString password() const;

    /*!
      Sets the password of this transport.
      \a passwd The new password.
    */
    void setPassword(const QString &passwd);

    /*!
      Makes sure the transport has a unique name.  Adds #1, #2, #3 etc. if
      necessary.
      \since 4.4
    */
    void forceUniqueName();

    /*!
      This function synchronizes the password of this transport with the
      password of the transport with the same ID that is managed by the
      transport manager. This is only useful for cloned transports, since
      their passwords don't automatically get updated when calling
      TransportManager::loadPasswordsAsync() or TransportManager::loadPasswords().

      @sa clone()
      \since 4.2
    */
    void updatePasswordState();

    /*!
      Returns true if all settings have been loaded.
      This is the way to find out if the password has already been loaded
      from the wallet.
    */
    [[nodiscard]] bool isComplete() const;

    /*!
      Returns a string representation of the authentication type.
    */
    [[nodiscard]] QString authenticationTypeString() const;

    /*!
      Returns a string representation of the authentication type.
      Convenience function when there isn't a Transport object
      instantiated.

      \since 4.5
    */
    [[nodiscard]] static QString authenticationTypeString(int type);

    /*!
      Returns a deep copy of this Transport object which will no longer be
      automatically updated. Use this if you need to store a Transport object
      over a longer time. However it is recommended to store transport identifiers
      instead if possible.

      @sa updatePasswordState()
    */
    Transport *clone() const;

    /*!
      Returns the type of this transport.
      \sa TransportType.
      \since 4.4
    */
    [[nodiscard]] TransportType transportType() const;

protected:
    /*!
      Creates a Transport object. Should only be used by TransportManager.
      \a cfgGroup The KConfig group to read its data from.
    */
    explicit Transport(const QString &cfgGroup);

    void usrRead() override;
    bool usrSave() override;

    /*!
      Returns true if the password was not stored in the wallet.
    */
    [[nodiscard]] bool needsWalletMigration() const;

    /*!
      Try to migrate the password from the config file to the wallet.
    */
    void migrateToWallet();

private Q_SLOTS:

    // Used by our friend, TransportManager
    void readPassword();

Q_SIGNALS:
    /*!
      Emitted when passwords have been loaded from QKeyChain.
    */
    void passwordLoaded();
    /*!
     * Emitted when the password is changed
     */
    void passwordChanged();
    /*!
     * Emitted when the transport type is changed
     */
    void transportTypeChanged();

private:
    MAILTRANSPORT_NO_EXPORT void readTransportPasswordFinished(QKeychain::Job *baseJob);
    MAILTRANSPORT_NO_EXPORT void loadPassword();

private:
    std::unique_ptr<TransportPrivate> const d;
};
} // namespace MailTransport
