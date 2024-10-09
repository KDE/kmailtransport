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
    TransportComboBoxPrivate(TransportComboBox *qq)
        : q(qq)
        , transportModel(new MailTransport::TransportModel(qq))
        , transportProxyModel(new MailTransport::TransportSortProxyModel(qq))
    {
        transportProxyModel->setSourceModel(transportModel);
        q->setModel(transportProxyModel);
    }
    void updateComboboxList()
    {
        const int oldTransport = q->currentTransportId();
        q->clear();

        int defaultId = 0;
        if (!TransportManager::self()->isEmpty()) {
            defaultId = TransportManager::self()->defaultTransportId();
        }

        if (oldTransport != -1) {
            q->setCurrentTransport(oldTransport);
        } else {
            q->setCurrentTransport(defaultId);
        }
    }
    TransportComboBox *const q;
    MailTransport::TransportModel *const transportModel;
    MailTransport::TransportSortProxyModel *const transportProxyModel;
};

TransportComboBox::TransportComboBox(QWidget *parent)
    : QComboBox(parent)
    , d(new TransportComboBoxPrivate(this))
{
    d->updateComboboxList();
    connect(TransportManager::self(), &TransportManager::transportRemoved, this, &TransportComboBox::transportRemoved);
    setModelColumn(MailTransport::TransportModel::NameRole);
}

TransportComboBox::~TransportComboBox() = default;

int TransportComboBox::currentTransportId() const
{
    return d->transportProxyModel->mapToSource(d->transportProxyModel->index(currentIndex(), MailTransport::TransportModel::TransportIdentifierRole))
        .data()
        .toInt();
}

bool TransportComboBox::setCurrentTransport(int transportId)
{
    const int idx = d->transportModel->indexOf(transportId);
    if (idx != -1) {
        const int newIndex = d->transportProxyModel->mapFromSource(d->transportModel->index(idx, MailTransport::TransportModel::TransportIdentifierRole)).row();
        setCurrentIndex(newIndex);
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

bool TransportComboBox::enablePlasmaActivities() const
{
    return d->transportProxyModel->enablePlasmaActivities();
}

void TransportComboBox::setEnablePlasmaActivities(bool newEnablePlasmaActivities)
{
    d->transportProxyModel->setEnablePlasmaActivities(newEnablePlasmaActivities);
}

#include "moc_transportcombobox.cpp"
