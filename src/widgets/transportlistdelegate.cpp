/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "transportlistdelegate.h"
using namespace MailTransport;
TransportListDelegate::TransportListDelegate(QObject *parent)
    : QItemDelegate{parent}
{
}

TransportListDelegate::~TransportListDelegate() = default;

QWidget *TransportListDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // TODO
    return {};
}

void TransportListDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // TODO
}

void TransportListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    // TODO
}
