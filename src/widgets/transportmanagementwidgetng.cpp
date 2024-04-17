/*
  SPDX-FileCopyrightText: Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportmanagementwidgetng.h"
#include "transport.h"
#include "transportmanager.h"
#include "ui_transportmanagementwidgetng.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QMenu>

using namespace MailTransport;

class MailTransport::TransportManagementWidgetNgPrivate
{
public:
    explicit TransportManagementWidgetNgPrivate(TransportManagementWidgetNg *parent);

    Ui::TransportManagementWidgetNg ui;
    TransportManagementWidgetNg *const q;

    // Slots
    void defaultClicked();
    void removeClicked();
    void renameClicked();
    void editClicked();
    void addClicked();
    void updateButtonState();
    void slotCustomContextMenuRequested(const QPoint &);
};

TransportManagementWidgetNgPrivate::TransportManagementWidgetNgPrivate(TransportManagementWidgetNg *parent)
    : q(parent)
{
}

TransportManagementWidgetNg::TransportManagementWidgetNg(QWidget *parent)
    : QWidget(parent)
    , d(new TransportManagementWidgetNgPrivate(this))
{
    d->ui.setupUi(this);
    d->updateButtonState();

    d->ui.transportTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(d->ui.transportTreeView, &QTreeView::doubleClicked, this, [this]() {
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
    connect(d->ui.transportTreeView, &QTreeView::customContextMenuRequested, this, [this](QPoint p) {
        d->slotCustomContextMenuRequested(p);
    });
    // TODO
    // connect(d->ui.transportTreeView, &QTreeView::itemSelectionChanged, this, [this]() {
    //     d->updateButtonState();
    // });
}

TransportManagementWidgetNg::~TransportManagementWidgetNg() = default;

void TransportManagementWidgetNgPrivate::updateButtonState()
{
#if 0 // TODO
    const auto nbItems{ui.transportTreeView->selectedItems().count()};
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
            if (ui.transportTreeView->currentItem()->data(0, Qt::UserRole) == TransportManager::self()->defaultTransportId()) {
                ui.defaultButton->setEnabled(false);
            } else {
                ui.defaultButton->setEnabled(true);
            }
        } else {
            ui.defaultButton->setEnabled(false);
        }
    }
#endif
}

void TransportManagementWidgetNgPrivate::addClicked()
{
    TransportManager::self()->showTransportCreationDialog(q);
}

void TransportManagementWidgetNgPrivate::editClicked()
{
#if 0 // TODO
    if (ui.transportTreeView->selectedItems().isEmpty()) {
        return;
    }

    const int currentId = ui.transportTreeView->selectedItems().constFirst()->data(0, Qt::UserRole).toInt();
    Transport *transport = TransportManager::self()->transportById(currentId);
    TransportManager::self()->configureTransport(transport->identifier(), transport, q);
#endif
}

void TransportManagementWidgetNgPrivate::renameClicked()
{
#if 0 // TODO
    if (ui.transportTreeView->selectedItems().isEmpty()) {
        return;
    }

    ui.transportTreeView->editItem(ui.transportTreeView->selectedItems().constFirst(), 0);
#endif
}

void TransportManagementWidgetNgPrivate::removeClicked()
{
#if 0 // TODO
    const auto selectedItems{ui.transportTreeView->selectedItems()};
    if (selectedItems.isEmpty()) {
        return;
    }
    const auto nbAccount{selectedItems.count()};
    const QString msg = (selectedItems.count() == 1)
        ? i18n("Do you want to remove outgoing account '%1'?", ui.transportTreeView->selectedItems().constFirst()->text(0))
        : i18np("Do you really want to remove this %1 outgoing account?", "Do you really want to remove these %1 outgoing accounts?", nbAccount);

    const int rc = KMessageBox::questionTwoActions(q, msg, i18n("Remove outgoing account?"), KStandardGuiItem::remove(), KStandardGuiItem::cancel());
    if (rc == KMessageBox::ButtonCode::SecondaryAction) {
        return;
    }

    QList<Transport::Id> lst;
    lst.reserve(nbAccount);
    for (QTreeViewItem *selecteditem : selectedItems) {
        lst << selecteditem->data(0, Qt::UserRole).toInt();
    }
    for (const auto id : std::as_const(lst)) {
        TransportManager::self()->removeTransport(id);
    }
#endif
}

void TransportManagementWidgetNgPrivate::defaultClicked()
{
#if 0 // TODO
    if (ui.transportTreeView->selectedItems().isEmpty()) {
        return;
    }

    TransportManager::self()->setDefaultTransport(ui.transportTreeView->selectedItems().constFirst()->data(0, Qt::UserRole).toInt());
#endif
}

void TransportManagementWidgetNgPrivate::slotCustomContextMenuRequested(const QPoint &pos)
{
#if 0 // TODO
    QMenu menu(q);
    menu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18nc("@action:inmenu", "Add..."), q, [this]() {
        addClicked();
    });
    QTreeViewItem *item = ui.transportTreeView->itemAt(pos);
    if (item) {
        menu.addAction(QIcon::fromTheme(QStringLiteral("document-edit")), i18nc("@action:inmenu", "Modify..."), q, [this]() {
            editClicked();
        });
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-rename")), i18nc("@action:inmenu", "Rename"), q, [this]() {
            renameClicked();
        });
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme(QStringLiteral("list-remove")), i18nc("@action:inmenu", "Remove"), q, [this]() {
            removeClicked();
        });
        if (item->data(0, Qt::UserRole) != TransportManager::self()->defaultTransportId()) {
            menu.addSeparator();
            menu.addAction(i18n("Set as Default"), q, [this]() {
                defaultClicked();
            });
        }
    }
    menu.exec(ui.transportTreeView->viewport()->mapToGlobal(pos));
#endif
}

#include "moc_transportmanagementwidgetng.cpp"