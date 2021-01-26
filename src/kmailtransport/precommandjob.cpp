/*
    SPDX-FileCopyrightText: 2007 Volker Krause <vkrause@kde.org>

    Based on KMail code by:
    SPDX-FileCopyrightText: 1996-1998 Stefan Taferner <taferner@kde.org>
    SPDX-FileCopyrightText: 2000-2002 Michael Haeckel <haeckel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "precommandjob.h"

#include <KLocalizedString>

#include <QProcess>

using namespace MailTransport;

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
class PreCommandJobPrivate
{
public:
    PreCommandJobPrivate(PrecommandJob *parent);
    QProcess *process = nullptr;
    QString precommand;
    PrecommandJob *const q;

    // Slots
    void slotFinished(int, QProcess::ExitStatus);
    void slotStarted();
    void slotError(QProcess::ProcessError error);
};

PreCommandJobPrivate::PreCommandJobPrivate(PrecommandJob *parent)
    : q(parent)
{
}

PrecommandJob::PrecommandJob(const QString &precommand, QObject *parent)
    : KJob(parent)
    , d(new PreCommandJobPrivate(this))
{
    d->precommand = precommand;
    d->process = new QProcess(this);
    connect(d->process, &QProcess::started, this, [this]() {
        d->slotStarted();
    });
    connect(d->process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        d->slotError(error);
    });
    connect(d->process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [this](int val, QProcess::ExitStatus status) {
        d->slotFinished(val, status);
    });
}

PrecommandJob::~PrecommandJob()
{
    delete d;
}

void PrecommandJob::start()
{
    d->process->start(d->precommand, QStringList());
}

void PreCommandJobPrivate::slotStarted()
{
    Q_EMIT q->infoMessage(q, i18n("Executing precommand"), i18n("Executing precommand '%1'.", precommand));
}

void PreCommandJobPrivate::slotError(QProcess::ProcessError error)
{
    q->setError(KJob::UserDefinedError);
    if (error == QProcess::FailedToStart) {
        q->setErrorText(i18n("Unable to start precommand '%1'.", precommand));
    } else {
        q->setErrorText(i18n("Error while executing precommand '%1'.", precommand));
    }
    q->emitResult();
}

bool PrecommandJob::doKill()
{
    delete d->process;
    d->process = nullptr;
    return true;
}

void PreCommandJobPrivate::slotFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit) {
        q->setError(KJob::UserDefinedError);
        q->setErrorText(i18n("The precommand crashed."));
    } else if (exitCode != 0) {
        q->setError(KJob::UserDefinedError);
        q->setErrorText(i18n("The precommand exited with code %1.", process->exitStatus()));
    }
    q->emitResult();
}

#include "moc_precommandjob.cpp"
