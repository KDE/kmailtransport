/*
    SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "addtransportdialogng_gui.h"
#include "../src/kmailtransport/widgets/addtransportdialogng.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStandardPaths>

AddTransportDialogNG_gui::AddTransportDialogNG_gui(QWidget *parent)
    : QWidget(parent)
{
    auto layout = new QHBoxLayout(this);
    auto button = new QPushButton(QStringLiteral("Add transport"));
    layout->addWidget(button);
    connect(button, &QPushButton::clicked, this, [this] {
        auto dlg = new MailTransport::AddTransportDialogNG(this);
        dlg->exec();
        delete dlg;
    });
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("addtransportnewng"));
    QStandardPaths::setTestModeEnabled(true);
    auto t = new AddTransportDialogNG_gui();
    t->show();
    app.exec();
    delete t;
}
