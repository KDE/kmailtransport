/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attributetest.h"

#include <addressattribute.h>
#include <attributefactory.h>
#include <qtest_akonadi.h>

#include <kmailtransportakonadi/dispatchmodeattribute.h>
#include <kmailtransportakonadi/errorattribute.h>
#include <kmailtransportakonadi/sentactionattribute.h>
#include <kmailtransportakonadi/sentbehaviourattribute.h>
#include <kmailtransportakonadi/transportattribute.h>

using namespace Akonadi;
using namespace MailTransport;

void AttributeTest::initTestCase()
{
}

void AttributeTest::testRegistrar()
{
    // The attributes should have been registered without any effort on our part.
    {
        Attribute *a = AttributeFactory::createAttribute("AddressAttribute");
        QVERIFY(dynamic_cast<AddressAttribute *>(a));
        delete a;
    }

    {
        Attribute *a = AttributeFactory::createAttribute("DispatchModeAttribute");
        QVERIFY(dynamic_cast<DispatchModeAttribute *>(a));
        delete a;
    }

    {
        Attribute *a = AttributeFactory::createAttribute("ErrorAttribute");
        QVERIFY(dynamic_cast<ErrorAttribute *>(a));
        delete a;
    }

    {
        Attribute *a = AttributeFactory::createAttribute("SentActionAttribute");
        QVERIFY(dynamic_cast<SentActionAttribute *>(a));
        delete a;
    }

    {
        Attribute *a = AttributeFactory::createAttribute("SentBehaviourAttribute");
        QVERIFY(dynamic_cast<SentBehaviourAttribute *>(a));
        delete a;
    }

    {
        Attribute *a = AttributeFactory::createAttribute("TransportAttribute");
        QVERIFY(dynamic_cast<TransportAttribute *>(a));
        delete a;
    }
}

