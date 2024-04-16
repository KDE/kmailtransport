/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transporttreeview.h"
#include "transportmodel.h"
#include "transportsortproxymodel.h"
#include "transporttreedelegate.h"
#include <QHeaderView>

using namespace MailTransport;
TransportTreeView::TransportTreeView(QWidget *parent)
    : QTreeView(parent)
    , mTransportSortProxyModel(new TransportSortProxyModel(this))
{
    setAlternatingRowColors(true);
    setSelectionMode(SingleSelection);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setRootIsDecorated(false);
    setSortingEnabled(true);
    setAllColumnsShowFocus(true);
    header()->setSectionsMovable(false);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    auto model = new MailTransport::TransportModel(this);

    mTransportSortProxyModel->setSourceModel(model);
    setModel(mTransportSortProxyModel);

    setEditTriggers(AllEditTriggers);
    setColumnHidden(MailTransport::TransportModel::TransportIdentifierRole, true);
    setColumnHidden(MailTransport::TransportModel::DefaultRole, true);
    setItemDelegateForColumn(MailTransport::TransportModel::NameRole, new TransportTreeDelegate(this));
}

TransportTreeView::~TransportTreeView() = default;

TransportActivitiesAbstract *TransportTreeView::transportActivitiesAbstract() const
{
    return mTransportSortProxyModel->transportActivitiesAbstract();
}

void TransportTreeView::setTransportActivitiesAbstract(TransportActivitiesAbstract *activitiesAbstract)
{
    mTransportSortProxyModel->setTransportActivitiesAbstract(activitiesAbstract);
}

#include "moc_transporttreeview.cpp"
