/*
    SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"
#include <QObject>
namespace MailTransport
{
class MAILTRANSPORT_EXPORT TransportActivitiesAbstract : public QObject
{
    Q_OBJECT
public:
    explicit TransportActivitiesAbstract(QObject *parent = nullptr);
    ~TransportActivitiesAbstract() override;

    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const = 0;

    [[nodiscard]] virtual bool hasActivitySupport() const;

Q_SIGNALS:
    void activitiesChanged();
};
}
