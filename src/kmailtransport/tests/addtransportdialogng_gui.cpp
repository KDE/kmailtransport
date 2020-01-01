/*
    Copyright (C) 2017-2020 Laurent Montel <montel@kde.org>

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
