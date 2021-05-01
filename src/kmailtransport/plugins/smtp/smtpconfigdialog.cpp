/*
    SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "smtpconfigdialog.h"

#include "smtpconfigwidget.h"
#include "transport.h"
#include "transportmanager.h"
#include "transporttype.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

using namespace MailTransport;

SmtpConfigDialog::SmtpConfigDialog(Transport *transport, QWidget *parent)
    : QDialog(parent)
{
    Q_ASSERT(transport);
    mTransport = transport;
    auto mainLayout = new QVBoxLayout(this);
    mConfigWidget = new SMTPConfigWidget(transport, this);
    mConfigWidget->setObjectName(QStringLiteral("smtpconfigwidget"));
    mainLayout->addWidget(mConfigWidget);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->setObjectName(QStringLiteral("buttons"));
    mOkButton = buttonBox->button(QDialogButtonBox::Ok);
    mOkButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    mainLayout->addWidget(buttonBox);

    connect(mOkButton, &QAbstractButton::clicked, this, &SmtpConfigDialog::okClicked);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SmtpConfigDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SmtpConfigDialog::reject);
}

SmtpConfigDialog::~SmtpConfigDialog()
{
}

void SmtpConfigDialog::okClicked()
{
    mConfigWidget->apply();
    mTransport->save();
}

void SmtpConfigDialog::slotTextChanged(const QString &text)
{
    mOkButton->setEnabled(!text.isEmpty());
}

#include "moc_smtpconfigdialog.cpp"
