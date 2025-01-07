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

#include <Pt/Convert.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <string>
#include <iostream>
#include <iterator>
#include <limits>
#include <iomanip>
#include <limits>

/** @internal @todo This is useful, move this to Pt/Iterator.h
*/
template<typename T>
class nullterm_array_iterator
{
    public:
      typedef std::input_iterator_tag iterator_category;
      typedef std::ptrdiff_t      difference_type;
      typedef T                   value_type;
      typedef T*                  pointer;
      typedef T&                  reference;

    public:
        nullterm_array_iterator()
        : _ptr(0)
        { }

        explicit nullterm_array_iterator(const T* ptr)
        : _ptr(ptr)
        {
            if(*_ptr == '\0')
                _ptr = 0;
        }

        nullterm_array_iterator<T>& operator=(const nullterm_array_iterator<T>& it)
        {
            _ptr = it._ptr;
            return *this;
        }

        bool operator==(const nullterm_array_iterator<T>& it) const
        {
            return _ptr == it._ptr;
        }

        bool operator!=(const nullterm_array_iterator<T>& it) const
        {
            return _ptr != it._ptr;
        }

        const T& operator*() const
        {
            return *_ptr;
        }

        nullterm_array_iterator<T>& operator++()
        {
            if(*++_ptr == '\0')
                _ptr = 0;

            return *this;
        }

        nullterm_array_iterator<T> operator++(int)
        {
            if(*++_ptr == '\0')
                _ptr = 0;

            return *this;
        }

    private:
        const T* _ptr;
};

class ConversionTest : public Pt::Unit::TestSuite
{
    public:
        ConversionTest()
        : Pt::Unit::TestSuite("ConversionTest")
        {
            //Pt::Unit::TestSuite::registerMethod( "Bool", *this, &ConversionTest::Bool );
            Pt::Unit::TestSuite::registerMethod( "NulltermIterator", *this, &ConversionTest::NulltermIterator );
            Pt::Unit::TestSuite::registerMethod( "NumberOverflow", *this, &ConversionTest::NumberOverflow );
            Pt::Unit::TestSuite::registerMethod( "IntToString", *this, &ConversionTest::IntToString );
            Pt::Unit::TestSuite::registerMethod( "StringToInt", *this, &ConversionTest::StringToInt );
            Pt::Unit::TestSuite::registerMethod( "UIntToString", *this, &ConversionTest::UIntToString );
            Pt::Unit::TestSuite::registerMethod( "StringToUInt", *this, &ConversionTest::StringToUInt );
            //Pt::Unit::TestSuite::registerMethod( "Char8", *this, &ConversionTest::Char8 );
            Pt::Unit::TestSuite::registerMethod( "UChar8", *this, &ConversionTest::UChar8 );
            Pt::Unit::TestSuite::registerMethod( "SChar8", *this, &ConversionTest::SChar8 );
            //Pt::Unit::TestSuite::registerMethod( "stdstring", *this, &ConversionTest::stdstring );
            Pt::Unit::TestSuite::registerMethod( "DoubleToString", *this, &ConversionTest::DoubleToString);
            Pt::Unit::TestSuite::registerMethod( "StringToDouble", *this, &ConversionTest::StringToDouble );
            Pt::Unit::TestSuite::registerMethod( "FloatToString", *this, &ConversionTest::FloatToString );
            Pt::Unit::TestSuite::registerMethod( "StringToFloat", *this, &ConversionTest::StringToFloat );
            //Pt::Unit::TestSuite::registerMethod( "VoidPtr", *this, &ConversionTest::VoidPtr );

            Pt::Unit::TestSuite::registerMethod( "ShortLimitMin", *this, &ConversionTest::LimitMin<short> );
            Pt::Unit::TestSuite::registerMethod( "IntLimitMin", *this, &ConversionTest::LimitMin<int> );
            Pt::Unit::TestSuite::registerMethod( "LongLimitMin", *this, &ConversionTest::LimitMin<long> );
            Pt::Unit::TestSuite::registerMethod( "UShortLimitMin", *this, &ConversionTest::LimitMin<unsigned short> );
            Pt::Unit::TestSuite::registerMethod( "UIntLimitMin", *this, &ConversionTest::LimitMin<unsigned int> );
            Pt::Unit::TestSuite::registerMethod( "ULongLimitMin", *this, &ConversionTest::LimitMin<unsigned long> );
            Pt::Unit::TestSuite::registerMethod( "CharLimitMin", *this, &ConversionTest::LimitMin<signed char> );
            Pt::Unit::TestSuite::registerMethod( "UCharLimitMin", *this, &ConversionTest::LimitMin<unsigned char> );

            Pt::Unit::TestSuite::registerMethod( "ShortLimitMax", *this, &ConversionTest::LimitMax<short> );
            Pt::Unit::TestSuite::registerMethod( "IntLimitMax", *this, &ConversionTest::LimitMax<int> );
            Pt::Unit::TestSuite::registerMethod( "LongLimitMax", *this, &ConversionTest::LimitMax<long> );
            Pt::Unit::TestSuite::registerMethod( "UShortLimitMax", *this, &ConversionTest::LimitMax<unsigned short> );
            Pt::Unit::TestSuite::registerMethod( "UIntLimitMax", *this, &ConversionTest::LimitMax<unsigned int> );
            Pt::Unit::TestSuite::registerMethod( "ULongLimitMax", *this, &ConversionTest::LimitMax<unsigned long> );
            Pt::Unit::TestSuite::registerMethod( "CharLimitMax", *this, &ConversionTest::LimitMax<signed char> );
            Pt::Unit::TestSuite::registerMethod( "UCharLimitMax", *this, &ConversionTest::LimitMax<unsigned char> );
        }

