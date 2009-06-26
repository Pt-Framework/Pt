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

#include <string>
#include <sstream>


struct DateRef
{
    Pt::Date* date;
};


void operator >>=(const Pt::SerializationInfo& si, DateRef& dr)
{
    //std::cerr << "NEED FIXUP: " << (void*)(&dr.date) << std::endl;
    si.getMember("date").getReference(dr.date);
    //si.getReference("date", dr.date );
}


void operator <<=(Pt::SerializationInfo& si, const DateRef& dr)
{
    si.addMember("date").setReference(dr.date);
    si.setTypeName("DateRef");
    //si.addReference("date", dr.date);
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

            DateRef dr;
            dr.date = &date1;
            //std::cerr << "PTR: "<< &(*(dr.date)) << " - " << &date1 << std::endl;

            std::stringstream output;
            Pt::Xml::XmlSerializer ser(output);
            ser.serialize(date1, "date1");
            ser.serialize(dr, "dref");

            ser.finish();
            ser.flush();

            Pt::Date date2(1, 1, 1);
            dr.date = 0;

            std::cerr << "\n--------------------" << std::endl;
            std::cerr << output.str();
            std::cerr << "---------------------\n" << std::endl;

            std::stringstream input( output.str() );
            Pt::Xml::XmlDeserializer deser(input);
            deser.deserialize(date2);
            deser.deserialize(dr);

            deser.link();
            //std::cerr << "FIXED POINTER: "<< dr.date << " - " << &date2 << std::endl;
            std::cerr << "RESULT: "<< dr.date->toIsoString() << std::endl;
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
