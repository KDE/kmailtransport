/*
    SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportabstractplugin.h"
#include "mailtransport_debug.h"
using namespace MailTransport;

TransportAbstractPlugin::TransportAbstractPlugin(QObject *parent)
    : QObject(parent)
{
}

TransportAbstractPlugin::~TransportAbstractPlugin()
{
}

void TransportAbstractPlugin::cleanUp(MailTransport::Transport *t)
{
    Q_UNUSED(t)
}

void TransportAbstractPlugin::initializeTransport(MailTransport::Transport *t, const QString &identifier)
{
    Q_UNUSED(identifier)
    Q_UNUSED(t)
}
