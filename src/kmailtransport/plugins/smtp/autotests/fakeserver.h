/*
  Copyright 2010 BetterInbox <contact@betterinbox.com>
      Author: Christophe Laveault <christophe@betterinbox.com>
              Gregory Schlomoff <gregory.schlomoff@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KSMTP_FAKESERVER_H
#define KSMTP_FAKESERVER_H

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>
#include <QStringList>

Q_DECLARE_METATYPE(QList<QByteArray>)

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

    QList< QList<QByteArray> > m_scenarios;
    QTcpServer *m_tcpServer = nullptr;
    mutable QMutex m_mutex;
    QList<QTcpSocket *> m_clientSockets;
};

#endif // KSMTP_FAKESERVER_H
