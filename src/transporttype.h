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
class TransportTypePrivate;

/*!
  \qmltype transportType
  \inqmlmodule org.kde.mailtransport
  \nativetype MailTransport::TransportType
  \brief Describes an available mail transport type to QML.

  This value is returned by MailTransport::TransportManager and is not
  constructed directly from QML.
*/
/*!
  \class MailTransport::TransportType
  \inmodule KMailTransport
  \inheaderfile MailTransport/TransportType
  \brief A representation of a transport type.

  Represents an available transport type.  SMTP is available

  All available transport types can be retrieved via TransportManager::types().

  \author Constantin Berzan <exit3219@gmail.com>
  \since 4.4
*/
class MAILTRANSPORT_EXPORT TransportType
{
    Q_GADGET

    /*! \qmlproperty bool TransportType::isValid
     * Whether the transport type is valid.
     */
    Q_PROPERTY(bool isValid READ isValid CONSTANT)

    /*! \qmlproperty string TransportType::name
     * The translated name of the transport type.
     */
    Q_PROPERTY(QString name READ name CONSTANT)

    /*! \qmlproperty string TransportType::description
     * A description of the transport type.
     */
    Q_PROPERTY(QString description READ description CONSTANT)

    /*! \qmlproperty string TransportType::identifier
     * The plugin identifier for the transport type.
     */
    Q_PROPERTY(QString identifier READ identifier CONSTANT)

    /*! \qmlproperty bool TransportType::isAkonadiResource
     * Whether this transport is an Akonadi resource.
     */
    Q_PROPERTY(bool isAkonadiResource READ isAkonadiResource CONSTANT)

    friend class AddTransportDialog;
    friend class Transport;
    friend class TransportManager;
    friend class TransportManagerPrivate;

public:
    /*!
      \typealias MailTransport::TransportType::List

      Describes a list of transport types.
    */
    using List = QList<TransportType>;

    /*!
      Constructs a new TransportType.
    */
    TransportType();

    /*!
      Creates a copy of the \a other TransportType.
    */
    TransportType(const TransportType &other);

    /*!
      Moves the \a other TransportType into this one.
    */
    TransportType(TransportType &&other) noexcept;

    /*!
      Destroys the TransportType.
    */
    ~TransportType();

    /*!
     * Replaces the transport type by the \a other.
     */
    TransportType &operator=(const TransportType &other);

    /*!
     * Moves the \a other transport type into this one.
     */
    TransportType &operator=(TransportType &&other) noexcept;

    /*!
     * Compares the transport type with the \a other.
     */
    [[nodiscard]] bool operator==(const TransportType &other) const;

    /*!
      Returns whether the transport type is valid.
    */
    [[nodiscard]] bool isValid() const;

    /*!
      Returns the i18n'ed name of the transport type.
    */
    [[nodiscard]] QString name() const;

    /*!
      Returns a description of the transport type.
    */
    [[nodiscard]] QString description() const;

    /*!
     * Returns a plugin identifier
     */
    [[nodiscard]] QString identifier() const;

    /// Returns whether this transport is an akonadi resource.
    [[nodiscard]] bool isAkonadiResource() const;

private:
    QSharedDataPointer<TransportTypePrivate> d;
};
} // namespace MailTransport

Q_DECLARE_TYPEINFO(MailTransport::TransportType, Q_RELOCATABLE_TYPE);
