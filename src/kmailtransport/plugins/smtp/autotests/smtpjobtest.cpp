/*
    SPDX-FileCopyrightText: 2017 Daniel Vrátil <dvratil@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "../smtpjob.h"
#include "fakeserver.h"

#include "transportbase.h"
#include "transportmanager.h"

#include <QObject>
#include <QStandardPaths>
#include <QTest>

Q_DECLARE_METATYPE(MailTransport::TransportBase::EnumAuthenticationType::type)

class SmtpJobTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
    }

    void smtpJobTest_data()
    {
        QTest::addColumn<QList<QByteArray>>("scenario");
        QTest::addColumn<MailTransport::TransportBase::EnumAuthenticationType::type>("authType");
        QTest::addColumn<QString>("from");
        QTest::addColumn<QStringList>("to");
        QTest::addColumn<QStringList>("cc");
        QTest::addColumn<QByteArray>("data");
        QTest::addColumn<bool>("success");

        QList<QByteArray> scenario;
        scenario << FakeServer::greetingAndEhlo() << "S: 250 AUTH PLAIN LOGIN"
                 << "C: AUTH LOGIN"
                 << "S: 334 VXNlcm5hbWU6"
                 << "C: bG9naW4=" // "login".toBase64()
                 << "S: 334 UGFzc3dvcmQ6"
                 << "C: cGFzc3dvcmQ=" // "password".toBase64()
                 << "S: 235 Authenticated."
                 << "C: MAIL FROM:<foo@bar.com>"
                 << "S: 250 ok"
                 << "C: RCPT TO:<bar@foo.com>"
                 << "S: 250 ok"
                 << "C: RCPT TO:<bar@bar.foo>"
                 << "S: 250 ok"
                 << "C: DATA"
                 << "S: 354 Ok go ahead"
                 << "C: Hi Bob"
                 << "C: "
                 << "C: ."
                 << "S: 250 Ok transfer done" << FakeServer::bye();
        QTest::newRow("simple") << scenario << MailTransport::TransportBase::EnumAuthenticationType::LOGIN << QStringLiteral("Foo Bar <foo@bar.com>")
                                << QStringList{} << QStringList{QStringLiteral("bar@foo.com"), QStringLiteral("<bar@bar.foo>")} << QByteArray("Hi Bob") << true;
    }

    void smtpJobTest()
    {
        QFETCH(QList<QByteArray>, scenario);
        QFETCH(MailTransport::TransportBase::EnumAuthenticationType::type, authType);
        QFETCH(QString, from);
        QFETCH(QStringList, to);
        QFETCH(QStringList, cc);
        QFETCH(QByteArray, data);
        QFETCH(bool, success);

        FakeServer server;
        server.setScenario(scenario);
        server.startAndWait();

        auto transport = MailTransport::TransportManager::self()->createTransport();
        transport->setSpecifyHostname(true);
        transport->setHost(QStringLiteral("127.0.0.1"));
        transport->setLocalHostname(QStringLiteral("127.0.0.1"));
        transport->setPort(5989);
        transport->setRequiresAuthentication(true);
        transport->setAuthenticationType(authType);
        transport->setStorePassword(false);
        transport->setUserName(QStringLiteral("login"));
        transport->setPassword(QStringLiteral("password"));

        {
            MailTransport::SmtpJob smtpJob(transport);
            smtpJob.setSender(from);
            smtpJob.setTo(to);
            smtpJob.setCc(cc);
            smtpJob.setData(data);

            QVERIFY(smtpJob.exec());
            if (success) {
                QCOMPARE(smtpJob.error(), 0);
            } else {
                QVERIFY(smtpJob.error() > 0);
            }

            // Make sure the smtpJob goes out-of-scope here and thus the
            // internal session pool is destroyed
        }
        // KSMTP time to stop the session
        QTest::qWait(10);

        QVERIFY(server.isAllScenarioDone());
        server.quit();
    }
};

QTEST_MAIN(SmtpJobTest)

#include "smtpjobtest.moc"
