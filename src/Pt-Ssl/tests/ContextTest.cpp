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
#include "Pt/Ssl/SSLContext.h"
#include "Pt/Ssl/SSLStreamBuf.h"
#include "Pt/System/Logger.h"
#include <string>

class ContextTest : public Pt::Unit::TestSuite
{
    public:
        ContextTest()
        : Pt::Unit::TestSuite("ContextTest")
        {
            //Pt::System::Logger::getTarget("Pt.Ssl").setLogLevel(Pt::System::Trace);

            this->registerMethod("Ciphers", *this, &ContextTest::Ciphers);
        }

        void setUp()
        { }

        void tearDown()
        { }

        void Ciphers();
};

Pt::Unit::RegisterTest<ContextTest> register_ContextTestTest;


void ContextTest::Ciphers()
{
    std::vector<std::string> cipherNames1;
    std::vector<std::string> cipherNames2;
    Pt::Ssl::SSLContext ctx;

    std::iostream ios(0);
    Pt::Ssl::SSLStreamBuf sb(ios, ctx);

    const Pt::Ssl::CipherList& ciphers1 = sb.ciphers();
    PT_UNIT_ASSERT(ciphers1.size() > 0);

    Pt::Ssl::CipherList::Iterator it;
    Pt::Ssl::CipherList::Iterator end = ciphers1.end();
    for(it = ciphers1.begin(); it != end; ++it)
    {
        cipherNames1.push_back( it->name() );
    }

    Pt::Ssl::CipherList ciphers2 = ciphers1;
    PT_UNIT_ASSERT(ciphers2.size() > 0);

    end = ciphers2.end();
    for(it = ciphers2.begin(); it != end; ++it)
    {
        cipherNames2.push_back( it->name() );
    }
    
    PT_UNIT_ASSERT(cipherNames1 == cipherNames2);
}

