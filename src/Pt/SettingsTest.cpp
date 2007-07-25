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
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Gfx/Brush.h"
#include "Pt/Gfx/Font.h"
#include <string>


class SettingsTest : public Pt::Unit::TestSuite
{
    public:
        SettingsTest()
        : Pt::Unit::TestSuite("SettingsTest")
        {
            Pt::Unit::TestSuite::registerMethod( "PlainValue", *this, &SettingsTest::PlainValue );
            Pt::Unit::TestSuite::registerMethod( "PlainQoutedValue", *this, &SettingsTest::PlainQoutedValue );
            Pt::Unit::TestSuite::registerMethod( "PlainArray", *this, &SettingsTest::PlainArray );
            Pt::Unit::TestSuite::registerMethod( "PlainQoutedArray", *this, &SettingsTest::PlainQoutedArray );
            Pt::Unit::TestSuite::registerMethod( "ComplexType", *this, &SettingsTest::ComplexType );
            Pt::Unit::TestSuite::registerMethod( "SimpleList", *this, &SettingsTest::SimpleList );
            Pt::Unit::TestSuite::registerMethod( "QoutedComplexType", *this, &SettingsTest::QoutedComplexType );
            Pt::Unit::TestSuite::registerMethod( "ComplexTypeWithTypename", *this, &SettingsTest::ComplexTypeWithTypename );
        }

    protected:
        void PlainValue()
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

            PT_UNIT_ASSERT( settings.getData(L"a.b.c.d") )
            PT_UNIT_ASSERT( settings.getData(L"a.b.c.d")->getEntry(L"v") )
            const Pt::Variant* s = settings.getData(L"a.b.c.d")->getEntry(L"v");
            PT_UNIT_ASSERT( s)
            PT_UNIT_ASSERT( *s == L"3")

