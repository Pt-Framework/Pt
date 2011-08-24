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
#include <iterator>
#include <limits>
#include <iomanip>

class ConversionTest : public Pt::Unit::TestSuite
{
    public:
        ConversionTest()
        : Pt::Unit::TestSuite("ConversionTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Octal", *this, &ConversionTest::Octal );
            Pt::Unit::TestSuite::registerMethod( "Bool", *this, &ConversionTest::Bool );
            Pt::Unit::TestSuite::registerMethod( "NumberOverflow", *this, &ConversionTest::NumberOverflow );
            Pt::Unit::TestSuite::registerMethod( "IntToString", *this, &ConversionTest::IntToString );
            Pt::Unit::TestSuite::registerMethod( "StringToInt", *this, &ConversionTest::StringToInt );
            Pt::Unit::TestSuite::registerMethod( "UIntToString", *this, &ConversionTest::UIntToString );
            Pt::Unit::TestSuite::registerMethod( "StringToUInt", *this, &ConversionTest::StringToUInt );
            Pt::Unit::TestSuite::registerMethod( "Char8", *this, &ConversionTest::Char8 );
            Pt::Unit::TestSuite::registerMethod( "UChar8", *this, &ConversionTest::UChar8 );
            Pt::Unit::TestSuite::registerMethod( "SChar8", *this, &ConversionTest::SChar8 );
            Pt::Unit::TestSuite::registerMethod( "stdstring", *this, &ConversionTest::stdstring );
            Pt::Unit::TestSuite::registerMethod( "FloatToString", *this, &ConversionTest::FloatToString );
            Pt::Unit::TestSuite::registerMethod( "StringToFloat", *this, &ConversionTest::StringToFloat );
            Pt::Unit::TestSuite::registerMethod( "Double", *this, &ConversionTest::Double );
            Pt::Unit::TestSuite::registerMethod( "VoidPtr", *this, &ConversionTest::VoidPtr );
        }

    protected:
        void Octal();
        void Bool();
        void NumberOverflow();
        void IntToString();
        void StringToInt();
        void UIntToString();
        void StringToUInt();
        void Char8();
        void UChar8();
        void SChar8();
        void stdstring();
        void FloatToString();
        void StringToFloat();
        void Double();
        void VoidPtr();
};

Pt::Unit::RegisterTest<ConversionTest> register_ConversionTest;


void ConversionTest::Octal()
{
    int n = -42;
    std::string s;
    Pt::putOctal(std::back_inserter(s), n, std::ios_base::internal|std::ios_base::showbase, 8, ' ');
    //std::cerr << "\nOCT: " << s << std::endl;
    
    s.clear();
    Pt::putHex(std::back_inserter(s), n, std::ios_base::internal|std::ios_base::showbase, 8, ' ');
    //std::cerr << "HEX: " << s << std::endl;
    
    s.clear();
    Pt::putDecimal(std::back_inserter(s), n, std::ios_base::internal, 8, ' ');
    //std::cerr << "DEC: " << s << std::endl;
}


void ConversionTest::Bool()
{
    bool b = true;
    Pt::String str = Pt::convert<Pt::String>(b);
    PT_UNIT_ASSERT( str == L"true" );

    str = L"false";
    b = Pt::convert<bool>(str);
    PT_UNIT_ASSERT( b == false );
}


void ConversionTest::NumberOverflow()
{
    Pt::String str = L"256";
    PT_UNIT_ASSERT_THROW( Pt::convert<unsigned char>(str), Pt::ConversionError );
    
    str = L"2000";
    PT_UNIT_ASSERT_THROW( Pt::convert<unsigned char>(str), Pt::ConversionError );
    
    str = L"128";
    PT_UNIT_ASSERT_THROW( Pt::convert<signed char>(str), Pt::ConversionError );
    
    str = L"1000";
    PT_UNIT_ASSERT_THROW( Pt::convert<signed char>(str), Pt::ConversionError );
}


void ConversionTest::IntToString()
{
    int value = 0;
    Pt::String str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 4) == L"0" );

    value = 1;
    str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 8) == L"1" );

    value = -1;
    str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 12) == L"-1" );
    
    value = 123456;
    str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 8) == L"123456" );
    
    value = -123456;
    str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 12) == L"-123456" );
}

