/*
    Copyright (c) 2010 Volker Krause <vkrause@kde.org>

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

#ifndef KIOSMTP_FAKESESSION_H
#define KIOSMTP_FAKESESSION_H

#include "smtpsessioninterface.h"

#include <QStringList>
#include <kio/slavebase.h>

namespace KioSMTP {
class FakeSession : public SMTPSessionInterface
{
public:
    FakeSession()
    {
        clear();
    }

    //
    // public members to control the API emulation below:
    //
    bool startTLSReturnCode;
    bool usesTLS; // ### unused below, most likely something wrong in the tests...
    int lastErrorCode;
    QString lastErrorMessage;
    QString lastMessageBoxText;
    QByteArray nextData;
    int nextDataReturnCode;
    QStringList caps;

    bool eightBitMime;
    bool lf2crlfAndDotStuff;
    bool pipelining;
    QString saslMethod;

    void clear()
    {
        startTLSReturnCode = true;
        usesTLS = false;
        lastErrorCode = 0;
        lastErrorMessage.clear();
        lastMessageBoxText.clear();
        nextData.resize(0);
        nextDataReturnCode = -1;
        caps.clear();

        lf2crlfAndDotStuff = false;
        saslMethod.clear();
    }

    //
    // emulated API:
    //
    bool startSsl() override
    {
        return startTLSReturnCode;
    }

    bool haveCapability(const char *cap) const override
    {
        return caps.contains(QLatin1String(cap));
    }

    void error(int id, const QString &msg) override
    {
        lastErrorCode = id;
        lastErrorMessage = msg;
        qWarning() << id << msg;
    }

    void informationMessageBox(const QString &msg, const QString &caption) override
    {
        Q_UNUSED(caption);
        lastMessageBoxText = msg;
    }

    bool openPasswordDialog(KIO::AuthInfo &) override
    {
        return true;
    }

    void dataReq() override
    {
        /* noop */
    }

    int readData(QByteArray &ba) override
    {
        ba = nextData;
        return nextDataReturnCode;
    }

    bool lf2crlfAndDotStuffingRequested() const override
    {
        return lf2crlfAndDotStuff;
    }

    QString requestedSaslMethod() const override
    {
        return saslMethod;
    }

    TLSRequestState tlsRequested() const override
    {
        return SMTPSessionInterface::UseTLSIfAvailable;
    }
};
}

#include "smtpsessioninterface.cpp"
#include "capabilities.cpp"

#endif
