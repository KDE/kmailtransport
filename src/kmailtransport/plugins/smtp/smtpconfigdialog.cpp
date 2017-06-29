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
#include "smtp/smtpconfigwidget.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>

using namespace MailTransport;

class MailTransport::SmtpConfigDialog::Private
{
public:
    Private(SmtpConfigDialog *qq)
        : transport(nullptr)
        , configWidget(nullptr)
        , q(qq)
        , okButton(nullptr)
    {
    }

    Transport *transport;
    SMTPConfigWidget *configWidget;
    SmtpConfigDialog *q;
    QPushButton *okButton;

    void okClicked();
    void slotTextChanged(const QString &text);
    void slotEnabledOkButton(bool);
};

void SmtpConfigDialog::Private::slotEnabledOkButton(bool b)
{
    okButton->setEnabled(b);
}

void SmtpConfigDialog::Private::okClicked()
{
    configWidget->apply();
    transport->save();
}

void SmtpConfigDialog::Private::slotTextChanged(const QString &text)
{
    okButton->setEnabled(!text.isEmpty());
}

SmtpConfigDialog::SmtpConfigDialog(Transport *transport, QWidget *parent)
    : QDialog(parent)
    , d(new Private(this))
{
    Q_ASSERT(transport);
    d->transport = transport;
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    bool pathIsEmpty = false;
    d->configWidget = new SMTPConfigWidget(transport, this);
    mainLayout->addWidget(d->configWidget);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    d->okButton = buttonBox->button(QDialogButtonBox::Ok);
    d->okButton->setEnabled(false);
    d->okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    mainLayout->addWidget(buttonBox);

    connect(d->okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SmtpConfigDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SmtpConfigDialog::reject);

    d->okButton->setEnabled(!pathIsEmpty);
}

SmtpConfigDialog::~SmtpConfigDialog()
{
    delete d;
}

#include "moc_smtpconfigdialog.cpp"
