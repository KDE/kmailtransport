/*
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "queuer.h"

#include <QDateTime>
#include <QPointer>
#include <QPushButton>

#include <KTextEdit>
#include <QApplication>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <control.h>

#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>

#include <kmailtransportakonadi/dispatchmodeattribute.h>
#include <kmailtransportakonadi/messagequeuejob.h>
#include <transport.h>
#include <transportmanager.h>

using namespace KMime;
using namespace MailTransport;

MessageQueuer::MessageQueuer()
{
    if (!Akonadi::Control::start()) {
        qFatal("Could not start Akonadi server.");
    }
    auto vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0, 0, 0, 0);

    mComboBox = new TransportComboBox(this);
    mComboBox->setEditable(true);
    vbox->addWidget(mComboBox);
    mSenderEdit = new QLineEdit(this);
    mSenderEdit->setPlaceholderText(QStringLiteral("Sender"));
    vbox->addWidget(mSenderEdit);
    mToEdit = new QLineEdit(this);
    mToEdit->setText(QStringLiteral("idanoka@gmail.com"));
    vbox->addWidget(mToEdit);
    mToEdit->setPlaceholderText(QStringLiteral("To"));
    mCcEdit = new QLineEdit(this);
    vbox->addWidget(mCcEdit);
    mCcEdit->setPlaceholderText(QStringLiteral("Cc"));
    mBccEdit = new QLineEdit(this);
    mBccEdit->setPlaceholderText(QStringLiteral("Bcc"));
    vbox->addWidget(mBccEdit);
    mMailEdit = new KTextEdit(this);
    mMailEdit->setText(QStringLiteral("test from queuer!"));
    mMailEdit->setAcceptRichText(false);
    mMailEdit->setLineWrapMode(QTextEdit::NoWrap);
    vbox->addWidget(mMailEdit);
    auto b = new QPushButton(QStringLiteral("&Send Now"), this);
    vbox->addWidget(b);
    connect(b, &QPushButton::clicked, this, &MessageQueuer::sendNowClicked);
    b = new QPushButton(QStringLiteral("Send &Queued"), this);
    vbox->addWidget(b);
    connect(b, &QPushButton::clicked, this, &MessageQueuer::sendQueuedClicked);
    b = new QPushButton(QStringLiteral("Send on &Date..."), this);
    vbox->addWidget(b);
    connect(b, &QPushButton::clicked, this, &MessageQueuer::sendOnDateClicked);
}

void MessageQueuer::sendNowClicked()
{
    MessageQueueJob *qjob = createQueueJob();
    qDebug() << "DispatchMode default (Automatic).";
    qjob->start();
}

void MessageQueuer::sendQueuedClicked()
{
    MessageQueueJob *qjob = createQueueJob();
    qDebug() << "DispatchMode Manual.";
    qjob->dispatchModeAttribute().setDispatchMode(DispatchModeAttribute::Manual);
    qjob->start();
}

void MessageQueuer::sendOnDateClicked()
{
    QPointer<QDialog> dialog = new QDialog(this);
    auto layout = new QVBoxLayout(dialog);
    auto dt = new QDateTimeEdit(dialog);
    dt->setDateTime(QDateTime::currentDateTime());
    dt->setDisplayFormat(QStringLiteral("hh:mm:ss"));
    layout->addWidget(dt);
    auto box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
    connect(box, &QDialogButtonBox::accepted, dialog.data(), &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, dialog.data(), &QDialog::reject);
    layout->addWidget(box);
    if (!dialog->exec() || !dialog) {
        return;
    }
    qDebug() << "DispatchMode AfterDueDate" << dt->dateTime();
    MessageQueueJob *qjob = createQueueJob();
    qjob->dispatchModeAttribute().setDispatchMode(DispatchModeAttribute::Automatic);
    qjob->dispatchModeAttribute().setSendAfter(dt->dateTime());
    qjob->start();
    delete dialog;
}

MessageQueueJob *MessageQueuer::createQueueJob()
{
    Message::Ptr msg = Message::Ptr(new Message);
    // No headers; need a '\n' to separate headers from body.
    // TODO: use real headers
    msg->setContent(QByteArray("\n") + mMailEdit->document()->toPlainText().toLatin1());
    qDebug() << "msg:" << msg->encodedContent(true);

    auto job = new MessageQueueJob();
    job->setMessage(msg);
    job->transportAttribute().setTransportId(mComboBox->currentTransportId());
    // default dispatch mode
    // default sent-mail collection
    job->addressAttribute().setFrom(mSenderEdit->text());
    job->addressAttribute().setTo(mToEdit->text().isEmpty() ? QStringList() : mToEdit->text().split(QLatin1Char(',')));
    job->addressAttribute().setCc(mCcEdit->text().isEmpty() ? QStringList() : mCcEdit->text().split(QLatin1Char(',')));
    job->addressAttribute().setBcc(mBccEdit->text().isEmpty() ? QStringList() : mBccEdit->text().split(QLatin1Char(',')));

    connect(job, &MessageQueueJob::result, this, &MessageQueuer::jobResult);
    connect(job, SIGNAL(percent(KJob *, ulong)), SLOT(jobPercent(KJob *, ulong)));
    connect(job, &KJob::infoMessage, this, &MessageQueuer::jobInfoMessage);

    return job;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("messagequeuer"));

    auto t = new MessageQueuer();
    t->show();
    app.exec();
    delete t;
}

void MessageQueuer::jobResult(KJob *job)
{
    if (job->error()) {
        qDebug() << "job error:" << job->errorText();
    } else {
        qDebug() << "job success.";
    }
}

void MessageQueuer::jobPercent(KJob *job, unsigned long percent)
{
    Q_UNUSED(job)
    qDebug() << percent << "%";
}

void MessageQueuer::jobInfoMessage(KJob *job, const QString &info, const QString &info2)
{
    Q_UNUSED(job)
    qDebug() << info;
    qDebug() << info2;
}
