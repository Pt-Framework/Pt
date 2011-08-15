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
#include "Pt/SerializationContext.h"
#include "Pt/Math.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <cstring> // for atoi only

#include "Pt/System/Clock.h"
#include "Pt/StringStream.h"

namespace Pt {

inline void convert(int& n, const Pt::String& str)
{
    // Pt::StringStream ssc;
    // ssc.clear();
    // ssc.str(str);
    // ssc >> n;

    //int value = 1234;
    //memcpy(&n, &value, sizeof(int));

    n = std::atoi("111");
}

}

#include "Pt/SerializationInfo.h"
#include "Pt/Deserializer.h"


class IntComposer : public Pt::IComposer
{
    public:
        typedef int value_type;

    public:
        IntComposer()
        : _type(0)
        , _parent(0)
        {}

        void setParent(IComposer* parent)
        { _parent = parent; }

        void begin(value_type& type)
        {
            _type = &type;
        }

        virtual void clear()
        { }

        virtual void clear(Pt::SerializationContext*)
        { }

        virtual void setName(const std::string& name)
        { }

        virtual void setId(const std::string& id)
        { }

        virtual void setTypeName(const std::string& type)
        { }

        virtual void setValue(const Pt::String& value)
        {
            convert(*_type, value);
        }

        virtual void setInt64(Pt::int64_t l)
        {
            //_si.setValue(l);
            *_type = static_cast<int>(l);
        }

        virtual Pt::IComposer* finish()
        {
            //_si >>= *_type;
            return _parent;
        }

        //Pt::SerializationInfo _si;
    private:
        value_type* _type;
        IComposer* _parent;
};

class VectorComposer : public Pt::IComposer
{
    public:
        typedef std::vector<int> value_type;
        typedef int elem_type;

    public:
        VectorComposer()
        : _type(0)
        {
            _deser.setParent(this);
        }

        void begin(value_type& type)
        {
            type.clear();
            type.reserve(5);
            _type = &type;
        }

        virtual void clear()
        { }

        virtual void clear(Pt::SerializationContext*)
        { }

        virtual void setName(const std::string& name)
        { }

        virtual void setId(const std::string& id)
        { }

        virtual void setTypeName(const std::string& type)
        { }

        virtual Pt::IComposer* beginMember(const std::string& name)
        {
            _type->push_back( elem_type() );
            _deser.begin( _type->back() );
            return &_deser;
        }

        virtual Pt::IComposer* beginElement()
        {
            _type->push_back( elem_type() );
            _deser.begin( _type->back() );
            return &_deser;
        }

        virtual Pt::IComposer* finish()
        {
            return 0;
        }

    private:
        value_type* _type;
        IntComposer _deser;
        //Pt::Composer<int> _deser;
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
            Pt::Unit::TestSuite::registerMethod( "Binary", *this, &SerializationTest::Binary );
            Pt::Unit::TestSuite::registerMethod( "BuiltInTypesTest", *this, &SerializationTest::BuiltInTypesTest );
            Pt::Unit::TestSuite::registerMethod( "StdVectorTest", *this, &SerializationTest::StdVectorTest );
            Pt::Unit::TestSuite::registerMethod( "DateTest", *this, &SerializationTest::DateTest );
            Pt::Unit::TestSuite::registerMethod( "TimeTest", *this, &SerializationTest::TimeTest );
        }

    protected:
        void Benchmark1();
        void Benchmark2();
        void Benchmark3();
        void Binary();
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
    //while(true)
    {
        input.clear(); // 105 000
        input.seekg(std::ios::beg);

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        si.addElement().setInt32(v);

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        si.addElement().setInt32(v);

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        si.addElement().setInt32(v);

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        si.addElement().setInt32(v);

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        si.addElement().setInt32(v);

        si >>= vec;
        si.clear();
        u += vec.size();

		// si <<= vec;
		// si.clear();
        // u += vec.size();
    }
    Pt::Timespan ts = clock.stop();
    std::cerr << "Time1: " << ts.toUSecs() << " " << u <<  std::endl;
    std::cerr << "sizeof(SerialitazionInfo): " << sizeof(Pt::SerializationInfo) << " " << u <<  std::endl;
    std::cerr << "sizeof(void*): " << sizeof(void*) << " " << u <<  std::endl;
    //std::exit(1);
}


