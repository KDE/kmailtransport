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
    }
    void updateComboboxList()
    {
        const int oldTransport = q->currentTransportId();
        q->clear();

        int defaultId = 0;
        if (!TransportManager::self()->isEmpty()) {
            const QStringList listNames = TransportManager::self()->transportNames();
            const QList<int> listIds = TransportManager::self()->transportIds();
            q->addItems(listNames);
            transports = listIds;
            defaultId = TransportManager::self()->defaultTransportId();
        }

        if (oldTransport != -1) {
            q->setCurrentTransport(oldTransport);
        } else {
            q->setCurrentTransport(defaultId);
        }
    }
    TransportComboBox *const q;
    QList<int> transports;
    MailTransport::TransportModel *const transportModel;
    MailTransport::TransportSortProxyModel *const transportProxyModel;
};

TransportComboBox::TransportComboBox(QWidget *parent)
    : QComboBox(parent)
    , d(new TransportComboBoxPrivate(this))
{
    d->updateComboboxList();
    connect(TransportManager::self(), &TransportManager::transportsChanged, this, [this]() {
        d->updateComboboxList();
    });
    connect(TransportManager::self(), &TransportManager::transportRemoved, this, &TransportComboBox::transportRemoved);
}

TransportComboBox::~TransportComboBox() = default;

int TransportComboBox::currentTransportId() const
{
#if 0
    return d->mIdentityProxyModel->mapToSource(d->mIdentityProxyModel->index(currentIndex(), MailTransport::TransportModel::TransportIdentifierRole))
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
#if 0
    const int i = findData(transportId, MailTransport::TransportModel::TransportIdentifierRole);
#else
    const int i = d->transports.indexOf(transportId);
    if (i >= 0 && i < count()) {
        setCurrentIndex(i);
        return true;
    }
    return false;
#endif
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
