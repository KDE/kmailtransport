/*
    Copyright (c) 2017 Laurent Montel <montel@kde.org>

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

#include "smtpconfigdialog.h"

#include "transport.h"
#include "transportmanager.h"
#include "transporttype.h"
#include "smtpconfigwidget.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>

using namespace MailTransport;


SmtpConfigDialog::SmtpConfigDialog(Transport *transport, QWidget *parent)
    : QDialog(parent)
{
    Q_ASSERT(transport);
    mTransport = transport;
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mConfigWidget = new SMTPConfigWidget(transport, this);
    mConfigWidget->setObjectName(QStringLiteral("smtpconfigwidget"));
    mainLayout->addWidget(mConfigWidget);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
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
