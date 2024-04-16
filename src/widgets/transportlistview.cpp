/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportlistview.h"
#include "transportmodel.h"
#include "transportsortproxymodel.h"

using namespace MailTransport;
TransportListView::TransportListView(QWidget *parent)
    : QTreeView(parent)
    , mTransportSortProxyModel(new TransportSortProxyModel(this))
{
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
