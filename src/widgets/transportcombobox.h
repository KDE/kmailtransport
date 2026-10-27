/*
  SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"
#include "transport.h"

#include <QComboBox>

#include <memory>

namespace MailTransport
{
class TransportComboBoxPrivate;
class TransportActivitiesAbstract;
/*!
  \class MailTransport::TransportComboBox
  \inmodule KMailTransport
  \inheaderfile MailTransport/TransportComboBox
  A combo-box for selecting a mail transport.
  It is updated automatically when transports are added, changed, or removed.
*/
class MAILTRANSPORT_EXPORT TransportComboBox : public QComboBox
{
    Q_OBJECT
public:
    /*!
      \brief Creates a new mail transport selection combo box.
      \a parent The paren widget.
    */
    explicit TransportComboBox(QWidget *parent = nullptr);

    /*!
      Destroys the combo box.
    */
    ~TransportComboBox() override;

    /**
      Returns identifier of the currently selected mail transport.
    */
    [[nodiscard]] int currentTransportId() const;

    /**
      Selects the given transport.
      @param transportId The transport identifier.
      @return return true if we success to find transportId
    */
    bool setCurrentTransport(int transportId);

    /**
      Returns the type of the selected transport.
    */
    [[nodiscard]] QString transportType() const;

    /// \since 6.1
    [[nodiscard]] MailTransport::TransportActivitiesAbstract *transportActivitiesAbstract() const;
    /// \since 6.1
    void setTransportActivitiesAbstract(MailTransport::TransportActivitiesAbstract *activitiesAbstract);

    /*!
     * Returns \c true if plasma activities filtering is enabled.
     */
    [[nodiscard]] bool enablePlasmaActivities() const;
    /*!
     * Enables or disables plasma activities filtering, according to \a newEnablePlasmaActivities.
     */
    void setEnablePlasmaActivities(bool newEnablePlasmaActivities);

Q_SIGNALS:
    /*!
     * Emitted when the transport \a id named \a name is removed.
     */
    void transportRemoved(Transport::Id id, const QString &name);

private:
    std::unique_ptr<TransportComboBoxPrivate> const d;
};
} // namespace MailTransport
