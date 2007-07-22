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
            Pt::Unit::TestSuite::registerMethod( "QoutedComplexType", *this, &SettingsTest::QoutedComplexType );

            Pt::Unit::TestSuite::registerMethod( "ComplexTypeWithTypename", *this, &SettingsTest::ComplexTypeWithTypename );
        }

    protected:
        void PlainValue()
        {
            std::stringstream ss;
            ss << "[ a.b.c]\n";
            ss << "d.v = 3\n";
            ss << "d.u = 4\n";
            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::SettingsReader reader(ts);
            Pt::Settings settings;
            reader.read(settings);

            PT_UNIT_ASSERT( settings.getData(L"a.b.c.d") )
            PT_UNIT_ASSERT( settings.getData(L"a.b.c.d")->getEntry(L"v") )
            const Pt::Variant* s = settings.getData(L"a.b.c.d")->getEntry(L"v");
            PT_UNIT_ASSERT( s)
            PT_UNIT_ASSERT( *s == L"3")

            Pt::StringStream sout;
            settings.save(sout);
            std::cerr << "\n" << sout.str().narrow() << std::endl;

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
            const Pt::SerializationData* a = settings.getData(L"a.b.c");
            for( Pt::SerializationData::ConstNodeIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toEntry()->value().str();
            }
            PT_UNIT_ASSERT( concat == L"123")

            concat.clear();
            a = settings.getData(L"g.h.i");
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
            const Pt::SerializationData* a = settings.getData(L"a.b.c");
            for( Pt::SerializationData::ConstNodeIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toEntry()->value().str();
            }
            PT_UNIT_ASSERT( concat == L"123")

            concat.clear();
            a = settings.getData(L"g.h.i");
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
            const Pt::SerializationData* a = settings.getData(L"a.b.c");
            PT_UNIT_ASSERT( a )

            const Pt::Variant* value = a->getEntry( L"d" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( *value == L"1" )

            value = a->getEntry( L"e" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( *value == L"2" )

            a = a->getData( L"f" );
            PT_UNIT_ASSERT( a )

            for( Pt::SerializationData::ConstNodeIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toEntry()->value().str();
            }

            PT_UNIT_ASSERT( concat == L"3")
        }

        void ComplexTypeWithTypename()
        {
            std::stringstream ss;
            //ss << "a.b.c = Color ( int d = 1 , e =2, f= test( g = 3) ) \n";
            //ss << "a.b.d = Color ( int d= 1, e =2, f= test( g = 3) )\n";
            //ss << "a.b.e = Color ( int d= 1 ,e=2, f= test( g = 3 ) )\n";
            //ss << "a = s(x=c(5))\n";

            ss << "a.b=1\n";
            ss << "p = P ( c = C ( red = char(255),\n"; // BUG: space after 255
            ss <<                 "green= char (0),\n";
            ss <<                 "blue =char( 0) ),\n";
            ss <<      "size = 1 )\n";


            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::SettingsReader reader(ts);
            Pt::Settings settings;
            reader.read(settings);

            /*
            Pt::String concat;
            const Pt::SerializationData* a = settings.getData(L"a.b.c");
            PT_UNIT_ASSERT( a )

            const Pt::Variant* value = a->getEntry( L"d" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( *value == L"1" )

            value = a->getEntry( L"e" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( *value == L"2" )

            a = a->getData( L"f" );
            PT_UNIT_ASSERT( a )

            for( Pt::SerializationData::ConstNodeIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toEntry()->value().str();
            }

            PT_UNIT_ASSERT( concat == L"3")
            */
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
            const Pt::SerializationData* a = settings.getData(L"a.b.c");
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

            Pt::StringStream sout;
            settings.save(sout);
            std::cerr << "\n" << sout.str().narrow() << std::endl;
        }
};

Pt::Unit::RegisterTest<SettingsTest> register_SettingsTest;
