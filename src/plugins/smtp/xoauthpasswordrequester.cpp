/*
    SPDX-FileCopyrightText: 2024 g10 Code GmbH
    SPDX-FileContributor: Daniel Vrátil <dvratil@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "xoauthpasswordrequester.h"
#include "gmailpasswordrequester.h"
#include "outlookpasswordrequester.h"
#include "transport.h"
#include "transportbase.h"

using namespace MailTransport;

namespace
{

bool isGmail(const QString &server)
{
    return server.endsWith(u".gmail.com") || server.endsWith(u".googlemail.com");
}

bool isOutlook(const QString &server)
{
    return server.endsWith(u".outlook.com") || server.endsWith(u".office365.com") || server.endsWith(u".hotmail.com");
}

} // namespace

XOAuthPasswordRequester::XOAuthPasswordRequester(Transport *transport, QObject *parent)
    : QObject(parent)
    , mTransport(transport)
{
}

XOAuthPasswordRequester::~XOAuthPasswordRequester() = default;

Transport *XOAuthPasswordRequester::transport() const
{
    return mTransport;
}

namespace MailTransport
{

XOAuthPasswordRequester *createXOAuthPasswordRequester(Transport *transport, QObject *parent)
{
    if (transport->authenticationType() == Transport::EnumAuthenticationType::XOAUTH2) {
        if (isGmail(transport->host())) {
            return new GmailPasswordRequester(transport, parent);
        }
        if (isOutlook(transport->host())) {
            return new OutlookPasswordRequester(transport, parent);
        }
    }

    return nullptr;
}

}

#include "moc_xoauthpasswordrequester.cpp"
