/*
  SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <mailtransport_export.h>
#include <transportbase.h>

#include <QComboBox>

class TransportComboBoxPrivate;

namespace MailTransport
{
/**
  A combo-box for selecting a mail transport.
  It is updated automatically when transports are added, changed, or removed.
*/
class MAILTRANSPORT_EXPORT TransportComboBox : public QComboBox
{
    Q_OBJECT
public:
    /**
      Creates a new mail transport selection combo box.
      @param parent The paren widget.
    */
    explicit TransportComboBox(QWidget *parent = nullptr);

    ~TransportComboBox() override;

    /**
      Returns identifier of the currently selected mail transport.
    */
    Q_REQUIRED_RESULT int currentTransportId() const;

    /**
      Selects the given transport.
      @param transportId The transport identifier.
      @return return true if we success to find transportId
    */
    bool setCurrentTransport(int transportId);

    /**
      Returns the type of the selected transport.
    */
    Q_REQUIRED_RESULT QString transportType() const;

Q_SIGNALS:
    void transportRemoved(int id, const QString &name);

protected:
    void setTransportList(const QVector<int> &transportList);

public Q_SLOTS:
    /**
     * @since 4.11
     */
    void updateComboboxList();

private:
    TransportComboBoxPrivate *const d;
};
} // namespace MailTransport

