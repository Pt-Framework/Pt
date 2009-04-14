/*
 * Copyright (C) 2007 by Marc Boris Duerner
 * Copyright (C) 2006 by Stefan Bueder
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

#include "Pt/SerializationInfo.h"
#include "Pt/Date.h"
#include "Pt/Time.h"
#include "Pt/DateTime.h"
#include "Pt/Math/MathUtils.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>

#include "Pt/System/Clock.h"

class SerializationTest : public Pt::Unit::TestSuite
{
    public:
        SerializationTest()
        : Pt::Unit::TestSuite("SerializationTest")
        {
            //Pt::Unit::TestSuite::registerMethod( "Benchmark1", *this, &SerializationTest::Benchmark1 );
            //Pt::Unit::TestSuite::registerMethod( "Benchmark2", *this, &SerializationTest::Benchmark2 );
            Pt::Unit::TestSuite::registerMethod( "BuiltInTypesTest", *this, &SerializationTest::BuiltInTypesTest );
            Pt::Unit::TestSuite::registerMethod( "StdVectorTest", *this, &SerializationTest::StdVectorTest );
            Pt::Unit::TestSuite::registerMethod( "DateTest", *this, &SerializationTest::DateTest );
            Pt::Unit::TestSuite::registerMethod( "TimeTest", *this, &SerializationTest::TimeTest );
        }
    
    protected:
        void Benchmark1();
        void Benchmark2();
        void BuiltInTypesTest();
        void StdVectorTest();
        void DateTest();
        void TimeTest();
        void DateTimeTest();
};

Pt::Unit::RegisterTest<SerializationTest> register_SerializationTest;


void SerializationTest::Benchmark1()
{
    std::string name;
    
    Pt::System::Clock clock;
    clock.start();
    for(unsigned n = 0; n < 50000; ++n)
    {
        std::vector<int> vec;
        
        Pt::SerializationInfo si;

        Pt::String num(L"111");
        si.addValue(name, num);

        num = L"222";
        si.addValue(name, num);

        num = L"333";
        si.addValue(name, num);

        num = L"444";
        si.addValue(name, num);

        num = L"555";
        si.addValue(name, num);
        num.clear();

        si >>= vec;
    }
    Pt::Timespan ts = clock.stop();
    std::cerr << "Time: " << ts.toUSecs() << std::endl;
}


void SerializationTest::Benchmark2()
{
    std::string name;
    
    Pt::System::Clock clock;
    clock.start();
    for(unsigned n = 0; n < 50000; ++n)
    {
        std::vector<int> vec;
        Pt::String num(L"111");
        vec.push_back( Pt::convert<int>(num) );

        num = L"22";
        vec.push_back( Pt::convert<int>(num) );

        num = L"333";
        vec.push_back( Pt::convert<int>(num) );

        num = L"444";
        vec.push_back( Pt::convert<int>(num) );

        num = L"555";
        vec.push_back( Pt::convert<int>(num) );
        num.clear();
    }
    Pt::Timespan ts = clock.stop();
    std::cerr << "Time: " << ts.toUSecs() << std::endl;
}


void SerializationTest::BuiltInTypesTest()
{
    Pt::SerializationInfo si;

    bool boolVal1 = true;
    bool boolVal2;
    si <<= boolVal1;
    PT_UNIT_ASSERT(si.category() == Pt::SerializationInfo::Value);
    PT_UNIT_ASSERT(si.typeName() == "bool");
    si.toValue(boolVal2);
    PT_UNIT_ASSERT(boolVal2 == true);
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"true") );
    si >>= boolVal2;
    PT_UNIT_ASSERT(boolVal2 == true);

    char charVal1 = 'c';
    char charVal2;
    si <<= charVal1;
    PT_UNIT_ASSERT(si.category() == Pt::SerializationInfo::Value);
    PT_UNIT_ASSERT(si.typeName() == "char");
    si.toValue(charVal2);
    PT_UNIT_ASSERT(charVal2 == 'c');
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"c") );
    si >>= charVal2;
    PT_UNIT_ASSERT(charVal2 == 'c');

    signed char sigCharVal1 = -127;
    signed char sigCharVal2;
    si <<= sigCharVal1;
    PT_UNIT_ASSERT(si.category() == Pt::SerializationInfo::Value);
    PT_UNIT_ASSERT(si.typeName() == "char");
    si.toValue(sigCharVal2);
    PT_UNIT_ASSERT(sigCharVal2 == -127);
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"-127") );
    si >>= sigCharVal2;
    PT_UNIT_ASSERT(sigCharVal2 == -127);

    unsigned char usigCharVal1 = 255;
    unsigned char usigCharVal2;
    si <<= usigCharVal1;
    PT_UNIT_ASSERT(si.category() == Pt::SerializationInfo::Value);
    PT_UNIT_ASSERT(si.typeName() == "char");
    si.toValue(usigCharVal2);
    PT_UNIT_ASSERT(usigCharVal2 == 255);
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"255") );
    si >>= usigCharVal2;
    PT_UNIT_ASSERT(usigCharVal2 == 255);

    short shortVal1 = -32767;
    short shortVal2;
    si <<= shortVal1;
    PT_UNIT_ASSERT(si.category() == Pt::SerializationInfo::Value);
    PT_UNIT_ASSERT(si.typeName() == "int");
    si.toValue(shortVal2);
    PT_UNIT_ASSERT(shortVal2 == -32767);
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"-32767") );
    si >>= shortVal2;
    PT_UNIT_ASSERT(shortVal2 == -32767);

    unsigned short ushortVal1 = 65535;
    unsigned short ushortVal2;
    si <<= ushortVal1;
    PT_UNIT_ASSERT(si.category() == Pt::SerializationInfo::Value);
    PT_UNIT_ASSERT(si.typeName() == "int");
    si.toValue(ushortVal2);
    PT_UNIT_ASSERT(ushortVal2 == 65535);
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"65535") );
    si >>= ushortVal2;
    PT_UNIT_ASSERT(ushortVal2 == 65535);

    int intVal1 = -32767;
    int intVal2;
    si <<= intVal1;
    PT_UNIT_ASSERT(si.category() == Pt::SerializationInfo::Value);
    PT_UNIT_ASSERT(si.typeName() == "int");
    si.toValue(intVal2);
    PT_UNIT_ASSERT(intVal2 == -32767);
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"-32767") );
    si >>= intVal2;
    PT_UNIT_ASSERT(intVal2 == -32767);

    unsigned int uintVal1 = 65535;
    unsigned int uintVal2;
    si <<= uintVal1;
    PT_UNIT_ASSERT(si.category() == Pt::SerializationInfo::Value);
    PT_UNIT_ASSERT(si.typeName() == "int");
    si.toValue(uintVal2);
    PT_UNIT_ASSERT(uintVal2 == 65535);
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"65535") );
    si >>= uintVal2;
    PT_UNIT_ASSERT(uintVal2 == 65535);

    long longVal1 = -32767;
    long longVal2;
    si <<= longVal1;
    PT_UNIT_ASSERT(si.category() == Pt::SerializationInfo::Value);
    PT_UNIT_ASSERT(si.typeName() == "int");
    si.toValue(longVal2);
    PT_UNIT_ASSERT(longVal2 == -32767);
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"-32767") );
    si >>= longVal2;
    PT_UNIT_ASSERT(longVal2 == -32767);

    unsigned long ulongVal1 = 65535;
    unsigned long ulongVal2;
    si <<= ulongVal1;
    PT_UNIT_ASSERT(si.category() == Pt::SerializationInfo::Value);
    PT_UNIT_ASSERT(si.typeName() == "int");
    si.toValue(ulongVal2);
    PT_UNIT_ASSERT(ulongVal2 == 65535);
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"65535") );
    si >>= ulongVal2;
    PT_UNIT_ASSERT(ulongVal2 == 65535);

    float floatVal1 = 77.3547f;
    float floatVal2;
    si <<= floatVal1;
    PT_UNIT_ASSERT(si.category() == Pt::SerializationInfo::Value);
    PT_UNIT_ASSERT(si.typeName() == "double");
    si.toValue(floatVal2);
    PT_UNIT_ASSERT( Pt::Math::equal(floatVal2, 77.3547f,   Pt::Math::Eps3) );
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"77.3547") );
    si >>= floatVal2;
    PT_UNIT_ASSERT(floatVal2 == 77.3547f);

    double doubleVal1 = 198.8196;
    double doubleVal2;
    si <<= doubleVal1;
    PT_UNIT_ASSERT(si.category() == Pt::SerializationInfo::Value);
    PT_UNIT_ASSERT(si.typeName() == "double");
    si.toValue(doubleVal2);
    PT_UNIT_ASSERT( Pt::Math::equal(doubleVal2, 198.8196, Pt::Math::Eps6) );
    Pt::String asString = si.toString();
    PT_UNIT_ASSERT( asString.find(L"198.8196") != Pt::String::npos );
    si >>= doubleVal2;
    PT_UNIT_ASSERT(doubleVal2 == 198.8196);
}


void SerializationTest::StdVectorTest()
{
    std::vector<int> iv, iv2;
    const int vectorSize = 123;

    for(int i=0; i<vectorSize; ++i)
    {
        iv.push_back(i);
    }

    Pt::SerializationInfo si;
    si <<= iv;
    PT_UNIT_ASSERT(si.category() == Pt::SerializationInfo::Array);

    Pt::SerializationInfo::Iterator it = si.begin();
    int counter = 0;
    int intValue;

    while(it!=si.end())
    {
        it->toValue(intValue);

        PT_UNIT_ASSERT(it->category() == Pt::SerializationInfo::Value);
        PT_UNIT_ASSERT(intValue == counter);

        ++it;
        ++counter;
    }
    PT_UNIT_ASSERT(counter == vectorSize);

    si >>= iv2;

    for(int j=0; j<vectorSize; ++j)
    {
        PT_UNIT_ASSERT(iv2[j] == j);
    }
}


void SerializationTest::DateTest()
{
    Pt::Date date(2000, 10, 20);
    Pt::SerializationInfo si;
    si <<=  date;

    Pt::Date date2(1,1,1);
    si >>= date2;

    PT_UNIT_ASSERT(date == date2);
}


void SerializationTest::TimeTest()
{
    Pt::Time time(18, 40, 5, 1);
    Pt::SerializationInfo si;
    si <<= time;

    Pt::Time time2;
    si >>= time2;

    PT_UNIT_ASSERT(time == time2);
}


void SerializationTest::DateTimeTest()
{
    Pt::DateTime datetime(2000, 10, 20, 18, 40, 5, 1);
    Pt::SerializationInfo si;
    si <<= datetime;

    Pt::DateTime datetime2;
    si >>= datetime2;

    const Pt::Date date = datetime.date();
    const Pt::Date date2 = datetime2.date();
    PT_UNIT_ASSERT(date == date2);

    const Pt::Time time = datetime.time();
    const Pt::Time time2 = datetime2.time();

    PT_UNIT_ASSERT(time == time2);
}
