/*
 * Copyright (C) 2007 Tobias Mueller
 * Copyright (C) 2007 PTV AG
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
#undef PT_API_EXPORT

#include "Pt/StringStream.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <iostream>
#include <iomanip>

class StringStreamTest : public Pt::Unit::TestSuite
{
    public:
        StringStreamTest()
        : Pt::Unit::TestSuite("StringStreamTest")
        {
            Pt::Unit::TestSuite::registerMethod("ReadFloat", *this, &StringStreamTest::ReadFloat );
            Pt::Unit::TestSuite::registerMethod("ReadInt", *this, &StringStreamTest::readInt );
            Pt::Unit::TestSuite::registerMethod("RdbufTest", *this, &StringStreamTest::rdbufTest );
            Pt::Unit::TestSuite::registerMethod("Getline", *this, &StringStreamTest::getline );
            Pt::Unit::TestSuite::registerMethod("WriteBool", *this, &StringStreamTest::WriteBool );
            Pt::Unit::TestSuite::registerMethod("WriteHex", *this, &StringStreamTest::WriteHex );
            Pt::Unit::TestSuite::registerMethod("WriteOct", *this, &StringStreamTest::WriteOct );
            Pt::Unit::TestSuite::registerMethod("WriteDec", *this, &StringStreamTest::WriteDec );
            Pt::Unit::TestSuite::registerMethod("WriteFixed", *this, &StringStreamTest::WriteFixed );
            Pt::Unit::TestSuite::registerMethod("WriteScientific", *this, &StringStreamTest::WriteScientific );
            Pt::Unit::TestSuite::registerMethod("WritePtr", *this, &StringStreamTest::WritePtr );

        }

        void readInt();
        void ReadFloat();
        void rdbufTest();
        void getline();
        void WriteBool();
        void WriteHex();
        void WriteOct();
        void WriteDec();
        void WriteFixed();
        void WriteScientific();
        void WritePtr();
};

Pt::Unit::RegisterTest<StringStreamTest> _registerStringStreamTest;


void StringStreamTest::readInt()
{
    Pt::String str = L"123";
    Pt::StringStream s(str);

    int i = 0;
    s >> i;

    PT_UNIT_ASSERT(i == 123);
}

void StringStreamTest::ReadFloat()
{
    Pt::String str;
    Pt::StringStream s;
    float f = 0.0f;

    s.clear();
    s.str(L"123.125");
    s >> f;
    PT_UNIT_ASSERT( std::fabs(f - 123.125) < 0.01);

    f = 0.0f;
    s.clear();
    s.str(L"-123.125");
    s >> f;
    s >> str;
    PT_UNIT_ASSERT(std::fabs(f + 123.125) < 0.01);

    str.clear();
    f = 0.0f;
    s.clear();
    s.str(L"   123.125  a ");
    s >> f;
    s >> str;
    PT_UNIT_ASSERT(std::fabs(f - 123.125) < 0.01);
    PT_UNIT_ASSERT(str == L"a");

    f = 0.0f;
    s.clear();
    s.str(L"1.2345e+02");
    s >> f;
    PT_UNIT_ASSERT(std::fabs(f - 123.45) < 0.01);

    f = 0.0f;
    s.clear();
    s.str(L"-1.2345e+02");
    s >> f;
    PT_UNIT_ASSERT( std::fabs(f + 123.45) < 0.01);

    str.clear();
    f = 0.0f;
    s.clear();
    s.str(L"   1.2345e+02   a ");
    s >> f;
    s >> str;
    PT_UNIT_ASSERT( std::fabs(f - 123.45) < 0.01);
    PT_UNIT_ASSERT(str == L"a");
}


void StringStreamTest::rdbufTest()
{
    Pt::String str = L"hier ist noch ein Test";
    Pt::StringStream s(str);

    std::basic_stringbuf<Pt::Char>* buffer = s.rdbuf();

    PT_UNIT_ASSERT(buffer->str() == L"hier ist noch ein Test");
}


void StringStreamTest::getline()
{
    Pt::String str = L"hier bin ich\n";
    Pt::StringStream s(str);

    Pt::String strLine;

    std::getline(s, strLine);

    PT_UNIT_ASSERT(strLine == L"hier bin ich");
}


void StringStreamTest::WriteBool()
{
    Pt::StringStream ss;
    Pt::String str;

    ss << true;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"1");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::internal <<  std::setw(10) << false;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"         0");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::boolalpha << std::right << std::setw(10) << true << std::flush;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"      true");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::boolalpha << std::left << std::setw(10) << false << std::flush;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"false     ");
}


void StringStreamTest::WriteHex()
{
    Pt::StringStream ss;
    Pt::String str;

    ss << std::hex << 42;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"2a");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::hex << std::internal <<  std::setw(10) << 42;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"        2a");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::hex << std::showbase << std::right << std::setw(10) << 42 << std::flush;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"      0x2a");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::hex << std::showbase << std::left << std::setw(10) << 42 << std::flush;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"0x2a      ");
}


void StringStreamTest::WriteOct()
{
    Pt::StringStream ss;
    Pt::String str;

    ss << std::oct << 42;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"52");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::oct << std::internal <<  std::setw(10) << 42;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"        52");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::oct << std::showbase << std::right << std::setw(10) << 42 << std::flush;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"       052");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::oct << std::showbase << std::left << std::setw(10) << 42 << std::flush;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"052       ");
}


void StringStreamTest::WriteDec()
{
    Pt::StringStream ss;
    Pt::String str;

    ss << std::dec << 42;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"42");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::dec << std::internal <<  std::setw(10) << 42;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"        42");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::dec << std::internal <<  std::setw(10) << -42;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"-       42");
    
    ss.clear();
    ss.str( Pt::String() );

    ss << std::dec << std::showpos << std::internal <<  std::setw(10) << 42;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"+       42");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::dec << std::noshowpos << std::right << std::setw(10) << 42 << std::flush;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"        42");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::dec << std::noshowpos << std::right << std::setw(10) << -42 << std::flush;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"       -42");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::dec << std::showpos << std::right << std::setw(10) << 42 << std::flush;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"       +42");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::dec << std::noshowpos << std::left << std::setw(10) << 42 << std::flush;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"42        ");
    
    ss.clear();
    ss.str( Pt::String() );

    ss << std::dec << std::noshowpos << std::left << std::setw(10) << -42 << std::flush;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"-42       ");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::dec << std::showpos << std::left << std::setw(10) << 42 << std::flush;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"+42       ");
}


void StringStreamTest::WriteFixed()
{
    Pt::StringStream ss;
    Pt::String str;
    
    ss << std::fixed << std::setprecision(3) << std::left << std::setw(10) << 0.0;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"0.000     ");
    
    ss.clear();
    ss.str( Pt::String() );

    ss << std::fixed << std::setprecision(3) << std::left << std::noshowpos << std::setw(10)  << 123.4568;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"123.457   ");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::fixed << std::setprecision(3) << std::left << std::showpos << std::setw(10) << 123.4568;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"+123.457  ");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::fixed << std::setprecision(3) << std::left << std::noshowpos << std::setw(10) << -123.4568;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"-123.457  ");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::fixed << std::setprecision(3) << std::internal << std::noshowpos << std::setw(10) << 123.4561;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"   123.456");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::fixed << std::setprecision(3) << std::internal << std::noshowpos << std::setw(10) << -123.4561;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"-  123.456");
    
    ss.clear();
    ss.str( Pt::String() );

    ss << std::fixed << std::setprecision(3) << std::internal << std::showpos << std::setw(10) << 123.4561;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"+  123.456");
    
    ss.clear();
    ss.str( Pt::String() );

    ss << std::fixed << std::setprecision(0) << std::right << std::noshowpoint << std::noshowpos << std::setw(10) << 2.8;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"         3");
    
    ss.clear();
    ss.str( Pt::String() );

    ss << std::fixed << std::setprecision(0) << std::right << std::showpoint << std::noshowpos << std::setw(10) << 2.8;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"        3.");
    
    ss.clear();
    ss.str( Pt::String() );

    ss << std::fixed << std::setprecision(1) << std::right << std::showpos << std::setw(10) << 3.14;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"      +3.1");
    
    ss.clear();
    ss.str( Pt::String() );

    ss << std::fixed << std::setprecision(2) << std::right << std::noshowpos << std::setw(10) << -10000.0;
    str = ss.str();
    PT_UNIT_ASSERT(str == L" -10000.00");
    
    
    ss.clear();
    ss.str( Pt::String() );

    ss << std::fixed << std::setprecision(5) << std::right << std::noshowpos << std::setw(10) << 0.00001;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"   0.00001");
}


void StringStreamTest::WriteScientific()
{
    Pt::StringStream ss;
    Pt::String str;

    ss << std::scientific << std::setprecision(3) << std::left  << std::setw(15) << 0.0;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"0.000e+000     ");
    
    ss.clear();
    ss.str( Pt::String() );
    
    ss << std::scientific << std::setprecision(3) << std::left << std::noshowpos << std::setw(15)  << 123.4568;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"1.235e+002     ");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::scientific << std::setprecision(3) << std::left << std::showpos << std::setw(15) << 123.4568;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"+1.235e+002    ");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::scientific << std::setprecision(3) << std::left << std::noshowpos << std::setw(15) << -123.4568;
    str = ss.str();

    PT_UNIT_ASSERT(str == L"-1.235e+002    ");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::scientific << std::setprecision(3) << std::internal << std::noshowpos << std::setw(15) << 123.4444;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"     1.234e+002");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::scientific << std::setprecision(3) << std::internal << std::noshowpos << std::setw(15) << -123.44444;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"-    1.234e+002");
    
    ss.clear();
    ss.str( Pt::String() );

    ss << std::scientific << std::setprecision(3) << std::internal << std::showpos << std::setw(15) << 123.44444;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"+    1.234e+002");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::scientific << std::setprecision(0) << std::right << std::noshowpoint << std::noshowpos << std::setw(15) << 2.8;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"         3e+000");
    
    ss.clear();
    ss.str( Pt::String() );

    ss << std::scientific << std::setprecision(0) << std::right << std::showpoint << std::noshowpos << std::setw(15) << 2.8;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"        3.e+000");
    
    ss.clear();
    ss.str( Pt::String() );

    ss << std::scientific << std::setprecision(1) << std::right << std::showpos << std::setw(15) << 3.14;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"      +3.1e+000");
    
    ss.clear();
    ss.str( Pt::String() );

    ss << std::scientific << std::setprecision(2) << std::right << std::noshowpos << std::setw(15) << -10000.0;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"     -1.00e+004");
    
    ss.clear();
    ss.str( Pt::String() );

    ss << std::scientific << std::setprecision(2) << std::right << std::noshowpos << std::setw(15) << 0.00001;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"      1.00e-005");
}


void StringStreamTest::WritePtr()
{
    Pt::StringStream ss;
    Pt::String str;

    const void* ptr = 0;
 
    ss << ptr;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"0");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::internal <<  std::setw(10) << ptr;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"         0");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::right << std::setw(10) << ptr << std::flush;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"         0");

    ss.clear();
    ss.str( Pt::String() );

    ss << std::left << std::setw(10) << ptr << std::flush;
    str = ss.str();
    PT_UNIT_ASSERT(str == L"0         ");
}
