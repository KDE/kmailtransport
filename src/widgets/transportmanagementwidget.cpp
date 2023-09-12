/*
  SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

  Based on KMail code by:
  SPDX-FileCopyrightText: 2001-2003 Marc Mutz <mutz@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportmanagementwidget.h"
#include "transport.h"
#include "transportmanager.h"
#include "ui_transportmanagementwidget.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QMenu>

using namespace MailTransport;

class MailTransport::TransportManagementWidgetPrivate
{
public:
    explicit TransportManagementWidgetPrivate(TransportManagementWidget *parent);

    Ui::TransportManagementWidget ui;
    TransportManagementWidget *const q;

    // Slots
    void defaultClicked();
    void removeClicked();
    void renameClicked();
    void editClicked();
    void addClicked();
    void updateButtonState();
    void slotCustomContextMenuRequested(const QPoint &);
};

TransportManagementWidgetPrivate::TransportManagementWidgetPrivate(TransportManagementWidget *parent)
    : q(parent)
{
}

TransportManagementWidget::TransportManagementWidget(QWidget *parent)
    : QWidget(parent)
    , d(new TransportManagementWidgetPrivate(this))
{
    d->ui.setupUi(this);
    d->updateButtonState();

    d->ui.transportList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(d->ui.transportList, &QTreeWidget::itemDoubleClicked, this, [this]() {
        d->editClicked();
    });
    connect(d->ui.addButton, &QPushButton::clicked, this, [this]() {
        d->addClicked();
    });
    connect(d->ui.editButton, &QPushButton::clicked, this, [this]() {
        d->editClicked();
    });
    connect(d->ui.renameButton, &QPushButton::clicked, this, [this]() {
        d->renameClicked();
    });
    connect(d->ui.removeButton, &QPushButton::clicked, this, [this]() {
        d->removeClicked();
    });
    connect(d->ui.defaultButton, &QPushButton::clicked, this, [this]() {
        d->defaultClicked();
    });
    connect(d->ui.transportList, &QTreeWidget::customContextMenuRequested, this, [this](QPoint p) {
        d->slotCustomContextMenuRequested(p);
    });
    connect(d->ui.transportList, &QTreeWidget::itemSelectionChanged, this, [this]() {
        d->updateButtonState();
    });
}

TransportManagementWidget::~TransportManagementWidget() = default;

void TransportManagementWidgetPrivate::updateButtonState()
{
    const auto nbItems{ui.transportList->selectedItems().count()};
    if (nbItems == 0) {
        ui.editButton->setEnabled(false);
        ui.renameButton->setEnabled(false);
        ui.removeButton->setEnabled(false);
        ui.defaultButton->setEnabled(false);
    } else {
        ui.editButton->setEnabled(nbItems == 1);
        ui.renameButton->setEnabled(nbItems == 1);
        ui.removeButton->setEnabled(nbItems >= 1);
        if (nbItems == 1) {
            if (ui.transportList->currentItem()->data(0, Qt::UserRole) == TransportManager::self()->defaultTransportId()) {
                ui.defaultButton->setEnabled(false);
            } else {
                ui.defaultButton->setEnabled(true);
            }
        } else {
            ui.defaultButton->setEnabled(false);
        }
    }
}

void TransportManagementWidgetPrivate::addClicked()
{
    TransportManager::self()->showTransportCreationDialog(q);
}

void TransportManagementWidgetPrivate::editClicked()
{
    if (ui.transportList->selectedItems().isEmpty()) {
        return;
    }

    const int currentId = ui.transportList->selectedItems().at(0)->data(0, Qt::UserRole).toInt();
    Transport *transport = TransportManager::self()->transportById(currentId);
    TransportManager::self()->configureTransport(transport->identifier(), transport, q);
}

void TransportManagementWidgetPrivate::renameClicked()
{
    if (ui.transportList->selectedItems().isEmpty()) {
        return;
    }

    ui.transportList->editItem(ui.transportList->selectedItems().at(0), 0);
}

void TransportManagementWidgetPrivate::removeClicked()
{
    const auto selectedItems{ui.transportList->selectedItems()};
    if (selectedItems.isEmpty()) {
        return;
    }
    const auto nbAccount{selectedItems.count()};
    const QString msg = (selectedItems.count() == 1)
        ? i18n("Do you want to remove outgoing account '%1'?", ui.transportList->selectedItems().at(0)->text(0))
        : i18np("Do you really want to remove this %1 outgoing account?", "Do you really want to remove these %1 outgoing accounts?", nbAccount);

    const int rc = KMessageBox::questionTwoActions(q, msg, i18n("Remove outgoing account?"), KStandardGuiItem::remove(), KStandardGuiItem::cancel());
    if (rc == KMessageBox::ButtonCode::SecondaryAction) {
        return;
    }

    QList<Transport::Id> lst;
    lst.reserve(nbAccount);
    for (QTreeWidgetItem *selecteditem : selectedItems) {
        lst << selecteditem->data(0, Qt::UserRole).toInt();
    }
    for (const auto id : std::as_const(lst)) {
        TransportManager::self()->removeTransport(id);
    }
}

void TransportManagementWidgetPrivate::defaultClicked()
{
    if (ui.transportList->selectedItems().isEmpty()) {
        return;
    }

    TransportManager::self()->setDefaultTransport(ui.transportList->selectedItems().at(0)->data(0, Qt::UserRole).toInt());
}

void TransportManagementWidgetPrivate::slotCustomContextMenuRequested(const QPoint &pos)
{
    QMenu menu(q);
    menu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add..."), q, [this]() {
        addClicked();
    });
    QTreeWidgetItem *item = ui.transportList->itemAt(pos);
    if (item) {
        menu.addAction(QIcon::fromTheme(QStringLiteral("document-edit")), i18n("Modify..."), q, [this]() {
            editClicked();
        });
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-rename")), i18n("Rename"), q, [this]() {
            renameClicked();
        });
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme(QStringLiteral("list-remove")), i18n("Remove"), q, [this]() {
            removeClicked();
        });
        if (item->data(0, Qt::UserRole) != TransportManager::self()->defaultTransportId()) {
            menu.addSeparator();
            menu.addAction(i18n("Set as Default"), q, [this]() {
                defaultClicked();
            });
        }
    }
    menu.exec(ui.transportList->viewport()->mapToGlobal(pos));
}

#include "moc_transportmanagementwidget.cpp"
