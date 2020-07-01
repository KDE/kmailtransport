/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "servertest.h"

#include <QApplication>
#include <QDebug>

using namespace MailTransport;

int main(int argc, char **argv)
{
    if (argc <= 2) {
        qFatal("Usage: servertest <protocol> <hostname>");
    }

    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("kmailtransport-servertest"));

    ServerTest test;
    test.setProtocol(app.arguments().at(1));
    test.setServer(app.arguments().at(2));
    test.start();
    QObject::connect(&test, &ServerTest::finished, [](const QVector<int> &encs) {
        qDebug() << encs;
        QCoreApplication::quit();
    });
    return app.exec();
}
