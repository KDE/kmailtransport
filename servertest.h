/* 
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

#ifndef MAILTRANSPORT_SERVERTEST_H
#define MAILTRANSPORT_SERVERTEST_H

#include <mailtransport/mailtransport_export.h>
#include <mailtransport/transport.h>

#include <QWidget>
#include <QHash>

class QProgressBar;
class QString;

namespace MailTransport
{

  class Socket;
  
  /**
   * @class ServerTest
   * This class can be used to test certain server to see if they support stuff.
   * @author Tom Albers <tomalbers@kde.nl>
   */
  class MAILTRANSPORT_EXPORT ServerTest : public QWidget
  {
      Q_OBJECT

    public:
      /**
       * Constructor
       * @param parent Parent Widget
       */
      ServerTest( QWidget* parent = 0 );

      /**
       * Destructor
       */
      ~ServerTest();

      /**
       * Set the server to test.
       */
      void setServer( const QString& server ) { m_server = server; };

      /**
       * Makes @p pb the progressbar to use. This class will call show()
       * and hide() and will count down. It does not take ownership of
       * the progressbar.
       */
      void setProgressBar( QProgressBar* pb ) { m_testProgress = pb; };

      /**
       * Set @p proto the protocol to test, currently supported are
       * "smtp" and "imap". This will be an enum soon.
       */
      void setProtocol( const QString& proto ) { m_proto = proto; };

      /**
       * Starts the test. Will emit finished() when done.
       */
      void start();

      /**
        * Get the protocols for the normal connections.. Call this only
        * after the finished() signals has been sent.
        * @return an enum of the type Transport::EnumAuthenticationType
        */
      QList< int > normalProtocols();

      /**
        * Get the protocols for the secure connections.. Call this only
        * after the finished() signals has been sent.
        * @return an enum of the type Transport::EnumAuthenticationType
       */
      QList< int > secureProtocols();

    Q_SIGNALS:
      /**
       * This will be emitted when the test is done. It will contain
       * the values from the enum EnumEncryption which are possible.
       */
      void finished( QList< int > );

    private:
      void finalResult();
      void read( int type, const QString& text );

      QString                 m_server;
      QString                 m_proto;

      Socket*                 m_normal;
      Socket*                 m_ssl;

      QList< int >    m_testResult;
      QHash< int, QList<int> > m_protocolResult;
      QTimer*                 m_normalTimer;
      QTimer*                 m_sslTimer;
      QTimer*                 m_progressTimer;

      QProgressBar*           m_testProgress;

      bool                    m_sslFinished;
      bool                    m_normalFinished;

    private Q_SLOTS:
      void slotNormalPossible();
      void slotNormalNotPossible();
      void slotSslPossible();
      void slotSslNotPossible();
      void slotReadNormal( const QString& text );
      void slotReadSecure( const QString& text );
      void slotUpdateProgress();
  };

}

#endif