void SerializationTest::Benchmark2()
{
    std::string name;
    Pt::String num(L"111");

    Pt::SerializationContext context;
    VectorComposer vecdes;
    Pt::IComposer* deser = &vecdes;

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
        deser->setInt64(v);
        deser = deser->finish();

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        deser = deser->beginMember(name);
        deser->setInt64(v);
        deser = deser->finish();

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        deser = deser->beginMember(name);
        deser->setInt64(v);
        deser = deser->finish();

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        deser = deser->beginMember(name);
        deser->setInt64(v);
        deser = deser->finish();

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        deser = deser->beginMember(name);
        deser->setInt64(v);
        deser = deser->finish();

        deser->finish();

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

    Pt::StringStream input(L"111 222 333 444 555");
    std::vector<int> vec;
    int u = 0;
    int v = 0;
    Pt::System::Clock clock;
    clock.start();
    for(unsigned n = 0; n < 50000; ++n)
    {
        Pt::Composer< std::vector<int> > com(&context);
        com.begin(vec);
        Pt::IComposer* composer = &com;

        input.clear();
        input.seekg(std::ios::beg);

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        composer = composer->beginElement();
        composer->setInt64(v);
        composer = composer->finish();

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        composer = composer->beginElement();
        composer->setInt64(v);
        composer = composer->finish();

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        composer = composer->beginElement();
        composer->setInt64(v);
        composer = composer->finish();

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        composer = composer->beginElement();
        composer->setInt64(v);
        composer = composer->finish();

        std::getline(input, num, Pt::Char(' '));
        convert(v, num);
        composer = composer->beginElement();
        composer->setInt64(v);
        composer = composer->finish();

        composer->finish();

        u += vec.size();
    }
    Pt::Timespan ts = clock.stop();

    std::cerr << "Time3: " << ts.toUSecs() << " " << u << std::endl;
    //std::exit(1);
}


void SerializationTest::Binary()
{
    const char buf[] = "01234567890123456789012345678901234567890123456789"
                       "01234567890123456789012345678901234567890123456789"
                       "01234567890123456789012345678901234567890123456789";

    Pt::SerializationInfo si;
    si.setBinary( buf, sizeof(buf) );

    size_t size = 0;
    const char* data = si.getBinary(size);
    PT_UNIT_ASSERT(size == sizeof(buf));
    PT_UNIT_ASSERT( 0 == std::memcmp(data, buf, sizeof(buf)) );

    short s = 42;
    data = reinterpret_cast<const char*>(&s);
    si.setBinary( data, sizeof(short) );

    s = 0;
    size = 0;
    data = si.getBinary(size);
    PT_UNIT_ASSERT(size == sizeof(short));

    std::memcpy(&s, data, size);
    PT_UNIT_ASSERT(s == 42);
}


