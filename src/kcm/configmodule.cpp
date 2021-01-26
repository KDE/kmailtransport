/*
    SPDX-FileCopyrightText: 2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "configmodule.h"
#include "widgets/transportmanagementwidget.h"

#include <KPluginFactory>

#include <QVBoxLayout>

using namespace MailTransport;

K_PLUGIN_FACTORY(MailTransportConfigFactory, registerPlugin<ConfigModule>();)

ConfigModule::ConfigModule(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
{
    setButtons(NoAdditionalButton);
    auto l = new QVBoxLayout(this);
    l->setContentsMargins({});
    auto tmw = new TransportManagementWidget(this);
    l->addWidget(tmw);
}

#include "configmodule.moc"
