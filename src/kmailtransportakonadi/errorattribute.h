/*
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <mailtransportakonadi_export.h>

#include <QString>

#include <attribute.h>

namespace MailTransport
{
/**
 * @short An Attribute to mark messages that failed to be sent.
 *
 * This attribute contains the error message encountered.
 *
 * @author Constantin Berzan <exit3219@gmail.com>
 * @since 4.4
 */
class MAILTRANSPORTAKONADI_EXPORT ErrorAttribute : public Akonadi::Attribute
{
public:
    /**
     * Creates a new error attribute.
     *
     * @param msg The i18n'ed error message.
     */
    explicit ErrorAttribute(const QString &msg = QString());

    /**
     * Destroys the error attribute.
     */
    ~ErrorAttribute() override;

    /**
     * Returns the i18n'ed error message.
     */
    Q_REQUIRED_RESULT QString message() const;

    /**
     * Sets the i18n'ed error message.
     */
    void setMessage(const QString &msg);

    /* reimpl */
    ErrorAttribute *clone() const override;
    Q_REQUIRED_RESULT QByteArray type() const override;
    Q_REQUIRED_RESULT QByteArray serialized() const override;
    void deserialize(const QByteArray &data) override;

private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond
};
}

