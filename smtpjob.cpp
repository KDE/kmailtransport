/*
    Copyright (c) 2007 Volker Krause <vkrause@kde.org>

    Based on KMail code by:
    Copyright (c) 1996-1998 Stefan Taferner <taferner@kde.org>

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

#include "smtpjob.h"
#include "transport.h"
#include "mailtransport_defs.h"
#include "precommandjob.h"

#include <klocale.h>
#include <kurl.h>
#include <kio/job.h>
#include <kio/scheduler.h>
#include <kio/passworddialog.h>

#include <qbuffer.h>
#include <qhash.h>

using namespace MailTransport;

static int slavePoolRef = 0;
static QHash<int,KIO::Slave*> slavePool;

static void removeSlaveFromPool( KIO::Slave *slave, bool disconnect = false )
{
  const int slaveKey = slavePool.key( slave );
  if (slaveKey > 0)
  {
    slavePool.remove( slaveKey );
    if ( disconnect )
      KIO::Scheduler::disconnectSlave( slave );
  }
}

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
class SmtpJobPrivate
{
  public:
    KIO::Slave* slave;
    enum State {
      Idle, Precommand, Smtp
    } currentState;
};

SmtpJob::SmtpJob(Transport * transport, QObject * parent) :
    TransportJob( transport, parent ), d( new SmtpJobPrivate )
{
  d->currentState = SmtpJobPrivate::Idle;
  d->slave = 0;
  slavePoolRef++;
  KIO::Scheduler::connect( SIGNAL(slaveError(KIO::Slave*,int,QString)),
                           this, SLOT(slaveError(KIO::Slave*,int,QString)) );
}

SmtpJob::~SmtpJob()
{
  slavePoolRef--;
  if ( slavePoolRef == 0 ) {
    kDebug(5324) << k_funcinfo << "clearing SMTP slave pool "
            << slavePool.count() << endl;
    foreach ( KIO::Slave *slave, slavePool.values() )
      KIO::Scheduler::disconnectSlave( slave );
    slavePool.clear();
  }
}

void SmtpJob::doStart()
{
  if ( slavePool.contains( transport()->id() ) ||
       transport()->precommand().isEmpty() ) {
    d->currentState = SmtpJobPrivate::Smtp;
    startSmtpJob();
  } else {
    d->currentState = SmtpJobPrivate::Precommand;
    PrecommandJob *job = new PrecommandJob( transport()->precommand(), this );
    addSubjob( job );
    job->start();
  }
}

void SmtpJob::startSmtpJob()
{
  KUrl destination;
  destination.setProtocol( (transport()->encryption() ==
          Transport::EnumEncryption::SSL) ? SMTPS_PROTOCOL : SMTP_PROTOCOL );
  destination.setHost( transport()->host() );
  destination.setPort( transport()->port() );

  destination.addQueryItem( QLatin1String("headers"), QLatin1String("0") );
  destination.addQueryItem( QLatin1String("from"), sender() );

  foreach ( QString str, to() )
    destination.addQueryItem( QLatin1String("to"), str );
  foreach ( QString str, cc() )
    destination.addQueryItem( QLatin1String("cc"), str );
  foreach ( QString str, bcc() )
    destination.addQueryItem( QLatin1String("bcc"), str );

  if ( transport()->specifyHostname() )
    destination.addQueryItem( QLatin1String("hostname"),
                              transport()->localHostname() );

#ifdef __GNUC__
#warning Argh!
#endif
//   if ( !kmkernel->msgSender()->sendQuotedPrintable() )
//     query += "&body=8bit";

  if ( transport()->requiresAuthentication() ) {
    if( (transport()->userName().isEmpty() || transport()->password().isEmpty())
         && transport()->authenticationType() !=
                Transport::EnumAuthenticationType::GSSAPI )
    {
      QString user = transport()->userName();
      QString passwd = transport()->password();
      int result;

#ifdef __GNUC__
#warning yet another KMail specific thing
#endif
//       KCursorSaver idle( KBusyPtr::idle() );
      bool keep = true;
      result = KIO::PasswordDialog::getNameAndPassword(
          user, passwd, &keep,
          i18n("You need to supply a username and a password to use this "
                  "SMTP server."),
          false, QString(), transport()->name(), QString() );

      if ( result != QDialog::Accepted ) {
        setError( KilledJobError );
        emitResult();
        return;
      }
      transport()->setUserName( user );
      transport()->setPassword( passwd );
      transport()->setStorePassword( keep );
      transport()->writeConfig();
    }
    destination.setUser( transport()->userName() );
    destination.setPass( transport()->password() );
  }

  // dotstuffing is now done by the slave (see setting of metadata)
  if ( !data().isEmpty() )
    // allow +5% for subsequent LF->CRLF and dotstuffing (an average
    // over 2G-lines gives an average line length of 42-43):
    destination.addQueryItem( QLatin1String("size"),
                              QString::number( qRound( data().length() * 1.05 ) ) );

  destination.setPath( QLatin1String("/send") );

  d->slave = slavePool.value( transport()->id() );
  if ( !d->slave ) {
    kDebug(5324) << k_funcinfo << "creating new SMTP slave" << endl;
    KIO::MetaData slaveConfig;
    slaveConfig.insert( QLatin1String("tls"),
                        (transport()->encryption() == Transport::EnumEncryption::TLS)
        ? QLatin1String("on") : QLatin1String("off") );
    if ( transport()->requiresAuthentication() )
      slaveConfig.insert( QLatin1String("sasl"),
                          transport()->authenticationTypeString() );
    d->slave = KIO::Scheduler::getConnectedSlave( destination, slaveConfig );
    slavePool.insert( transport()->id(), d->slave );
  } else {
    kDebug(5324) << k_funcinfo << "re-using existing slave" << endl;
  }

  KIO::TransferJob *job = KIO::put( destination, -1, false, false, false );
  if ( !d->slave || !job ) {
    setError( UserDefinedError );
    setErrorText( i18n("Unable to create SMTP job.") );
    emitResult();
    return;
  }

  job->addMetaData( QLatin1String("lf2crlf+dotstuff"), QLatin1String("slave") );
  connect( job, SIGNAL(dataReq(KIO::Job*,QByteArray&)),
           SLOT(dataRequest(KIO::Job*,QByteArray&)) );

  addSubjob( job );
  KIO::Scheduler::assignJobToSlave( d->slave, job );

  setTotalAmount( KJob::Bytes, data().length() );
}

bool SmtpJob::doKill()
{
  if ( !hasSubjobs() )
    return true;
  if ( d->currentState == SmtpJobPrivate::Precommand )
    return subjobs().first()->kill();
  else if ( d->currentState == SmtpJobPrivate::Smtp ) {
    KIO::SimpleJob *job = static_cast<KIO::SimpleJob*>( subjobs().first() );
    clearSubjobs();
    KIO::Scheduler::cancelJob( job );
    return true;
  }
  return false;
}

void SmtpJob::slotResult(KJob * job)
{
  TransportJob::slotResult( job );
  if ( error() && d->currentState == SmtpJobPrivate::Smtp ) {
    removeSlaveFromPool( d->slave, error() != KIO::ERR_SLAVE_DIED );
    return;
  }
  if ( !error() && d->currentState == SmtpJobPrivate::Precommand ) {
    d->currentState = SmtpJobPrivate::Smtp;
    startSmtpJob();
    return;
  }
  if ( !error() )
    emitResult();
}

void SmtpJob::dataRequest(KIO::Job * job, QByteArray & data)
{
  Q_ASSERT( job );
  if ( buffer()->atEnd() )
    data.clear();
  else
    data = buffer()->read( 32*1024 );
  setProcessedAmount( KJob::Bytes, buffer()->pos() );
}

void SmtpJob::slaveError(KIO::Slave * slave, int errorCode,
                         const QString & errorMsg)
{
  removeSlaveFromPool( slave, errorCode != KIO::ERR_SLAVE_DIED );
  if ( d->slave == slave ) {
    setError( errorCode );
    setErrorText( KIO::buildErrorString( errorCode, errorMsg ) );
    emitResult();
  }
}

#include "smtpjob.moc"
