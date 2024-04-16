/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class TransportTreeViewTest : public QObject
{
    Q_OBJECT
public:
    explicit TransportTreeViewTest(QObject *parent = nullptr);
    ~TransportTreeViewTest() override = default;

private Q_SLOTS:
    void shouldHaveDefaultValues();
};
