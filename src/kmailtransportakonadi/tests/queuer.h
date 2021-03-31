/*
    SPDX-FileCopyrightText: 2006-2007 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>
#include <widgets/transportcombobox.h>

class KJob;
class QLineEdit;
class KTextEdit;

namespace MailTransport
{
class MessageQueueJob;
}

/**
  Mostly stolen from transportmgr.{h,cpp}
*/
class MessageQueuer : public QWidget
{
    Q_OBJECT

public:
    MessageQueuer();

private Q_SLOTS:
    void sendNowClicked();
    void sendQueuedClicked();
    void sendOnDateClicked();
    void jobResult(KJob *job);
    void jobPercent(KJob *job, unsigned long percent);
    void jobInfoMessage(KJob *job, const QString &info, const QString &info2);

private:
    MailTransport::TransportComboBox *mComboBox = nullptr;
    QLineEdit *mSenderEdit = nullptr;
    QLineEdit *mToEdit = nullptr;
    QLineEdit *mCcEdit = nullptr;
    QLineEdit *mBccEdit = nullptr;
    KTextEdit *mMailEdit = nullptr;

    MailTransport::MessageQueueJob *createQueueJob();
};

