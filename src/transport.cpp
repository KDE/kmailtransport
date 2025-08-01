/*
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transport.h"
using namespace Qt::Literals::StringLiterals;

#include "mailtransport_defs.h"
#include "transport_p.h"
#include "transportmanager.h"
#include "transporttype_p.h"

#include "mailtransport_debug.h"
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStringHandler>

#include <qt6keychain/keychain.h>
using namespace QKeychain;
using namespace MailTransport;

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

Transport::~Transport() = default;

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

QString Transport::password() const
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
    Q_EMIT passwordChanged();
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
        Q_EMIT passwordChanged();
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
        return u"LOGIN"_s;
    case EnumAuthenticationType::PLAIN:
        return u"PLAIN"_s;
    case EnumAuthenticationType::CRAM_MD5:
        return u"CRAM-MD5"_s;
    case EnumAuthenticationType::DIGEST_MD5:
        return u"DIGEST-MD5"_s;
    case EnumAuthenticationType::NTLM:
        return u"NTLM"_s;
    case EnumAuthenticationType::GSSAPI:
        return u"GSSAPI"_s;
    case EnumAuthenticationType::CLEAR:
        return i18nc("Authentication method", "Clear text");
    case EnumAuthenticationType::APOP:
        return u"APOP"_s;
    case EnumAuthenticationType::ANONYMOUS:
        return i18nc("Authentication method", "Anonymous");
    case EnumAuthenticationType::XOAUTH2:
        return u"XOAUTH2"_s;
    }
    Q_ASSERT(false);
    return {};
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
        Q_EMIT transportTypeChanged();
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
        if (QKeychain::isAvailable()) {
            // TODO: Needs to replaced with a check, if a backend is available.
            //  2022-10-12: QtKeyChain has no method to request, if there is any backend.
            //  see https://github.com/frankosterfeld/qtkeychain/issues/224
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
        auto writeJob = new WritePasswordJob(WALLET_FOLDER, this);
        connect(writeJob, &Job::finished, this, [this, writeJob, storePassword] {
            if (writeJob->error()) {
                qWarning(MAILTRANSPORT_LOG()) << "WritePasswordJob failed with: " << writeJob->errorString();
                // wallet saving failed, ask if we should store in the config file instead
                if (d->storePasswordInFile
                    || KMessageBox::warningTwoActions(nullptr,
                                                      i18n("QKeychain not found a backend for storing your password. "
                                                           "It is strongly recommended to use strong backend for managing your passwords.\n"
                                                           "However, the password can be stored in the configuration "
                                                           "file instead. The password is stored in an obfuscated format, "
                                                           "but should not be considered secure from decryption efforts "
                                                           "if access to the configuration file is obtained.\n"
                                                           "Do you want to store the password for server '%1' in the "
                                                           "configuration file?",
                                                           name()),
                                                      i18nc("@title:window", "KWallet Not Available"),
                                                      KGuiItem(i18nc("@action:button", "Store Password")),
                                                      KGuiItem(i18nc("@action:button", "Do Not Store Password")))
                        == KMessageBox::ButtonCode::PrimaryAction) {
                    // write to config file
                    KConfigGroup group(config(), currentGroup());
                    group.writeEntry("password", KStringHandler::obscure(storePassword));
                    d->storePasswordInFile = true;
                }
            }
        });

        writeJob->setKey(QString::number(id()));
        writeJob->setTextData(storePassword);
        QEventLoop loop;
        connect(writeJob, &Job::finished, &loop, &QEventLoop::quit);
        writeJob->start();
        loop.exec();
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
        qCWarning(MAILTRANSPORT_LOG) << "We have an error during reading password for" << id() << job->errorString();
        Q_EMIT passwordChanged();
    } else {
        setPassword(job->textData());
    }
    Q_EMIT passwordLoaded();
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

#include "moc_transport.cpp"
