/*
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportmgr.h"

#include <QVBoxLayout>
#include <transport.h>
#include <transportjob.h>
#include <transportmanager.h>
#include <widgets/transportmanagementwidget.h>

#include <KTextEdit>
#include <QApplication>
#include <QDebug>
#include <QLineEdit>

#include <QPushButton>

using namespace MailTransport;

TransportMgr::TransportMgr()
    : mCurrentJob(nullptr)
{
    auto vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0, 0, 0, 0);

    vbox->addWidget(new TransportManagementWidget(this));
    mComboBox = new TransportComboBox(this);
    mComboBox->setEditable(true);
    vbox->addWidget(mComboBox);
    auto b = new QPushButton(QStringLiteral("&Edit"), this);
    vbox->addWidget(b);
    connect(b, &QPushButton::clicked, this, &TransportMgr::editBtnClicked);
    b = new QPushButton(QStringLiteral("&Remove all transports"), this);
    vbox->addWidget(b);
    connect(b, &QPushButton::clicked, this, &TransportMgr::removeAllBtnClicked);
    mSenderEdit = new QLineEdit(this);
    mSenderEdit->setPlaceholderText(QStringLiteral("Sender"));
    vbox->addWidget(mSenderEdit);
    mToEdit = new QLineEdit(this);
    mToEdit->setPlaceholderText(QStringLiteral("To"));
    vbox->addWidget(mToEdit);
    mCcEdit = new QLineEdit(this);
    mCcEdit->setPlaceholderText(QStringLiteral("Cc"));
    vbox->addWidget(mCcEdit);
    mBccEdit = new QLineEdit(this);
    mBccEdit->setPlaceholderText(QStringLiteral("Bcc"));
    vbox->addWidget(mBccEdit);
    mMailEdit = new KTextEdit(this);
    mMailEdit->setAcceptRichText(false);
    mMailEdit->setLineWrapMode(QTextEdit::NoWrap);
    vbox->addWidget(mMailEdit);
    b = new QPushButton(QStringLiteral("&Send"), this);
    connect(b, &QPushButton::clicked, this, &TransportMgr::sendBtnClicked);
    vbox->addWidget(b);
    b = new QPushButton(QStringLiteral("&Cancel"), this);
    connect(b, &QPushButton::clicked, this, &TransportMgr::cancelBtnClicked);
    vbox->addWidget(b);
}

void TransportMgr::removeAllBtnClicked()
{
    MailTransport::TransportManager *manager = MailTransport::TransportManager::self();
    const QList<Transport *> transports = manager->transports();
    for (int i = 0; i < transports.count(); i++) {
        MailTransport::Transport *transport = transports.at(i);
        qDebug() << transport->host();
        manager->removeTransport(transport->id());
    }
}

void TransportMgr::editBtnClicked()
{
    const int index = mComboBox->currentTransportId();
    if (index < 0) {
        return;
    }
    const auto transport = TransportManager::self()->transportById(index);
    TransportManager::self()->configureTransport(transport->identifier(), transport, this);
}

void TransportMgr::sendBtnClicked()
{
    TransportJob *job = TransportManager::self()->createTransportJob(mComboBox->currentTransportId());
    if (!job) {
        qDebug() << "Invalid transport!";
        return;
    }
    job->setSender(mSenderEdit->text());
    job->setTo(mToEdit->text().isEmpty() ? QStringList() : mToEdit->text().split(QLatin1Char(',')));
    job->setCc(mCcEdit->text().isEmpty() ? QStringList() : mCcEdit->text().split(QLatin1Char(',')));
    job->setBcc(mBccEdit->text().isEmpty() ? QStringList() : mBccEdit->text().split(QLatin1Char(',')));
    job->setData(mMailEdit->document()->toPlainText().toLatin1());
    connect(job, &KJob::result, this, &TransportMgr::jobResult);
    connect(job, SIGNAL(percent(KJob *, ulong)), SLOT(jobPercent(KJob *, ulong)));
    connect(job, &KJob::infoMessage, this, &TransportMgr::jobInfoMessage);
    mCurrentJob = job;
    TransportManager::self()->schedule(job);
}

void TransportMgr::cancelBtnClicked()
{
    if (mCurrentJob) {
        qDebug() << "kill success:" << mCurrentJob->kill();
    }
    mCurrentJob = nullptr;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("transportmgr"));

    auto t = new TransportMgr();
    t->show();
    app.exec();
    delete t;
}

void TransportMgr::jobResult(KJob *job)
{
    qDebug() << job->error() << job->errorText();
    mCurrentJob = nullptr;
}

void TransportMgr::jobPercent(KJob *job, unsigned long percent)
{
    Q_UNUSED(job)
    qDebug() << percent << "%";
}

void TransportMgr::jobInfoMessage(KJob *job, const QString &info, const QString &info2)
{
    Q_UNUSED(job)
    qDebug() << info;
    qDebug() << info2;
}
