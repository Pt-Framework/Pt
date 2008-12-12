/*
 * Copyright (C) 2005-2006 by Marc Boris Duerner
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
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Unit/TextProtocol.h"

#include "Pt/Date.h"
#include "Pt/Text/TextStream.h"
#include "Pt/Text/Utf8Codec.h"
#include <string>

class Protocol : public Pt::Unit::TestProtocol
{
    public:
        virtual void run(Pt::Unit::TestSuite& suite)
        {
            std::stringstream ss;
            ss << "myDate = ( julianDays = 400000 )";
            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            //Pt::PropertiesReader reader(ts);
            //Pt::Settings archive;
            //reader.read(archive);

            //const Pt::Archive* ar = archive.getArchive(L"myDate");

            //suite.runTest("test", *ar);
        }
} prot;


Pt::Unit::TextProtocol tprot("TextProtocolTest.ctp");

class TextProtocolTest : public Pt::Unit::TestSuite
{
    public:
        TextProtocolTest()
        : Pt::Unit::TestSuite("TextProtocolTest", tprot)
        {
            Pt::Unit::TestSuite::registerMethod( "test", *this, &TextProtocolTest::test );
        }

    protected:
        void test(const Pt::Date& date)
        {
            //PT_UNIT_ASSERT( date.julian() == 400000 );
        }
};

Pt::Unit::RegisterTest<TextProtocolTest> register_TextProtocolTest;

