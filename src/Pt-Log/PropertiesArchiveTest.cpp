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
#undef PT_API_LOG_EXPORT

#include "PropertiesArchive.h"
#include "PropertiesReader.h"
#include "PropertiesWriter.h"

#include "Pt/Archive.h"
#include "Pt/Date.h"
#include "Pt/Time.h"
#include "Pt/DateTime.h"
#include "Pt/Text/TextStream.h"
#include "Pt/Text/Utf8Codec.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>


class PropertiesArchiveTest : public Pt::Unit::TestSuite
{
    public:
        PropertiesArchiveTest()
        : Pt::Unit::TestSuite("PropertiesArchiveTest")
        {
            Pt::Unit::TestSuite::registerMethod( "PlainValue", *this, &PropertiesArchiveTest::PlainValue );
            Pt::Unit::TestSuite::registerMethod( "PlainQoutedValue", *this, &PropertiesArchiveTest::PlainQoutedValue );
            Pt::Unit::TestSuite::registerMethod( "PlainArray", *this, &PropertiesArchiveTest::PlainArray );
            Pt::Unit::TestSuite::registerMethod( "PlainQoutedArray", *this, &PropertiesArchiveTest::PlainQoutedArray );
            Pt::Unit::TestSuite::registerMethod( "ComplexType", *this, &PropertiesArchiveTest::ComplexType );
            Pt::Unit::TestSuite::registerMethod( "QoutedComplexType", *this, &PropertiesArchiveTest::QoutedComplexType );
            //Pt::Unit::TestSuite::registerMethod( "Date", *this, &PropertiesArchiveTest::Date );
            //Pt::Unit::TestSuite::registerMethod( "Time", *this, &PropertiesArchiveTest::Time );
            //Pt::Unit::TestSuite::registerMethod( "DateTime", *this, &PropertiesArchiveTest::DateTime );
        }

    protected:
        void PlainValue()
        {
            std::stringstream ss;
            ss << "[ a.b.c]\n";
            ss << "d.v = 3\n";
            ss << "d.u = 4\n";
            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::PropertiesReader reader(ts);
            Pt::Settings archive;
            reader.read(archive);

            const Pt::String* s = archive.getArchive(L"a.b.c.d")->getValue(L"v");
            PT_UNIT_ASSERT( s)
            PT_UNIT_ASSERT( s->narrow() == "3")

            s = archive.getArchive(L"a.b.c.d")->getValue(L"u");
            PT_UNIT_ASSERT( s)
            PT_UNIT_ASSERT( s->narrow() == "4")
        }

        void PlainQoutedValue()
        {
            std::stringstream ss;
            ss << "[a.b.c ]\n";
            ss << "d.v = \"3\" \n \"4\"\n";
            ss << "d.u = \"3\" \n \"4\"\n";

            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::PropertiesReader reader(ts);
            Pt::Settings archive;
            reader.read(archive);

            const Pt::String* s = archive.getArchive(L"a.b.c.d")->getValue(L"v");
            PT_UNIT_ASSERT( s)
            PT_UNIT_ASSERT( s->narrow() == "34")

            s = archive.getArchive(L"a.b.c.d")->getValue(L"u");
            PT_UNIT_ASSERT( s)
            PT_UNIT_ASSERT( s->narrow() == "34")
        }

        void PlainArray()
        {
            std::stringstream ss;
            ss << "a.b.c = { 1,2,3 }\n";
            ss << "d.e.f = {1,2,3}\n";
            ss << "g.h.i = { 1,\"\\n2\", 3 }\n";

            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::PropertiesReader reader(ts);
            Pt::Settings archive;
            reader.read(archive);

            Pt::String concat;
            const Pt::Archive* a = archive.getArchive(L"a.b.c");
            for( Pt::Archive::ConstIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toValue()->value();
            }
            PT_UNIT_ASSERT( concat.narrow() == "123")

            concat.clear();
            a = archive.getArchive(L"g.h.i");
            for( Pt::Archive::ConstIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toValue()->value();
            }
            PT_UNIT_ASSERT( concat.narrow() == "1\n23")
        }

