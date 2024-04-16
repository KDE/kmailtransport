/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QStyledItemDelegate>
class QLineEdit;
namespace MailTransport
{
class TransportTreeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TransportTreeDelegate(QObject *parent = nullptr);
    ~TransportTreeDelegate() override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const override;

private:
    QLineEdit *mLineEdit = nullptr;
};
}
