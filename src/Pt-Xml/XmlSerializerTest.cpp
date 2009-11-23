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
#include "Pt/Xml/StartElement.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/TextStream.h"
#include "Pt/Utf8Codec.h"
#include "Pt/DateTime.h"
#include "Pt/SmartPtr.h"

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

// do fixup from Composer with ctx >>= T
// Derive BreakDownInfo from SerializationInfoy

void fixup(DateRef& fixme, void* target, const std::type_info& targetType)
{
    if( typeid(Pt::Date) != targetType )
    {
        throw Pt::SerializationError("type mismatch during pointer fixup");
    }

    Pt::Date* to = static_cast< Pt::Date* >(target);
    fixme.setDate(to);
}


void operator >>=(const Pt::SerializationInfo& si, DateRef& dr)
{
    si.getMember("date").loadReference(dr);

    int n = 0;
    si.getMember("n") >>= n;
    dr.setN(n);
}


void operator <<=(Pt::ISerializationInfo& si, const DateRef& dr)
{
    si.addMember("date") <<= dr.date();
    si.addMember("n") <<= dr.n();
}


void operator <<=(Pt::SerializationInfo& si, const DateRef& dr)
{
    si.addMember("date") <<= dr.date();
    si.addMember("n") <<= dr.n();
}


/*void breakdown(Pt::BreakDown& b, const DateRef& dr)
{
    std::cerr << "### breakdown DateRef" << dr.date() << std::endl;
    breakdown(b, dr.date());
}


void symbolize(Pt::BreakDown& b, const DateRef& dr, const char* name)
{
    std::cerr << "### symbolize DateRef " << dr.date() << std::endl;
    //b.save(dr, name);

    //b._ctx.beginSave(&dr, name);
    //b.prepareId( dr.date() );

    b._ctx <<= Pt::save() <<= dr;

    //b._ctx.finishSave();
    //std::cerr << "### DONE symbolize DateRef " << dr.date() << std::endl;
}*/

} // namespace test


namespace Pt {

typedef SmartPtr<Date> DateSmartPtr;
/*
void symbolize(Pt::BreakDown& b, const DateSmartPtr& sp, const char* name)
{
    if( ! sp.getPointer() || ! b.save( *sp, name ) )
    {
        b.prepareId( sp.getPointer() );
    }
}


void breakdown(Pt::BreakDown& b, const DateSmartPtr& sp)
{
    // if( ! sp.getPointer() || ! b.save( *sp ) )
    // {
    //     b.prepareId( sp.getPointer() );
    // }
}
*/

void operator <<=(Pt::ISaveInfo& si, const DateSmartPtr& sp)
{
    if( ! sp.getPointer() || ! si.save( *sp ) )
    {
        si.put( sp.getPointer() );
    }
}


void operator <<=(Pt::SaveInfo& si, const DateSmartPtr& sp)
{
    if( ! sp.getPointer() || ! si.save( *sp ) )
    {
        si.put( sp.getPointer() );
    }
}


inline void operator >>=(const LoadInfo& li, DateSmartPtr& sp)
{
    if(li.in().category() == Pt::SerializationInfo::Reference)
    {
        li.in().loadReference(sp);
    }
    else
    {
        li.load(sp);
    }
}


void fixup(DateSmartPtr& fixme, void* target, const std::type_info& targetType)
{
    if( target && targetType == typeid(DateSmartPtr) )
    {
        DateSmartPtr* to = static_cast< DateSmartPtr* >(target);
        fixme = *to;
    }
    else if(target == 0)
    {
        fixme = DateSmartPtr();
    }
    else
    {
        throw SerializationError("smart pointer fixup failed");
    }
}


void operator >>=(const Pt::SerializationInfo& si, DateSmartPtr& sp)
{
    sp = new Date();
    si >>= *sp;
}

}


