/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"
#include "transport.h"

#include <QString>

namespace MailTransport
{
class AddTransportDialog;
class TransportManager;

/**
  @short A representation of a transport type.

  Represents an available transport type.  SMTP is available

  All available transport types can be retrieved via TransportManager::types().

  @author Constantin Berzan <exit3219@gmail.com>
  @since 4.4
*/
class MAILTRANSPORT_EXPORT TransportType
{
    friend class AddTransportDialog;
    friend class Transport;
    friend class TransportManager;
    friend class TransportManagerPrivate;

public:
    /**
      Describes a list of transport types.
    */
    using List = QVector<TransportType>;

    /**
      Constructs a new TransportType.
    */
    TransportType();

    /**
      Creates a copy of the @p other TransportType.
    */
    TransportType(const TransportType &other);

    /**
      Destroys the TransportType.
    */
    ~TransportType();

    /**
     * Replaces the transport type by the @p other.
     */
    TransportType &operator=(const TransportType &other);

    /**
     * Compares the transport type with the @p other.
     */
    Q_REQUIRED_RESULT bool operator==(const TransportType &other) const;

    /**
      Returns whether the transport type is valid.
    */
    Q_REQUIRED_RESULT bool isValid() const;

    /**
      Returns the i18n'ed name of the transport type.
    */
    Q_REQUIRED_RESULT QString name() const;

    /**
      Returns a description of the transport type.
    */
    Q_REQUIRED_RESULT QString description() const;

    /**
     * Returns a plugin identifier
     */
    Q_REQUIRED_RESULT QString identifier() const;

    Q_REQUIRED_RESULT bool isAkonadiResource() const;

private:
    //@cond PRIVATE
    class Private;
    QSharedDataPointer<Private> d;
    //@endcond
};
} // namespace MailTransport

Q_DECLARE_METATYPE(MailTransport::TransportType)
Q_DECLARE_TYPEINFO(MailTransport::TransportType, Q_MOVABLE_TYPE);

