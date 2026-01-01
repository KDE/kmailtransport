/*
    SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "addtransportdialogng_gui.h"
using namespace Qt::Literals::StringLiterals;

#include "widgets/addtransportdialogng.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStandardPaths>

AddTransportDialogNG_gui::AddTransportDialogNG_gui(QWidget *parent)
    : QWidget(parent)
{
    auto layout = new QHBoxLayout(this);
    auto button = new QPushButton(u"Add transport"_s);
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
    app.setApplicationName(u"addtransportnewng"_s);
    QStandardPaths::setTestModeEnabled(true);
    auto t = new AddTransportDialogNG_gui();
    t->show();
    app.exec();
    delete t;
}

#include "moc_addtransportdialogng_gui.cpp"
