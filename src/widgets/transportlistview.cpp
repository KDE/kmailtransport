/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportlistview.h"
#include "transportmodel.h"
#include "transportsortproxymodel.h"
#include <QHeaderView>

using namespace MailTransport;
TransportListView::TransportListView(QWidget *parent)
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
}

TransportListView::~TransportListView() = default;

TransportActivitiesAbstract *TransportListView::transportActivitiesAbstract() const
{
    return mTransportSortProxyModel->transportActivitiesAbstract();
}

void TransportListView::setTransportActivitiesAbstract(TransportActivitiesAbstract *activitiesAbstract)
{
    mTransportSortProxyModel->setTransportActivitiesAbstract(activitiesAbstract);
}

#include "moc_transportlistview.cpp"
