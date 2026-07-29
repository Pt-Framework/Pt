/*
 * Copyright (C) 2010 - 2012 by Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Pkcs12Data.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Ssl/Certificate.h"
#include "Pt/Ssl/CertificateStore.h"
#include "Pt/Ssl/Context.h"
#include "Pt/Ssl/StreamBuffer.h"
#include "Pt/Ssl/IOStream.h"
#include "Pt/System/Logger.h"

class StreamBufferTest : public Pt::Unit::TestSuite
{
    public:
        StreamBufferTest()
        : Pt::Unit::TestSuite("StreamBufferTest")
        {
            //Pt::System::Logger::setLogLevel("Pt.Ssl", Pt::System::Trace);
            this->registerMethod("Handshake", *this, &StreamBufferTest::Handshake);
        }

        void Handshake();
};


Pt::Unit::RegisterTest<StreamBufferTest> register_StreamBufferTest;

void StreamBufferTest::Handshake()
{
    const char* serverCerts = reinterpret_cast<const char*>(serverPkcs12);
    const char* clientCerts = reinterpret_cast<const char*>(clientPkcs12);
    const char* caCerts = reinterpret_cast<const char*>(caPkcs12);

    Pt::Ssl::CertificateStore store;
    store.loadPkcs12(serverCerts, sizeof(serverPkcs12), "123");
    store.loadPkcs12(clientCerts, sizeof(clientPkcs12), "123");
    store.loadPkcs12(caCerts, sizeof(caPkcs12), "123");

    // Server context
    Pt::Ssl::Context serverContext(Pt::Ssl::TLSv1_2);
    serverContext.setVerifyMode(Pt::Ssl::AlwaysVerify);

    const Pt::Ssl::Certificate* servCert = store.findCertificate("SGC Mainframe");
    PT_UNIT_ASSERT( servCert );
    serverContext.setIdentity( *servCert );

    const Pt::Ssl::Certificate* servCA = store.findCertificate("SGC Certificate Authority");
    PT_UNIT_ASSERT( servCA );
    serverContext.addCACertificate(*servCA);

    // Client context
    Pt::Ssl::Context clientContext(Pt::Ssl::TLSv1_2);
    clientContext.setVerifyMode(Pt::Ssl::AlwaysVerify);

    const Pt::Ssl::Certificate* clientCert = store.findCertificate("Atlantis Mainframe");
    PT_UNIT_ASSERT( clientCert );
    clientContext.setIdentity( *clientCert );

    const Pt::Ssl::Certificate* clientCA = store.findCertificate("SGC Certificate Authority");
    PT_UNIT_ASSERT( clientCA );
    clientContext.addCACertificate(*clientCA);

    // client begins the handshake
    std::stringstream data;
    Pt::Ssl::StreamBuffer client(clientContext, data, Pt::Ssl::Connect);
    client.setPeerName("SGC Mainframe");

    // server begins the handskake
    Pt::Ssl::StreamBuffer server(serverContext, data, Pt::Ssl::Accept);

    for( int n = 0; n < 20; ++n)
    {
        // client handshake progress
        while( client.writeHandshake() )
            ;

        PT_UNIT_ASSERT(data.str().size() > 0);
        data.str( data.str() );
        data.clear();

        // server handshake progress
        while( server.readHandshake() )
            ;

        data.clear();
        data.str( std::string() );

        while( server.writeHandshake() )
            ;

        PT_UNIT_ASSERT(data.str().size() > 0);
        data.str( data.str() );
        data.clear();

        // client handshake progress
        while( client.readHandshake() )
            ;

        data.clear();
        data.str( std::string() );

        if( client.isConnected() )
            break;
    }

    PT_UNIT_ASSERT( client.isConnected() );
    PT_UNIT_ASSERT( server.isConnected() );

    data.clear();
    data.str( std::string() );

    client.sputn("Hello Server!", 13);
    client.pubsync();

    char buf[16] = {0};
    server.sgetn( buf, sizeof(buf) - 1 );

    data.clear();
    data.str( std::string() );

    server.sputn("Hello Client!", 13);
    server.pubsync();

    char buf2[16] = {0};
    client.sgetn( buf2, sizeof(buf2) - 1 );
    PT_UNIT_ASSERT( std::string(buf2) == "Hello Client!" );
}
