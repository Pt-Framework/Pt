/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duerner                     *
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

#include "Pt/Settings.h"
#include "Pt/Text/TextStream.h"
#include "Pt/Text/Utf8Codec.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>

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
};

Pt::Unit::RegisterTest<SettingsTest> register_SettingsTest;

/*
std::ifstream in("/home/marc/Desktop/maprenderer.settings");
PT_UNIT_ASSERT(in)

Pt::Text::TextIStream ts(in, new Pt::Text::Utf8Codec);
Pt::Settings settings;
settings.load(&ts);
*/

void SettingsTest::Comment()
{
    std::stringstream ss;
    ss << ";first comment\n";
    ss << "a = \"1#;2\"\n";
    ss << "#second comment\n";
    ss << "b = 2\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(&ts);

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
    settings.load(&ts);

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
    settings.load(&ts);

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
    settings.load(&ts);

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
    settings.load(&ts);

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
    settings.load(&ts);
}

void SettingsTest::SimpleNamedArray()
{
    std::stringstream ss;
    ss << "a=array{1,2,3}\n";
    ss << "b = array { 4 , 5 , 6 } \n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(&ts);
}

void SettingsTest::SimpleQoutedArray()
{
    std::stringstream ss;
    ss << "a={\"1\",\"2\",\"3\"}\n";
    ss << "b = { \"4\" , \"5\" , \"6\" } \n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(&ts);
}

void SettingsTest::SimpleTypedArray()
{
    std::stringstream ss;
    ss << "a={int(1),int(2),int(3)}\n";
    ss << "b = { int( 4 ) , int( 5 ) , int( 6 ) }\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(&ts);
}

void SettingsTest::SimpleArrayQoutedTypedValues()
{
    std::stringstream ss;
    ss << "a={int(\"1\"),int(\"2\"),int(\"3\")}\n";
    ss << "b = { int( \"4\" ) , int( \"5\" ) , int( \"6\" ) }\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(&ts);
}

void SettingsTest::ComplexType()
{
    std::stringstream ss;
    ss << "a={red=1,green=2,blue=3}\n";
    ss << "b = { red = 4 , green = 5 , blue = 6 }\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(&ts);

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
    settings.load(&ts);

    PT_UNIT_ASSERT(3 == settings.getMember("a").memberCount() );
    PT_UNIT_ASSERT(3 ==  settings.getMember("b").memberCount() );
}

void SettingsTest::ComplexNamedType()
{
    std::stringstream ss;
    ss << "a=Color{red=int(1),green=int(2),blue=int(3)}\n";
    ss << "b = Color { red = int ( 4 ) , green = int ( 5 ) , blue = int ( 6 ) }\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(&ts);

    PT_UNIT_ASSERT(3 == settings.getMember("a").memberCount() );
    PT_UNIT_ASSERT(3 ==  settings.getMember("b").memberCount() );
}

void SettingsTest::Section()
{
    std::stringstream ss;
    ss << "[a.b.c]\n";
    ss << "d.v = 1\n";
    ss << "d.u = 2\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(&ts);

    PT_UNIT_ASSERT( settings.findMember("a.b.c.d") )
    PT_UNIT_ASSERT( settings.findMember("a.b.c.d")->getValue<std::string>("v") == "1");
    PT_UNIT_ASSERT( settings.findMember("a.b.c.d")->getValue<std::string>("u") == "2");
}
