/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duerner                       *
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
#include <Pt/StringStream.h>
#include "Pt/Text/TextStream.h"
#include "Pt/Text/Utf8Codec.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>


class SettingsTest : public Pt::Unit::TestSuite
{
    public:
        SettingsTest()
        : Pt::Unit::TestSuite("SettingsTest")
        {
            Pt::Unit::TestSuite::registerMethod( "PlainValue", *this, &SettingsTest::PlainValue );
            Pt::Unit::TestSuite::registerMethod( "ComplexArray", *this, &SettingsTest::ComplexArray );
            Pt::Unit::TestSuite::registerMethod( "PlainQoutedValue", *this, &SettingsTest::PlainQoutedValue );
            Pt::Unit::TestSuite::registerMethod( "PlainArray", *this, &SettingsTest::PlainArray );
            Pt::Unit::TestSuite::registerMethod( "PlainQoutedArray", *this, &SettingsTest::PlainQoutedArray );
            Pt::Unit::TestSuite::registerMethod( "ComplexType", *this, &SettingsTest::ComplexType );
            Pt::Unit::TestSuite::registerMethod( "ComplexType3", *this, &SettingsTest::ComplexType3 );
            Pt::Unit::TestSuite::registerMethod( "SimpleList", *this, &SettingsTest::SimpleList );
            Pt::Unit::TestSuite::registerMethod( "SimpleQoutedList", *this, &SettingsTest::SimpleQoutedList );
            Pt::Unit::TestSuite::registerMethod( "QoutedComplexType", *this, &SettingsTest::QoutedComplexType );
            Pt::Unit::TestSuite::registerMethod( "SimpleTypedList", *this, &SettingsTest::SimpleTypedList );
            Pt::Unit::TestSuite::registerMethod( "SimpleTypedQuotedList", *this, &SettingsTest::SimpleTypedQuotedList );
            Pt::Unit::TestSuite::registerMethod( "ComplexTypeWithTypename", *this, &SettingsTest::ComplexTypeWithTypename );
        }

    protected:

        void PlainValue();
        void ComplexArray();
        void PlainQoutedValue();
        void PlainArray();
        void PlainQoutedArray();
        void ComplexType();
        void ComplexType3();
        void SimpleList();
        void SimpleQoutedList();
        void QoutedComplexType();
        void SimpleTypedList();
        void SimpleTypedQuotedList();
        void ComplexTypeWithTypename();




};

Pt::Unit::RegisterTest<SettingsTest> register_SettingsTest;


void SettingsTest::PlainValue()
{
    std::stringstream ss;
    ss << "[ a.b.c]\n";
    ss << "d.v = g(\"3\")\n";
    ss << "d.u = 4\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
    Pt::SettingsReader reader(ts);
    Pt::Settings settings;
    reader.read(settings);

    //Pt::StringStream sout;
    //settings.save(sout);
    //std::cerr << "\n" << sout.str().narrow() << std::endl;

    PT_UNIT_ASSERT( settings.findMember("a.b.c.d") )
    PT_UNIT_ASSERT( settings.findMember("a.b.c.d")->getValue<std::string>("v") == "3")
    PT_UNIT_ASSERT( settings.findMember("a.b.c.d")->getValue<std::string>("u") == "4")
}

void SettingsTest::PlainQoutedValue()
{
    std::stringstream ss;
    ss << "[a.b.c ]\n";
    ss << "d.v = \";3\" \n \"4\"\n";
    ss << "d.u = \"3\" \n \"4\"\n";

    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
    Pt::SettingsReader reader(ts);
    Pt::Settings settings;
    reader.read(settings);

    PT_UNIT_ASSERT( settings.findMember("a.b.c.d")->getValue<std::string>("v") == ";34")
    PT_UNIT_ASSERT( settings.findMember("a.b.c.d")->getValue<std::string>("u") == "34")
}

void SettingsTest::PlainArray()
{
    std::stringstream ss;
    ss << "a.b.c = { 1,2,3 }\n";
    ss << "d.e.f = {1,2,3}\n";
    ss << "g.h.i = { 1,\"2\", 3 }\n";

    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
    Pt::SettingsReader reader(ts);
    Pt::Settings settings;
    reader.read(settings);

    std::string concat;
    const Pt::SerializationInfo* a = settings.findMember("a.b");
    PT_UNIT_ASSERT(a)

    a = a->findMember("c");
    PT_UNIT_ASSERT(a)

    for( Pt::SerializationInfo::ConstIterator it = a->begin(); it != a->end(); ++it)
    {
        concat += it->toValue<std::string>();
    }
    PT_UNIT_ASSERT( concat == "123")

    concat.clear();

    a = settings.findMember("g.h");
    PT_UNIT_ASSERT(a)

    a = a->findMember("i");
    PT_UNIT_ASSERT(a)

    for( Pt::SerializationInfo::ConstIterator it = a->begin(); it != a->end(); ++it)
    {
        concat += it->toValue<std::string>();
    }

    PT_UNIT_ASSERT( concat == "123")
}

