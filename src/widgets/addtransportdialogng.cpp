/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "addtransportdialogng.h"
using namespace Qt::Literals::StringLiterals;

#include "transport.h"
#include "transportmanager.h"
#include "transporttype.h"
#include "ui_addtransportdialog.h"

#include <QDialogButtonBox>

#include <QPushButton>

using namespace MailTransport;

/**
  @internal
*/
class MailTransport::AddTransportDialogNGPrivate
{
public:
    explicit AddTransportDialogNGPrivate(AddTransportDialogNG *qq)
        : q(qq)
    {
    }

    [[nodiscard]] QString selectedType() const;

    /**
      Enables the OK button if a type is selected.
    */
    void updateOkButton(); // slot
    void doubleClicked(); // slot
    void writeConfig();
    void readConfig();

    AddTransportDialogNG *const q;
    QPushButton *okButton = nullptr;
    ::Ui::AddTransportDialog ui;
};

void AddTransportDialogNGPrivate::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), u"AddTransportDialog"_s);
    group.writeEntry("Size", q->size());
}

void AddTransportDialogNGPrivate::readConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), u"AddTransportDialog"_s);
    const QSize sizeDialog = group.readEntry("Size", QSize(300, TransportManager::self()->types().size() > 1 ? 300 : 160));
    if (sizeDialog.isValid()) {
        q->resize(sizeDialog);
    }
}

QString AddTransportDialogNGPrivate::selectedType() const
{
    const QList<QTreeWidgetItem *> sel = ui.typeListView->selectedItems();
    if (!sel.empty()) {
        return sel.first()->data(0, Qt::UserRole).toString();
    }
    return {};
}

void AddTransportDialogNGPrivate::doubleClicked()
{
    if (!selectedType().isEmpty() && !ui.name->text().trimmed().isEmpty()) {
        q->accept();
    }
}

void AddTransportDialogNGPrivate::updateOkButton()
{
    // Make sure a type is selected before allowing the user to continue.
    okButton->setEnabled(!selectedType().isEmpty() && !ui.name->text().trimmed().isEmpty());
}

AddTransportDialogNG::AddTransportDialogNG(QWidget *parent)
    : QDialog(parent)
    , d(new AddTransportDialogNGPrivate(this))
{
    // Setup UI.
    {
        auto mainLayout = new QVBoxLayout(this);
        auto widget = new QWidget(this);
        d->ui.setupUi(widget);
        mainLayout->addWidget(widget);
        setWindowTitle(i18nc("@title:window", "Create Outgoing Account"));
        auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
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
        auto treeItem = new QTreeWidgetItem(d->ui.typeListView);
        treeItem->setText(0, type.name());
        treeItem->setText(1, type.description());
        treeItem->setToolTip(1, type.description());
        treeItem->setData(0, Qt::UserRole, type.identifier()); // the transport type
        if (type.identifier() == "SMTP"_L1) {
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
    connect(d->ui.typeListView, &QTreeWidget::itemClicked, this, [this]() {
        d->updateOkButton();
    });
    connect(d->ui.typeListView, &QTreeWidget::itemSelectionChanged, this, [this]() {
        d->updateOkButton();
    });
    connect(d->ui.typeListView, &QTreeWidget::itemDoubleClicked, this, [this]() {
        d->doubleClicked();
    });
    connect(d->ui.name, &QLineEdit::textChanged, this, [this]() {
        d->updateOkButton();
    });
    d->readConfig();
}

AddTransportDialogNG::~AddTransportDialogNG()
{
    d->writeConfig();
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
