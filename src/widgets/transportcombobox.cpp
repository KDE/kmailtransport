/*
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportcombobox.h"
#include "transportmanager.h"
#include "transportmodel.h"
#include "transportsortproxymodel.h"

using namespace MailTransport;

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
class TransportComboBoxPrivate
{
public:
    QList<int> transports;
    MailTransport::TransportModel *transportModel = nullptr;
    MailTransport::TransportSortProxyModel *transportProxyModel = nullptr;
};

TransportComboBox::TransportComboBox(QWidget *parent)
    : QComboBox(parent)
    , d(new TransportComboBoxPrivate)
{
    d->transportModel = new MailTransport::TransportModel(this);
    d->transportProxyModel = new MailTransport::TransportSortProxyModel(this);
    d->transportProxyModel->setSourceModel(d->transportModel);

    updateComboboxList();
    connect(TransportManager::self(), &TransportManager::transportsChanged, this, &TransportComboBox::updateComboboxList);
    connect(TransportManager::self(), &TransportManager::transportRemoved, this, &TransportComboBox::transportRemoved);
}

TransportComboBox::~TransportComboBox() = default;

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

TransportActivitiesAbstract *TransportComboBox::transportActivitiesAbstract() const
{
    return d->transportProxyModel->transportActivitiesAbstract();
}

void TransportComboBox::setTransportActivitiesAbstract(TransportActivitiesAbstract *activitiesAbstract)
{
    d->transportProxyModel->setTransportActivitiesAbstract(activitiesAbstract);
}

void TransportComboBox::updateComboboxList()
{
    const int oldTransport = currentTransportId();
    clear();

    int defaultId = 0;
    if (!TransportManager::self()->isEmpty()) {
        const QStringList listNames = TransportManager::self()->transportNames();
        const QList<int> listIds = TransportManager::self()->transportIds();
        addItems(listNames);
        d->transports = listIds;
        defaultId = TransportManager::self()->defaultTransportId();
    }

    if (oldTransport != -1) {
        setCurrentTransport(oldTransport);
    } else {
        setCurrentTransport(defaultId);
    }
}

#include "moc_transportcombobox.cpp"
