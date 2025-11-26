/*
    SPDX-FileCopyrightText: 2024 g10 Code GmbH
    SPDX-FileContributor: Daniel Vrátil <dvratil@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

namespace MailTransport
{

class Transport;

class XOAuthPasswordRequester : public QObject
{
    Q_OBJECT

public:
    enum Result {
        Error = 0,
        PasswordRetrieved = 1,
    };
    Q_ENUM(Result);

    explicit XOAuthPasswordRequester(Transport *transport, QObject *parent = nullptr);
    ~XOAuthPasswordRequester() override;

    virtual void requestPassword(bool forceRefresh) = 0;

Q_SIGNALS:
    void done(MailTransport::XOAuthPasswordRequester::Result result, const QString &password);

protected:
    Transport *transport() const;

private:
    Transport *const mTransport;
};

XOAuthPasswordRequester *createXOAuthPasswordRequester(Transport *transport, QObject *parent = nullptr);

} // namespace