            s = settings.getData(L"a.b.c.d")->getEntry(L"u");
            PT_UNIT_ASSERT( s)
            PT_UNIT_ASSERT( *s == L"4")
        }

        void PlainQoutedValue()
        {
            std::stringstream ss;
            ss << "[a.b.c ]\n";
            ss << "d.v = \"3\" \n \"4\"\n";
            ss << "d.u = \"3\" \n \"4\"\n";

            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::SettingsReader reader(ts);
            Pt::Settings settings;
            reader.read(settings);

            const Pt::Variant* s = settings.getData(L"a.b.c.d")->getEntry(L"v");
            PT_UNIT_ASSERT( s)
            PT_UNIT_ASSERT( *s == L"34")

            s = settings.getData(L"a.b.c.d")->getEntry(L"u");
            PT_UNIT_ASSERT( s)
            PT_UNIT_ASSERT( *s == L"34")
        }

        void PlainArray()
        {
            std::stringstream ss;
            ss << "a.b.c = { 1,2,3 }\n";
            ss << "d.e.f = {1,2,3}\n";
            ss << "g.h.i = { 1,\"\\n2\", 3 }\n";

            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::SettingsReader reader(ts);
            Pt::Settings settings;
            reader.read(settings);

            Pt::String concat;
            const Pt::SerializationData* a = settings.getData(L"a.b");
            PT_UNIT_ASSERT(a)
            
            a = a->getData(L"c");
            PT_UNIT_ASSERT(a)
            
            for( Pt::SerializationData::ConstNodeIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toEntry()->value().str();
            }
            PT_UNIT_ASSERT( concat == L"123")

            concat.clear();

            a = settings.getData(L"g.h");
            PT_UNIT_ASSERT(a)
            
            a = a->getData(L"i");
            PT_UNIT_ASSERT(a)

            for( Pt::SerializationData::ConstNodeIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toEntry()->value().str();
            }
            PT_UNIT_ASSERT( concat == L"1\n23")
        }

        void PlainQoutedArray()
        {
            std::stringstream ss;
            ss << "a.b.c = { \"1\" , \"2\", \"3\"}\n";
            ss << "g.h.i = {\"1\", \"2\" \"2\" , \"3\" }\n";

            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::SettingsReader reader(ts);
            Pt::Settings settings;
            reader.read(settings);

            Pt::String concat;
            const Pt::SerializationData* a = settings.getData(L"a.b");
            PT_UNIT_ASSERT(a)
            
            a = a->getData(L"c");
            PT_UNIT_ASSERT(a)
            
            for( Pt::SerializationData::ConstNodeIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toEntry()->value().str();
            }
            PT_UNIT_ASSERT( concat == L"123")

            concat.clear();
            
            a = settings.getData(L"g.h");
            PT_UNIT_ASSERT(a)
            
            a = a->getData(L"i");
            PT_UNIT_ASSERT(a)
            
            for( Pt::SerializationData::ConstNodeIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toEntry()->value().str();
            }
            PT_UNIT_ASSERT( concat == L"1223")
        }

        void ComplexType()
        {
            std::stringstream ss;
            ss << "a.b.c = ( d = 1, e =2, f= ( g = 3) )\n";

            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::SettingsReader reader(ts);
            Pt::Settings settings;
            reader.read(settings);

            Pt::String concat;
            const Pt::SerializationData* data = settings.getData(L"a.b");
            PT_UNIT_ASSERT( data )
            
            data = data->getData(L"c");
            PT_UNIT_ASSERT( data )

            const Pt::Variant* value = data->getEntry( L"d" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( *value == L"1" )

            value = data->getEntry( L"e" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( *value == L"2" )

            data = data->getData( L"f" );
            PT_UNIT_ASSERT( data )

            for( Pt::SerializationData::ConstNodeIterator it = data->begin(); it != data->end(); ++it)
            {
                concat += (*it).toEntry()->value().str();
            }

            PT_UNIT_ASSERT( concat == L"3")
        }

        void ComplexTypeWithTypename()
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
            settings.load(ts);

            Pt::Gfx::Pen a;
            settings.set(a, L"myPen");

            Pt::Gfx::Pen p;
            settings.get(p, L"myPen");

            Pt::Gfx::Font f("Arial");
            settings.set(f, L"myFont");

            Pt::Gfx::Brush b;
            settings.set(b, L"myBrush");

            Pt::StringStream sout;
            settings.save(sout);
            std::cerr << "\n" << sout.str().narrow() << std::endl;
        }

        void SimpleList()
        {
            std::stringstream ss;
            ss << "a.b.c = ( d = 1, e =2, f= 3 )\n";

            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::Settings settings;
            settings.load(ts);

            Pt::String concat;
            const Pt::SerializationData* data = settings.getData(L"a.b");
            PT_UNIT_ASSERT( data )
            
            data = data->getData(L"c");
            PT_UNIT_ASSERT( data )

            const Pt::Variant* value = data->getEntry( L"d" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( *value == L"1" )

            value = data->getEntry( L"e" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( *value == L"2" )

            value = data->getEntry( L"f" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( *value == L"3" )
        }

        void QoutedComplexType()
        {
            std::stringstream ss;
            ss << "a.b.c = ( d =\"1\", x= ( g =9) , e = \"2\" \"2\" , f= ( g =\"3\") )\n";

            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::SettingsReader reader(ts);
            Pt::Settings settings;
            reader.read(settings);

            Pt::String concat;
            const Pt::SerializationData* a = settings.getData(L"a.b");
            PT_UNIT_ASSERT( a )
            
            a = a->getData(L"c");
            PT_UNIT_ASSERT( a )

            const Pt::Variant* value = a->getEntry( L"d" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( *value == L"1" )

            value = a->getEntry( L"e" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( *value == L"22" )

            a = a->getData( L"f" );
            PT_UNIT_ASSERT( a )

            for( Pt::Settings::ConstNodeIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toEntry()->value().str();
            }

            PT_UNIT_ASSERT( concat == L"3")

            //Pt::StringStream sout;
            //settings.save(sout);
            //std::cerr << "\n" << sout.str().narrow() << std::endl;
        }
};

Pt::Unit::RegisterTest<SettingsTest> register_SettingsTest;
