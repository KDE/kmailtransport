/*
    SPDX-FileCopyrightText: 2007 Volker Krause <vkrause@kde.org>

    Based on KMail code by:
    SPDX-FileCopyrightText: 1996-1998 Stefan Taferner <taferner@kde.org>
    SPDX-FileCopyrightText: 2000-2002 Michael Haeckel <haeckel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mailtransport_export.h"

#include <KJob>

class PreCommandJobPrivate;

namespace MailTransport
{
/**
  Job to execute a command.
  This is used often for sending or receiving mails, for example to set up
  a tunnel of VPN connection.
  Basically this is just a KJob wrapper around a QProcess.

  @since 4.4
 */
class MAILTRANSPORT_EXPORT PrecommandJob : public KJob
{
    Q_OBJECT

public:
    /**
      Creates a new precommand job.
      @param precommand The command to run.
      @param parent The parent object.
    */
    explicit PrecommandJob(const QString &precommand, QObject *parent = nullptr);

    /**
      Destroys this job.
    */
    ~PrecommandJob() override;

    /**
      Executes the precommand.
      Reimplemented from KJob.
    */
    void start() override;

protected:
    /**
      Reimplemented from KJob.
    */
    bool doKill() override;

private:
    friend class ::PreCommandJobPrivate;
    PreCommandJobPrivate *const d;
};
} // namespace MailTransport

