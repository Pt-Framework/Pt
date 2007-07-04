/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Dürner                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Date.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Unit/TextProtocol.h"
#include <string>
#include "../Pt-Log/PropertiesArchive.h"
#include "../Pt-Log/PropertiesReader.h"
#include "Pt/Archive.h"
#include "Pt/Date.h"
#include "Pt/Text/TextStream.h"
#include "Pt/Text/Utf8Codec.h"

class Protocol : public Pt::Unit::TestProtocol
{
    public:
        virtual void run(Pt::Unit::TestSuite& suite)
        {
            std::stringstream ss;
            ss << "myDate = ( julianDays = 400000 )";
            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::PropertiesReader reader(ts);
            Pt::Settings archive;
            reader.read(archive);

            const Pt::Archive* ar = archive.getArchive(L"myDate");

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
            Pt::Unit::TestSuite::registerTest( "test", *this, &TextProtocolTest::test );
        }

    protected:
        void test(const Pt::Date& date)
        {
            //PT_UNIT_ASSERT( date.julian() == 400000 );
        }
};

Pt::Unit::RegisterTest<TextProtocolTest> register_TextProtocolTest;

