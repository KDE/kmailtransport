/*
    SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "widgets/transportmanagementwidget.h"
#include <QApplication>
#include <QStandardPaths>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("transportmanagementwidget"));
    QStandardPaths::setTestModeEnabled(true);
    auto t = new MailTransport::TransportManagementWidget();
    t->show();
    app.exec();
    delete t;
}
