/*
    Copyright (c) 2017 Laurent Montel <montel@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#ifndef TRANSPORTABSTRACTPLUGIN_H
#define TRANSPORTABSTRACTPLUGIN_H

#include <QObject>
#include <QVector>
#include "mailtransport_export.h"
namespace MailTransport {
class TransportJob;
class Transport;

struct MAILTRANSPORT_EXPORT TransportAbstractPluginInfo
{
    QString name;
    QString identifier;
    QString description;
    bool isAkonadi = false;
};

class MAILTRANSPORT_EXPORT TransportAbstractPlugin : public QObject
{
    Q_OBJECT
public:
    explicit TransportAbstractPlugin(QObject *parent = nullptr);
    ~TransportAbstractPlugin();

    virtual TransportJob *createTransportJob(MailTransport::Transport *t, const QString &identifier) = 0;
    virtual QVector<TransportAbstractPluginInfo> names() const = 0;
    virtual bool configureTransport(const QString &identifier, Transport *transport, QWidget *parent) = 0;
    virtual void cleanUp(const QString &identifier);
    virtual void initializeTransport(MailTransport::Transport *t, const QString &identifier);
};
}
Q_DECLARE_TYPEINFO(MailTransport::TransportAbstractPluginInfo, Q_MOVABLE_TYPE);

#endif // TRANSPORTABSTRACTPLUGIN_H
