/*
    SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "addtransportdialogng_gui.h"
#include "../src/kmailtransport/widgets/addtransportdialogng.h"

#include <QApplication>
#include <QStandardPaths>
#include <QHBoxLayout>
#include <QPushButton>

AddTransportDialogNG_gui::AddTransportDialogNG_gui(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    QPushButton *button = new QPushButton(QStringLiteral("Add transport"));
    layout->addWidget(button);
    connect(button, &QPushButton::clicked, this, [this] {
        MailTransport::AddTransportDialogNG *dlg = new MailTransport::AddTransportDialogNG(this);
        dlg->exec();
        delete dlg;
    });
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("addtransportnewng"));
    QStandardPaths::setTestModeEnabled(true);
    AddTransportDialogNG_gui *t = new AddTransportDialogNG_gui();
    t->show();
    app.exec();
    delete t;
}
