/***************************************************************************
 *   Copyright (C) 2007 Tobias Mueller                                     *
 *   Copyright (C) 2007 PTV AG                                             *
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
#undef PT_API_EXPORT

#include "Pt/StringStream.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/TestSchedule.h"
#include "Pt/Unit/RegisterTest.h"

#include <iostream>


class StringStreamTest : public Pt::Unit::TestSuite
{
	public:
        StringStreamTest()
        : Pt::Unit::TestSuite("StringStreamTest")
        {
            Pt::Unit::TestSuite::registerMethod("Getline", *this, &StringStreamTest::getline );
            Pt::Unit::TestSuite::registerMethod("ReadFloat", *this, &StringStreamTest::readInt );
            Pt::Unit::TestSuite::registerMethod("ReadInt", *this, &StringStreamTest::readFloat );
            Pt::Unit::TestSuite::registerMethod("RdbufTest", *this, &StringStreamTest::rdbufTest );

        }

        void getline();
        void readInt();
        void readFloat();
        void rdbufTest();
};

Pt::Unit::RegisterTest<StringStreamTest> _registerStringStreamTest;



void StringStreamTest::getline()
{
    Pt::String str = L"hier bin ich\n";
    Pt::StringStream s(str);

    Pt::String strLine;

    std::getline(s, strLine);

    PT_UNIT_ASSERT(strLine == L"hier bin ich");
}


void StringStreamTest::readInt()
{
    Pt::String str = L"123";
    Pt::StringStream s(str);

    int i;
    s >> i;

    PT_UNIT_ASSERT(i == 123);
}

void StringStreamTest::readFloat()
{
    Pt::String str = L"123.125";
    Pt::StringStream s(str);

    float f;
    s >> f;

    PT_UNIT_ASSERT(f == 123.125);
}


void StringStreamTest::rdbufTest()
{
    Pt::String str = L"hier ist noch ein Test";
    Pt::StringStream s(str);

    std::basic_stringbuf<Pt::Char>* buffer = s.rdbuf();

    PT_UNIT_ASSERT(buffer->str() == L"hier ist noch ein Test");
}


