/*
 * Copyright (C) 2006 by Marc Boris Duerner
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

#include "Pt/Convert.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <iostream>

class ConversionTest : public Pt::Unit::TestSuite
{
    public:
        ConversionTest()
        : Pt::Unit::TestSuite("ConversionTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Bool", *this, &ConversionTest::Bool );
            Pt::Unit::TestSuite::registerMethod( "Int", *this, &ConversionTest::Int );
            Pt::Unit::TestSuite::registerMethod( "Char8", *this, &ConversionTest::Char8 );
            Pt::Unit::TestSuite::registerMethod( "UChar8", *this, &ConversionTest::UChar8 );
            Pt::Unit::TestSuite::registerMethod( "SChar8", *this, &ConversionTest::SChar8 );
            Pt::Unit::TestSuite::registerMethod( "stdstring", *this, &ConversionTest::stdstring );
            Pt::Unit::TestSuite::registerMethod( "Float", *this, &ConversionTest::Float );
            Pt::Unit::TestSuite::registerMethod( "Double", *this, &ConversionTest::Double );
            Pt::Unit::TestSuite::registerMethod( "VoidPtr", *this, &ConversionTest::VoidPtr );
        }

    protected:
        void Bool();
        void Int();
        void Char8();
        void UChar8();
        void SChar8();
        void stdstring();
        void Float();
        void Double();
        void VoidPtr();
};

Pt::Unit::RegisterTest<ConversionTest> register_ConversionTest;


void ConversionTest::Bool()
{
    bool b = true;
    Pt::String str = Pt::convert<Pt::String>(b);
    PT_UNIT_ASSERT( str == L"true" );

    str = L"false";
    b = Pt::convert<bool>(str);
    PT_UNIT_ASSERT( b == false );
}


void ConversionTest::Int()
{
    int value = 10;
    Pt::String str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str == L"10" );

    str = L"20";
    value = Pt::convert<int>(str);
    PT_UNIT_ASSERT( value == 20 );
}


void ConversionTest::Char8()
{
    char value = 'a';
    Pt::String str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str == L"a" );

    str = L"b";
    value = Pt::convert<char>(str);
    PT_UNIT_ASSERT( value == 'b' );
}


void ConversionTest::UChar8()
{
    unsigned char value = 1;
    Pt::String str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str == L"1" );

    str = L"2";
    value = Pt::convert<unsigned char>(str);
    PT_UNIT_ASSERT( value == 2 );
}


void ConversionTest::SChar8()
{
    signed char value = 1;
    Pt::String str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str == L"1" );

    str = L"2";
    value = Pt::convert<signed char>(str);
    PT_UNIT_ASSERT( value == 2 );
}


void ConversionTest::stdstring()
{
    std::string value = "aaa";
    Pt::String str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str == L"aaa" );

    str = L"bbb";
    value = Pt::convert<std::string>(str);
    PT_UNIT_ASSERT( value == "bbb" );
}


void ConversionTest::Float()
{
    float value = 1.23f;
    Pt::String str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str == L"1.23" );

    str = L"2.34";
    value = Pt::convert<float>(str);
    PT_UNIT_ASSERT( value > 2.3F && value < 2.4f );
}

void ConversionTest::Double()
{
    double value = 1.2345678910;
    Pt::String str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str == L"1.234567891000000" );

    str = L"2.3456789";
    value = Pt::convert<double>(str);
    PT_UNIT_ASSERT( value > 2.3 && value < 2.4 );
}

void ConversionTest::VoidPtr()
{
    void* value = (void*)0x12345678;
    Pt::String str = Pt::convert<Pt::String>(value);
    void* value2 = Pt::convert<void*>(str);
    PT_UNIT_ASSERT( value == value2 );
}
