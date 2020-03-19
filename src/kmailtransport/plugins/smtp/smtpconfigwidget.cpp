/*
    Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>

    Based on MailTransport code by:
    Copyright (c) 2006 - 2007 Volker Krause <vkrause@kde.org>
    Copyright (c) 2007 KovoKs <kovoks@kovoks.nl>

    Based on KMail code by:
    Copyright (c) 2001-2002 Michael Haeckel <haeckel@kde.org>

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

#include "smtpconfigwidget.h"
#include "ui_smtpsettings.h"

#include "widgets/transportconfigwidget_p.h"
#include "transport.h"
#include "transportmanager.h"
#include "servertest.h"
#include "mailtransport_defs.h"
#include "mailtransportplugin_smtp_debug.h"

#include <QAbstractButton>
#include <QButtonGroup>

#include <KProtocolInfo>
#include "mailtransport_debug.h"
#include <KMessageBox>

using namespace MailTransport;

class MailTransport::SMTPConfigWidgetPrivate : public TransportConfigWidgetPrivate
{
public:
    ::Ui::SMTPSettings ui;

    ServerTest *serverTest = nullptr;
    QButtonGroup *encryptionGroup = nullptr;

    // detected authentication capabilities
    QVector<int> noEncCapa, sslCapa, tlsCapa;

    bool serverTestFailed;

    static void addAuthenticationItem(QComboBox *combo, int authenticationType)
    {
        combo->addItem(Transport::authenticationTypeString(authenticationType),
                       QVariant(authenticationType));
    }

    void resetAuthCapabilities()
    {
        noEncCapa.clear();
        noEncCapa << Transport::EnumAuthenticationType::LOGIN
                  << Transport::EnumAuthenticationType::PLAIN
                  << Transport::EnumAuthenticationType::CRAM_MD5
                  << Transport::EnumAuthenticationType::DIGEST_MD5
                  << Transport::EnumAuthenticationType::NTLM
                  << Transport::EnumAuthenticationType::GSSAPI
                  << Transport::EnumAuthenticationType::XOAUTH2;
        sslCapa = tlsCapa = noEncCapa;
        updateAuthCapbilities();
    }

    void enablePasswordLine()
    {
        ui.password->setEnabled(ui.kcfg_storePassword->isChecked());
    }

    void updateAuthCapbilities()
    {
        if (serverTestFailed) {
            return;
        }

        QVector<int> capa = noEncCapa;
        if (ui.encryptionSsl->isChecked()) {
            capa = sslCapa;
        } else if (ui.encryptionTls->isChecked()) {
            capa = tlsCapa;
        }

        ui.authCombo->clear();
        for (int authType : qAsConst(capa)) {
            addAuthenticationItem(ui.authCombo, authType);
        }

        if (transport->isValid()) {
            const int idx = ui.authCombo->findData(transport->authenticationType());

            if (idx != -1) {
                ui.authCombo->setCurrentIndex(idx);
            }
        }

        if (capa.isEmpty()) {
            ui.noAuthPossible->setVisible(true);
            ui.kcfg_requiresAuthentication->setChecked(false);
            ui.kcfg_requiresAuthentication->setEnabled(false);
            ui.kcfg_requiresAuthentication->setVisible(false);
            ui.authCombo->setEnabled(false);
            ui.authLabel->setEnabled(false);
        } else {
            ui.noAuthPossible->setVisible(false);
            ui.kcfg_requiresAuthentication->setEnabled(true);
            ui.kcfg_requiresAuthentication->setVisible(true);
            ui.authCombo->setEnabled(true);
            ui.authLabel->setEnabled(true);
            enablePasswordLine();
        }
    }
};

SMTPConfigWidget::SMTPConfigWidget(Transport *transport, QWidget *parent)
    : TransportConfigWidget(*new SMTPConfigWidgetPrivate, transport, parent)
{
    init();
}

static void checkHighestEnabledButton(QButtonGroup *group)
{
    Q_ASSERT(group);

    for (int i = group->buttons().count() - 1; i >= 0; --i) {
        QAbstractButton *b = group->buttons().at(i);
        if (b && b->isEnabled()) {
            b->animateClick();
            return;
        }
    }
}

void SMTPConfigWidget::init()
{
    Q_D(SMTPConfigWidget);
    d->serverTest = nullptr;

    connect(TransportManager::self(), &TransportManager::passwordsChanged, this, &SMTPConfigWidget::passwordsLoaded);

    d->serverTestFailed = false;

    d->ui.setupUi(this);
    d->manager->addWidget(this);   // otherwise it doesn't find out about these widgets
    d->manager->updateWidgets();

    d->ui.password->setWhatsThis(i18n("The password to send to the server for authorization."));

    d->ui.kcfg_userName->setClearButtonEnabled(true);
    d->encryptionGroup = new QButtonGroup(this);
    d->encryptionGroup->addButton(d->ui.encryptionNone, Transport::EnumEncryption::None);
    d->encryptionGroup->addButton(d->ui.encryptionSsl, Transport::EnumEncryption::SSL);
    d->encryptionGroup->addButton(d->ui.encryptionTls, Transport::EnumEncryption::TLS);

    d->ui.encryptionNone->setChecked(d->transport->encryption() == Transport::EnumEncryption::None);
    d->ui.encryptionSsl->setChecked(d->transport->encryption() == Transport::EnumEncryption::SSL);
    d->ui.encryptionTls->setChecked(d->transport->encryption() == Transport::EnumEncryption::TLS);

    d->resetAuthCapabilities();

    if (!KProtocolInfo::capabilities(SMTP_PROTOCOL).contains(QLatin1String("SASL"))) {
        d->ui.authCombo->removeItem(d->ui.authCombo->findData(
                                        Transport::EnumAuthenticationType::NTLM));
        d->ui.authCombo->removeItem(d->ui.authCombo->findData(
                                        Transport::EnumAuthenticationType::GSSAPI));
    }

    connect(d->ui.checkCapabilities, &QPushButton::clicked, this, &SMTPConfigWidget::checkSmtpCapabilities);
    connect(d->ui.kcfg_host, &QLineEdit::textChanged, this, &SMTPConfigWidget::hostNameChanged);

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(d->encryptionGroup, qOverload<int>(&QButtonGroup::buttonClicked), this, &SMTPConfigWidget::encryptionChanged);
#else
    connect(d->encryptionGroup, qOverload<QAbstractButton *>(&QButtonGroup::buttonClicked), this, &SMTPConfigWidget::encryptionAbstractButtonChanged);
#endif
    connect(d->ui.kcfg_requiresAuthentication, &QCheckBox::toggled, this, &SMTPConfigWidget::ensureValidAuthSelection);
    connect(d->ui.kcfg_storePassword, &QCheckBox::toggled, this, &SMTPConfigWidget::enablePasswordLine);
    if (!d->transport->isValid()) {
        checkHighestEnabledButton(d->encryptionGroup);
    }

    // load the password
    d->transport->updatePasswordState();
    if (d->transport->isComplete()) {
        d->ui.password->setPassword(d->transport->password());
    } else {
        if (d->transport->requiresAuthentication()) {
            TransportManager::self()->loadPasswordsAsync();
        }
    }

    hostNameChanged(d->transport->host());
}

void SMTPConfigWidget::enablePasswordLine()
{
    Q_D(SMTPConfigWidget);
    d->enablePasswordLine();
}

void SMTPConfigWidget::checkSmtpCapabilities()
{
    Q_D(SMTPConfigWidget);

    d->serverTest = new ServerTest(this);
    d->serverTest->setProtocol(SMTP_PROTOCOL);
    d->serverTest->setServer(d->ui.kcfg_host->text().trimmed());
    if (d->ui.kcfg_specifyHostname->isChecked()) {
        d->serverTest->setFakeHostname(d->ui.kcfg_localHostname->text());
    }
    QAbstractButton *encryptionChecked = d->encryptionGroup->checkedButton();
    if (encryptionChecked == d->ui.encryptionNone) {
        d->serverTest->setPort(Transport::EnumEncryption::None, d->ui.kcfg_port->value());
    } else if (encryptionChecked == d->ui.encryptionSsl) {
        d->serverTest->setPort(Transport::EnumEncryption::SSL, d->ui.kcfg_port->value());
    }
    d->serverTest->setProgressBar(d->ui.checkCapabilitiesProgress);
    d->ui.checkCapabilitiesStack->setCurrentIndex(1);
    qApp->setOverrideCursor(Qt::BusyCursor);

    connect(d->serverTest, &ServerTest::finished, this, &SMTPConfigWidget::slotFinished);
    connect(d->serverTest, &ServerTest::finished, qApp, [](){
        qApp->restoreOverrideCursor();
    });
    d->ui.checkCapabilities->setEnabled(false);
    d->serverTest->start();
    d->serverTestFailed = false;
}

void SMTPConfigWidget::apply()
{
    Q_D(SMTPConfigWidget);
    Q_ASSERT(d->manager);
    d->manager->updateSettings();
    if (!d->ui.kcfg_storePassword->isChecked() && d->ui.kcfg_requiresAuthentication->isChecked()) {
        //Delete stored password
        TransportManager::self()->removePasswordFromWallet(d->transport->id());
    }
    d->transport->setPassword(d->ui.password->password());

    KConfigGroup group(d->transport->config(), d->transport->currentGroup());
    const int index = d->ui.authCombo->currentIndex();
    if (index >= 0) {
        group.writeEntry("authtype", d->ui.authCombo->itemData(index).toInt());
    }

    if (d->ui.encryptionNone->isChecked()) {
        d->transport->setEncryption(Transport::EnumEncryption::None);
    } else if (d->ui.encryptionSsl->isChecked()) {
        d->transport->setEncryption(Transport::EnumEncryption::SSL);
    } else if (d->ui.encryptionTls->isChecked()) {
        d->transport->setEncryption(Transport::EnumEncryption::TLS);
    }

    TransportConfigWidget::apply();
}

void SMTPConfigWidget::passwordsLoaded()
{
    Q_D(SMTPConfigWidget);

    // Load the password from the original to our cloned copy
    d->transport->updatePasswordState();

    if (d->ui.password->password().isEmpty()) {
        d->ui.password->setPassword(d->transport->password());
    }
}

// TODO rename
void SMTPConfigWidget::slotFinished(const QVector<int> &results)
{
    Q_D(SMTPConfigWidget);

    d->ui.checkCapabilitiesStack->setCurrentIndex(0);

    d->ui.checkCapabilities->setEnabled(true);
    d->serverTest->deleteLater();

    // If the servertest did not find any usable authentication modes, assume the
    // connection failed and don't disable any of the radioboxes.
    if (results.isEmpty()) {
        KMessageBox::error(this, i18n("Failed to check capabilities. Please verify port and authentication mode."), i18n("Check Capabilities Failed"));
        d->serverTestFailed = true;
        d->serverTest->deleteLater();
        return;
    }

    // encryption method
    d->ui.encryptionNone->setEnabled(results.contains(Transport::EnumEncryption::None));
    d->ui.encryptionSsl->setEnabled(results.contains(Transport::EnumEncryption::SSL));
    d->ui.encryptionTls->setEnabled(results.contains(Transport::EnumEncryption::TLS));
    checkHighestEnabledButton(d->encryptionGroup);

    d->noEncCapa = d->serverTest->normalProtocols();
    if (d->ui.encryptionTls->isEnabled()) {
        d->tlsCapa = d->serverTest->tlsProtocols();
    } else {
        d->tlsCapa.clear();
    }
    d->sslCapa = d->serverTest->secureProtocols();
    d->updateAuthCapbilities();
    //Show correct port from capabilities.
    if (d->ui.encryptionSsl->isEnabled()) {
        const int portValue = d->serverTest->port(Transport::EnumEncryption::SSL);
        d->ui.kcfg_port->setValue(portValue == -1 ? SMTPS_PORT : portValue);
    } else if (d->ui.encryptionNone->isEnabled()) {
        const int portValue = d->serverTest->port(Transport::EnumEncryption::None);
        d->ui.kcfg_port->setValue(portValue == -1 ? SMTP_PORT : portValue);
    }
    d->serverTest->deleteLater();
}

void SMTPConfigWidget::hostNameChanged(const QString &text)
{
    // TODO: really? is this done at every change? wtf

    Q_D(SMTPConfigWidget);

    // sanitize hostname...
    const int pos = d->ui.kcfg_host->cursorPosition();
    d->ui.kcfg_host->blockSignals(true);
    d->ui.kcfg_host->setText(text.trimmed());
    d->ui.kcfg_host->blockSignals(false);
    d->ui.kcfg_host->setCursorPosition(pos);

    d->resetAuthCapabilities();
    if (d->encryptionGroup) {
        for (int i = 0; i < d->encryptionGroup->buttons().count(); ++i) {
            d->encryptionGroup->buttons().at(i)->setEnabled(true);
        }
    }
}

void SMTPConfigWidget::ensureValidAuthSelection()
{
    Q_D(SMTPConfigWidget);

    // adjust available authentication methods
    d->updateAuthCapbilities();
    d->enablePasswordLine();
}

void SMTPConfigWidget::encryptionAbstractButtonChanged(QAbstractButton *button)
{
    Q_D(SMTPConfigWidget);
    if (button) {
        encryptionChanged(d->encryptionGroup->id(button));
    }
}

void SMTPConfigWidget::encryptionChanged(int enc)
{
    Q_D(SMTPConfigWidget);
    qCDebug(MAILTRANSPORT_SMTP_LOG) << enc;

    // adjust port
    if (enc == Transport::EnumEncryption::SSL) {
        if (d->ui.kcfg_port->value() == SMTP_PORT) {
            d->ui.kcfg_port->setValue(SMTPS_PORT);
        }
    } else {
        if (d->ui.kcfg_port->value() == SMTPS_PORT) {
            d->ui.kcfg_port->setValue(SMTP_PORT);
        }
    }

    ensureValidAuthSelection();
}
