// SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "widgets/transportcombobox.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>

class IdentityComboboxWidget : public QWidget
{
public:
    explicit IdentityComboboxWidget(QWidget *parent = nullptr);
    ~IdentityComboboxWidget() override = default;
};

IdentityComboboxWidget::IdentityComboboxWidget(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);
    auto combobox = new MailTransport::TransportComboBox(this);
    mainLayout->addWidget(combobox);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    auto w = new IdentityComboboxWidget;
    w->show();
    return app.exec();
}
