/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QItemDelegate>
namespace MailTransport
{
class TransportListDelegate : public QItemDelegate
{
public:
    explicit TransportListDelegate(QObject *parent = nullptr);
    ~TransportListDelegate() override;
};
}
