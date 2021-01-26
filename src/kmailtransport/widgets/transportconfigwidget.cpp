/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    Based on MailTransport code by:
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2007 KovoKs <kovoks@kovoks.nl>

    Based on KMail code by:
    SPDX-FileCopyrightText: 2001-2002 Michael Haeckel <haeckel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportconfigwidget.h"
#include "transport.h"
#include "transportconfigwidget_p.h"
#include "transportmanager.h"

#include "mailtransport_debug.h"
#include <KConfigDialogManager>

using namespace MailTransport;

TransportConfigWidget::TransportConfigWidget(Transport *transport, QWidget *parent)
    : QWidget(parent)
    , d_ptr(new TransportConfigWidgetPrivate)
{
    init(transport);
}

TransportConfigWidget::TransportConfigWidget(TransportConfigWidgetPrivate &dd, Transport *transport, QWidget *parent)
    : QWidget(parent)
    , d_ptr(&dd)
{
    init(transport);
}

TransportConfigWidget::~TransportConfigWidget()
{
    delete d_ptr;
}

void TransportConfigWidget::init(Transport *transport)
{
    Q_D(TransportConfigWidget);
    qCDebug(MAILTRANSPORT_LOG) << "this" << this << "d" << d;
    Q_ASSERT(transport);
    d->transport = transport;

    d->manager = new KConfigDialogManager(this, transport);
}

KConfigDialogManager *TransportConfigWidget::configManager() const
{
    Q_D(const TransportConfigWidget);
    Q_ASSERT(d->manager);
    return d->manager;
}

void TransportConfigWidget::apply()
{
    Q_D(TransportConfigWidget);
    d->manager->updateSettings();
    d->transport->forceUniqueName();
    d->transport->save();
    qCDebug(MAILTRANSPORT_LOG) << "Config written.";
}
