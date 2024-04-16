/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "transporttreedelegate.h"
#include "transportmodel.h"
#include <QLineEdit>
using namespace MailTransport;
TransportTreeDelegate::TransportTreeDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{
}

TransportTreeDelegate::~TransportTreeDelegate() = default;

QWidget *TransportTreeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);
    auto *lineEdit = new QLineEdit(parent);
    auto *that = const_cast<TransportTreeDelegate *>(this);
    connect(lineEdit, &QLineEdit::editingFinished, this, [=]() {
        Q_EMIT that->commitData(mLineEdit);
        Q_EMIT that->closeEditor(mLineEdit);
    });
    const_cast<TransportTreeDelegate *>(this)->mLineEdit = lineEdit;
    return lineEdit;
}

void TransportTreeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    auto loggingIndex = index.model()->index(index.row(), MailTransport::TransportModel::NameRole);
    qobject_cast<QLineEdit *>(editor)->setText(loggingIndex.data().toString());
}

void TransportTreeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    auto loggingIndex = index.model()->index(index.row(), MailTransport::TransportModel::NameRole);
    model->setData(loggingIndex, qobject_cast<QLineEdit *>(editor)->text());
}

void TransportTreeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & /*index*/) const
{
    editor->setGeometry(option.rect);
}
