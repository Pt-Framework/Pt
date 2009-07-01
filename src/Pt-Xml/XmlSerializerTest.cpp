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
#include "Pt/DateTime.h"
#include "Pt/SmartPtr.h"

#include <string>
#include <sstream>


class DateRef
{
    public:
        DateRef(Pt::Date* date = 0)
        : _date(date)
        { }

        void setDate(Pt::Date* date)
        { _date = date; }

        Pt::Date* date() const
        { return _date; }

    private:
        Pt::Date* _date;
};


void FixupDateRef(void* fixme, const std::type_info& fixmeType,
                  void* target, const std::type_info& targetType)
{
    DateRef* from = static_cast< DateRef* >(fixme);
    Pt::Date* to   = static_cast< Pt::Date* >(target);
    from->setDate(to);
}


void operator >>=(const Pt::SerializationInfo& si, DateRef& dr)
{
    //std::cerr << "NEED FIXUP: " << (void*)(&dr.date) << std::endl;
    si.getReference(&dr, typeid(Pt::Date), FixupDateRef);
}


void operator <<=(Pt::SerializationInfo& si, const DateRef& dr)
{
    si <<= dr.date();
}

typedef Pt::SmartPtr<Pt::Date> DateSmartPtr;

namespace Pt {

void FixupSmartPtr(void* fixme, const std::type_info& fixmeType,
                   void* target, const std::type_info& targetType)
{
    std::cerr << "FixupSmartPtr: " << fixmeType.name() << " ==> " << targetType.name() << std::endl;
    DateSmartPtr* from = static_cast< DateSmartPtr* >(fixme);
    //DateSmartPtr* to   = static_cast< DateSmartPtr* >(target);

    Pt::Date* to   = static_cast< Pt::Date* >(target);
    //*from = *to;
}


void operator <<= (Pt::SerializationInfo& six, const DateSmartPtr& sp)
{
    six <<= Pt::id() <<= *sp; // only create id if not present
}


void operator >>=(const Pt::SerializationInfo& si, DateSmartPtr& sp)
{
    DateSmartPtr* sptr = &sp;

    if(si.category() == Pt::SerializationInfo::Reference)
    {
        si.getReference(sptr, typeid(DateSmartPtr), FixupSmartPtr);
    }
    else
    {
        sp = new Pt::Date;
        si  >>= Pt::id() >>= *sp;
    }
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
        }

    protected:
        void Reference()
        {
            Pt::Date date1(1889, 4, 20);

            const Pt::Date* dateptr = &date1;
            DateSmartPtr datesp( new Pt::Date(2000, 6, 25) );
            DateSmartPtr datesp2 = datesp;

            DateRef dr( &date1 );

            std::stringstream output;
            Pt::Xml::XmlSerializer ser(output);
            ser.serialize(date1, "date1");
            ser.serialize(dr, "dref");
            ser.serialize(dateptr, "dateptr");
            ser.serialize(datesp, "datesp");
            ser.serialize(datesp2, "datesp2");

            ser.finish();
            ser.flush();

            Pt::Date date2(1, 1, 1);
            dr.setDate(0);
            Pt::Date* dateptr2 = 0; // const ?
            DateSmartPtr datesp3;
            DateSmartPtr datesp4;

            std::cerr << "\n--------------------" << std::endl;
            std::cerr << output.str();
            std::cerr << "---------------------\n" << std::endl;

            std::stringstream input( output.str() );
            Pt::Xml::XmlDeserializer deser(input);
            deser.deserialize(date2);
            deser.deserialize(dr);
            deser.deserialize(dateptr2);
            deser.deserialize(datesp3);
            deser.deserialize(datesp4);

            deser.link();
            //std::cerr << "FIXED POINTER: "<< dr.date << " - " << &date2 << std::endl;
            std::cerr << "RESULT: "<< dr.date()->toIsoString() << std::endl;
            std::cerr << "RESULT: "<< dateptr2->toIsoString() << std::endl;
            std::cerr << "RESULT: "<< datesp3->toIsoString() << std::endl;
            //std::cerr << "RESULT: "<< datesp4->toIsoString() << std::endl;
            //std::cerr << "========================\n" << std::endl;

            PT_UNIT_ASSERT( date1 == date2);
        }

        void Object()
        {
            Pt::DateTime date1(1889, 4, 20, 1, 2, 3, 4);
            std::stringstream output;
            Pt::Xml::XmlSerializer ser(output);
            ser.serialize(date1, "date1");
            ser.finish();
            ser.flush();

            Pt::DateTime date2(1, 1, 1, 1, 1, 1, 1);
            std::stringstream input( output.str() );
            Pt::Xml::XmlDeserializer deser(input);
            deser.deserialize(date2);
            deser.link();

            PT_UNIT_ASSERT( date1 == date2);
        }
};

Pt::Unit::RegisterTest<XmlSerializerTest> register_XmlSerializerTest;