        void PlainQoutedArray()
        {
            std::stringstream ss;
            ss << "a.b.c = { \"1\" , \"2\", \"3\"}\n";
            ss << "g.h.i = {\"1\", \"2\" \"2\" , \"3\" }\n";

            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::PropertiesReader reader(ts);
            Pt::Settings archive;
            reader.read(archive);

            Pt::String concat;
            const Pt::Archive* a = archive.getArchive(L"a.b.c");
            for( Pt::Archive::ConstIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toValue()->value();
            }
            PT_UNIT_ASSERT( concat.narrow() == "123")

            concat.clear();
            a = archive.getArchive(L"g.h.i");
            for( Pt::Archive::ConstIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toValue()->value();
            }
            PT_UNIT_ASSERT( concat.narrow() == "1223")
        }

        void ComplexType()
        {
            std::stringstream ss;
            ss << "a.b.c = ( d = 1, e =2, f= ( g = 3) )\n";

            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::PropertiesReader reader(ts);
            Pt::Settings archive;
            reader.read(archive);

            Pt::String concat;
            const Pt::Archive* a = archive.getArchive(L"a.b.c");
            PT_UNIT_ASSERT( a )

            const Pt::String* value = a->getValue( L"d" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( value->narrow() == "1" )

            value = a->getValue( L"e" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( value->narrow() == "2" )

            a = a->getArchive( L"f" );
            PT_UNIT_ASSERT( a )

            for( Pt::Archive::ConstIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toValue()->value();
            }

            PT_UNIT_ASSERT( concat.narrow() == "3")
        }

        void QoutedComplexType()
        {
            std::stringstream ss;
            ss << "a.b.c = ( d =\"1\", e = \"2\" \"2\" , f= ( g =\"3\") )\n";

            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::PropertiesReader reader(ts);
            Pt::Settings archive;
            reader.read(archive);

            Pt::String concat;
            const Pt::Archive* a = archive.getArchive(L"a.b.c");
            PT_UNIT_ASSERT( a )

            const Pt::String* value = a->getValue( L"d" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( value->narrow() == "1" )

            value = a->getValue( L"e" );
            PT_UNIT_ASSERT( value )
            PT_UNIT_ASSERT( value->narrow() == "22" )

            a = a->getArchive( L"f" );
            PT_UNIT_ASSERT( a )

            for( Pt::Archive::ConstIterator it = a->begin(); it != a->end(); ++it)
            {
                concat += (*it).toValue()->value();
            }

            PT_UNIT_ASSERT( concat.narrow() == "3")
        }

        void Date()
        {
            std::stringstream ss;
            //ss << "myDate.julianDays = 400000";
            ss << "myDate = [ julianDays = 400000 ]";
            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::PropertiesReader reader(ts);
            Pt::Settings archive;
            reader.read(archive);

            Pt::Date date;
            archive.extract(date, L"myDate");

            //std::cerr << date.julian() << " days."<< std::endl;
            //std::cerr << date.toIsoString() << std::endl;

            PT_UNIT_ASSERT(date.julian() == 400000);
        }

        void Time()
        {
            std::stringstream ss;
            ss << "myTime.msecs = 50000000";

            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::PropertiesReader reader(ts);
            Pt::Settings archive;
            reader.read(archive);

            Pt::Time time;
            archive.extract(time, L"myTime");

            //std::cerr << time.totalMSecs() << " msecs."<< std::endl;
            //std::cerr << time.toIsoString() << std::endl;

            PT_UNIT_ASSERT(time.totalMSecs() == 50000000);
        }

        void DateTime()
        {
            std::stringstream ss;
            ss << "myDateTime.msecs = 50000000 \n";
            ss << "myDateTime.julianDays = 400000 \n";

            Pt::Text::TextIStream ts(ss, new Pt::Text::Utf8Codec);
            Pt::PropertiesReader reader(ts);
            Pt::Settings archive;
            reader.read(archive);

            Pt::DateTime date;
            archive.extract(date, L"myDateTime");

            //std::cerr << date.date().julian() << " days."<< std::endl;
            //std::cerr << date.time().totalMSecs() << std::endl;
            //std::cerr << date.toIsoString() << std::endl;

            PT_UNIT_ASSERT(date.date().julian() == 400000);
            PT_UNIT_ASSERT(date.time().totalMSecs() == 50000000);
        }
};

Pt::Unit::RegisterTest<PropertiesArchiveTest> register_PropertiesArchiveTest;
