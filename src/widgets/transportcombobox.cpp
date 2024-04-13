/*
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportcombobox.h"
#include "transportmanager.h"
#include "transportmodel.h"
#include "transportsortproxymodel.h"
// #define PORT_MODEL 1

using namespace MailTransport;

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
class TransportComboBoxPrivate
{
public:
    TransportComboBoxPrivate(TransportComboBox *qq)
        : q(qq)
        , transportModel(new MailTransport::TransportModel(qq))
        , transportProxyModel(new MailTransport::TransportSortProxyModel(qq))
    {
        transportProxyModel->setSourceModel(transportModel);
#ifdef PORT_MODEL
        q->setModel(transportProxyModel);
#endif
    }
    void updateComboboxList()
    {
        const int oldTransport = q->currentTransportId();
        q->clear();

        int defaultId = 0;
        if (!TransportManager::self()->isEmpty()) {
#ifndef PORT_MODEL
            const QStringList listNames = TransportManager::self()->transportNames();
            const QList<int> listIds = TransportManager::self()->transportIds();
            q->addItems(listNames);
            transports = listIds;
#endif
            defaultId = TransportManager::self()->defaultTransportId();
        }

        if (oldTransport != -1) {
            q->setCurrentTransport(oldTransport);
        } else {
            q->setCurrentTransport(defaultId);
        }
    }
    TransportComboBox *const q;
#ifndef PORT_MODEL
    QList<int> transports;
#endif
    MailTransport::TransportModel *const transportModel;
    MailTransport::TransportSortProxyModel *const transportProxyModel;
};

TransportComboBox::TransportComboBox(QWidget *parent)
    : QComboBox(parent)
    , d(new TransportComboBoxPrivate(this))
{
    d->updateComboboxList();
#ifndef PORT_MODEL
    connect(TransportManager::self(), &TransportManager::transportsChanged, this, [this]() {
        d->updateComboboxList();
    });
#endif
    connect(TransportManager::self(), &TransportManager::transportRemoved, this, &TransportComboBox::transportRemoved);
#ifdef PORT_MODEL
    setModelColumn(MailTransport::TransportModel::NameRole);
#endif
}

TransportComboBox::~TransportComboBox() = default;

int TransportComboBox::currentTransportId() const
{
#ifdef PORT_MODEL
    return d->transportProxyModel->mapToSource(d->transportProxyModel->index(currentIndex(), MailTransport::TransportModel::TransportIdentifierRole))
        .data()
        .toInt();
#else
    if (currentIndex() >= 0 && currentIndex() < d->transports.count()) {
        return d->transports.at(currentIndex());
    }
    return -1;
#endif
}

bool TransportComboBox::setCurrentTransport(int transportId)
{
#ifdef PORT_MODEL
    const int idx = d->transportModel->indexOf(transportId);
    if (idx != -1) {
        const int newIndex = d->transportProxyModel->mapFromSource(d->transportModel->index(idx, MailTransport::TransportModel::TransportIdentifierRole)).row();
        setCurrentIndex(newIndex);
        return true;
    }
#else
    const int i = d->transports.indexOf(transportId);
    if (i >= 0 && i < count()) {
        setCurrentIndex(i);
        return true;
    }
#endif
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

#include "moc_transportcombobox.cpp"