void SerializationTest::BuiltInTypesTest()
{
    Pt::SerializationInfo si;

    bool boolVal1 = true;
    bool boolVal2;
    si <<= boolVal1;
    PT_UNIT_ASSERT(si.isScalar());
    PT_UNIT_ASSERT( 0 == std::strcmp(si.typeName(), "bool") );
    si >>= boolVal2;
    PT_UNIT_ASSERT(boolVal2 == true);
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"true") );

    char charVal1 = 'c';
    char charVal2 = 'x';
    si <<= charVal1;
    PT_UNIT_ASSERT(si.isScalar());
    PT_UNIT_ASSERT( 0 == std::strcmp(si.typeName(), "char") );
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"c") );
    si >>= charVal2;
    PT_UNIT_ASSERT(charVal2 == 'c');

    Pt::int8_t sigCharVal1 = -127;
    Pt::int8_t sigCharVal2;
    si <<= sigCharVal1;
    PT_UNIT_ASSERT(si.isScalar());
    PT_UNIT_ASSERT( 0 == std::strcmp(si.typeName(), "char") );
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"-127") );
    si >>= sigCharVal2;
    PT_UNIT_ASSERT(sigCharVal2 == -127);

    Pt::uint8_t usigCharVal1 = 255;
    Pt::uint8_t usigCharVal2;
    si <<= usigCharVal1;
    PT_UNIT_ASSERT(si.isScalar());
    PT_UNIT_ASSERT( 0 == std::strcmp(si.typeName(), "char") );
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"255") );
    si >>= usigCharVal2;
    PT_UNIT_ASSERT(usigCharVal2 == 255);

    Pt::int16_t shortVal1 = -32767;
    Pt::int16_t shortVal2;
    si <<= shortVal1;
    PT_UNIT_ASSERT(si.isScalar());
    PT_UNIT_ASSERT( 0 == std::strcmp(si.typeName(), "int") );
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"-32767") );
    si >>= shortVal2;
    PT_UNIT_ASSERT(shortVal2 == -32767);

    Pt::uint16_t ushortVal1 = 65535;
    Pt::uint16_t ushortVal2;
    si <<= ushortVal1;
    PT_UNIT_ASSERT(si.isScalar());
    PT_UNIT_ASSERT( 0 == std::strcmp(si.typeName(),"int") );
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"65535") );
    si >>= ushortVal2;
    PT_UNIT_ASSERT(ushortVal2 == 65535);

    Pt::int32_t intVal1 = -32767;
    Pt::int32_t intVal2;
    si <<= intVal1;
    PT_UNIT_ASSERT(si.isScalar());
    PT_UNIT_ASSERT( 0 == std::strcmp(si.typeName(),"int") );
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"-32767") );
    si >>= intVal2;
    PT_UNIT_ASSERT(intVal2 == -32767);

    Pt::uint32_t uintVal1 = 65535;
    Pt::uint32_t uintVal2;
    si <<= uintVal1;
    PT_UNIT_ASSERT(si.isScalar());
    PT_UNIT_ASSERT( 0 == std::strcmp(si.typeName(), "int") );
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"65535") );
    si >>= uintVal2;
    PT_UNIT_ASSERT(uintVal2 == 65535);

    Pt::int64_t longVal1 = -32767;
    Pt::int64_t longVal2;
    si <<= longVal1;
    PT_UNIT_ASSERT(si.isScalar());
    PT_UNIT_ASSERT( 0 == std::strcmp(si.typeName(), "int") );
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"-32767") );
    si >>= longVal2;
    PT_UNIT_ASSERT(longVal2 == -32767);

    Pt::uint64_t ulongVal1 = 65535;
    Pt::uint64_t ulongVal2;
    si <<= ulongVal1;
    PT_UNIT_ASSERT(si.isScalar());
    PT_UNIT_ASSERT( 0 == std::strcmp(si.typeName(),"int") );
    PT_UNIT_ASSERT(si.toString() == Pt::String(L"65535") );
    si >>= ulongVal2;
    PT_UNIT_ASSERT(ulongVal2 == 65535);

    float floatVal1 = 77.3547f;
    float floatVal2;
    si <<= floatVal1;
    PT_UNIT_ASSERT(si.isScalar());
    PT_UNIT_ASSERT( 0 == std::strcmp(si.typeName(), "double") );
    PT_UNIT_ASSERT(si.toString().find(Pt::String(L"77.354"))== 0 );
    si >>= floatVal2;
    PT_UNIT_ASSERT(floatVal2 == 77.3547f);

    double doubleVal1 = 198.8196;
    double doubleVal2;
    si <<= doubleVal1;
    PT_UNIT_ASSERT(si.isScalar());
    PT_UNIT_ASSERT( 0 == std::strcmp(si.typeName(),"double") );
    Pt::String toString = si.toString();
    PT_UNIT_ASSERT( toString.find(L"198.8196") != Pt::String::npos );
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
    PT_UNIT_ASSERT(si.isSequence());

    Pt::SerializationInfo::Iterator it = si.begin();
    int counter = 0;
    int intValue;

    while(it != si.end())
    {
        *it >>= intValue;

        PT_UNIT_ASSERT(it->isScalar());
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