void AttributeTest::testSerialization()
{
    {
        QString from(QStringLiteral("from@me.org"));
        QStringList to(QStringLiteral("to1@me.org"));
        to << QStringLiteral("to2@me.org");
        QStringList cc(QStringLiteral("cc1@me.org"));
        cc << QStringLiteral("cc2@me.org");
        QStringList bcc(QStringLiteral("bcc1@me.org"));
        bcc << QStringLiteral("bcc2@me.org");
        auto a = new AddressAttribute(from, to, cc, bcc);
        QByteArray data = a->serialized();
        delete a;
        a = new AddressAttribute;
        a->deserialize(data);
        QCOMPARE(from, a->from());
        QCOMPARE(to, a->to());
        QCOMPARE(cc, a->cc());
        QCOMPARE(bcc, a->bcc());
        delete a;
    }

    {
        DispatchModeAttribute::DispatchMode mode = DispatchModeAttribute::Automatic;
        QDateTime date = QDateTime::currentDateTime();
        // The serializer does not keep track of milliseconds, so forget them.
        qDebug() << "ms" << date.toString(QStringLiteral("z"));
        int ms = date.toString(QStringLiteral("z")).toInt();
        date = date.addMSecs(-ms);
        auto a = new DispatchModeAttribute(mode);
        a->setSendAfter(date);
        QByteArray data = a->serialized();
        delete a;
        a = new DispatchModeAttribute;
        a->deserialize(data);
        QCOMPARE(mode, a->dispatchMode());
        QCOMPARE(date, a->sendAfter());
        delete a;
    }

    {
        QString msg(QStringLiteral("The #!@$ing thing failed!"));
        auto a = new ErrorAttribute(msg);
        QByteArray data = a->serialized();
        delete a;
        a = new ErrorAttribute;
        a->deserialize(data);
        QCOMPARE(msg, a->message());
        delete a;
    }

    {
        auto a = new SentActionAttribute();
        const qlonglong id = 123456789012345ll;

        a->addAction(SentActionAttribute::Action::MarkAsReplied, QVariant(id));
        a->addAction(SentActionAttribute::Action::MarkAsForwarded, QVariant(id));

        QByteArray data = a->serialized();
        delete a;
        a = new SentActionAttribute;
        a->deserialize(data);

        const SentActionAttribute::Action::List actions = a->actions();
        QCOMPARE(actions.count(), 2);

        QCOMPARE(SentActionAttribute::Action::MarkAsReplied, actions[0].type());
        QCOMPARE(id, actions[0].value().toLongLong());

        QCOMPARE(SentActionAttribute::Action::MarkAsForwarded, actions[1].type());
        QCOMPARE(id, actions[1].value().toLongLong());
        delete a;
    }

    {
        SentBehaviourAttribute::SentBehaviour beh = SentBehaviourAttribute::MoveToCollection;
        Collection::Id id = 123456789012345ll;
        auto a = new SentBehaviourAttribute(beh, Collection(id));
        bool sendSilently = true;
        a->setSendSilently(sendSilently);
        QByteArray data = a->serialized();
        delete a;
        a = new SentBehaviourAttribute;
        a->deserialize(data);
        QCOMPARE(beh, a->sentBehaviour());
        QCOMPARE(id, a->moveToCollection().id());
        QCOMPARE(sendSilently, a->sendSilently());
        delete a;
    }

    // MoveToCollection + silently
    {
        SentBehaviourAttribute::SentBehaviour beh = SentBehaviourAttribute::MoveToCollection;
        Collection::Id id = 123456789012345ll;
        auto a = new SentBehaviourAttribute(beh, Collection(id));
        bool sendSilently = true;
        a->setSendSilently(sendSilently);
        QByteArray data = a->serialized();
        delete a;
        a = new SentBehaviourAttribute;
        a->deserialize(data);
        QCOMPARE(beh, a->sentBehaviour());
        QCOMPARE(id, a->moveToCollection().id());
        QCOMPARE(sendSilently, a->sendSilently());
        SentBehaviourAttribute *copy = a->clone();
        QCOMPARE(copy->sentBehaviour(), beh);
        QCOMPARE(copy->moveToCollection().id(), id);
        QCOMPARE(copy->sendSilently(), sendSilently);
        delete a;
        delete copy;
    }

    // Delete + silently
    {
        SentBehaviourAttribute::SentBehaviour beh = SentBehaviourAttribute::Delete;
        Collection::Id id = 123456789012345ll;
        auto a = new SentBehaviourAttribute(beh, Collection(id));
        bool sendSilently = true;
        a->setSendSilently(sendSilently);
        QByteArray data = a->serialized();
        delete a;
        a = new SentBehaviourAttribute;
        a->deserialize(data);
        QCOMPARE(beh, a->sentBehaviour());
        // When delete we move to -1
        QCOMPARE(a->moveToCollection().id(), -1);
        QCOMPARE(sendSilently, a->sendSilently());
        SentBehaviourAttribute *copy = a->clone();
        QCOMPARE(copy->sentBehaviour(), beh);
        // When delete we move to -1
        QCOMPARE(copy->moveToCollection().id(), -1);
        QCOMPARE(copy->sendSilently(), sendSilently);
        delete a;
        delete copy;
    }

    // MoveToDefaultSentCollection + silently
    {
        SentBehaviourAttribute::SentBehaviour beh = SentBehaviourAttribute::MoveToDefaultSentCollection;
        Collection::Id id = 123456789012345ll;
        auto a = new SentBehaviourAttribute(beh, Collection(id));
        bool sendSilently = true;
        a->setSendSilently(sendSilently);
        QByteArray data = a->serialized();
        delete a;
        a = new SentBehaviourAttribute;
        a->deserialize(data);
        QCOMPARE(beh, a->sentBehaviour());
        // When movetodefaultsendCollection we move to -1
        QCOMPARE(a->moveToCollection().id(), -1);
        QCOMPARE(sendSilently, a->sendSilently());
        SentBehaviourAttribute *copy = a->clone();
        QCOMPARE(copy->sentBehaviour(), beh);
        // When movetodefaultsendCollection we move to -1
        QCOMPARE(copy->moveToCollection().id(), -1);
        QCOMPARE(copy->sendSilently(), sendSilently);
        delete a;
        delete copy;
    }

    {
        int id = 3219;
        auto a = new TransportAttribute(id);
        QByteArray data = a->serialized();
        delete a;
        a = new TransportAttribute;
        a->deserialize(data);
        QCOMPARE(id, a->transportId());
        delete a;
    }
}

QTEST_AKONADIMAIN(AttributeTest)
