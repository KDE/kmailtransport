/*
    Copyright (c) 2006 - 2007 Volker Krause <vkrause@kde.org>

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

#include "transportmgr.h"

#include <widgets/transportconfigdialog.h>
#include <transportmanager.h>
#include <widgets/transportmanagementwidget.h>
#include <transportjob.h>
#include <transport.h>
#include <QVBoxLayout>

#include <QApplication>
#include <QLineEdit>
#include <QDebug>
#include <KTextEdit>

#include <QPushButton>

using namespace MailTransport;

TransportMgr::TransportMgr()
    : mCurrentJob(nullptr)
{
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setMargin(0);
    setLayout(vbox);

    vbox->addWidget(new TransportManagementWidget(this));
    mComboBox = new TransportComboBox(this);
    mComboBox->setEditable(true);
    vbox->addWidget(mComboBox);
    QPushButton *b = new QPushButton(QStringLiteral("&Edit"), this);
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
    QList<Transport *> transports = manager->transports();
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
    TransportManager::self()->configureTransport(TransportManager::self()->transportById(index), this);
}

void TransportMgr::sendBtnClicked()
{
    TransportJob *job;
    job = TransportManager::self()->createTransportJob(mComboBox->currentTransportId());
    if (!job) {
        qDebug() << "Invalid transport!";
        return;
    }
    job->setSender(mSenderEdit->text());
    job->setTo(mToEdit->text().isEmpty() ? QStringList() : mToEdit->text().split(QLatin1Char(',')));
    job->setCc(mCcEdit->text().isEmpty() ? QStringList() : mCcEdit->text().split(QLatin1Char(',')));
    job->setBcc(mBccEdit->text().isEmpty() ? QStringList() : mBccEdit->text().split(QLatin1Char(',')));
    job->setData(mMailEdit->document()->toPlainText().toLatin1());
    connect(job, SIGNAL(result(KJob *)),
            SLOT(jobResult(KJob *)));
    connect(job, SIGNAL(percent(KJob *,ulong)),
            SLOT(jobPercent(KJob *,ulong)));
    connect(job, SIGNAL(infoMessage(KJob *,QString,QString)),
            SLOT(jobInfoMessage(KJob *,QString,QString)));
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

    TransportMgr *t = new TransportMgr();
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
    Q_UNUSED(job);
    qDebug() << percent << "%";
}

void TransportMgr::jobInfoMessage(KJob *job, const QString &info, const QString &info2)
{
    Q_UNUSED(job);
    qDebug() << info;
    qDebug() << info2;
}
