/*
    SPDX-FileCopyrightText: 2007 Volker Krause <vkrause@kde.org>

    Based on KMail code by:
    SPDX-FileCopyrightText: 1996-1998 Stefan Taferner <taferner@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KSMTP/Session>
#include <transportjob.h>

namespace KIO
{
class Job;
class Slave;
}

namespace KGAPI2
{
class AccountPromise;
}

class SmtpJobPrivate;

namespace MailTransport
{
/**
  Mail transport job for SMTP.
  Internally, all jobs for a specific transport are queued to use the same
  KIO::Slave. This avoids multiple simultaneous connections to the server,
  which is not always allowed. Also, re-using an already existing connection
  avoids the login overhead and can improve performance.

  Precommands are automatically executed, once per opening a connection to the
  server (not necessarily once per message).
*/
class SmtpJob : public TransportJob
{
    Q_OBJECT
public:
    /**
      Creates a SmtpJob.
      @param transport The transport settings.
      @param parent The parent object.
    */
    explicit SmtpJob(Transport *transport, QObject *parent = nullptr);

    /**
      Deletes this job.
    */
    ~SmtpJob() override;

protected:
    void doStart() override;
    bool doKill() override;

protected Q_SLOTS:
    void slotResult(KJob *job) override;
    void sessionStateChanged(KSmtp::Session::State state);

private:
    void startPasswordRetrieval(bool forceRefresh = false);
    void onTokenRequestFinished(KGAPI2::AccountPromise *result);
    void startSmtpJob();
    void startLoginJob();
    void startSendJob();

private:
    friend class ::SmtpJobPrivate;
    SmtpJobPrivate *const d;
};
} // namespace MailTransport

