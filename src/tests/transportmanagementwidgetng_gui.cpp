/*
    SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "widgets/transportmanagementwidgetng.h"
#include <QApplication>
#include <QStandardPaths>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("transportmanagementwidgetng"));
    QStandardPaths::setTestModeEnabled(true);
    auto t = new MailTransport::TransportManagementWidgetNg();
    t->show();
    app.exec();
    delete t;
}
