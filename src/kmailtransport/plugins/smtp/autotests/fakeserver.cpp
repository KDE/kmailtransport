/*
  SPDX-FileCopyrightText: 2010 BetterInbox <contact@betterinbox.com>
  SPDX-FileContributor: Christophe Laveault <christophe@betterinbox.com>
  SPDX-FileContributor: Gregory Schlomoff <gregory.schlomoff@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "fakeserver.h"

#include <QDebug>
#include <QFile>
#include <QTcpServer>
#include <QTest>

FakeServer::FakeServer(QObject *parent)
    : QThread(parent)
    , m_tcpServer(nullptr)
{
    moveToThread(this);
}

QByteArray FakeServer::greeting()
{
    return "S: 220 localhost ESMTP xx777xx";
}

QList<QByteArray> FakeServer::greetingAndEhlo(bool multiline)
{
    return QList<QByteArray>() << greeting() << "C: EHLO 127.0.0.1" << QByteArray("S: 250") + (multiline ? '-' : ' ') + "Localhost ready to roll";
}

QList<QByteArray> FakeServer::bye()
{
    return {"C: QUIT", "S: 221 So long, and thanks for all the fish", "X: "};
}

FakeServer::~FakeServer()
{
    quit();
    wait();
}

void FakeServer::startAndWait()
{
    start();
    // this will block until the event queue starts
    QMetaObject::invokeMethod(this, &FakeServer::started, Qt::BlockingQueuedConnection);
}

void FakeServer::dataAvailable()
{
    QMutexLocker locker(&m_mutex);

    auto socket = qobject_cast<QTcpSocket *>(sender());
    Q_ASSERT(socket != nullptr);

    int scenarioNumber = m_clientSockets.indexOf(socket);

    QVERIFY(!m_scenarios[scenarioNumber].isEmpty());

    readClientPart(scenarioNumber);
    writeServerPart(scenarioNumber);
}

void FakeServer::newConnection()
{
    QMutexLocker locker(&m_mutex);

    m_clientSockets << m_tcpServer->nextPendingConnection();
    connect(m_clientSockets.last(), &QIODevice::readyRead, this, &FakeServer::dataAvailable);
    // m_clientParsers << new KIMAP::ImapStreamParser( m_clientSockets.last(), true );

    QVERIFY(m_clientSockets.size() <= m_scenarios.size());

    writeServerPart(m_clientSockets.size() - 1);
}

void FakeServer::run()
{
    m_tcpServer = new QTcpServer();
    if (!m_tcpServer->listen(QHostAddress(QHostAddress::LocalHost), 5989)) {
        qFatal("Unable to start the server");
        return;
    }

    connect(m_tcpServer, &QTcpServer::newConnection, this, &FakeServer::newConnection);

    exec();

    qDeleteAll(m_clientSockets);

    delete m_tcpServer;
}

void FakeServer::started()
{
    // do nothing: this is a dummy slot used by startAndWait()
}

void FakeServer::setScenario(const QList<QByteArray> &scenario)
{
    QMutexLocker locker(&m_mutex);

    m_scenarios.clear();
    m_scenarios << scenario;
}

void FakeServer::addScenario(const QList<QByteArray> &scenario)
{
    QMutexLocker locker(&m_mutex);

    m_scenarios << scenario;
}

void FakeServer::addScenarioFromFile(const QString &fileName)
{
    QFile file(fileName);
    file.open(QFile::ReadOnly);

    QList<QByteArray> scenario;

    // When loading from files we never have the authentication phase
    // force jumping directly to authenticated state.
    // scenario << preauth();

    while (!file.atEnd()) {
        scenario << file.readLine().trimmed();
    }

    file.close();

    addScenario(scenario);
}

bool FakeServer::isScenarioDone(int scenarioNumber) const
{
    QMutexLocker locker(&m_mutex);

    if (scenarioNumber < m_scenarios.size()) {
        return m_scenarios[scenarioNumber].isEmpty();
    } else {
        return true; // Non existent hence empty, right?
    }
}

bool FakeServer::isAllScenarioDone() const
{
    QMutexLocker locker(&m_mutex);

    for (const auto &scenario : std::as_const(m_scenarios)) {
        if (!scenario.isEmpty()) {
            qDebug() << scenario;
            return false;
        }
    }

    return true;
}

void FakeServer::writeServerPart(int scenarioNumber)
{
    QList<QByteArray> scenario = m_scenarios[scenarioNumber];
    QTcpSocket *clientSocket = m_clientSockets[scenarioNumber];

    while (!scenario.isEmpty() && (scenario.first().startsWith("S: ") || scenario.first().startsWith("W: "))) {
        QByteArray rule = scenario.takeFirst();

        if (rule.startsWith("S: ")) {
            QByteArray payload = rule.mid(3);
            clientSocket->write(payload + "\r\n");
        } else {
            int timeout = rule.mid(3).toInt();
            QTest::qWait(timeout);
        }
    }

    if (!scenario.isEmpty() && scenario.first().startsWith('X')) {
        scenario.takeFirst();
        clientSocket->close();
    }

    if (!scenario.isEmpty()) {
        QVERIFY(scenario.first().startsWith("C: "));
    }

    m_scenarios[scenarioNumber] = scenario;
}

void FakeServer::readClientPart(int scenarioNumber)
{
    QList<QByteArray> scenario = m_scenarios[scenarioNumber];
    QTcpSocket *clientSocket = m_clientSockets[scenarioNumber];

    while (!scenario.isEmpty() && scenario.first().startsWith("C: ")) {
        QByteArray line = clientSocket->readLine();
        QByteArray received = "C: " + line.trimmed();
        QByteArray expected = scenario.takeFirst();

        if (expected == "C: SKIP" && !scenario.isEmpty()) {
            expected = scenario.takeFirst();
            while (received != expected) {
                received = "C: " + clientSocket->readLine().trimmed();
            }
        }

        QCOMPARE(QString::fromUtf8(received), QString::fromUtf8(expected));
        QCOMPARE(received, expected);
    }

    if (!scenario.isEmpty()) {
        QVERIFY(scenario.first().startsWith("S: "));
    }

    m_scenarios[scenarioNumber] = scenario;
}