    protected:
        //void Bool();
        void NulltermIterator();
        void NumberOverflow();
        void IntToString();
        void StringToInt();
        void UIntToString();
        void StringToUInt();
        //void Char8();
        void UChar8();
        void SChar8();
        //void stdstring();
        void DoubleToString();
        void StringToDouble();
        void FloatToString();
        void StringToFloat();
        //void VoidPtr();
        template <typename T> void LimitMin();
        template <typename T> void LimitMax();
};

Pt::Unit::RegisterTest<ConversionTest> register_ConversionTest;


//void ConversionTest::Bool()
//{
//    bool b = true;
//    Pt::String str = Pt::convert<Pt::String>(b);
//    PT_UNIT_ASSERT( str == L"true" );
//
//    str = L"false";
//    b = Pt::convert<bool>(str);
//    PT_UNIT_ASSERT( b == false );
//}

void ConversionTest::NulltermIterator()
{
    const char* str = "123";
    int value = 0;

    nullterm_array_iterator<char> it(str);
    nullterm_array_iterator<char> end;
    it = Pt::parseInt( it, end, value);
}

void ConversionTest::NumberOverflow()
{
    unsigned char n = 0;
    Pt::String str = "256";
    PT_UNIT_ASSERT_THROW( Pt::parseInt( str.begin(), str.end(), n), Pt::ConversionError );
    
    str = "2000";
    PT_UNIT_ASSERT_THROW( Pt::parseInt( str.begin(), str.end(), n), Pt::ConversionError );
    
    signed char m = 0;
    str = "128";
    PT_UNIT_ASSERT_THROW( Pt::parseInt( str.begin(), str.end(), m), Pt::ConversionError );
    
    str = "1000";
    PT_UNIT_ASSERT_THROW( Pt::parseInt( str.begin(), str.end(), m), Pt::ConversionError );
}


void ConversionTest::IntToString()
{
    int value = 0;
    Pt::String str;
    Pt::formatInt(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 4) == "0" );

    value = 1;
    str.clear();
    Pt::formatInt(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 8) == "1" );

    value = -1;
    str.clear();
    Pt::formatInt(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 12) == "-1" );
    
    value = 123456;
    str.clear();
    Pt::formatInt(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 8) == "123456" );
    
    value = -123456;
    str.clear();
    Pt::formatInt(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 12) == "-123456" );
}

void ConversionTest::StringToInt()
{
    Pt::String str = L"0";
    int value = 0;
    Pt::parseInt( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value == 0 );

    str = L"1";
    Pt::parseInt( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value == 1 );
    
    str = L"+1";
    Pt::parseInt( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value == 1 );
    
    str = L"-1";
    Pt::parseInt( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value == -1 );
    
    str = L"123456";
    Pt::parseInt( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value == 123456 );
    
    str = L"-123456";
    Pt::parseInt( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value == -123456 );

    str.clear();
    bool ok = false;
    Pt::parseInt( str.begin(), str.end(), value, ok);
    PT_UNIT_ASSERT( ! ok );
}


void ConversionTest::UIntToString()
{
    unsigned int value = 0;
    Pt::String str;
    Pt::formatInt(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 4) == L"0" );

    value = 1;
    str.clear();
    Pt::formatInt(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 8) == L"1" );
    
    value = 123456;
    str.clear();
    Pt::formatInt(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 8) == L"123456" );
}


void ConversionTest::StringToUInt()
{
    Pt::String str = L"0";
    int value = 1;
    Pt::parseInt( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value == 0 );

    str = L"1";
    Pt::parseInt( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value == 1 );
    
    str = L"+1";
    Pt::parseInt( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value == 1 );
    
    str = L"123456";
    Pt::parseInt( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value == 123456 );
}


//void ConversionTest::Char8()
//{
//    char value = 'a';
//    Pt::String str = Pt::convert<Pt::String>(value);
//    PT_UNIT_ASSERT( str == L"a" );
//
//    str = L"b";
//    value = Pt::convert<char>(str);
//    PT_UNIT_ASSERT( value == 'b' );
//}


void ConversionTest::UChar8()
{
    unsigned char value = 1;
    Pt::String str;
    Pt::formatInt(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str == L"1" );

    str = L"2";
    Pt::parseInt( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value == 2 );
}


void ConversionTest::SChar8()
{
    signed char value = 1;
    Pt::String str;
    Pt::formatInt(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str == L"1" );

    str = L"2";
    Pt::parseInt( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value == 2 );
}


