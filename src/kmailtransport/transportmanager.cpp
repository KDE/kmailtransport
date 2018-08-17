/*
  Copyright (c) 2006 - 2007 Volker Krause <vkrause@kde.org>

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

#include "transportmanager.h"
#include "mailtransport_defs.h"
#include "transport.h"
#include "transport_p.h"
#include "transportjob.h"
#include "transporttype.h"
#include "transporttype_p.h"
#include "plugins/transportpluginmanager.h"
#include "plugins/transportabstractplugin.h"
#include "widgets/addtransportdialogng.h"
#include <MailTransport/TransportAbstractPlugin>

#include <QApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>
#include <QPointer>
#include <QRegularExpression>
#include <QStringList>

#include <KConfig>
#include <KConfigGroup>
#include "mailtransport_debug.h"
#include <KEMailSettings>
#include <KLocalizedString>
#include <KMessageBox>
#include <KRandom>
#include <Kdelibs4ConfigMigrator>

#include <KWallet>

using namespace MailTransport;
using namespace KWallet;

namespace MailTransport {
/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
class TransportManagerPrivate
{
public:
    TransportManagerPrivate(TransportManager *parent)
        : config(nullptr)
        , wallet(nullptr)
        , q(parent)
    {
    }

    ~TransportManagerPrivate()
    {
        delete config;
        qDeleteAll(transports);
    }

    KConfig *config = nullptr;
    QList<Transport *> transports;
    TransportType::List types;
    bool myOwnChange;
    bool appliedChange;
    KWallet::Wallet *wallet = nullptr;
    bool walletOpenFailed;
    bool walletAsyncOpen;
    int defaultTransportId;
    bool isMainInstance;
    QList<TransportJob *> walletQueue;
    TransportManager *q = nullptr;

    void readConfig();
    void writeConfig();
    void fillTypes();
    int createId() const;
    void prepareWallet();
    void validateDefault();
    void migrateToWallet();
    void updatePluginList();

    // Slots
    void slotTransportsChanged();
    void slotWalletOpened(bool success);
    void dbusServiceUnregistered();
    void jobResult(KJob *job);
};
}

class StaticTransportManager : public TransportManager
{
public:
    StaticTransportManager() : TransportManager()
    {
    }
};

StaticTransportManager *sSelf = nullptr;

static void destroyStaticTransportManager()
{
    delete sSelf;
}

TransportManager::TransportManager()
    : QObject()
    , d(new TransportManagerPrivate(this))
{
    Kdelibs4ConfigMigrator migrate(QStringLiteral("transportmanager"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("mailtransports"));
    migrate.migrate();

    qAddPostRoutine(destroyStaticTransportManager);
    d->myOwnChange = false;
    d->appliedChange = false;
    d->wallet = nullptr;
    d->walletOpenFailed = false;
    d->walletAsyncOpen = false;
    d->defaultTransportId = -1;
    d->config = new KConfig(QStringLiteral("mailtransports"));

    QDBusConnection::sessionBus().registerObject(DBUS_OBJECT_PATH, this,
                                                 QDBusConnection::ExportScriptableSlots
                                                 |QDBusConnection::ExportScriptableSignals);

    QDBusServiceWatcher *watcher
        = new QDBusServiceWatcher(DBUS_SERVICE_NAME, QDBusConnection::sessionBus(),
                                  QDBusServiceWatcher::WatchForUnregistration, this);
    connect(watcher, &QDBusServiceWatcher::serviceUnregistered, this, [this]() {
        d->dbusServiceUnregistered();
    });

    QDBusConnection::sessionBus().connect(QString(), QString(),
                                          DBUS_INTERFACE_NAME, DBUS_CHANGE_SIGNAL,
                                          this, SLOT(slotTransportsChanged()));

    d->isMainInstance = QDBusConnection::sessionBus().registerService(DBUS_SERVICE_NAME);

    d->fillTypes();
}

TransportManager::~TransportManager()
{
    qRemovePostRoutine(destroyStaticTransportManager);
    delete d;
}

TransportManager *TransportManager::self()
{
    if (!sSelf) {
        sSelf = new StaticTransportManager;
        sSelf->d->readConfig();
    }
    return sSelf;
}

Transport *TransportManager::transportById(int id, bool def) const
{
    for (Transport *t : qAsConst(d->transports)) {
        if (t->id() == id) {
            return t;
        }
    }

    if (def || (id == 0 && d->defaultTransportId != id)) {
        return transportById(d->defaultTransportId, false);
    }
    return nullptr;
}

Transport *TransportManager::transportByName(const QString &name, bool def) const
{
    for (Transport *t : qAsConst(d->transports)) {
        if (t->name() == name) {
            return t;
        }
    }
    if (def) {
        return transportById(0, false);
    }
    return nullptr;
}

QList< Transport * > TransportManager::transports() const
{
    return d->transports;
}

TransportType::List TransportManager::types() const
{
    return d->types;
}

Transport *TransportManager::createTransport() const
{
    int id = d->createId();
    Transport *t = new Transport(QString::number(id));
    t->setId(id);
    return t;
}

void TransportManager::addTransport(Transport *transport)
{
    if (d->transports.contains(transport)) {
        qCDebug(MAILTRANSPORT_LOG) << "Already have this transport.";
        return;
    }

    qCDebug(MAILTRANSPORT_LOG) << "Added transport" << transport;
    d->transports.append(transport);
    d->validateDefault();
    emitChangesCommitted();
}

void TransportManager::schedule(TransportJob *job)
{
    connect(job, &TransportJob::result, this, [this](KJob *job) {
        d->jobResult(job);
    });

    // check if the job is waiting for the wallet
    if (!job->transport()->isComplete()) {
        qCDebug(MAILTRANSPORT_LOG) << "job waits for wallet:" << job;
        d->walletQueue << job;
        loadPasswordsAsync();
        return;
    }

    job->start();
}

void TransportManager::createDefaultTransport()
{
    KEMailSettings kes;
    Transport *t = createTransport();
    t->setName(i18n("Default Transport"));
    t->setHost(kes.getSetting(KEMailSettings::OutServer));
    if (t->isValid()) {
        t->save();
        addTransport(t);
    } else {
        qCWarning(MAILTRANSPORT_LOG) << "KEMailSettings does not contain a valid transport.";
    }
}

bool TransportManager::showTransportCreationDialog(QWidget *parent, ShowCondition showCondition)
{
    if (showCondition == IfNoTransportExists) {
        if (!isEmpty()) {
            return true;
        }

        const int response = KMessageBox::messageBox(parent,
                                                     KMessageBox::WarningContinueCancel,
                                                     i18n("You must create an outgoing account before sending."),
                                                     i18n("Create Account Now?"),
                                                     KGuiItem(i18n("Create Account Now")));
        if (response != KMessageBox::Continue) {
            return false;
        }
    }

    QPointer<AddTransportDialogNG> dialog = new AddTransportDialogNG(parent);
    const bool accepted = (dialog->exec() == QDialog::Accepted);
    delete dialog;
    return accepted;
}

void TransportManager::initializeTransport(const QString &identifier, Transport *transport)
{
    TransportAbstractPlugin *plugin = TransportPluginManager::self()->plugin(identifier);
    if (plugin) {
        plugin->initializeTransport(transport, identifier);
    }
}

bool TransportManager::configureTransport(const QString &identifier, Transport *transport, QWidget *parent)
{
    TransportAbstractPlugin *plugin = TransportPluginManager::self()->plugin(identifier);
    if (plugin) {
        return plugin->configureTransport(identifier, transport, parent);
    }
    return false;
}

TransportJob *TransportManager::createTransportJob(int transportId)
{
    Transport *t = transportById(transportId, false);
    if (!t) {
        return nullptr;
    }
    t = t->clone(); // Jobs delete their transports.
    t->updatePasswordState();
    TransportAbstractPlugin *plugin = TransportPluginManager::self()->plugin(t->identifier());
    if (plugin) {
        return plugin->createTransportJob(t, t->identifier());
    }
    Q_ASSERT(false);
    return nullptr;
}

TransportJob *TransportManager::createTransportJob(const QString &transport)
{
    bool ok = false;
    Transport *t = nullptr;

    int transportId = transport.toInt(&ok);
    if (ok) {
        t = transportById(transportId);
    }

    if (!t) {
        t = transportByName(transport, false);
    }

    if (t) {
        return createTransportJob(t->id());
    }

    return nullptr;
}

bool TransportManager::isEmpty() const
{
    return d->transports.isEmpty();
}

QVector<int> TransportManager::transportIds() const
{
    QVector<int> rv;
    rv.reserve(d->transports.count());
    for (Transport *t : qAsConst(d->transports)) {
        rv << t->id();
    }
    return rv;
}

QStringList TransportManager::transportNames() const
{
    QStringList rv;
    rv.reserve(d->transports.count());
    for (Transport *t : qAsConst(d->transports)) {
        rv << t->name();
    }
    return rv;
}

QString TransportManager::defaultTransportName() const
{
    Transport *t = transportById(d->defaultTransportId, false);
    if (t) {
        return t->name();
    }
    return QString();
}

int TransportManager::defaultTransportId() const
{
    return d->defaultTransportId;
}

void TransportManager::setDefaultTransport(int id)
{
    if (id == d->defaultTransportId || !transportById(id, false)) {
        return;
    }
    d->defaultTransportId = id;
    d->writeConfig();
}

void TransportManager::removeTransport(int id)
{
    Transport *t = transportById(id, false);
    if (!t) {
        return;
    }
    auto plugin = MailTransport::TransportPluginManager::self()->plugin(t->identifier());
    if (plugin) {
        plugin->cleanUp(t);
    }
    emit transportRemoved(t->id(), t->name());

    d->transports.removeAll(t);
    d->validateDefault();
    QString group = t->currentGroup();
    if (t->storePassword()) {
        Wallet *currentWallet = wallet();
        if (currentWallet) {
            currentWallet->removeEntry(QString::number(t->id()));
        }
    }
    delete t;
    d->config->deleteGroup(group);
    d->writeConfig();
}

void TransportManagerPrivate::readConfig()
{
    QList<Transport *> oldTransports = transports;
    transports.clear();

    QRegularExpression re(QStringLiteral("^Transport (.+)$"));
    const QStringList groups = config->groupList().filter(re);
    for (const QString &s : groups) {
        QRegularExpressionMatch match = re.match(s);
        if (!match.hasMatch()) {
            continue;
        }
        Transport *t = nullptr;
        // see if we happen to have that one already
        foreach (Transport *old, oldTransports) {
            if (old->currentGroup() == QLatin1String("Transport ") + match.captured(1)) {
                qCDebug(MAILTRANSPORT_LOG) << "reloading existing transport:" << s;
                t = old;
                t->d->passwordNeedsUpdateFromWallet = true;
                t->load();
                oldTransports.removeAll(old);
                break;
            }
        }

        if (!t) {
            t = new Transport(match.captured(1));
        }
        if (t->id() <= 0) {
            t->setId(createId());
            t->save();
        }
        transports.append(t);
    }

    qDeleteAll(oldTransports);
    oldTransports.clear();

    // read default transport
    KConfigGroup group(config, "General");
    defaultTransportId = group.readEntry("default-transport", 0);
    if (defaultTransportId == 0) {
        // migrated default transport contains the name instead
        QString name = group.readEntry("default-transport", QString());
        if (!name.isEmpty()) {
            Transport *t = q->transportByName(name, false);
            if (t) {
                defaultTransportId = t->id();
                writeConfig();
            }
        }
    }
    validateDefault();
    migrateToWallet();
    q->loadPasswordsAsync();
}

void TransportManagerPrivate::writeConfig()
{
    KConfigGroup group(config, "General");
    group.writeEntry("default-transport", defaultTransportId);
    config->sync();
    q->emitChangesCommitted();
}

void TransportManagerPrivate::fillTypes()
{
    Q_ASSERT(types.isEmpty());

    updatePluginList();
    QObject::connect(MailTransport::TransportPluginManager::self(), &TransportPluginManager::updatePluginList, q, &TransportManager::updatePluginList);
}

void TransportManagerPrivate::updatePluginList()
{
    types.clear();
    for (MailTransport::TransportAbstractPlugin *plugin : MailTransport::TransportPluginManager::self()->pluginsList()) {
        if (plugin->names().isEmpty()) {
            qCDebug(MAILTRANSPORT_LOG) << "Plugin " << plugin << " doesn't provide plugin";
        }
        for (const MailTransport::TransportAbstractPluginInfo &info : plugin->names()) {
            TransportType type;
            type.d->mName = info.name;
            type.d->mDescription = info.description;
            type.d->mIdentifier = info.identifier;
            type.d->mIsAkonadiResource = info.isAkonadi;
            types << type;
        }
    }
}

void TransportManager::updatePluginList()
{
    d->updatePluginList();
}

void TransportManager::emitChangesCommitted()
{
    d->myOwnChange = true; // prevent us from reading our changes again
    d->appliedChange = false; // but we have to read them at least once
    emit transportsChanged();
    emit changesCommitted();
}

void TransportManagerPrivate::slotTransportsChanged()
{
    if (myOwnChange && appliedChange) {
        myOwnChange = false;
        appliedChange = false;
        return;
    }

    qCDebug(MAILTRANSPORT_LOG);
    config->reparseConfiguration();
    // FIXME: this deletes existing transport objects!
    readConfig();
    appliedChange = true; // to prevent recursion
    emit q->transportsChanged();
}

int TransportManagerPrivate::createId() const
{
    QVector<int> usedIds;
    usedIds.reserve(1 + transports.count());
    for (Transport *t : qAsConst(transports)) {
        usedIds << t->id();
    }
    usedIds << 0; // 0 is default for unknown
    int newId;
    do {
        newId = KRandom::random();
    } while (usedIds.contains(newId));
    return newId;
}

KWallet::Wallet *TransportManager::wallet()
{
    if (d->wallet && d->wallet->isOpen()) {
        return d->wallet;
    }

    if (!Wallet::isEnabled() || d->walletOpenFailed) {
        return nullptr;
    }

    WId window = 0;
    if (qApp->activeWindow()) {
        window = qApp->activeWindow()->winId();
    } else if (!QApplication::topLevelWidgets().isEmpty()) {
        window = qApp->topLevelWidgets().first()->winId();
    }

    delete d->wallet;
    d->wallet = Wallet::openWallet(Wallet::NetworkWallet(), window);

    if (!d->wallet) {
        d->walletOpenFailed = true;
        return nullptr;
    }

    d->prepareWallet();
    return d->wallet;
}

void TransportManagerPrivate::prepareWallet()
{
    if (!wallet) {
        return;
    }
    if (!wallet->hasFolder(WALLET_FOLDER)) {
        wallet->createFolder(WALLET_FOLDER);
    }
    wallet->setFolder(WALLET_FOLDER);
}

void TransportManager::loadPasswords()
{
    for (Transport *t : qAsConst(d->transports)) {
        t->readPassword();
    }

    // flush the wallet queue
    const QList<TransportJob *> copy = d->walletQueue;
    d->walletQueue.clear();
    for (TransportJob *job : copy) {
        job->start();
    }

    emit passwordsChanged();
}

void TransportManager::loadPasswordsAsync()
{
    qCDebug(MAILTRANSPORT_LOG);

    // check if there is anything to do at all
    bool found = false;
    for (Transport *t : qAsConst(d->transports)) {
        if (!t->isComplete()) {
            found = true;
            break;
        }
    }
    if (!found) {
        return;
    }

    // async wallet opening
    if (!d->wallet && !d->walletOpenFailed) {
        WId window = 0;
        if (qApp->activeWindow()) {
            window = qApp->activeWindow()->winId();
        } else if (!QApplication::topLevelWidgets().isEmpty()) {
            window = qApp->topLevelWidgets().first()->winId();
        }

        d->wallet = Wallet::openWallet(Wallet::NetworkWallet(), window,
                                       Wallet::Asynchronous);
        if (d->wallet) {
            connect(d->wallet, &KWallet::Wallet::walletOpened, this, [this](bool status) { d->slotWalletOpened(status);});
            d->walletAsyncOpen = true;
        } else {
            d->walletOpenFailed = true;
            loadPasswords();
        }
        return;
    }
    if (d->wallet && !d->walletAsyncOpen) {
        loadPasswords();
    }
}

void TransportManagerPrivate::slotWalletOpened(bool success)
{
    qCDebug(MAILTRANSPORT_LOG);
    walletAsyncOpen = false;
    if (!success) {
        walletOpenFailed = true;
        delete wallet;
        wallet = nullptr;
    } else {
        prepareWallet();
    }
    q->loadPasswords();
}

void TransportManagerPrivate::validateDefault()
{
    if (!q->transportById(defaultTransportId, false)) {
        if (q->isEmpty()) {
            defaultTransportId = -1;
        } else {
            defaultTransportId = transports.first()->id();
            writeConfig();
        }
    }
}

void TransportManagerPrivate::migrateToWallet()
{
    // check if we tried this already
    static bool firstRun = true;
    if (!firstRun) {
        return;
    }
    firstRun = false;

    // check if we are the main instance
    if (!isMainInstance) {
        return;
    }

    // check if migration is needed
    QStringList names;
    for (Transport *t : qAsConst(transports)) {
        if (t->needsWalletMigration()) {
            names << t->name();
        }
    }
    if (names.isEmpty()) {
        return;
    }

    // ask user if he wants to migrate
    int result = KMessageBox::questionYesNoList(
        nullptr,
        i18n("The following mail transports store their passwords in an "
             "unencrypted configuration file.\nFor security reasons, "
             "please consider migrating these passwords to KWallet, the "
             "KDE Wallet management tool,\nwhich stores sensitive data "
             "for you in a strongly encrypted file.\n"
             "Do you want to migrate your passwords to KWallet?"),
        names, i18n("Question"),
        KGuiItem(i18n("Migrate")), KGuiItem(i18n("Keep")),
        QStringLiteral("WalletMigrate"));
    if (result != KMessageBox::Yes) {
        return;
    }

    // perform migration
    for (Transport *t : qAsConst(transports)) {
        if (t->needsWalletMigration()) {
            t->migrateToWallet();
        }
    }
}

void TransportManagerPrivate::dbusServiceUnregistered()
{
    QDBusConnection::sessionBus().registerService(DBUS_SERVICE_NAME);
}

void TransportManagerPrivate::jobResult(KJob *job)
{
    walletQueue.removeAll(static_cast<TransportJob *>(job));
}

#include "moc_transportmanager.cpp"
