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
    setEditTriggers(QAbstractItemView::EditKeyPressed);
    setAllColumnsShowFocus(true);
    header()->setSectionsMovable(false);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    auto model = new MailTransport::TransportModel(this);

    model->setShowDefault(true);
    mTransportSortProxyModel->setSourceModel(model);
    setModel(mTransportSortProxyModel);

    setColumnHidden(MailTransport::TransportModel::TransportIdentifierRole, true);
    setColumnHidden(MailTransport::TransportModel::DefaultRole, true);
    setColumnHidden(MailTransport::TransportModel::ActivitiesRole, true);
    setColumnHidden(MailTransport::TransportModel::EnabledActivitiesRole, true);
    setItemDelegateForColumn(MailTransport::TransportModel::NameRole, new TransportTreeDelegate(this));
    setUniformRowHeights(true);
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

TransportSortProxyModel *TransportTreeView::transportSortProxyModel() const
{
    return mTransportSortProxyModel;
}

bool TransportTreeView::enablePlasmaActivities() const
{
    return mTransportSortProxyModel->enablePlasmaActivities();
}

void TransportTreeView::setEnablePlasmaActivities(bool newEnablePlasmaActivities)
{
    mTransportSortProxyModel->setEnablePlasmaActivities(newEnablePlasmaActivities);
}

#include "moc_transporttreeview.cpp"
