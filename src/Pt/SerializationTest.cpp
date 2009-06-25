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
#include "Pt/StringStream.h"


namespace Pt {

inline void convert(int& n, const Pt::String& str)
{
	static Pt::StringStream ssc;
	ssc.clear();
	ssc.str(str);
	ssc >> n;

	//int value = 1234;
	//memcpy(&n, &value, sizeof(int));
	
	//n = atoi( "111");
}

}


#include "Pt/SerializationInfo.h"
#include "Pt/Deserializer.h"


class IntDeserializer : public Pt::IDeserializer
{
    public:
        typedef int value_type;

    public:
        IntDeserializer()
        : _type(0)
        {}

        void begin(value_type& type)
        {
            _type = &type;
        }

        virtual void setContext(Pt::SerializationContext& cx)
        { }

        virtual void setName(const std::string& name)
        { }

        virtual void setId(const std::string& id)
        { }

        virtual void setValue(const Pt::String& value)
        {
            convert(*_type, value);
        }

        virtual void setInt(long l)
        {
            *_type = static_cast<int>(l);
        }

        virtual void setReference(const std::string& id)
        { }

        virtual Pt::IDeserializer* beginMember(const std::string& name)
        {
            return 0;
        }

        virtual Pt::IDeserializer* beginMember()
        {
            return 0;
        }

        virtual Pt::IDeserializer* leaveMember()
        {
            return parent();
        }

        virtual void leave()
        { }

        virtual void prepareLink(Pt::SerializationContext& context)
        { }

    private:
        value_type* _type;
};

class VectorDeserializer : public Pt::IDeserializer
{
    public:
        typedef std::vector<int> value_type;
        typedef int elem_type;

    public:
        VectorDeserializer()
        : _type(0)
        {
            _deser.setParent(this);
        }

        void begin(value_type& type)
        {
            type.clear();
            _type = &type;
        }

        virtual void setContext(Pt::SerializationContext& cx)
        { }

        virtual void setName(const std::string& name)
        { }

        virtual void setId(const std::string& id)
        { }

        virtual void setValue(const Pt::String& value)
        { }

        virtual void setInt(long)
        { }

        virtual void setReference(const std::string& id)
        { }

        virtual Pt::IDeserializer* beginMember(const std::string& name)
        {
            _type->push_back( elem_type() );
            _deser.begin( _type->back() );
            return &_deser;
        }

        virtual Pt::IDeserializer* beginMember()
        {
            _type->push_back( elem_type() );
            _deser.begin( _type->back() );
            return &_deser;
        }

        virtual Pt::IDeserializer* leaveMember()
        {
            return this;
        }

        virtual void leave()
        { }

        virtual void prepareLink(Pt::SerializationContext& context)
        { }

    private:
        value_type* _type;
        IntDeserializer _deser;
};


class SerializationTest : public Pt::Unit::TestSuite
{
    public:
        SerializationTest()
        : Pt::Unit::TestSuite("SerializationTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Benchmark1", *this, &SerializationTest::Benchmark1 );
            Pt::Unit::TestSuite::registerMethod( "Benchmark2", *this, &SerializationTest::Benchmark2 );
            Pt::Unit::TestSuite::registerMethod( "Benchmark3", *this, &SerializationTest::Benchmark3 );
            Pt::Unit::TestSuite::registerMethod( "BuiltInTypesTest", *this, &SerializationTest::BuiltInTypesTest );
            Pt::Unit::TestSuite::registerMethod( "StdVectorTest", *this, &SerializationTest::StdVectorTest );
            Pt::Unit::TestSuite::registerMethod( "DateTest", *this, &SerializationTest::DateTest );
            Pt::Unit::TestSuite::registerMethod( "TimeTest", *this, &SerializationTest::TimeTest );
        }
    
    protected:
        void Benchmark1();
        void Benchmark2();
        void Benchmark3();
        void BuiltInTypesTest();
        void StdVectorTest();
        void DateTest();
        void TimeTest();
        void DateTimeTest();
};

Pt::Unit::RegisterTest<SerializationTest> register_SerializationTest;

// Apple
// without 900676
// with SI 905312
// with DS 906900