void SettingsTest::PlainQoutedArray()
{
    std::stringstream ss;
    ss << "a.b.c = {\"1\", \"2\", \"3\"}\n";
    ss << "g.h.i = {\"1\", \"2\", \"2\" , \"3\" }\n";

    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
    Pt::SettingsReader reader(ts);
    Pt::Settings settings;
    reader.read(settings);

    std::string concat;
    const Pt::SerializationInfo* a = settings.findMember("a.b");
    PT_UNIT_ASSERT(a)

    a = a->findMember("c");
    PT_UNIT_ASSERT(a)

    for( Pt::SerializationInfo::ConstIterator it = a->begin(); it != a->end(); ++it)
    {
        concat += it->toValue<std::string>();
    }
    PT_UNIT_ASSERT( concat == "123")

    concat.clear();

    a = settings.findMember("g.h");
    PT_UNIT_ASSERT(a)

    a = a->findMember("i");
    PT_UNIT_ASSERT(a)

    for( Pt::SerializationInfo::ConstIterator it = a->begin(); it != a->end(); ++it)
    {
        concat += it->toValue<std::string>();
    }
    PT_UNIT_ASSERT( concat == "1223")
}

void SettingsTest::ComplexArray()
{
    std::stringstream ss;
    ss << "a.b.c = { Point{x=\"0.0\", y=\"0.0\"}, Point{x=\"75.0\", y=\"0.75\"} }\n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
    Pt::SettingsReader reader(ts);
    Pt::Settings settings;
    reader.read(settings);

    Pt::StringStream sout;
    settings.save(&sout);
//    std::cerr << "################\n" << sout.str().narrow() << std::endl;
}

void SettingsTest::ComplexType()
{
    std::stringstream ss;
    ss << "a.b.c = ( d = 1, e =2, f= ( g = 3) )\n";

    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
    Pt::SettingsReader reader(ts);
    Pt::Settings settings;
    reader.read(settings);

    std::string concat;
    const Pt::SerializationInfo* data = settings.findMember("a.b");
    PT_UNIT_ASSERT( data )

    data = data->findMember("c");
    PT_UNIT_ASSERT( data )

    PT_UNIT_ASSERT( data->getValue<std::string>( "d" ) == "1" )
    PT_UNIT_ASSERT( data->getValue<std::string>( "e" ) == "2" )

    data = data->findMember( "f" );
    PT_UNIT_ASSERT( data )

    for( Pt::SerializationInfo::ConstIterator it = data->begin(); it != data->end(); ++it)
    {
        concat += it->toValue<std::string>();
    }

    PT_UNIT_ASSERT( concat == "3")
}

void SettingsTest::ComplexTypeWithTypename()
{
    std::stringstream ss;
    ss << "Painter.pen =Pen( color= Color ( red = char(255 ),  \n";
    ss << "                                 green= char (0),   \n";
    ss << "                                 blue =char( 0) ),  \n";
    ss << "                  size=1 )                          \n";
    ss << "Painter.alpha = 50                                  \n";
    ss << "b.x = B( char( 30), char(40 ) ,char (50) , char(60) ) \n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(&ts);

    //Pt::StringStream sout;
    //settings.save(sout);
    //std::cerr << "\n" << sout.str().narrow() << std::endl;
}

void SettingsTest::ComplexType3()
{
    std::stringstream ss;
    ss << "a.b.n = { { Pen(\"1\"), Pen(\"#112233\") } }   \n";
    ss << "a.b.n = v1{ v2{ Pen(\"1\"), Pen(\"#112233\") } }   \n";
    ss << "a.b.colors = array{ Color{ red = char(255 ),      \n";
    ss << "                           green= char (0),       \n";
    ss << "                           blue =char( 0) },      \n";
    ss << "                    Color { red = char(255 ),     \n";
    ss << "                            green= char (0),      \n";
    ss << "                            blue =char( 0) } }    \n";
    ss << "b = char(5) \n";
    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);

    Pt::Settings settings;
    settings.load(&ts);

    //Pt::StringStream sout;
    //settings.save(sout);
    //std::cerr << "\n" << sout.str().narrow() << std::endl;
}

