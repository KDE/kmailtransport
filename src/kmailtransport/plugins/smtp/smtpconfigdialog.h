/*
    SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QDialog>

namespace MailTransport
{
class Transport;
class SMTPConfigWidget;
class SmtpConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SmtpConfigDialog(Transport *transport, QWidget *parent = nullptr);
    ~SmtpConfigDialog() override;

private:
    void okClicked();
    void slotTextChanged(const QString &text);

    Transport *mTransport = nullptr;
    SMTPConfigWidget *mConfigWidget = nullptr;
    QPushButton *mOkButton = nullptr;
};
} // namespace MailTransport