void ConversionTest::StringToInt()
{
    Pt::String str = L"0";
    int value = Pt::convert<int>(str);
    PT_UNIT_ASSERT( value == 0 );

    str = L"1";
    value = Pt::convert<int>(str);
    PT_UNIT_ASSERT( value == 1 );
    
    str = L"+1";
    value = Pt::convert<int>(str);
    PT_UNIT_ASSERT( value == 1 );
    
    str = L"-1";
    value = Pt::convert<int>(str);
    PT_UNIT_ASSERT( value == -1 );
    
    str = L"123456";
    value = Pt::convert<int>(str);
    PT_UNIT_ASSERT( value == 123456 );
    
    str = L"-123456";
    value = Pt::convert<int>(str);
    PT_UNIT_ASSERT( value == -123456 );
}


void ConversionTest::UIntToString()
{
    unsigned int value = 0;
    Pt::String str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 4) == L"0" );

    value = 1;
    str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 8) == L"1" );
    
    value = 123456;
    str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 8) == L"123456" );
}


void ConversionTest::StringToUInt()
{
    Pt::String str = L"0";
    int value = Pt::convert<unsigned int>(str);
    PT_UNIT_ASSERT( value == 0 );

    str = L"1";
    value = Pt::convert<unsigned int>(str);
    PT_UNIT_ASSERT( value == 1 );
    
    //str = L"+1";
    //value = Pt::convert<unsigned int>(str);
    //PT_UNIT_ASSERT( value == 1 );
    
    str = L"123456";
    value = Pt::convert<unsigned int>(str);
    PT_UNIT_ASSERT( value == 123456 );
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


void ConversionTest::FloatToString()
{
    float value = 1.234f;
    Pt::String str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 4) == L"1.23" );

    value = -123.4567f;
    str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 8) == L"-123.456" );
    
    value = 1000000000.01f;
    str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 12) == L"1000000000.0" );
    
    value = 0.00000000011f;
    str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 12) == L"0.0000000001" );
    
    value = std::numeric_limits<float>::quiet_NaN();
    str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 3) == L"nan" );
    
    value = std::numeric_limits<float>::infinity();
    str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 3) == L"inf" );

    value = - std::numeric_limits<float>::infinity();
    str = Pt::convert<Pt::String>(value);
    PT_UNIT_ASSERT( str.substr(0, 4) == L"-inf" );
}

void ConversionTest::StringToFloat()
{
    Pt::String str = L"1.234";
    float value = Pt::convert<float>(str);
    PT_UNIT_ASSERT( value > 1.23f && value < 1.24f );

    str = L"-123.4567";
    value = Pt::convert<float>(str);
    PT_UNIT_ASSERT( value < -123.4f && value > -123.5f );
    
    str = L"1000000000.0";
    value = Pt::convert<float>(str);
    PT_UNIT_ASSERT( value < 1000010000.0f && value > 999999000.0f );
    
    str = L"-1000000000.0";
    value = Pt::convert<float>(str);
    PT_UNIT_ASSERT( value > -1000010000.0f && value < -999999000.0f );
    
    str = L"nan";
    value = Pt::convert<float>(str);
    PT_UNIT_ASSERT(value != value);

    str = L"NaN";
    value = Pt::convert<float>(str);
    PT_UNIT_ASSERT(value != value);
 
    str = L"inf";
    value = Pt::convert<float>(str);
    PT_UNIT_ASSERT(value == std::numeric_limits<float>::infinity());

    str = L"-inf";
    value = Pt::convert<float>(str);
    PT_UNIT_ASSERT(value == - std::numeric_limits<float>::infinity());
    
    str = L"infinity";
    value = Pt::convert<float>(str);
    PT_UNIT_ASSERT(value == std::numeric_limits<float>::infinity());
}

void ConversionTest::Double()
{
    double value = 1.2345678910;
    Pt::String str = Pt::convert<Pt::String>(value);

    PT_UNIT_ASSERT( str.substr(0, 10) == L"1.23456789" );

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
