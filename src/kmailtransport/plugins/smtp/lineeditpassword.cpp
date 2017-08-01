/*
  Copyright (c) 2017 Montel Laurent <montel@kde.org>

  based on lineedit in kpassworddialog.cpp

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "lineeditpassword.h"
#include <KLocalizedString>
#include <QIcon>
#include <QAction>
#include <QHBoxLayout>
#include <QLineEdit>

using namespace MailTransport;

LineEditPassword::LineEditPassword(QWidget *parent)
    : QWidget(parent)
    , mIsToggleEchoModeAvailable(true)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));
    mainLayout->setMargin(0);
    mPasswordLineEdit = new QLineEdit(this);
    mPasswordLineEdit->setObjectName(QStringLiteral("passwordlineedit"));
    mPasswordLineEdit->setEchoMode(QLineEdit::Password);
    mainLayout->addWidget(mPasswordLineEdit);
    initialize();
}

LineEditPassword::~LineEditPassword()
{
}

void LineEditPassword::initialize()
{
    QIcon visibilityIcon = QIcon::fromTheme(QStringLiteral("visibility"), QIcon(QStringLiteral(":/icons/visibility.svg")));
    mToggleEchoModeAction = mPasswordLineEdit->addAction(visibilityIcon, QLineEdit::TrailingPosition);
    mToggleEchoModeAction->setObjectName(QStringLiteral("visibilityAction"));
    mToggleEchoModeAction->setVisible(false);
    mToggleEchoModeAction->setToolTip(i18n("Change the visibility of the password"));

    connect(mToggleEchoModeAction, &QAction::triggered, this, &LineEditPassword::toggleEchoMode);

    connect(mPasswordLineEdit, &QLineEdit::textChanged, this, &LineEditPassword::showToggleEchoModeAction);
}

void LineEditPassword::toggleEchoMode()
{
    if (mPasswordLineEdit->echoMode() == QLineEdit::Password) {
        mPasswordLineEdit->setEchoMode(QLineEdit::Normal);
        mToggleEchoModeAction->setIcon(QIcon::fromTheme(QStringLiteral("hint"), QIcon(QStringLiteral(":/icons/hint.svg"))));
    } else if (mPasswordLineEdit->echoMode() == QLineEdit::Normal) {
        mPasswordLineEdit->setEchoMode(QLineEdit::Password);
        mToggleEchoModeAction->setIcon(QIcon::fromTheme(QStringLiteral("visibility"), QIcon(QStringLiteral(":/icons/visibility.svg"))));
    }
}

void LineEditPassword::setPassword(const QString &p)
{
    mIsToggleEchoModeAvailable = p.isEmpty();
    mPasswordLineEdit->setText(p);
}

QString LineEditPassword::password() const
{
    return mPasswordLineEdit->text();
}

void LineEditPassword::showToggleEchoModeAction(const QString &text)
{
    mToggleEchoModeAction->setVisible(mIsToggleEchoModeAvailable && (mPasswordLineEdit->echoMode() == QLineEdit::Normal || !text.isEmpty()));
}

QAction *LineEditPassword::toggleEchoModeAction() const
{
    return mToggleEchoModeAction;
}
