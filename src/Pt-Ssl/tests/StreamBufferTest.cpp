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
#include <fstream>

class StreamBufferTest : public Pt::Unit::TestSuite
{
    public:
        StreamBufferTest()
        : Pt::Unit::TestSuite("StreamBufferTest")
        {
            Pt::System::Logger::setLogLevel("", Pt::System::Error);

            this->registerMethod("Handshake", *this, &StreamBufferTest::Handshake);
            this->registerMethod("CertificateStore", *this, &StreamBufferTest::CertificateStore);
        }

        void setUp()
        { }

        void tearDown()
        { }

        void CertificateStore();
        void Handshake();
};


Pt::Unit::RegisterTest<StreamBufferTest> register_StreamBufferTest;

void StreamBufferTest::CertificateStore()
{
    // adjust the path
    #ifdef _WIN32
        std::ifstream ifs("src\\Pt-Ssl\\tests\\cert\\ca-with-password.p12", std::ios::binary);
    #else
        std::ifstream ifs("src/Pt-Ssl/tests/cert/ca-with-password.p12", std::ios::binary);
    #endif

    Pt::Ssl::CertificateStore certStore;

    // load without password, as in the other test
    certStore.loadPkcs12(ifs, "123");
}


void StreamBufferTest::Handshake()
{
    // adjust the path
    #ifdef _WIN32
        std::ifstream ifs("src\\Pt-Ssl\\tests\\cert\\client-with-password.p12", std::ios::binary);
    #else
        std::ifstream ifs("src/Pt-Ssl/tests/cert/client-with-password.p12", std::ios::binary);
    #endif

    #ifdef _WIN32
        std::ifstream ifs_ca("src\\Pt-Ssl\\tests\\cert\\ca-with-password.p12", std::ios::binary);
    #else
        std::ifstream ifs_ca("src/Pt-Ssl/tests/cert/ca-with-password.p12", std::ios::binary);
    #endif

    #ifdef _WIN32
        std::ifstream server_ifs("src\\Pt-Ssl\\tests\\cert\\server-with-password.p12", std::ios::binary);
    #else
        std::ifstream server_ifs("src/Pt-Ssl/tests/cert/server-with-password.p12", std::ios::binary);
    #endif

    //Pt::Ssl::CertificateStore certStore;
    //certStore.loadPkcs12(ifs, "123");
    //PT_UNIT_ASSERT( ! certStore.empty() );

    Pt::Ssl::CertificateStore caStore;
    caStore.loadPkcs12(ifs_ca, "123");
    PT_UNIT_ASSERT( ! caStore.empty() );

    Pt::Ssl::CertificateStore serverCerts;
    serverCerts.loadPkcs12(server_ifs, "123");
    PT_UNIT_ASSERT( ! serverCerts.empty() );

    Pt::Ssl::Context serverContext;
    serverContext.setCertificate( *serverCerts.begin() );
    serverContext.setCACertificates( caStore );
    serverContext.setVerifyMode(Pt::Ssl::Context::VerifyPeerRequired);

    // client-side SSL context
    Pt::Ssl::Context clientContext;
    clientContext.loadPkcs12(ifs, "123");
    const Pt::Ssl::Certificate* cert = clientContext.findCertificate("Atlantis Mainframe");
    PT_UNIT_ASSERT( cert != 0 );

    clientContext.setCertificate( *cert );
    clientContext.setCACertificates( caStore );
    clientContext.setVerifyMode(Pt::Ssl::Context::VerifyPeer);

    // client begins the handshake
    std::stringstream data;
    Pt::Ssl::StreamBuffer client(clientContext, *data.rdbuf(), Pt::Ssl::StreamBuffer::Connect);

    // server begins the handskake
    Pt::Ssl::StreamBuffer server(serverContext, *data.rdbuf(), Pt::Ssl::StreamBuffer::Accept);

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
}

