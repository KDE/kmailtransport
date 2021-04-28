/*
  SPDX-FileCopyrightText: 2010 BetterInbox <contact@betterinbox.com>
  SPDX-FileContributor: Christophe Laveault <christophe@betterinbox.com>
  SPDX-FileContributor: Gregory Schlomoff <gregory.schlomoff@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#pragma once

#include <QMutex>
#include <QTcpSocket>
#include <QThread>

Q_DECLARE_METATYPE(QList<QByteArray>)
class QTcpServer;
class FakeServer : public QThread
{
    Q_OBJECT

public:
    explicit FakeServer(QObject *parent = nullptr);
    ~FakeServer() override;

    void startAndWait();
    void run() override;

    static QByteArray greeting();
    static QList<QByteArray> greetingAndEhlo(bool multiline = true);
    static QList<QByteArray> bye();

    void setScenario(const QList<QByteArray> &scenario);
    void addScenario(const QList<QByteArray> &scenario);
    void addScenarioFromFile(const QString &fileName);
    bool isScenarioDone(int scenarioNumber) const;
    bool isAllScenarioDone() const;

private Q_SLOTS:
    void newConnection();
    void dataAvailable();
    void started();

private:
    void writeServerPart(int scenarioNumber);
    void readClientPart(int scenarioNumber);

    QList<QList<QByteArray>> m_scenarios;
    QTcpServer *m_tcpServer = nullptr;
    mutable QMutex m_mutex;
    QList<QTcpSocket *> m_clientSockets;
};