void SerializationTest::Benchmark1()
{
    std::string name;
    Pt::String num(L"111");
    Pt::StringStream input(L"111 222 333 444 555");
    std::vector<int> vec;
    int u = 0;
    int v = 0;

    Pt::SerializationContext context;
    Pt::SerializationInfo si(&context);

    Pt::System::Clock clock;
    clock.start();
    for(unsigned n = 0; n < 50000; ++n)
    {
        input.clear(); // 105 000
        input.seekg(std::ios::beg);

        //std::cerr << "getline" << std::endl;
        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        si.addValue(name, v);

        //std::cerr << "getline" << std::endl;
        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        si.addValue(name, v);

        //std::cerr << "getline" << std::endl;
        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        si.addValue(name, v);

        //std::cerr << "getline 4" << std::endl;
        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        si.addValue(name, v);

        //std::cerr << "getline 5" << std::endl;
        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        si.addValue(name, v);

        //vec.reserve(5);
        si >>= vec; //55 000

        si.clear(); // 130 000

        u += vec.size();
        //std::cerr << "RESULT: " << vec.size() << " " << vec[4] << std::endl;
        vec.clear();
    }
    Pt::Timespan ts = clock.stop();
    std::cerr << "Time1: " << ts.toUSecs() << " " << u <<  std::endl;
    //std::exit(1);
}


void SerializationTest::Benchmark2()
{
    std::string name;
    Pt::String num(L"111");

    Pt::SerializationContext context;
    VectorDeserializer vecdes;
    Pt::IDeserializer* deser = &vecdes;

    Pt::StringStream input(L"111 222 333 444 555");
    std::vector<int> vec;
    int u = 0;
    int v = 0;

    Pt::System::Clock clock;
    clock.start();
    for(unsigned n = 0; n < 50000; ++n)
    {
        vecdes.begin(vec);

        input.clear();
        input.seekg(std::ios::beg);

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        deser = deser->beginMember(name);
        deser->setInt(v);
        deser = deser->leaveMember();

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        deser = deser->beginMember(name);
        deser->setInt(v);
        deser = deser->leaveMember();

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        deser = deser->beginMember(name);
        deser->setInt(v);
        deser = deser->leaveMember();

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        deser = deser->beginMember(name);
        deser->setInt(v);
        deser = deser->leaveMember();

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        deser = deser->beginMember(name);
        deser->setInt(v);
        deser = deser->leaveMember();

        deser->leave();

        u += vec.size();
    }
    Pt::Timespan ts = clock.stop();

    std::cerr << "Time2: " << ts.toUSecs() << " " << u << std::endl;

    //std::exit(1);
}


void SerializationTest::Benchmark3()
{
    std::string name;
    Pt::String num(L"111");

    Pt::SerializationContext context;
    Pt::Deserializer< std::vector<int> > vecdes;
    Pt::IDeserializer* deser = &vecdes;

    Pt::StringStream input(L"111 222 333 444 555");
    std::vector<int> vec;
    int u = 0;
    Pt::System::Clock clock;
    clock.start();
    for(unsigned n = 0; n < 50000; ++n)
    {
        vecdes.begin(vec);

        input.clear();
        input.seekg(std::ios::beg);

        std::getline(input, num, Pt::Char(' '));
        deser = deser->beginMember(name);
        deser->setValue(num);
        deser = deser->leaveMember();

        std::getline(input, num, Pt::Char(' '));
        deser = deser->beginMember(name);
        deser->setValue(num);
        deser = deser->leaveMember();

        std::getline(input, num, Pt::Char(' '));
        deser = deser->beginMember(name);
        deser->setValue(num);
        deser = deser->leaveMember();

        std::getline(input, num, Pt::Char(' '));
        deser = deser->beginMember(name);
        deser->setValue(num);
        deser = deser->leaveMember();

        std::getline(input, num, Pt::Char(' '));
        deser = deser->beginMember(name);
        deser->setValue(num);
        deser = deser->leaveMember();

        deser->leave();

        u += vec.size();
    }
    Pt::Timespan ts = clock.stop();

    std::cerr << "Time3: " << ts.toUSecs() << " " << u << std::endl;

    //std::exit(1);
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
    PT_UNIT_ASSERT(si.toString().find(Pt::String(L"77.354"))== 0 );
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
