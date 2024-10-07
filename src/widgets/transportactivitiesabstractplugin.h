/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "mailtransport_export.h"
#include <QWidget>
namespace MailTransport
{
class MAILTRANSPORT_EXPORT TransportActivitiesAbstractPlugin : public QWidget
{
    Q_OBJECT
public:
    explicit TransportActivitiesAbstractPlugin(QWidget *parent = nullptr);
    ~TransportActivitiesAbstractPlugin() override;
};
}
