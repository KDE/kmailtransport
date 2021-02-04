/*
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transport.h"
#include "mailtransport_defs.h"
#include "transport_p.h"
#include "transportmanager.h"
#include "transporttype_p.h"

#include "mailtransport_debug.h"
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStringHandler>
#include <KWallet/KWallet>
#include <qt5keychain/keychain.h>
using namespace QKeychain;
using namespace MailTransport;
using namespace KWallet;

Transport::Transport(const QString &cfgGroup)
    : TransportBase(cfgGroup)
    , d(new TransportPrivate)
{
    qCDebug(MAILTRANSPORT_LOG) << cfgGroup;
    d->passwordLoaded = false;
    d->passwordDirty = false;
    d->storePasswordInFile = false;
    d->needsWalletMigration = false;
    load();
    loadPassword();
}

Transport::~Transport()
{
    delete d;
}

bool Transport::isValid() const
{
    return (id() > 0) && !host().isEmpty() && port() <= 65536;
}

void Transport::loadPassword()
{
    if (!d->passwordLoaded && requiresAuthentication() && storePassword() && d->password.isEmpty()) {
        readPassword();
    }
}

QString Transport::password()
{
    return d->password;
}

void Transport::setPassword(const QString &passwd)
{
    d->passwordLoaded = true;
    if (d->password == passwd) {
        return;
    }
    d->passwordDirty = true;
    d->password = passwd;
}

void Transport::forceUniqueName()
{
    QStringList existingNames;
    const auto lstTransports = TransportManager::self()->transports();
    for (Transport *t : lstTransports) {
        if (t->id() != id()) {
            existingNames << t->name();
        }
    }
    int suffix = 1;
    QString origName = name();
    while (existingNames.contains(name())) {
        setName(
            i18nc("%1: name; %2: number appended to it to make "
                  "it unique among a list of names",
                  "%1 #%2",
                  origName,
                  suffix));
        ++suffix;
    }
}

void Transport::updatePasswordState()
{
    Transport *original = TransportManager::self()->transportById(id(), false);
    if (original == this) {
        qCWarning(MAILTRANSPORT_LOG) << "Tried to update password state of non-cloned transport.";
        return;
    }
    if (original) {
        d->password = original->d->password;
        d->passwordLoaded = original->d->passwordLoaded;
        d->passwordDirty = original->d->passwordDirty;
    } else {
        qCWarning(MAILTRANSPORT_LOG) << "Transport with this ID not managed by transport manager.";
    }
}

bool Transport::isComplete() const
{
    return !requiresAuthentication() || !storePassword() || d->passwordLoaded;
}

QString Transport::authenticationTypeString() const
{
    return Transport::authenticationTypeString(authenticationType());
}

QString Transport::authenticationTypeString(int type)
{
    switch (type) {
    case EnumAuthenticationType::LOGIN:
        return QStringLiteral("LOGIN");
    case EnumAuthenticationType::PLAIN:
        return QStringLiteral("PLAIN");
    case EnumAuthenticationType::CRAM_MD5:
        return QStringLiteral("CRAM-MD5");
    case EnumAuthenticationType::DIGEST_MD5:
        return QStringLiteral("DIGEST-MD5");
    case EnumAuthenticationType::NTLM:
        return QStringLiteral("NTLM");
    case EnumAuthenticationType::GSSAPI:
        return QStringLiteral("GSSAPI");
    case EnumAuthenticationType::CLEAR:
        return i18nc("Authentication method", "Clear text");
    case EnumAuthenticationType::APOP:
        return QStringLiteral("APOP");
    case EnumAuthenticationType::ANONYMOUS:
        return i18nc("Authentication method", "Anonymous");
    case EnumAuthenticationType::XOAUTH2:
        return QStringLiteral("XOAUTH2");
    }
    Q_ASSERT(false);
    return QString();
}

void Transport::usrRead()
{
    TransportBase::usrRead();

    setHost(host().trimmed());

    if (d->oldName.isEmpty()) {
        d->oldName = name();
    }

    // Set TransportType.
    {
        d->transportType = TransportType();
        d->transportType.d->mIdentifier = identifier();
        // qCDebug(MAILTRANSPORT_LOG) << "type" << identifier();
        // Now we have the type and possibly agentType.  Get the name, description
        // etc. from TransportManager.
        const TransportType::List &types = TransportManager::self()->types();
        int index = types.indexOf(d->transportType);
        if (index != -1) {
            d->transportType = types[index];
        } else {
            qCWarning(MAILTRANSPORT_LOG) << "Type unknown to manager.";
            d->transportType.d->mName = i18nc("An unknown transport type", "Unknown");
        }
    }

    // we have everything we need
    if (!storePassword()) {
        return;
    }

    if (d->passwordLoaded) {
        return;
    }

    // try to find a password in the config file otherwise
    KConfigGroup group(config(), currentGroup());
    if (group.hasKey("password")) {
        d->password = KStringHandler::obscure(group.readEntry("password"));
    }

    if (!d->password.isEmpty()) {
        d->passwordLoaded = true;
        if (Wallet::isEnabled()) {
            d->needsWalletMigration = true;
        } else {
            d->storePasswordInFile = true;
        }
    }
}

bool Transport::usrSave()
{
    if (requiresAuthentication() && storePassword() && d->passwordDirty) {
        const QString storePassword = d->password;
        Wallet *wallet = TransportManager::self()->wallet();
        if (!wallet || wallet->writePassword(QString::number(id()), d->password) != 0) {
            // wallet saving failed, ask if we should store in the config file instead
            if (d->storePasswordInFile
                || KMessageBox::warningYesNo(nullptr,
                                             i18n("KWallet is not available. It is strongly recommended to use "
                                                  "KWallet for managing your passwords.\n"
                                                  "However, the password can be stored in the configuration "
                                                  "file instead. The password is stored in an obfuscated format, "
                                                  "but should not be considered secure from decryption efforts "
                                                  "if access to the configuration file is obtained.\n"
                                                  "Do you want to store the password for server '%1' in the "
                                                  "configuration file?",
                                                  name()),
                                             i18n("KWallet Not Available"),
                                             KGuiItem(i18n("Store Password")),
                                             KGuiItem(i18n("Do Not Store Password")))
                    == KMessageBox::Yes) {
                // write to config file
                KConfigGroup group(config(), currentGroup());
                group.writeEntry("password", KStringHandler::obscure(storePassword));
                d->storePasswordInFile = true;
            }
        }
        d->passwordDirty = false;
    }

    if (!TransportBase::usrSave()) {
        return false;
    }
    TransportManager::self()->emitChangesCommitted();
    if (name() != d->oldName) {
        Q_EMIT TransportManager::self()->transportRenamed(id(), d->oldName, name());
        d->oldName = name();
    }

    return true;
}

void Transport::readPassword()
{
    // no need to load a password if the account doesn't require auth
    if (!requiresAuthentication()) {
        return;
    }
    d->passwordLoaded = true;

    auto readJob = new ReadPasswordJob(WALLET_FOLDER, this);
    connect(readJob, &Job::finished, this, &Transport::readTransportPasswordFinished);
    readJob->setKey(QString::number(id()));
    readJob->start();
}

void Transport::readTransportPasswordFinished(QKeychain::Job *baseJob)
{
    auto job = qobject_cast<ReadPasswordJob *>(baseJob);
    Q_ASSERT(job);
    if (job->error()) {
        d->password.clear();
        d->passwordLoaded = false;
        qWarning() << "We have an error during reading password " << job->errorString();
    } else {
        setPassword(job->textData());
    }
}

bool Transport::needsWalletMigration() const
{
    return d->needsWalletMigration;
}

void Transport::migrateToWallet()
{
    qCDebug(MAILTRANSPORT_LOG) << "migrating" << id() << "to wallet";
    d->needsWalletMigration = false;
    KConfigGroup group(config(), currentGroup());
    group.deleteEntry("password");
    d->passwordDirty = true;
    d->storePasswordInFile = false;
    save();
}

Transport *Transport::clone() const
{
    const QString id = currentGroup().mid(10);
    return new Transport(id);
}

TransportType Transport::transportType() const
{
    if (!d->transportType.isValid()) {
        qCWarning(MAILTRANSPORT_LOG) << "Invalid transport type.";
    }
    return d->transportType;
}
