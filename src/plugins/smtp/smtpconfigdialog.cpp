/*
    SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "smtpconfigdialog.h"
using namespace Qt::Literals::StringLiterals;

#include "smtpconfigwidget.h"
#include "transport.h"
#include "transportmanager.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>

using namespace MailTransport;

SmtpConfigDialog::SmtpConfigDialog(Transport *transport, QWidget *parent)
    : QDialog(parent)
{
    Q_ASSERT(transport);
    mTransport = transport;
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins({});
    mConfigWidget = new SMTPConfigWidget(transport, this);
    mConfigWidget->setObjectName("smtpconfigwidget"_L1);
    mainLayout->addWidget(mConfigWidget);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->setContentsMargins(style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
                                  style()->pixelMetric(QStyle::PM_LayoutTopMargin),
                                  style()->pixelMetric(QStyle::PM_LayoutRightMargin),
                                  style()->pixelMetric(QStyle::PM_LayoutBottomMargin));
    buttonBox->setObjectName("buttons"_L1);
    mOkButton = buttonBox->button(QDialogButtonBox::Ok);
    mOkButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    mainLayout->addWidget(buttonBox);

    connect(mOkButton, &QAbstractButton::clicked, this, &SmtpConfigDialog::okClicked);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SmtpConfigDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SmtpConfigDialog::reject);
}

SmtpConfigDialog::~SmtpConfigDialog() = default;

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
