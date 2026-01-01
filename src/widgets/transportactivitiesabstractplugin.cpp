/*
  SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportactivitiesabstractplugin.h"

using namespace MailTransport;
TransportActivitiesAbstractPlugin::TransportActivitiesAbstractPlugin(QWidget *parent)
    : QWidget{parent}
{
}

TransportActivitiesAbstractPlugin::~TransportActivitiesAbstractPlugin() = default;

#include "moc_transportactivitiesabstractplugin.cpp"
