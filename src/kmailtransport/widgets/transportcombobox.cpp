/*
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportcombobox.h"
#include "transport.h"
#include "transportmanager.h"

using namespace MailTransport;

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
class TransportComboBoxPrivate
{
public:
    QVector<int> transports;
};

TransportComboBox::TransportComboBox(QWidget *parent)
    : QComboBox(parent)
    , d(new TransportComboBoxPrivate)
{
    updateComboboxList();
    connect(TransportManager::self(), &TransportManager::transportsChanged, this, &TransportComboBox::updateComboboxList);
    connect(TransportManager::self(), &TransportManager::transportRemoved, this, &TransportComboBox::transportRemoved);
}

TransportComboBox::~TransportComboBox()
{
    delete d;
}

int TransportComboBox::currentTransportId() const
{
    if (currentIndex() >= 0 && currentIndex() < d->transports.count()) {
        return d->transports.at(currentIndex());
    }
    return -1;
}

bool TransportComboBox::setCurrentTransport(int transportId)
{
    const int i = d->transports.indexOf(transportId);
    if (i >= 0 && i < count()) {
        setCurrentIndex(i);
        return true;
    }
    return false;
}

QString TransportComboBox::transportType() const
{
    return TransportManager::self()->transportById(currentTransportId())->identifier();
}

void TransportComboBox::updateComboboxList()
{
    const int oldTransport = currentTransportId();
    clear();

    int defaultId = 0;
    if (!TransportManager::self()->isEmpty()) {
        const QStringList listNames = TransportManager::self()->transportNames();
        const QVector<int> listIds = TransportManager::self()->transportIds();
        addItems(listNames);
        setTransportList(listIds);
        defaultId = TransportManager::self()->defaultTransportId();
    }

    if (oldTransport != -1) {
        setCurrentTransport(oldTransport);
    } else {
        setCurrentTransport(defaultId);
    }
}

void TransportComboBox::setTransportList(const QVector<int> &transportList)
{
    d->transports = transportList;
}
