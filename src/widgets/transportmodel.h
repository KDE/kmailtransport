/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QAbstractListModel>
namespace MailTransport
{
class TransportModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit TransportModel(QObject *parent = nullptr);
    ~TransportModel() override;
};
}
