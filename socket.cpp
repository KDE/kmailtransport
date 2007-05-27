/*
    Copyright (C) 2006-2007 KovoKs <info@kovoks.nl>

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

// Uncomment the next line for full comm debug
// #define comm_debug

// Own
#include "socket.h"

// Qt
#include <QRegExp>
#include <QByteArray>
#include <QSslSocket>

// KDE
#include <kdebug.h>
#include <ksocketfactory.h>
#include <klocale.h>

using namespace MailTransport;

namespace MailTransport
{
  class SocketPrivate
  {
    public:
      SocketPrivate( Socket* s);
      Socket* const       q;
      QSslSocket*         socket;
      QString             server;
      QString             protocol;
      int                 port;
      bool                secure;

      // slots
      void slotConnected();
      void slotStateChanged( QAbstractSocket::SocketState state );
      void slotModeChanged( QSslSocket::SslMode  state );
      void slotSocketRead();
      void slotSslErrors( const QList<QSslError> & errors );
  };
}

SocketPrivate::SocketPrivate( Socket* s) : q(s)
{
}

void SocketPrivate::slotConnected()
{
  kDebug( 5324 ) << k_funcinfo << endl;

  if ( !secure ) {
    kDebug( 5324 ) << "normal connect" << endl;
    emit q->connected();
  } else {
    kDebug( 5324 ) << "encrypted connect" << endl;
    socket->startClientEncryption();
  }
}

void SocketPrivate::slotStateChanged( QAbstractSocket::SocketState state )
{
#ifdef comm_debug
  kDebug( 5324 ) << objectName() << " "
      << "State is now: " << ( int ) state << endl;
#endif
  if ( state == QAbstractSocket::UnconnectedState )
    emit q->failed();
}

void SocketPrivate::slotModeChanged( QSslSocket::SslMode  state )
{
#ifdef comm_debug
  kDebug( 5324 ) << objectName() << " "  << "Mode is now: " << state << endl;
#endif
}

void SocketPrivate::slotSocketRead()
{
  // kDebug(5324) << objectName() << " " << k_funcinfo << endl;

  if ( !socket )
    return;

  static QString msg;
  msg += QLatin1String( socket->readAll() );

  if ( !msg.endsWith( QLatin1Char( '\n' ) ) )
    return;

#ifdef comm_debug
  kDebug( 5324 ) << socket->isEncrypted() << msg.trimmed() << endl;
#endif

  emit q->data( msg );
  msg.clear();
}

void SocketPrivate::slotSslErrors( const QList<QSslError> & )
{
  kDebug( 5324 ) << k_funcinfo << endl;
  /* We can safely ignore the errors, we are only interested in the
  capabilities. We're not sending auth info. */
  socket->ignoreSslErrors();
  emit q->connected();
}


// ------------------ end private ---------------------------//

Socket::Socket( QObject* parent )
    : QObject( parent ), d( new SocketPrivate( this ) )
{
  d->socket = 0;
  d->port = 0;
  d->secure = false;
  kDebug( 5324 ) << k_funcinfo << endl;
}

Socket::~Socket()
{
  kDebug( 5324 ) << objectName() << " " << k_funcinfo << endl;
}

void Socket::reconnect()
{
  kDebug( 5324 ) << objectName() << " " << "Connecting to: " << d->server
          <<  ":" <<  d->port << endl;

#ifdef comm_debug
  // kDebug(5324) << objectName() << d->protocol << endl;
#endif

  if ( d->socket )
    return;

  d->socket = static_cast<QSslSocket*>
             ( KSocketFactory::connectToHost( d->protocol, d->server, d->port, this
                                            ) );

  d->socket->setProtocol( QSslSocket::AnyProtocol );

  connect( d->socket, SIGNAL( stateChanged( QAbstractSocket::SocketState ) ),
           SLOT( slotStateChanged( QAbstractSocket::SocketState ) ) );
  connect( d->socket, SIGNAL( modeChanged( QSslSocket::SslMode ) ),
           SLOT( slotModeChanged( QSslSocket::SslMode ) ) );
  connect( d->socket, SIGNAL( connected() ), SLOT( slotConnected() ) );
  connect( d->socket, SIGNAL( readyRead() ), SLOT( slotSocketRead() ) );
  connect( d->socket, SIGNAL( encrypted() ), SIGNAL( connected() ) );
  connect( d->socket, SIGNAL( sslErrors( const QList<QSslError> & ) ),
           SLOT( slotSslErrors( const QList<QSslError>& ) ) );
}

void Socket::write( const QString& text )
{
  // kDebug(5324) << objectName() << " " << k_funcinfo << endl;
  // Eat things in the queue when there is no connection. We need
  // to get a connection first don't we...
  if ( !d->socket || !available() )
    return;

  QByteArray cs = ( text + QLatin1String( "\r\n" ) ).toLatin1();

#ifdef comm_debug
  kDebug( 5324 ) << objectName() << " " << "C   : "
  << cs << endl;
#endif

  d->socket->write( cs.data(), cs.size() );
}

bool Socket::available()
{
  // kDebug(5324) << objectName() << " " << k_funcinfo << endl;
  bool ok = d->socket && d->socket->state() == QAbstractSocket::ConnectedState;
  return ok;
}

void Socket::setProtocol( const QString& proto )
{
  d->protocol = proto;
}

void Socket::setServer( const QString& server )
{
  d->server = server;
}
      
void Socket::setPort( int port )
{
  d->port = port;
}

void Socket::setSecure( bool what )
{
  d->secure = what;
}

#include "socket.moc"
