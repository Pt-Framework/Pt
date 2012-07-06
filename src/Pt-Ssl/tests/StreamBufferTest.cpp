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

#include "PemData.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Ssl/CertificateList.h"
#include "Pt/Ssl/Context.h"
#include "Pt/Ssl/StreamBuffer.h"
#include "Pt/System/Logger.h"
#include <sstream>

class StreamBufferTest : public Pt::Unit::TestSuite
{
    public:
        StreamBufferTest()
        : Pt::Unit::TestSuite("StreamBufferTest")
        {
            //Pt::System::Logger::getTarget("Pt.Ssl").setLogLevel(Pt::System::Trace);

            this->registerMethod("Handshake", *this, &StreamBufferTest::Handshake);
        }

        void setUp()
        { }

        void tearDown()
        { }

        void Handshake();
};


Pt::Unit::RegisterTest<StreamBufferTest> register_StreamBufferTest;


void StreamBufferTest::Handshake()
{
    std::stringstream data;

    Pt::Ssl::CertificateList caCert;
    caCert.fromPem(caPemData, sizeof(caPemData));

    // server-side SSL context
    Pt::Ssl::CertificateList serverCert;
    serverCert.fromPem(serverCertPemData, sizeof(serverCertPemData));

    Pt::Ssl::PrivateKey serverPrivKey("abc123");
    serverPrivKey.fromPem(serverKeyData, sizeof(serverKeyData));

    Pt::Ssl::Context serverContext;
    serverContext.setCACertificates(caCert);
    serverContext.setCertificateChain(serverCert);
    serverContext.setPrivateKey(serverPrivKey);

    // client-side SSL context
    Pt::Ssl::CertificateList clientCert;
    clientCert.fromPem(clientCertPemData, sizeof(clientCertPemData));

    Pt::Ssl::PrivateKey clientPrivKey("");
    clientPrivKey.fromPem(clientKeyData, sizeof(clientKeyData));

    Pt::Ssl::Context clientContext;
    clientContext.setCACertificates(caCert);
    clientContext.setCertificateChain(clientCert);
    clientContext.setPrivateKey(clientPrivKey);

    // client begins the handshake
    Pt::Ssl::StreamBuffer client(clientContext, *data.rdbuf());
    client.setConnecting(true);

    // server begins the handskake
    Pt::Ssl::StreamBuffer server(serverContext, *data.rdbuf());
    server.setAccepting(true, true);

    for( ; ; )
    {
        // client handshake progress
        while( client.writeHandshake() )
            ;
    
        PT_UNIT_ASSERT(data.str().size() > 0);
        data.clear();

        // server handshake progress
        while( server.readHandshake() )
            ;
    
        data.clear();
        data.str( std::string() );
    
        while( server.writeHandshake() )
            ;
    
        PT_UNIT_ASSERT(data.str().size() > 0);
        data.clear();

        // client handshake progress
        while( client.readHandshake() )
            ;

        data.clear();
        data.str( std::string() );

        if( client.connected() )
            break;
    }

    PT_UNIT_ASSERT( client.connected() );
    PT_UNIT_ASSERT( server.connected() );
}