void SettingsTest::SimpleTypedList()
{
    std::stringstream ss;
    ss << "a.b.c = List( d =char( 1), e = char (2), f= char(3 ) )\n";

    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
    Pt::Settings settings;
    settings.load(&ts);

    Pt::String concat;
    const Pt::SerializationInfo* data = settings.findMember("a.b");
    PT_UNIT_ASSERT( data )

    data = data->findMember("c");
    PT_UNIT_ASSERT( data )
    PT_UNIT_ASSERT( data->typeName() == "List" )

    PT_UNIT_ASSERT( data->getValue<std::string>( "d" ) == "1" )
    PT_UNIT_ASSERT( data->getValue<std::string>( "e" ) == "2" )
    PT_UNIT_ASSERT( data->getValue<std::string>( "f" ) == "3" )
}

void SettingsTest::SimpleTypedQuotedList()
{
    std::stringstream ss;
    ss << "a.b.c = List( d =char( \"1\"), e = char (\"2\"), f= char(\"3\" ) )\n";

    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
    Pt::Settings settings;
    settings.load(&ts);

    Pt::String concat;
    const Pt::SerializationInfo* data = settings.findMember("a.b");
    PT_UNIT_ASSERT( data )

    data = data->findMember("c");
    PT_UNIT_ASSERT( data )
    PT_UNIT_ASSERT( data->typeName() == "List" )

    PT_UNIT_ASSERT( data->getValue<std::string>( "d" ) == "1" )
    PT_UNIT_ASSERT( data->getValue<std::string>( "e" ) == "2" )
    PT_UNIT_ASSERT( data->getValue<std::string>( "f" ) == "3" )
}

void SettingsTest::SimpleList()
{
    std::stringstream ss;
    ss << "a.b.c = List( d = 1, e =2, f= 3 )\n";

    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
    Pt::Settings settings;
    settings.load(&ts);

    Pt::String concat;
    const Pt::SerializationInfo* data = settings.findMember("a.b");
    PT_UNIT_ASSERT( data )

    data = data->findMember("c");
    PT_UNIT_ASSERT( data )
    PT_UNIT_ASSERT( data->typeName() == "List" )
    PT_UNIT_ASSERT( data->getValue<std::string>( "d" ) == "1" )
    PT_UNIT_ASSERT( data->getValue<std::string>( "e" ) == "2" )
    PT_UNIT_ASSERT( data->getValue<std::string>( "f" ) == "3" )
}

void SettingsTest::SimpleQoutedList()
{
    std::stringstream ss;
    ss << "a.b.c = List( d = \"1\", e =\"2\", f= \"3\" )\n";

    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
    Pt::Settings settings;
    settings.load(&ts);

    Pt::String concat;
    const Pt::SerializationInfo* data = settings.findMember("a.b");
    PT_UNIT_ASSERT( data )

    data = data->findMember("c");
    PT_UNIT_ASSERT( data )
    PT_UNIT_ASSERT( data->typeName() == "List" )

    PT_UNIT_ASSERT( data->getValue<std::string>( "d" ) == "1" )
    PT_UNIT_ASSERT( data->getValue<std::string>( "e" ) == "2" )
    PT_UNIT_ASSERT( data->getValue<std::string>( "f" ) == "3" )
}

void SettingsTest::QoutedComplexType()
{
    std::stringstream ss;
    ss << "a.b.c = ( d =\"1\", x= ( g =9) , e = \"2\" \"2\" , f= ( g =\"3\") )\n";

    Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
    Pt::SettingsReader reader(ts);
    Pt::Settings settings;
    reader.read(settings);

    std::string concat;
    const Pt::SerializationInfo* a = settings.findMember("a.b");
    PT_UNIT_ASSERT( a )

    a = a->findMember("c");
    PT_UNIT_ASSERT( a )

    PT_UNIT_ASSERT( a->getValue<std::string>( "d" ) == "1" )
    PT_UNIT_ASSERT( a->getValue<std::string>( "e" ) == "22" )

    a = a->findMember( "f" );
    PT_UNIT_ASSERT( a )

    for( Pt::Settings::ConstIterator it = a->begin(); it != a->end(); ++it)
    {
        concat += it->toValue<std::string>();
    }

    PT_UNIT_ASSERT( concat == "3")

    //Pt::StringStream sout;
    //settings.save(sout);
    //std::cerr << "\n" << sout.str().narrow() << std::endl;
}
