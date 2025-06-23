/*
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "transportmgr.h"
using namespace Qt::Literals::StringLiterals;

#include "transport.h"
#include "transportjob.h"
#include "transportmanager.h"
#include "widgets/transportmanagementwidgetng.h"
#include <QVBoxLayout>

#include <QApplication>
#include <QDebug>
#include <QLineEdit>

#include <QPlainTextEdit>
#include <QPushButton>

using namespace MailTransport;

TransportMgr::TransportMgr()

{
    auto vbox = new QVBoxLayout(this);

    vbox->addWidget(new TransportManagementWidgetNg(this));
    mComboBox = new TransportComboBox(this);
    mComboBox->setEditable(true);
    vbox->addWidget(mComboBox);
    auto b = new QPushButton(u"&Edit"_s, this);
    vbox->addWidget(b);
    connect(b, &QPushButton::clicked, this, &TransportMgr::editBtnClicked);
    b = new QPushButton(u"&Remove all transports"_s, this);
    vbox->addWidget(b);
    connect(b, &QPushButton::clicked, this, &TransportMgr::removeAllBtnClicked);
    mSenderEdit = new QLineEdit(this);
    mSenderEdit->setPlaceholderText(u"Sender"_s);
    vbox->addWidget(mSenderEdit);
    mToEdit = new QLineEdit(this);
    mToEdit->setPlaceholderText(u"To"_s);
    vbox->addWidget(mToEdit);
    mCcEdit = new QLineEdit(this);
    mCcEdit->setPlaceholderText(u"Cc"_s);
    vbox->addWidget(mCcEdit);
    mBccEdit = new QLineEdit(this);
    mBccEdit->setPlaceholderText(u"Bcc"_s);
    vbox->addWidget(mBccEdit);
    mMailEdit = new QPlainTextEdit(this);
    mMailEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    vbox->addWidget(mMailEdit);
    b = new QPushButton(u"&Send"_s, this);
    connect(b, &QPushButton::clicked, this, &TransportMgr::sendBtnClicked);
    vbox->addWidget(b);
    b = new QPushButton(u"&Cancel"_s, this);
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
    job->setTo(mToEdit->text().isEmpty() ? QStringList() : mToEdit->text().split(u','));
    job->setCc(mCcEdit->text().isEmpty() ? QStringList() : mCcEdit->text().split(u','));
    job->setBcc(mBccEdit->text().isEmpty() ? QStringList() : mBccEdit->text().split(u','));
    job->setData(mMailEdit->toPlainText().toLatin1());
    connect(job, &KJob::result, this, &TransportMgr::jobResult);
    connect(job, &TransportJob::percentChanged, this, &TransportMgr::jobPercent);
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
    app.setApplicationName(u"transportmgr"_s);

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

void TransportMgr::jobInfoMessage(KJob *job, const QString &info)
{
    Q_UNUSED(job)
    qDebug() << info;
}

#include "moc_transportmgr.cpp"
