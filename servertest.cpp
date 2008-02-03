/*
    Copyright (c) 2006 - 2007 Volker Krause <vkrause@kde.org>
    Copyright (C) 2007 KovoKs <info@kovoks.nl>

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

// Own
#include "servertest.h"
#include "socket.h"

#include <mailtransport/transportbase.h>
#include <mailtransport/mailtransport_defs.h>

// Qt
#include <QHostInfo>
#include <QProgressBar>
#include <QTimer>

// KDE
#include <klocale.h>
#include <kdebug.h>

using namespace MailTransport;

namespace MailTransport
{

class ServerTestPrivate
{
  public:
    ServerTestPrivate( ServerTest *test );

    ServerTest *const              q;
    QString                        server;
    QString                        fakeHostname;
    QString                        testProtocol;

    MailTransport::Socket         *normalSocket;
    MailTransport::Socket         *secureSocket;

    QSet< int >                    connectionResults;
    QHash< int, QList<int> >       authenticationResults;
    QSet< ServerTest::Capability > capabilityResults;
    QTimer                        *normalSocketTimer;
    QTimer                        *secureSocketTimer;
    QTimer                        *progressTimer;

    QProgressBar                  *testProgress;

    bool                           secureSocketFinished;
    bool                           normalSocketFinished;
    bool                           tlsFinished;
    bool                           popSupportsTLS;
    int                            normalStage;
    int                            secureStage;

    void finalResult();
    void handleSMTPIMAPResponse( int type, const QString &text );
    void sendInitialCapabilityQuery( MailTransport::Socket *socket );
    bool handlePopConversation( MailTransport::Socket *socket, int type, int stage,
                                const QString &response, bool *shouldStartTLS );
    QList< int > parseAuthenticationList( const QStringList &authentications );

    // slots
    void slotNormalPossible();
    void slotNormalNotPossible();
    void slotSslPossible();
    void slotSslNotPossible();
    void slotTlsDone();
    void slotReadNormal( const QString &text );
    void slotReadSecure( const QString &text );
    void slotUpdateProgress();
};

}

ServerTestPrivate::ServerTestPrivate( ServerTest *test )
  : q( test )
{
}

void ServerTestPrivate::finalResult()
{
  if ( !secureSocketFinished || !normalSocketFinished || !tlsFinished) {
    return;
  }

  kDebug() << "Modes:" << connectionResults;
  kDebug() << "Capabilities:" << capabilityResults;

  testProgress->hide();
  progressTimer->stop();

  emit q->finished( connectionResults.toList() );
}

QList< int > ServerTestPrivate::parseAuthenticationList( const QStringList &authentications )
{
  QList< int > result;
  for ( QStringList::ConstIterator it = authentications.begin();
        it != authentications.end(); ++it )  {
    if ( *it == QLatin1String( "LOGIN" ) ) {
      result << Transport::EnumAuthenticationType::LOGIN;
    } else if ( *it == QLatin1String( "PLAIN" ) ) {
      result << Transport::EnumAuthenticationType::PLAIN;
    } else if ( *it == QLatin1String( "CRAM-MD5" ) ) {
      result << Transport::EnumAuthenticationType::CRAM_MD5;
    } else if ( *it == QLatin1String( "DIGEST-MD5" ) ) {
      result << Transport::EnumAuthenticationType::DIGEST_MD5;
    } else if ( *it == QLatin1String( "NTLM" ) ) {
      result << Transport::EnumAuthenticationType::NTLM;
    } else if ( *it == QLatin1String( "GSSAPI" ) ) {
      result << Transport::EnumAuthenticationType::GSSAPI;
    }
    // APOP is handled by handlePopConversation()
  }

  // LOGIN doesn't offer anything over PLAIN, requires more server
  // roundtrips and is not an official SASL mechanism, but a MS-ism,
  // so only enable it if PLAIN isn't available:
  if ( result.contains( Transport::EnumAuthenticationType::PLAIN ) ) {
    result.removeAll( Transport::EnumAuthenticationType::LOGIN );
  }

  return result;
}

void ServerTestPrivate::handleSMTPIMAPResponse( int type, const QString &text )
{
  if ( !text.contains( QLatin1String( "AUTH" ), Qt::CaseInsensitive ) ) {
    kDebug() << "No authentication possible";
    return;
  }

  QStringList protocols;
  protocols << QLatin1String( "LOGIN" ) << QLatin1String( "PLAIN" )
            << QLatin1String( "CRAM-MD5" ) << QLatin1String( "DIGEST-MD5" )
            << QLatin1String( "NTLM" ) << QLatin1String( "GSSAPI" );

  QStringList results;
  for ( int i = 0; i < protocols.count(); ++i ) {
    if ( text.contains( protocols.at( i ), Qt::CaseInsensitive ) ) {
      results.append( protocols.at( i ) );
    }
  }

  authenticationResults[type] = parseAuthenticationList( results );
  kDebug() << "For type" << type << ", we have:" << authenticationResults[type];
}

void ServerTestPrivate::slotNormalPossible()
{
  normalSocketTimer->stop();
  connectionResults << Transport::EnumEncryption::None;
}

void ServerTestPrivate::sendInitialCapabilityQuery( MailTransport::Socket *socket )
{
  kDebug();
  if ( testProtocol == IMAP_PROTOCOL ) {
    socket->write( QLatin1String( "1 CAPABILITY" ) );
  }

  else if ( testProtocol == SMTP_PROTOCOL ) {

      // Detect the hostname which we send with the EHLO command.
      // If there is a fake one set, use that, otherwise use the
      // local host name (and make sure it contains a domain, so the
      // server thinks it is valid).
    QString hostname;
    if ( !fakeHostname.isNull() ) {
      hostname = fakeHostname;
    } else {
      hostname = QHostInfo::localHostName();
      if( hostname.isEmpty() ) {
        hostname = QLatin1String( "localhost.invalid" );
      } else if ( !hostname.contains( QChar::fromAscii( '.' ) ) ) {
        hostname += QLatin1String( ".localnet" );
      }
    }
    kDebug() << "Hostname for EHLO is" << hostname;

    socket->write( QLatin1String( "EHLO " ) + hostname );
  }
}

void ServerTestPrivate::slotTlsDone()
{
  kDebug();

  // The server will not send a response after starting TLS. Therefore, we have to manually
  // call slotReadNormal(), because this is not triggered by a data received signal this time.
  slotReadNormal( QString() );
}

bool ServerTestPrivate::handlePopConversation( MailTransport::Socket *socket, int type, int stage,
                                               const QString &response, bool *shouldStartTLS )
{
  Q_ASSERT( shouldStartTLS != 0 );

  // Initial Greeting
  if ( stage == 0 ) {

    //Regexp taken from POP3 ioslave
    QString responseWithoutCRLF = response;
    responseWithoutCRLF.chop( 2 );
    QRegExp re( QLatin1String( "<[A-Za-z0-9\\.\\-_]+@[A-Za-z0-9\\.\\-_]+>$" ), Qt::CaseInsensitive );
    if ( responseWithoutCRLF.indexOf( re ) != -1 ) {
      authenticationResults[type] << Transport::EnumAuthenticationType::APOP;
    }

    //Each server is supposed to support clear text login
    authenticationResults[type] << Transport::EnumAuthenticationType::CLEAR;

    // If we are in TLS stage, the server does not send the initial greeting. Just assume that the
    // APOP availibility is the same as with an unsecured connection.
    if ( type == Transport::EnumEncryption::TLS &&
         authenticationResults[Transport::EnumEncryption::None]
             .contains( Transport::EnumAuthenticationType::APOP ) )
      authenticationResults[Transport::EnumEncryption::TLS]
          << Transport::EnumAuthenticationType::APOP;

    socket->write( QLatin1String( "CAPA" ) );
    return true;
  }

  // CAPA result
  else if( stage == 1 ) {
//     Example:
//     CAPA
//     +OK
//     TOP
//     USER
//     SASL LOGIN CRAM-MD5
//     UIDL
//     RESP-CODES
//     .
    if ( response.contains( QLatin1String( "TOP" ) ) )
      capabilityResults += ServerTest::Top;
    if ( response.contains( QLatin1String( "PIPELINING" ) ) )
      capabilityResults += ServerTest::Pipelining;
    if ( response.contains( QLatin1String( "UIDL" ) ) )
      capabilityResults += ServerTest::UIDL;
    if ( response.contains( QLatin1String( "STLS" ) ) ) {
      connectionResults << Transport::EnumEncryption::TLS;
      popSupportsTLS = true;
    }
    socket->write( QLatin1String( "AUTH" ) );
    return true;
  }

  // AUTH response
  else if( stage == 2 ) {
//     Example:
//     C: AUTH
//     S: +OK List of supported authentication methods follows
//     S: LOGIN
//     S: CRAM-MD5
//     S:.
    QString formattedReply = response;

    // Get rid of trailling ".CRLF"
    formattedReply.chop( 3 );

    // Get rid of the first +OK line
    formattedReply = formattedReply.right( formattedReply.size() -
                                           formattedReply.indexOf( QLatin1Char( '\n' ) ) - 1 );
    formattedReply = formattedReply.replace( QLatin1Char( ' ' ), QLatin1Char( '-' ) )
                                   .replace( QLatin1String( "\r\n" ), QLatin1String( " " ) );

    authenticationResults[type] += parseAuthenticationList( formattedReply.split( QLatin1Char( ' ' ) ) );
  }

  *shouldStartTLS = popSupportsTLS;
  return false;
}

// slotReadNormal() handles normal (no) encryption and TLS encryption.
// At first, the communication is not encrypted, but if the server supports
// the STARTTLS/STLS keyword, the same authentication query is done again
// with TLS.
void ServerTestPrivate::slotReadNormal( const QString &text )
{
  static const int tlsHandshakeStage = 42;
  static int encryptionMode = Transport::EnumEncryption::None;

  kDebug() << "Stage" << normalStage + 1 << ", Mode" << encryptionMode;

  // If we are in stage 42, we just do the handshake for TLS encryption and then reset the
  // stage to -1, so that all authentication modes and capabilites are queried again for
  // TLS encryption (some servers have different authentication  methods in normal and in
  // TLS mode).
  if ( normalStage == tlsHandshakeStage ) {
    Q_ASSERT( encryptionMode == Transport::EnumEncryption::TLS );
    normalStage = -1;
    normalSocket->startTLS();
    return;
  }

  bool shouldStartTLS = false;
  normalStage++;

  // Handle the whole POP converstation separatly, it is very different from
  // IMAP and SMTP
  if ( testProtocol == POP_PROTOCOL ) {
    if ( handlePopConversation( normalSocket, encryptionMode, normalStage, text,
                                &shouldStartTLS ) )
      return;
  }

  // Handle the SMTP/IMAP conversation here. We just send the EHLO command in
  // sendInitialCapabilityQuery.
  else {
    if ( normalStage == 0 ) {
      sendInitialCapabilityQuery( normalSocket );
      return;
    }

    if ( text.contains( QLatin1String( "STARTTLS" ), Qt::CaseInsensitive ) ) {
      connectionResults << Transport::EnumEncryption::TLS;
      shouldStartTLS = true;
    }
    handleSMTPIMAPResponse( encryptionMode, text );
  }

  // If we reach here, the normal authentication/capabilites query is completed.
  // Now do the same for TLS.
  normalSocketFinished = true;

  // If the server announced that STARTTLS/STLS is available, we'll add TLS to the
  // connection result, do the command and set the stage to 42 to start the handshake.
  if ( shouldStartTLS && encryptionMode == Transport::EnumEncryption::None ) {
    kDebug() << "Trying TLS...";
    connectionResults << Transport::EnumEncryption::TLS;
    if ( testProtocol == POP_PROTOCOL )
      normalSocket->write( QLatin1String( "STLS" ) );
    else
      normalSocket->write( QLatin1String( "STARTTLS" ) );
    encryptionMode = Transport::EnumEncryption::TLS;
    normalStage = tlsHandshakeStage;
    return;
  }

  // If we reach here, either the TLS authentication/capabilites query is finished
  // or the server does not support the STARTTLS/STLS command.
  tlsFinished = true;
  finalResult();
}

void ServerTestPrivate::slotReadSecure( const QString &text )
{
  secureStage++;
  if ( testProtocol == POP_PROTOCOL ) {
    bool dummy;
    if ( handlePopConversation( secureSocket, Transport::EnumEncryption::SSL,
                                secureStage, text, &dummy ) )
      return;
  }
  else {
    if ( secureStage == 0 ) {
      sendInitialCapabilityQuery( secureSocket );
      return;
    }
    handleSMTPIMAPResponse( Transport::EnumEncryption::SSL, text );
  }
  secureSocketFinished = true;
  finalResult();
}

void ServerTestPrivate::slotNormalNotPossible()
{
  normalSocketFinished = true;
  finalResult();
}

void ServerTestPrivate::slotSslPossible()
{
  secureSocketTimer->stop();
  connectionResults << Transport::EnumEncryption::SSL;
}

void ServerTestPrivate::slotSslNotPossible()
{
  secureSocketFinished = true;
  finalResult();
}

void ServerTestPrivate::slotUpdateProgress()
{
  testProgress->setValue( testProgress->value() + 1 );
}

//---------------------- end private class -----------------------//

ServerTest::ServerTest( QWidget *parent )
  : QWidget( parent ), d( new ServerTestPrivate( this ) )
{
  d->normalSocketTimer = new QTimer( this );
  d->normalSocketTimer->setSingleShot( true );
  connect( d->normalSocketTimer, SIGNAL( timeout() ), SLOT( slotNormalNotPossible() ) );

  d->secureSocketTimer = new QTimer( this );
  d->secureSocketTimer->setSingleShot( true );
  connect( d->secureSocketTimer, SIGNAL( timeout() ), SLOT( slotSslNotPossible() ) );

  d->progressTimer = new QTimer( this );
  connect( d->progressTimer, SIGNAL( timeout() ), SLOT( slotUpdateProgress() ) );
}

ServerTest::~ServerTest()
{
  delete d;
}

void ServerTest::start()
{
  kDebug() << d;

  d->connectionResults.clear();
  d->authenticationResults.clear();
  d->capabilityResults.clear();
  d->popSupportsTLS = false;
  d->normalStage = -1;
  d->secureStage = -1;

  d->testProgress->setMaximum( 20 );
  d->testProgress->setValue( 0 );
  d->testProgress->setTextVisible( true );
  d->testProgress->show();
  d->progressTimer->start( 1000 );

  d->normalSocket = new MailTransport::Socket( this );
  d->secureSocket = new MailTransport::Socket( this );
  d->normalSocket->setObjectName( QLatin1String( "normal" ) );
  d->normalSocket->setServer( d->server );
  d->normalSocket->setProtocol( d->testProtocol );
  if ( d->testProtocol == IMAP_PROTOCOL ) {
    d->normalSocket->setPort( IMAP_PORT );
    d->secureSocket->setPort( IMAPS_PORT );
  } else if ( d->testProtocol == SMTP_PROTOCOL ) {
    d->normalSocket->setPort( SMTP_PORT );
    d->secureSocket->setPort( SMTPS_PORT );
  } else if ( d->testProtocol == POP_PROTOCOL ) {
    d->normalSocket->setPort( POP_PORT );
    d->secureSocket->setPort( POPS_PORT );
  }

  connect( d->normalSocket, SIGNAL(connected()), SLOT(slotNormalPossible()) );
  connect( d->normalSocket, SIGNAL(failed()), SLOT(slotNormalNotPossible()) );
  connect( d->normalSocket, SIGNAL(data(const QString&)),
           SLOT(slotReadNormal(const QString&)) );
  connect( d->normalSocket, SIGNAL(tlsDone()), SLOT(slotTlsDone())); 
  d->normalSocket->reconnect();
  d->normalSocketTimer->start( 10000 );

  d->secureSocket->setObjectName( QLatin1String( "secure" ) );
  d->secureSocket->setServer( d->server );
  d->secureSocket->setProtocol( d->testProtocol + QLatin1Char( 's' ) );
  d->secureSocket->setSecure( true );
  connect( d->secureSocket, SIGNAL(connected()), SLOT(slotSslPossible()) );
  connect( d->secureSocket, SIGNAL(failed()), SLOT(slotSslNotPossible()) );
  connect( d->secureSocket, SIGNAL(data(const QString&) ),
           SLOT(slotReadSecure(const QString&)) );
  d->secureSocket->reconnect();
  d->secureSocketTimer->start( 10000 );
}

void ServerTest::setFakeHostname( const QString &fakeHostname )
{
  d->fakeHostname = fakeHostname;
}

QString ServerTest::fakeHostname()
{
  return d->fakeHostname;
}

void ServerTest::setServer( const QString &server )
{
  d->server = server;
}

void ServerTest::setProgressBar( QProgressBar *pb )
{
  d->testProgress = pb;
}

void ServerTest::setProtocol( const QString &protocol )
{
  d->testProtocol = protocol;
}

QString ServerTest::protocol()
{
  return d->testProtocol;
}

QString ServerTest::server()
{
  return d->server;
}

QProgressBar *ServerTest::progressBar()
{
  return d->testProgress;
}

QList< int > ServerTest::normalProtocols()
{
  return d->authenticationResults[TransportBase::EnumEncryption::None];
}

QList< int > ServerTest::tlsProtocols()
{
  return d->authenticationResults[TransportBase::EnumEncryption::TLS];
}

QList< int > ServerTest::secureProtocols()
{
  return d->authenticationResults[Transport::EnumEncryption::SSL];
}

QList< ServerTest::Capability > ServerTest::capabilities() const
{
  return d->capabilityResults.toList();
}

#include "servertest.moc"