//void ConversionTest::stdstring()
//{
//    std::string value = "aaa";
//    Pt::String str = Pt::convert<Pt::String>(value);
//    PT_UNIT_ASSERT( str == L"aaa" );
//
//    str = L"bbb";
//    value = Pt::convert<std::string>(str);
//    PT_UNIT_ASSERT( value == "bbb" );
//}


void ConversionTest::DoubleToString()
{
    std::string str;
    double value = 0.0;
    
    value = 3.141592653579893;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT_EQUALS( str.substr(0, 7), "3.14159" );

    value = 0.314;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT_EQUALS( str, "0.314" );

    value = 0.0314;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT_EQUALS( str, "0.0314" );

    value = 0.00123;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT_EQUALS( str, "0.00123" );

    value = 123456789.55555555;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT_EQUALS( str.substr(0, 12), "123456789.55" );

    value = 99.999999999999965;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT_EQUALS( str, "100.0" );

    value = 100.00000000000015;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT_EQUALS( str, "100.0" );

    value = 0.;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT_EQUALS( str, "0.0" );

    value = 1.;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT_EQUALS( str, "1.0" );

    value = 1.4567e17;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT_EQUALS( str.substr(0, 6), "145670" );

    value = 1.4567e-17;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT_EQUALS( str.substr(0, 10), "0.00000000" );

    value = 12345;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT_EQUALS( str, "12345.0" );

    value = 0.2;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT_EQUALS( str, "0.2" );

    value = 12;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT_EQUALS( str, "12.0" );
}


void ConversionTest::StringToDouble()
{
    Pt::String str = L"2.3456789";
    double value = 0;
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value > 2.345 && value < 2.346 );
}


void ConversionTest::FloatToString()
{
    float value = 1.234f;
    Pt::String str;
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 4) == L"1.23" );

    value = -123.456f;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 8) == L"-123.456" );
    
    value = 1000000000.01f;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 12) == L"1000000000.0" );
    
    value = 0.00001f;
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 12) == L"0.00001" );
    
    value = std::numeric_limits<float>::quiet_NaN();
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 3) == L"nan" );
    
    value = std::numeric_limits<float>::infinity();
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 3) == L"inf" );

    value = - std::numeric_limits<float>::infinity();
    str.clear();
    Pt::formatFloat(std::back_inserter(str), value);
    PT_UNIT_ASSERT( str.substr(0, 4) == L"-inf" );
}

// TODO: float and double
//"1.5"
//" -345.75 "
//"\n1e6\r"
//"7.0e4"
//"-2e-3"
//"-8E-5"
//"-3.0e-12"
//"-8.5E-23"

void ConversionTest::StringToFloat()
{
    Pt::String str = L"1.234";
    float value = 0;
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value > 1.23f && value < 1.24f );

    str = L"-123.4567";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value < -123.4f && value > -123.5f );
    
    str = L"1000000000.0";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value < 1000010000.0f && value > 999999000.0f );
    
    str = L"-1000000000.0";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value > -1000010000.0f && value < -999999000.0f );
    
    str = L"1e6";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value > 999990.0f && value < 1000010.0f );

    str = L"7.0e4";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value > 69999.0f && value < 70001.0f );

    str = L"-2e-3";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value > -0.0021f && value < -0.0019f );

    str = L"-8E-5";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value > -0.000081f && value < -0.000079f );
    
    str = L"-3.0e-12";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value > -0.0000000000031f && value < -0.0000000000029f );
    
    str = L"-8.5E-23";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT( value > -0.000000000000000000000086f && value < -0.000000000000000000000084f );

    str = L"nan";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT(value != value);

    str = L"NaN";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT(value != value);
 
    str = L"NANQ";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT(value != value);

    str = L"inf";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT(value == std::numeric_limits<float>::infinity());

    str = L"-inf";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT(value == - std::numeric_limits<float>::infinity());
    
    str = L"infinity";
    Pt::parseFloat( str.begin(), str.end(), value);
    PT_UNIT_ASSERT(value == std::numeric_limits<float>::infinity());
}


//void ConversionTest::VoidPtr()
//{
//    void* value = (void*)0x12345678;
//    Pt::String str = Pt::convert<Pt::String>(value);
//
//    void* value2 = Pt::convert<void*>(str);
//    PT_UNIT_ASSERT( value == value2 );
//}


template <typename T>
void ConversionTest::LimitMin()
{
    std::string str;
    Pt::formatInt( std::back_inserter(str), 
                   std::numeric_limits<T>::min() );

    T value;
    Pt::parseInt( str.begin(), str.end(), value );
    PT_UNIT_ASSERT_EQUALS(value, std::numeric_limits<T>::min());
}


template <typename T>
void ConversionTest::LimitMax()
{
    std::string str;
    Pt::formatInt( std::back_inserter(str), 
                   std::numeric_limits<T>::max() );

    T value;
    Pt::parseInt( str.begin(), str.end(), value );
    PT_UNIT_ASSERT_EQUALS(value, std::numeric_limits<T>::max());
}
