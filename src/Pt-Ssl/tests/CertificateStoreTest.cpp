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

#include "Pkcs12Data.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Ssl/Certificate.h"
#include "Pt/Ssl/CertificateStore.h"
#include "Pt/Ssl/StreamBuffer.h"
#include "Pt/System/Logger.h"
#include <fstream>

void makePkc12Data();

class CertificateStoreTest : public Pt::Unit::TestSuite
{
    public:
        CertificateStoreTest()
        : Pt::Unit::TestSuite("CertificateStoreTest")
        {
            Pt::System::Logger::setLogLevel("Pt.Ssl", Pt::System::Error);

            this->registerMethod("Import", *this, &CertificateStoreTest::Import);

            //makePkc12Data();
        }

        void Import()
        {
            const char* certChain = reinterpret_cast<const char*>(chainPkcs12);

            Pt::Ssl::CertificateStore store;
            store.loadPkcs12(certChain, sizeof(chainPkcs12), "123");

            const Pt::Ssl::Certificate* cert = store.findCertificate("Server");
            PT_UNIT_ASSERT(cert);

            cert = store.findCertificate("Intermediate CA 2");
            PT_UNIT_ASSERT(cert);

            std::size_t certCount = 0;
            Pt::Ssl::CertificateStore::ConstIterator it;
            for(it = store.begin(); it != store.end(); ++it)
            {
                ++certCount;
            }

            PT_UNIT_ASSERT(certCount != 0);
            PT_UNIT_ASSERT_EQUALS( certCount, store.size() );
        }
};

Pt::Unit::RegisterTest<CertificateStoreTest> register_CertificateStoreTest;

void makePkc12Data()
{
    #ifdef _WIN32
        std::ifstream ifs("src\\Pt-Ssl\\tests\\cert\\client.p12", std::ios::binary);
    #else
        std::ifstream ifs("src/Pt-Ssl/tests/cert/client.p12", std::ios::binary);
    #endif

    bool first = true;
    std::cout << "const unsigned char chainPkcs12[] = {";
    while(ifs)
    {
        char buf[16];
        ifs.read( buf, sizeof(buf) );
        std::streamsize size = ifs.gcount();

        if(first)
            std::cout << "\n    ";
        else if(size > 0)
            std::cout << ",\n    ";

        first = false;

        for(std::streamsize n = 0; n < size; ++n)
        {
            if(n != 0)
                std::cout << ", ";

            std::cout << (int)(unsigned char)(buf[n]);
        }
    }

    std::cout << "\n};\n";
}