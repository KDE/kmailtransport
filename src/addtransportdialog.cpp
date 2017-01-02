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

#include "addtransportdialog.h"
#include "transport.h"
#include "transportconfigwidget.h"
#include "transportmanager.h"
#include "transporttype.h"
#include "ui_addtransportdialog.h"

#include "mailtransport_debug.h"
#include <QDialogButtonBox>

#include <QPushButton>
#include <agentinstance.h>
#include <agentinstancecreatejob.h>

using namespace MailTransport;

/**
  @internal
*/
class AddTransportDialog::Private
{
public:
    Private(AddTransportDialog *qq)
        : q(qq), okButton(nullptr)
    {
    }

    /**
      Returns the currently selected type in the type selection widget, or
      an invalid type if none is selected.
    */
    TransportType selectedType() const;

    /**
      Enables the OK button if a type is selected.
    */
    void updateOkButton(); // slot
    void doubleClicked(); //slot
    void writeConfig();
    void readConfig();

    AddTransportDialog *const q;
    QPushButton *okButton;
    ::Ui::AddTransportDialog ui;
};

void AddTransportDialog::Private::writeConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "AddTransportDialog");
    group.writeEntry("Size", q->size());
}

void AddTransportDialog::Private::readConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "AddTransportDialog");
    const QSize sizeDialog = group.readEntry("Size", QSize(300, TransportManager::self()->types().size() > 1 ? 200 : 80));
    if (sizeDialog.isValid()) {
        q->resize(sizeDialog);
    }
}

TransportType AddTransportDialog::Private::selectedType() const
{
    QList<QTreeWidgetItem *> sel = ui.typeListView->selectedItems();
    if (!sel.empty()) {
        return sel.first()->data(0, Qt::UserRole).value<TransportType>();
    }
    return TransportType();
}

void AddTransportDialog::Private::doubleClicked()
{
    if (selectedType().isValid() && !ui.name->text().trimmed().isEmpty()) {
        q->accept();
    }
}

void AddTransportDialog::Private::updateOkButton()
{
    // Make sure a type is selected before allowing the user to continue.
    okButton->setEnabled(selectedType().isValid() && !ui.name->text().trimmed().isEmpty());
}

AddTransportDialog::AddTransportDialog(QWidget *parent)
    : QDialog(parent), d(new Private(this))
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
        connect(buttonBox, &QDialogButtonBox::accepted, this, &AddTransportDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &AddTransportDialog::reject);
    }

    // Populate type list.
    const auto transportTypes = TransportManager::self()->types();
    foreach (const TransportType &type, transportTypes) {
        QTreeWidgetItem *treeItem = new QTreeWidgetItem(d->ui.typeListView);
        treeItem->setText(0, type.name());
        treeItem->setText(1, type.description());
        treeItem->setData(0, Qt::UserRole, QVariant::fromValue(type));     // the transport type
        if (type.type() == TransportBase::EnumType::SMTP)
            treeItem->setSelected(true); // select SMTP by default
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
    connect(d->ui.typeListView, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(updateOkButton()));
    connect(d->ui.typeListView, SIGNAL(itemSelectionChanged()),
            this, SLOT(updateOkButton()));
    connect(d->ui.typeListView, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(doubleClicked()));
    connect(d->ui.name, SIGNAL(textChanged(QString)),
            this, SLOT(updateOkButton()));
    d->readConfig();
}

AddTransportDialog::~AddTransportDialog()
{
    d->writeConfig();
    delete d;
}

void AddTransportDialog::accept()
{
    if (!d->selectedType().isValid()) {
        return;
    }

    // Create a new transport and configure it.
    Transport *transport = TransportManager::self()->createTransport();
    transport->setTransportType(d->selectedType());
    if (d->selectedType().type() == Transport::EnumType::Akonadi) {
        // Create a resource instance if Akonadi-type transport.
        using namespace Akonadi;
        AgentInstanceCreateJob *cjob = new AgentInstanceCreateJob(d->selectedType().agentType());
        if (!cjob->exec()) {
            qCWarning(MAILTRANSPORT_LOG) << "Failed to create agent instance of type"
                                         << d->selectedType().agentType().identifier();
            return;
        }
        transport->setHost(cjob->instance().identifier());
    }
    transport->setName(d->ui.name->text().trimmed());
    transport->forceUniqueName();
    if (TransportManager::self()->configureTransport(transport, this)) {
        // The user clicked OK and the transport settings were saved.
        TransportManager::self()->addTransport(transport);
        if (d->ui.setDefault->isChecked()) {
            TransportManager::self()->setDefaultTransport(transport->id());
        }
        QDialog::accept();
    }
}

#include "moc_addtransportdialog.cpp"
