/*
 * Copyright (C) 2005-2007 by Marc Boris Duerner
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
#undef PT_XML_API_EXPORT

#include "Pt/Xml/XmlSerializer.h"
#include "Pt/Xml/XmlDeserializer.h"
#include "Pt/Xml/XmlReader.h"
#include "Pt/Xml/InputSource.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/SerializationSurrogate.h"
#include "Pt/TextStream.h"
#include "Pt/Utf8Codec.h"
#include "Pt/DateTime.h"
#include "Pt/SmartPtr.h"
#include "Pt/Any.h"

#include <string>
#include <sstream>

namespace test {

class DateRef
{
    public:
        DateRef(Pt::Date* date = 0)
        : _date(date)
        , _n(5)
        { }

        void setDate(Pt::Date* date)
        { _date = date; }

        const Pt::Date* date() const
        { return _date; }

        int n() const
        { return _n; }

        void setN(int n)
        { _n = n; }

    private:
        Pt::Date* _date;
        int _n;
};


void fixup(const Pt::FixupInfo& fixup, DateRef& fixme)
{
    fixme.setDate(0);

    if( ! fixup.isNull() )
        fixme.setDate( fixup.getTarget<Pt::Date>() );
}


void operator >>=(const Pt::SerializationInfo& si, DateRef& dr)
{
    si.getMember("date").loadReference(dr, 1);

    int n = 0;
    si.getMember("n") >>= n;
    dr.setN(n);
}


void operator <<=(Pt::SerializationInfo& si, const DateRef& dr)
{
    si.addMember("date") <<= dr.date();
    si.addMember("n") <<= dr.n();
}

} // namespace test


namespace Pt {

inline void operator >>=(const Pt::SerializationInfo& si, std::multiset<Pt::Date>& dset)
{
    std::multiset<Pt::Date>::iterator pos;

    dset.clear();
    for(Pt::SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        Pt::Date tmp;
        *it >> Pt::load() >>= tmp;
        pos = dset.insert(tmp);

        Pt::Date& dt = const_cast<Pt::Date&>(*pos);
        it->rebind(&dt);
    }
}

}


void toXmlString(Pt::SerializationInfo& si, const Pt::Date& date)
{
    si.setString( Pt::String::widen( date.toIsoString() ) );
}


void fromXmlString(const Pt::SerializationInfo& si, Pt::Date& date)
{
    Pt::String str;
    si.getString(str);
    date = Pt::Date::fromIsoString( str.narrow() );
}


class XmlSerializerTest: public Pt::Unit::TestSuite
{
    public:
        typedef Pt::SmartPtr<Pt::Date> DateSmartPtr;

        XmlSerializerTest()
        : Pt::Unit::TestSuite("XmlSerializerTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Reference", *this, &XmlSerializerTest::Reference );
            Pt::Unit::TestSuite::registerMethod( "MultiSet", *this, &XmlSerializerTest::MultiSet );
            Pt::Unit::TestSuite::registerMethod( "Dict", *this, &XmlSerializerTest::Dict );
            Pt::Unit::TestSuite::registerMethod( "Object", *this, &XmlSerializerTest::Object );
            Pt::Unit::TestSuite::registerMethod( "AdvanceObject", *this, &XmlSerializerTest::AdvanceObject );
        }

    protected:
        void Reference()
        {
            Pt::Date date1(1889, 4, 20);
            test::DateRef dr( &date1 );
            const Pt::Date* dateptr = &date1;
            DateSmartPtr datesp( new Pt::Date(2000, 6, 25) );
            DateSmartPtr datesp2 = datesp;
            DateSmartPtr dateNull;

            std::stringstream output;
            Pt::TextOStream tos(output, new Pt::Utf8Codec);
            Pt::Xml::XmlWriter writer;
            writer.reset(tos);
            Pt::Xml::XmlSerializer ser(writer);
            ser.context()->registerSurrogate("date", &fromXmlString, &toXmlString);

            ser.begin(date1, "date1");
            ser.begin(dr, "dr");
            ser.begin(dateptr, "dateptr");
            ser.begin(datesp, "datesp");
            ser.begin(datesp2, "datesp2");
            ser.begin(dateNull, "dateNull");

            ser.finish();
            tos.flush();

            Pt::Date date2(1, 1, 1);
            dr.setDate(0);
            Pt::Date* dateptr2 = 0; // const ?
            DateSmartPtr datesp3;
            DateSmartPtr datesp4;
            DateSmartPtr nullDate( new Pt::Date(1 ,1, 1) );

            std::cerr << std::endl << output.str() << std::endl << std::endl;

            std::stringstream input( output.str() );

            Pt::Xml::BinaryInputSource is(input);
            Pt::Xml::XmlReader reader(is);
            Pt::Xml::XmlDeserializer deser(reader);
            deser.context()->registerSurrogate("date", &fromXmlString, &toXmlString);

            deser.begin(date2);
            deser.finish();

            deser.begin(dr);
            deser.finish();
            
            deser.begin(dateptr2);
            deser.finish();
            
            deser.begin(datesp3);
            deser.finish();
            
            deser.begin(datesp4);
            deser.finish();
            
            deser.begin(nullDate);
            deser.finish();

            deser.fixup();
            //std::cerr << "FIXED POINTER: "<< dr.date << " - " << &date2 << std::endl;
            //std::cerr << "RESULT: "<< dr.date()->toIsoString() << std::endl;
            /*std::cerr << "RESULT: "<< dateptr2->toIsoString() << std::endl;
            std::cerr << "RESULT: "<< datesp3->toIsoString() << std::endl;
            std::cerr << "RESULT: "<< datesp4->toIsoString() << std::endl;*/
            //std::cerr << "========================\n" << std::endl;

            PT_UNIT_ASSERT( date1 == date2);
            PT_UNIT_ASSERT( datesp3.get() == datesp4.get() );
            PT_UNIT_ASSERT( nullDate.get() == 0);
        }

        void MultiSet()
        {
            std::multiset<Pt::Date> dates;
            dates.insert( Pt::Date(2000, 4,18) );
            dates.insert( Pt::Date(2000, 4,17) );
            dates.insert( Pt::Date(2000, 4,19) );
            const Pt::Date* dateptr = &(*dates.begin() );

            std::stringstream output;
            Pt::TextOStream tos(output, new Pt::Utf8Codec);
            Pt::Xml::XmlWriter writer;
            writer.reset(tos);
            Pt::Xml::XmlSerializer ser(writer);

            ser.begin(dates, "dates");
            ser.begin(dateptr, "dateptr");

            ser.finish();
            tos.flush();

            std::cerr << std::endl << output.str() << std::endl << std::endl;

            dates.clear();
            dateptr = 0;

            std::stringstream input( output.str() );
            //Pt::TextIStream tis(input, new Pt::Utf8Codec);

            Pt::Xml::BinaryInputSource is(input);
            Pt::Xml::XmlReader reader(is);
            Pt::Xml::XmlDeserializer deser(reader);

            deser.begin(dates);
            deser.finish();

            deser.begin(dateptr);
            deser.finish();

            deser.fixup();

            PT_UNIT_ASSERT( dateptr );
            PT_UNIT_ASSERT( dates.size() == 3 );
            PT_UNIT_ASSERT( dateptr == &(*dates.begin()) );
            std::cerr << "dateptr:" << dateptr->toIsoString() << std::endl;
        }

        void Dict()
        {
            std::map<int, int> numbers;
            numbers[1] = 4;
            numbers[2] = 5;
            numbers[3] = 6;

            std::stringstream output;
            Pt::TextOStream tos(output, new Pt::Utf8Codec);
            Pt::Xml::XmlWriter writer(tos);
            Pt::Xml::XmlSerializer ser(writer);

            ser.begin(numbers, "numbers");

            while( ! ser.advance() )
                ;

            ser.finish();
            tos.flush();

            std::cerr << std::endl << output.str() << std::endl << std::endl;

            numbers.clear();

            std::stringstream input( output.str() );
            Pt::Xml::BinaryInputSource is(input);
            Pt::Xml::XmlReader reader(is);
            Pt::Xml::XmlDeserializer deser(reader);

            deser.begin(numbers);
            deser.finish();

            deser.fixup();

            PT_UNIT_ASSERT( numbers.size() == 3 );
        }

        void Object()
        {
            Pt::DateTime date1(1889, 4, 20, 1, 2, 3, 4);
            Pt::Date date2(2000, 4,18);

            std::stringstream output;
            Pt::TextOStream tos(output, new Pt::Utf8Codec);
            Pt::Xml::XmlWriter writer;
            writer.reset(tos);
            Pt::Xml::XmlSerializer ser(writer);
            ser.context()->enableReferencing(false);

            ser.begin(date1, "date1a");
            ser.begin(date2, "date2a");
            ser.begin(date1, "date1b");
            ser.begin(date2, "date2b");

            ser.finish();
            tos.flush();

            std::cerr << std::endl << output.str() << std::endl << std::endl;

            Pt::DateTime date3(1, 1, 1, 1, 1, 1, 1);
            Pt::Date date4(1800, 7, 6);

            std::stringstream input( output.str() );
            //Pt::TextIStream tis(input, new Pt::Utf8Codec);

            Pt::Xml::BinaryInputSource is(input);
            Pt::Xml::XmlReader reader(is);
            Pt::Xml::XmlDeserializer deser(reader);
            deser.context()->enableReferencing(false);

            deser.begin(date3);
            deser.finish();
            
            deser.begin(date4);
            deser.finish();
            
            deser.fixup();

            // std::cerr << "IMPORT: " << tis.buffer().import() << std::endl;
            // std::cerr << "AVAIL: " << tis.buffer().in_avail() << std::endl;
            // Pt::Composer<Pt::DateTime> des;
            // des.begin(date2);
            // Pt::IComposer* d = deser.advance(&des);
            // std::cerr << "D (null): " << d << std::endl;
            // std::cerr << "DATE: " << date2.toIsoString() << std::endl;
            // deser.finish();

            PT_UNIT_ASSERT(date1 == date3);
            PT_UNIT_ASSERT(date2 == date4);
        }

        void AdvanceObject()
        {
            try {

            Pt::Date date1a(1889, 4,20);
            Pt::Date date2a(1945, 4,29);
            Pt::Date* dateptr1a = &date1a;

            std::stringstream output;
            Pt::TextOStream tos(output, new Pt::Utf8Codec);
            Pt::Xml::XmlWriter writer(tos);

            Pt::Xml::XmlSerializer ser(writer);

            ser.begin(date1a, "date1");
            ser.begin(date2a, "date2");
            ser.begin(dateptr1a, "dateptr1");

            //std::cerr << "\n--------------------" << std::endl;

            while( ! ser.advance() )
            {
                //ser.flush();
                //std::cerr << output.str() << "- ADVANCE -" << std::endl;
                //output.str( std::string() );
            }

            ser.finish();
            tos.flush();

            std::cerr << std::endl << output.str() << std::endl;
            //std::cerr << "---------------------\n" << std::endl;

            Pt::Date date1b;
            Pt::Date date2b;
            Pt::Date* dateptr1b = 0;
            std::stringstream input( output.str() );
            
            Pt::Xml::BinaryInputSource is(input);
            Pt::Xml::XmlReader reader(is);
            Pt::Xml::XmlDeserializer deser(reader);

            deser.begin(date1b);
            PT_UNIT_ASSERT( deser.advance() );

            deser.begin(date2b);
            PT_UNIT_ASSERT( deser.advance() );

            deser.begin(dateptr1b);
            PT_UNIT_ASSERT( deser.advance() );

            deser.fixup();

            //std::cerr << "date1b: " << date1b.toIsoString() << std::endl;
            //std::cerr << "date2b: " << date2b.toIsoString() << std::endl;
            PT_UNIT_ASSERT(date1a == date1b);
            PT_UNIT_ASSERT(date2a == date2b);
            PT_UNIT_ASSERT(dateptr1b == &date1b);
            }
            catch(const Pt::Xml::SyntaxError& ex)
            {
                std::cerr << ex.what() << " " << ex.line() << std::endl;
                std::exit(1);
                throw;
            }
        }

        void DynamicObject();
};

Pt::Unit::RegisterTest<XmlSerializerTest> register_XmlSerializerTest;
