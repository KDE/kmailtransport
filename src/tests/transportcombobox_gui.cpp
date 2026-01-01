// SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "widgets/transportcombobox.h"
using namespace Qt::Literals::StringLiterals;

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
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

    auto labelTransport = new QLabel(this);
    connect(combobox, &MailTransport::TransportComboBox::activated, this, [combobox, labelTransport]() {
        labelTransport->setText(QString::number(combobox->currentTransportId()));
    });
    mainLayout->addWidget(labelTransport);

    {
        auto hbox = new QHBoxLayout;
        auto transportLineEdit = new QLineEdit(this);

        hbox->addWidget(new QLabel(u"set transport identifier:"_s, this));
        hbox->addWidget(transportLineEdit);

        auto identityNameButton = new QPushButton(u"Apply"_s, this);
        hbox->addWidget(identityNameButton);

        connect(identityNameButton, &QPushButton::clicked, this, [transportLineEdit, combobox]() {
            combobox->setCurrentTransport(transportLineEdit->text().toInt());
        });

        mainLayout->addLayout(hbox);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    auto w = new IdentityComboboxWidget;
    w->show();
    return app.exec();
}
