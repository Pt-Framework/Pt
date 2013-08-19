/*
 * Copyright (C) 2006 - 2011 by Marc Boris Duerner
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

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Ssl/Certificate.h"
#include "Pt/Ssl/Context.h"
#include "Pt/Ssl/StreamBuffer.h"
#include "Pt/System/Logger.h"
#include <string>
#include <fstream>

const char clientPem [] = 
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDkjCCAnqgAwIBAgIBZTANBgkqhkiG9w0BAQUFADBVMQswCQYDVQQGEwJVUzEa\n"
    "MBgGA1UEBxMRQ2hleWVubmUgTW91bnRhaW4xGTAXBgNVBAoTEFN0YXJnYXRlIENv\n"
    "bW1hbmQxDzANBgNVBAMTBlNHQyBDQTAeFw0xMTA2MDcwMjU0NDdaFw0xMjA2MDYw\n"
    "MjU0NDdaMFMxCzAJBgNVBAYTAlhYMRAwDgYDVQQHEwdMYW50YW5hMRYwFAYDVQQK\n"
    "Ew1BdGxhbnRpcyBDaXR5MRowGAYDVQQDExFBbmNpZW50IE1haW5mcmFtZTCCASIw\n"
    "DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL1wssBiOCiEHc033rcg7XPzx/ka\n"
    "Ol1XPvowBwPhOBrNpVMogC+CU9f9C4qFzjPwYXd07CE/wMcbEepbYNqW5u810rWF\n"
    "SM1y6E5f1Ow5d+lb81ZAtrauhZsrNheJ9qJWPgcOhQ4RfCP9JNW/hUkzXuBN+G5F\n"
    "swpI9xwksjW8AOLXyn0ayFGcVQT/4Lz7eFHGdr7rVVi5GtnV6UmKkQe6dvjBqKxn\n"
    "s3V/gtFffZDsZj9IxWmNubdUiUHru1PTmHpGZ+684w86+ldJEb3RCeteaGnJR1kR\n"
    "+qliCA06O0DVQ685hrnfuH4Dbk8mmBsNAi/MfSjExE3ZLP7Y/2vKukHCX70CAwEA\n"
    "AaNvMG0wCQYDVR0TBAIwADALBgNVHQ8EBAMCBLAwEwYDVR0lBAwwCgYIKwYBBQUH\n"
    "AwIwEQYJYIZIAYb4QgEBBAQDAgeAMCsGA1UdHwQkMCIwIKAeoByGGmh0dHA6Ly90\n"
    "ZXN0Y2EubG9jYWwvY2EuY3JsMA0GCSqGSIb3DQEBBQUAA4IBAQC2RzOA/D5XPKfi\n"
    "46oznIVx13cHMxoAf/0ACWZVpcXtyfXLr1/SzuLisXf5nCuk0jSKhbo0eeQAXxEW\n"
    "/xFFo1bABBoKRQnFy0eyGeTicUf6o0O2V9vEs6rc8PM9IZo9mLIVuJBOJMDxFG7u\n"
    "YHSgY9ZgQ1nkoOE8oO1gPSjCsYHI2SlUJ8kURedKwLxErwUpkXTquEclgQZWW5f6\n"
    "niGdGfNUWlP/y4KTd+RyGXNfrITKG63uCEqFBeJuBfA0FjLroxxyL6umI6XRgiH0\n"
    "nAHRivMOTNZjyft/nmHlbpGeHuQ6dBTMCGhHc+krzm/uf2vyy8guphBZNO/1A4SQ\n"
    "jhoJkRV8\n"
    "-----END CERTIFICATE-----\n";

class ContextTest : public Pt::Unit::TestSuite
{
    public:
        ContextTest()
        : Pt::Unit::TestSuite("ContextTest")
        {
            //Pt::System::Logger::getTarget("Pt.Ssl").setLogLevel(Pt::System::Trace);

            //this->registerMethod("Ciphers", *this, &ContextTest::Ciphers);

            this->registerMethod("Import", *this, &ContextTest::Import);
            this->registerMethod("Assign", *this, &ContextTest::Assign);
        }

        void setUp()
        { }

        void tearDown()
        { }

        void Import()
        {
            // adjust the path
            #ifdef _WIN32
                std::ifstream ifs("src\\Pt-Ssl\\tests\\cert\\multiple\\server_chain-with-password.p12", std::ios::binary);
            #else
                std::ifstream ifs("src/Pt-Ssl/tests/cert/multiple/server_chain-with-password.p12", std::ios::binary);
            #endif

            Pt::Ssl::CertificateStore store;
            store.loadPkcs12(ifs, "123");
    
            const Pt::Ssl::Certificate* cert = store.findCertificate("Server");
            PT_UNIT_ASSERT(cert);
    
            cert = store.findCertificate("Intermediate CA 2");
            PT_UNIT_ASSERT(cert);
        }

        void Assign()
        {
            #ifdef _WIN32
                std::ifstream server_ifs("src\\Pt-Ssl\\tests\\cert\\server-with-password.p12", std::ios::binary);
            #else
                std::ifstream server_ifs("src/Pt-Ssl/tests/cert/server-with-password.p12", std::ios::binary);
            #endif
            
            #ifdef _WIN32
                std::ifstream ifs_ca("src\\Pt-Ssl\\tests\\cert\\ca-with-password.p12", std::ios::binary);
            #else
                std::ifstream ifs_ca("src/Pt-Ssl/tests/cert/ca-with-password.p12", std::ios::binary);
            #endif

            Pt::Ssl::CertificateStore store;
            store.loadPkcs12(server_ifs, "123");
            store.loadPkcs12(ifs_ca, "123");

            Pt::Ssl::Context ctx;

            const Pt::Ssl::Certificate* cert = store.findCertificate("SGC Mainframe");
            PT_UNIT_ASSERT(cert);
            ctx.setCertificate(*cert);

            cert = store.findCertificate("SGC Certificate Authority");
            PT_UNIT_ASSERT(cert);
            ctx.addCACertificate(*cert);
   
            Pt::Ssl::Context ctx2;
            ctx2.assign(ctx);
    
            //cert = ctx2.findCertificate("SGC Mainframe");
            //PT_UNIT_ASSERT(cert);

            //cert = ctx2.findCertificate("SGC Certificate Authority");
            //PT_UNIT_ASSERT(cert);
    
            //cert = ctx2.findCertificate("Root CA");
            //PT_UNIT_ASSERT(cert);
            //std::exit(0);
        }
};

Pt::Unit::RegisterTest<ContextTest> register_ContextTestTest;


//void ContextTest::Ciphers()
//{
//    std::vector<std::string> cipherNames1;
//    std::vector<std::string> cipherNames2;
//    Pt::Ssl::Context ctx;
//
//    std::iostream ios(0);
//    Pt::Ssl::StreamBuffer sb(ctx, *ios.rdbuf());
//
//    Pt::Ssl::CipherList::ConstIterator it;
//    Pt::Ssl::CipherList ciphers1 = sb.ciphers();
//    for(it = ciphers1.begin(); it != ciphers1.end(); ++it)
//    {
//        cipherNames1.push_back( it->name() );
//    }
//
//    Pt::Ssl::CipherList ciphers2 = ciphers1;
//    for(it = ciphers2.begin(); it != ciphers2.end(); ++it)
//    {
//        cipherNames2.push_back( it->name() );
//    }
//    
//    PT_UNIT_ASSERT(cipherNames1.size() > 0);
//    PT_UNIT_ASSERT(cipherNames1 == cipherNames2);
//}
