/*
    Copyright (c) 2015 Volker Krause <vkrause@kde.org>

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

#include "servertest.h"

#include <QApplication>
#include <QDebug>

using namespace MailTransport;

int main(int argc, char **argv)
{
    if (argc <= 2)
        qFatal("Usage: servertest <protocol> <hostname>");

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

