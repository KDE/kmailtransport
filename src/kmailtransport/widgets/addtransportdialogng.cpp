/*
    Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>

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

#include "addtransportdialogng.h"
#include "transport.h"
#include "transportconfigwidget.h"
#include "transportmanager.h"
#include "transporttype.h"
#include "ui_addtransportdialog.h"

#include "mailtransport_debug.h"
#include <QDialogButtonBox>

#include <QPushButton>

using namespace MailTransport;

/**
  @internal
*/
class AddTransportDialogNG::Private
{
public:
    Private(AddTransportDialogNG *qq)
        : q(qq)
    {
    }

    QString selectedType() const;

    /**
      Enables the OK button if a type is selected.
    */
    void updateOkButton(); // slot
    void doubleClicked(); //slot
    void writeConfig();
    void readConfig();

    AddTransportDialogNG *const q;
    QPushButton *okButton = nullptr;
    ::Ui::AddTransportDialog ui;
};

void AddTransportDialogNG::Private::writeConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "AddTransportDialog");
    group.writeEntry("Size", q->size());
}

void AddTransportDialogNG::Private::readConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "AddTransportDialog");
    const QSize sizeDialog = group.readEntry("Size", QSize(300, TransportManager::self()->types().size() > 1 ? 200 : 80));
    if (sizeDialog.isValid()) {
        q->resize(sizeDialog);
    }
}

QString AddTransportDialogNG::Private::selectedType() const
{
    QList<QTreeWidgetItem *> sel = ui.typeListView->selectedItems();
    if (!sel.empty()) {
        return sel.first()->data(0, Qt::UserRole).toString();
    }
    return {};
}

void AddTransportDialogNG::Private::doubleClicked()
{
    if (!selectedType().isEmpty() && !ui.name->text().trimmed().isEmpty()) {
        q->accept();
    }
}

void AddTransportDialogNG::Private::updateOkButton()
{
    // Make sure a type is selected before allowing the user to continue.
    okButton->setEnabled(!selectedType().isEmpty() && !ui.name->text().trimmed().isEmpty());
}

AddTransportDialogNG::AddTransportDialogNG(QWidget *parent)
    : QDialog(parent)
    , d(new Private(this))
{
    // Setup UI.
    {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        QWidget *widget = new QWidget(this);
        d->ui.setupUi(widget);
        mainLayout->addWidget(widget);
        setWindowTitle(i18n("Create Outgoing Account"));
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        d->okButton = buttonBox->button(QDialogButtonBox::Ok);
        d->okButton->setText(i18nc("create and configure a mail transport", "Create and Configure"));
        d->okButton->setEnabled(false);
        d->okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
        mainLayout->addWidget(buttonBox);
        connect(buttonBox, &QDialogButtonBox::accepted, this, &AddTransportDialogNG::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &AddTransportDialogNG::reject);
    }

    // Populate type list.
    const auto transportTypes = TransportManager::self()->types();
    for (const TransportType &type : transportTypes) {
        QTreeWidgetItem *treeItem = new QTreeWidgetItem(d->ui.typeListView);
        treeItem->setText(0, type.name());
        treeItem->setText(1, type.description());
        treeItem->setToolTip(1, type.description());
        treeItem->setData(0, Qt::UserRole, type.identifier());     // the transport type
        if (type.identifier() == QStringLiteral("SMTP")) {
            treeItem->setSelected(true); // select SMTP by default
        }
    }
    d->ui.typeListView->resizeColumnToContents(0);

    // if we only have one type, don't bother the user with this
    if (d->ui.typeListView->invisibleRootItem()->childCount() == 1) {
        d->ui.descLabel->hide();
        d->ui.typeListView->hide();
    }

    updateGeometry();
    d->ui.typeListView->setFocus();

    // Connect user input.
    connect(d->ui.typeListView, &QTreeWidget::itemClicked, this, [this]() { d->updateOkButton(); } );
    connect(d->ui.typeListView, &QTreeWidget::itemSelectionChanged, this, [this]() { d->updateOkButton(); } );
    connect(d->ui.typeListView, &QTreeWidget::itemDoubleClicked, this, [this]() { d->doubleClicked(); } );
    connect(d->ui.name, &QLineEdit:: textChanged, this, [this]() { d->updateOkButton(); } );
    d->readConfig();
}

AddTransportDialogNG::~AddTransportDialogNG()
{
    d->writeConfig();
    delete d;
}

void AddTransportDialogNG::accept()
{
    if (d->selectedType().isEmpty()) {
        return;
    }

    // Create a new transport and configure it.
    Transport *transport = TransportManager::self()->createTransport();
    transport->setName(d->ui.name->text().trimmed());
    const QString identifier = d->selectedType();
    transport->setIdentifier(identifier);
    transport->forceUniqueName();
    TransportManager::self()->initializeTransport(identifier, transport);
    if (TransportManager::self()->configureTransport(identifier, transport, this)) {
        // The user clicked OK and the transport settings were saved.
        TransportManager::self()->addTransport(transport);
        if (d->ui.setDefault->isChecked()) {
            TransportManager::self()->setDefaultTransport(transport->id());
        }
        QDialog::accept();
    }
}

#include "moc_addtransportdialogng.cpp"