class XmlSerializerTest: public Pt::Unit::TestSuite
{
    public:
        XmlSerializerTest()
        : Pt::Unit::TestSuite("XmlSerializerTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Reference", *this, &XmlSerializerTest::Reference );
            Pt::Unit::TestSuite::registerMethod( "Object", *this, &XmlSerializerTest::Object );
            //Pt::Unit::TestSuite::registerMethod( "AdvanceObject", *this, &XmlSerializerTest::AdvanceObject );
        }

        class IsoDateSurrogate : public Pt::SerializationSurrogate
        {
            public:
                virtual void pack(Pt::SerializationInfo& si) const
                {
                    int year = 0;
                    unsigned month = 0, day = 0;

                    si.getMember("year") >>= year;
                    si.getMember("month") >>= month;
                    si.getMember("day") >>= day;

                    Pt::Date date(year, month, day);
                    std::string s = date.toIsoString();
                    si.setValue(s);
                    si.setTypeName("date");
                }

                virtual void unpack(Pt::SerializationInfo& to, const Pt::SerializationInfo& from) const
                {
                    std::string isoString;
                    from >>= isoString;
                    Pt::Date date = Pt::Date::fromIsoString(isoString);
                    to.addMember("year") <<= date.year();
                    to.addMember("month") <<= date.month();
                    to.addMember("day") <<= date.day();
                }
        };

    protected:
        void Reference()
        {
            Pt::Date date1(1889, 4, 20);
            test::DateRef dr( &date1 );
            const Pt::Date* dateptr = &date1;
            Pt::DateSmartPtr datesp( new Pt::Date(2000, 6, 25) );
            Pt::DateSmartPtr datesp2 = datesp;
            Pt::DateSmartPtr dateNull;

            std::stringstream output;
            Pt::Xml::XmlSerializer ser(output);
            ser.context().addSurrogate("date", new IsoDateSurrogate);
            ser.serialize(date1, "date1");
            //ser.serialize(date1, "date_1b");
            ser.serialize(dr, "dref");

            ser.serialize(dateptr, "dateptr");
            ser.serialize(datesp, "datesp");
            ser.serialize(datesp2, "datesp2");
            ser.serialize(dateNull, "dateNull");

            ser.finish();
            ser.flush();

            Pt::Date date2(1, 1, 1);
            dr.setDate(0);
            Pt::Date* dateptr2 = 0; // const ?
            Pt::DateSmartPtr datesp3;
            Pt::DateSmartPtr datesp4;
            Pt::DateSmartPtr nullDate( new Pt::Date(1 ,1, 1) );

            std::cerr << "\n--------------------" << std::endl;
            std::cerr << output.str();
            std::cerr << "---------------------\n" << std::endl;

            std::stringstream input( output.str() );
            Pt::Xml::XmlDeserializer deser(input);
            deser.context().addSurrogate("date", new IsoDateSurrogate);
            deser.deserialize(date2);
            deser.deserialize(dr);
            deser.deserialize(dateptr2);
            deser.deserialize(datesp3);
            deser.deserialize(datesp4);
            deser.deserialize(nullDate);

            deser.finish();
            //std::cerr << "FIXED POINTER: "<< dr.date << " - " << &date2 << std::endl;
            std::cerr << "RESULT: "<< dr.date()->toIsoString() << std::endl;
            /*std::cerr << "RESULT: "<< dateptr2->toIsoString() << std::endl;
            std::cerr << "RESULT: "<< datesp3->toIsoString() << std::endl;
            std::cerr << "RESULT: "<< datesp4->toIsoString() << std::endl;*/
            //std::cerr << "========================\n" << std::endl;

            PT_UNIT_ASSERT( date1 == date2);
            PT_UNIT_ASSERT( datesp3.getPointer() == datesp4.getPointer() );
            PT_UNIT_ASSERT( nullDate.getPointer() == 0);
        }

        static void dateToIso(Pt::SerializationInfo& si, Pt::Formatter& formatter)
        {
            int year = 0;
            unsigned month = 0;
            unsigned day = 0;

            si.getMember("year") >>= year;
            si.getMember("month") >>= month;
            si.getMember("day") >>= day;

            Pt::Date date(year, month, day);
            std::string s = date.toIsoString();
            formatter.addValue( si.name(), si.typeName(), Pt::String::widen(s), si.id() );
        }

        static void isoToDate(Pt::SerializationInfo& si, Pt::Deserializer& deser)
        {
            std::string isoString;
            si >>= isoString;
            Pt::Date date = Pt::Date::fromIsoString(isoString);
            si.addMember("year") <<= date.year();
            si.addMember("month") <<= date.month();
            si.addMember("day") <<= date.day();
        }

        void Object()
        {
            Pt::DateTime date1(1889, 4, 20, 1, 2, 3, 4);
            Pt::Date date_2(2000, 4,18);
            std::stringstream output;
            Pt::Xml::XmlSerializer ser(output);
            //ser.setFormatRule("Pt::Date", &XmlSerializerTest::dateToIso);
            ser.context().addSurrogate("date", new IsoDateSurrogate);
            ser.serialize(date1, "date1");
            ser.serialize(date_2, "date2");
            ser.finish();
            ser.flush();

            std::cerr << "\n--------------------" << std::endl;
            std::cerr << output.str();
            std::cerr << "---------------------\n" << std::endl;

            Pt::DateTime date2(1, 1, 1, 1, 1, 1, 1);
            Pt::Date date3(1800, 7, 6);
            std::stringstream input( output.str() );
            Pt::TextIStream tis(input, new Pt::Utf8Codec);
            Pt::Xml::XmlReader reader(tis);
            Pt::Xml::XmlDeserializer deser(reader);
            //deser.setLoadRule("Pt::Date", &XmlSerializerTest::isoToDate);
            deser.context().addSurrogate("date", new IsoDateSurrogate);
            deser.deserialize(date2);
            deser.deserialize(date3);
            deser.finish();
            std::cerr << "Date3: " << date3.toIsoString() << std::endl;
            // std::cerr << "IMPORT: " << tis.buffer().import() << std::endl;
            // std::cerr << "AVAIL: " << tis.buffer().in_avail() << std::endl;
            // Pt::Composer<Pt::DateTime> des;
            // des.begin(date2);
            // Pt::IComposer* d = deser.advance(&des);
            // std::cerr << "D (null): " << d << std::endl;
            // std::cerr << "DATE: " << date2.toIsoString() << std::endl;
            // deser.finish();

            //PT_UNIT_ASSERT( date1 == date2);
        }

        void AdvanceObject()
        {
            Pt::Date date_2(2000, 4,18);
            std::stringstream output;
            Pt::Xml::XmlSerializer ser(output);

            ser.serialize(date_2, "dateA");

            std::cerr << "\n--------------------" << std::endl;
            ser.beginFormat();
            while( ser.advance() )
            {
                ser.flush();
                std::cerr << output.str() << "- ADVANCE -" << std::endl;
                output.str( std::string() );
            }

            ser.finish();
            ser.flush();

            std::cerr << output.str();
            std::cerr << "---------------------\n" << std::endl;
        }
};

Pt::Unit::RegisterTest<XmlSerializerTest> register_XmlSerializerTest;
