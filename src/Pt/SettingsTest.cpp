/*
 * Copyright (C) 2005-2007 by Dr. Marc Boris Duerner
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

#include "Pt/Settings.h"
#include "Pt/Date.h"
#include "Pt/Text/TextStream.h"
#include "Pt/Text/Utf8Codec.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <sstream>

class SettingsTest : public Pt::Unit::TestSuite
{
    public:
        SettingsTest()
        : Pt::Unit::TestSuite("SettingsTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Comment", *this, &SettingsTest::Comment );

            Pt::Unit::TestSuite::registerMethod( "SimpleValue", *this, &SettingsTest::SimpleValue );
            Pt::Unit::TestSuite::registerMethod( "SimpleTypedValue", *this, &SettingsTest::SimpleTypedValue );
            Pt::Unit::TestSuite::registerMethod( "SimpleQoutedValue", *this, &SettingsTest::SimpleQoutedValue );

            Pt::Unit::TestSuite::registerMethod( "SimpleArray", *this, &SettingsTest::SimpleArray );
            Pt::Unit::TestSuite::registerMethod( "SimpleNamedArray", *this, &SettingsTest::SimpleNamedArray );
            Pt::Unit::TestSuite::registerMethod( "SimpleQoutedArray", *this, &SettingsTest::SimpleQoutedArray );
            Pt::Unit::TestSuite::registerMethod( "SimpleTypedArray", *this, &SettingsTest::SimpleTypedArray );
            Pt::Unit::TestSuite::registerMethod( "SimpleArrayQoutedTypedValues", *this, &SettingsTest::SimpleArrayQoutedTypedValues );

            Pt::Unit::TestSuite::registerMethod( "ComplexType", *this, &SettingsTest::ComplexType );
            Pt::Unit::TestSuite::registerMethod( "ComplexTypeNamedQoutedValues", *this, &SettingsTest::ComplexTypeNamedQoutedValues );
            Pt::Unit::TestSuite::registerMethod( "ComplexNamedType", *this, &SettingsTest::ComplexNamedType );

            Pt::Unit::TestSuite::registerMethod( "Section", *this, &SettingsTest::Section );
            Pt::Unit::TestSuite::registerMethod( "ArrayOfArrays", *this, &SettingsTest::ArrayOfArrays );
            Pt::Unit::TestSuite::registerMethod( "LoadSaveSerializable", *this, &SettingsTest::LoadSaveSerializable );
            Pt::Unit::TestSuite::registerMethod( "Iterator", *this, &SettingsTest::Iterator );
            Pt::Unit::TestSuite::registerMethod( "ConstIterator", *this, &SettingsTest::ConstIterator );
        }

    protected:
        void Comment();
        void SimpleValue();
        void SimpleTypedValue();
        void SimpleQoutedValue();
        void SimpleArray();
        void SimpleNamedArray();
        void SimpleQoutedArray();
        void SimpleTypedArray();
        void SimpleArrayQoutedTypedValues();
        void ComplexNamedType();
        void ComplexType();
        void ComplexTypeNamedQoutedValues();
        void Section();
        void ArrayOfArrays();
        void LoadSaveSerializable();
        void Iterator();
        void ConstIterator();
};

Pt::Unit::RegisterTest<SettingsTest> register_SettingsTest;


void SettingsTest::Iterator()
{
    Pt::Settings settings;
    PT_UNIT_ASSERT( settings.begin() == settings.end() );
    PT_UNIT_ASSERT( settings.root().begin() == settings.root().end() );

    Pt::Date date(2001, 11, 15);
    settings.root().add("myDate", date);
    PT_UNIT_ASSERT( settings.begin() != settings.end() );
    PT_UNIT_ASSERT( settings.root().begin() != settings.root().end() );

    Pt::Settings::Entry& entry = *settings.begin();
    PT_UNIT_ASSERT( entry.name() == "myDate" );

    Pt::Date date2(2000, 1, 1);
    entry.get(date2);
    PT_UNIT_ASSERT( date2.year() == 2001 );
    PT_UNIT_ASSERT( date2.month() == 11 );
    PT_UNIT_ASSERT( date2.day() == 15 );

    int n = 0;
    entry.set(42);
    PT_UNIT_ASSERT( entry.get(n) );
    PT_UNIT_ASSERT( n == 42 );
}

void SettingsTest::ConstIterator()
{
    Pt::Settings s;

    const Pt::Settings& settings = s;
    PT_UNIT_ASSERT( settings.begin() == settings.end() );
    PT_UNIT_ASSERT( settings.root().begin() == settings.root().end() );

    s.root().add("number", 42);
    PT_UNIT_ASSERT( settings.begin() != settings.end() );
    PT_UNIT_ASSERT( settings.root().begin() != settings.root().end() );

    Pt::Settings::ConstEntry& entry = *settings.begin();
    PT_UNIT_ASSERT( entry.name() == "number" );

    int n = 0;
    PT_UNIT_ASSERT( entry.get(n) );
    PT_UNIT_ASSERT( n == 42 );
}

void SettingsTest::LoadSaveSerializable()
{
    Pt::Date date(2001, 11, 15);
    Pt::Settings settings;
    settings.setObject(date, "myDate");

    std::ostringstream ss;
    Pt::Text::TextOStream ts(ss, new Pt::Text::Utf8Codec);
    settings.save(ts);

    Pt::Date date2(2000, 1, 1);
    settings.entry("myDate").get(date2);
    PT_UNIT_ASSERT( date2.year() == 2001 );
    PT_UNIT_ASSERT( date2.month() == 11 );
    PT_UNIT_ASSERT( date2.day() == 15 );
}


void SettingsTest::Comment()
{
    std::stringstream ss;
    ss << ";first comment\n";
    ss << "a = \"1#;2\"\n";
    ss << "#second comment\n";
    ss << "b = 2\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    PT_UNIT_ASSERT("1#;2" == settings.getValue<std::string>("a") );
    PT_UNIT_ASSERT(2 == settings.getValue<int>("b") );
}

void SettingsTest::ArrayOfArrays()
{
    std::stringstream ss;
    ss << "a={array{1,2,3},array{4,5,6}}\n";
    ss << "b = { { 1 , 2 , 3 } , { 4 , 5 , 6 } }\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    PT_UNIT_ASSERT(2 == settings.getMember("a").memberCount() );
    PT_UNIT_ASSERT(2 == settings.getMember("b").memberCount() );
}


void SettingsTest::SimpleValue()
{
    std::stringstream ss;
    ss << "a = 5\n";
    ss << "b=6\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    PT_UNIT_ASSERT(5 == settings.getValue<int>("a") );
    PT_UNIT_ASSERT(6 == settings.getValue<int>("b") );
}

void SettingsTest::SimpleTypedValue()
{
    std::stringstream ss;
    ss << "a = int ( 5 )\n";
    ss << "b = int(6)\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    PT_UNIT_ASSERT(5 == settings.getValue<int>("a") );
    PT_UNIT_ASSERT(6 == settings.getValue<int>("b") );
}

void SettingsTest::SimpleQoutedValue()
{
    std::stringstream ss;
    ss << "a=\"a b c\"\n";
    ss << "b = \"a b c\"\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    PT_UNIT_ASSERT("a b c" == settings.getValue<std::string>("a") );
    PT_UNIT_ASSERT("a b c" == settings.getValue<std::string>("b") );
}

void SettingsTest::SimpleArray()
{
    std::stringstream ss;
    ss << "a={1,2,3}\n";
    ss << "b = { 4 , 5 , 6 } \n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);
}

void SettingsTest::SimpleNamedArray()
{
    std::stringstream ss;
    ss << "a=array{1,2,3}\n";
    ss << "b = array { 4 , 5 , 6 } \n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);
}

void SettingsTest::SimpleQoutedArray()
{
    std::stringstream ss;
    ss << "a={\"1\",\"2\",\"3\"}\n";
    ss << "b = { \"4\" , \"5\" , \"6\" } \n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);
}

void SettingsTest::SimpleTypedArray()
{
    std::stringstream ss;
    ss << "a={int(1),int(2),int(3)}\n";
    ss << "b = { int( 4 ) , int( 5 ) , int( 6 ) }\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);
}

void SettingsTest::SimpleArrayQoutedTypedValues()
{
    std::stringstream ss;
    ss << "a={int(\"1\"),int(\"2\"),int(\"3\")}\n";
    ss << "b = { int( \"4\" ) , int( \"5\" ) , int( \"6\" ) }\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);
}

void SettingsTest::ComplexType()
{
    std::stringstream ss;
    ss << "a={red=1,green=2,blue=3}\n";
    ss << "b = { red = 4 , green = 5 , blue = 6 }\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    PT_UNIT_ASSERT(3 == settings.getMember("a").memberCount() );
    PT_UNIT_ASSERT(3 ==  settings.getMember("b").memberCount() );
}

void SettingsTest::ComplexTypeNamedQoutedValues()
{
    std::stringstream ss;
    ss << "a={red=\"1\",green=\"2\",blue=\"3\"}\n";
    ss << "b = { red = \"4\" , green = \"5\" , blue = \"6\" }\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    PT_UNIT_ASSERT(3 == settings.getMember("a").memberCount() );
    PT_UNIT_ASSERT(3 ==  settings.getMember("b").memberCount() );
}

void SettingsTest::ComplexNamedType()
{
    std::stringstream ss;
    ss << "a=Color{red=int(1),green=int(2),blue=int(3)}\n";
    ss << "b = Color { red = int ( 4 ) , green = int ( 5 ) , blue = int ( 6 ) }";
    ss << "[section]\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    PT_UNIT_ASSERT(3 == settings.getMember("a").memberCount() );
    PT_UNIT_ASSERT(3 ==  settings.getMember("b").memberCount() );
}

void SettingsTest::Section()
{
    std::stringstream ss;
    ss << "[a.b.c]\n";
    ss << "d.v = 1\n";
    ss << "d.u = \"2\"\n";
    ss << "[x.y.z]\n";
    ss << "u.v = 3\n";
    ss << "u.w = \"4\"\n";
    ss << "[e.f.g]\n";
    ss << "u.v = \"5\"\n";
    ss << "u.w = 6\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    PT_UNIT_ASSERT( settings.findMember("a.b.c.d") )
    PT_UNIT_ASSERT( settings.findMember("a.b.c.d")->getValue<std::string>("v") == "1");
    PT_UNIT_ASSERT( settings.findMember("a.b.c.d")->getValue<std::string>("u") == "2");

    PT_UNIT_ASSERT( settings.findMember("x.y.z.u") )
    PT_UNIT_ASSERT( settings.findMember("x.y.z.u")->getValue<std::string>("v") == "3");
    PT_UNIT_ASSERT( settings.findMember("x.y.z.u")->getValue<std::string>("w") == "4");

    PT_UNIT_ASSERT( settings.findMember("e.f.g.u") )
    PT_UNIT_ASSERT( settings.findMember("e.f.g.u")->getValue<std::string>("v") == "5");
    PT_UNIT_ASSERT( settings.findMember("e.f.g.u")->getValue<std::string>("w") == "6");
}
