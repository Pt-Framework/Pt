/***************************************************************************
 *   Copyright (C) 2005-2007 by Marc Boris Duerner                         *
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
#undef PT_XML_API_EXPORT

#include "Pt/Xml/XmlSerializer.h"
#include "Pt/Xml/XmlReader.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/DateTime.h"

#include <string>
#include <sstream>


class XmlSerializerTest: public Pt::Unit::TestSuite
{
    public:
        XmlSerializerTest()
        : Pt::Unit::TestSuite("XmlSerializerTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Date", *this, &XmlSerializerTest::Date );
            Pt::Unit::TestSuite::registerMethod( "DateTime", *this, &XmlSerializerTest::DateTime );
        }

    protected:
        void Date()
        {
            Pt::Date date(1,1,1);
            //Pt::Xml::XmlSerializer ser;

            std::stringstream input;
            input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
            input << "<date>";
            input << "    <year>2007</year>";
            input << "    <month>7</month>";
            input << "    <day>5</day>";
            input << "</date>";

            Pt::Xml::XmlReader reader(input);
            Pt::Xml::XmlDeserializer deser(reader);
            Pt::ObjectData data;
            deser.deserialize(data);
            
            const Pt::SerializationData* subdata = data.getData(L"date");
            *subdata >> date;
        }

        void DateTime()
        {
            Pt::DateTime datetime;
            //Pt::Xml::XmlSerializer ser;

            std::stringstream input;
            input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
            input << "<dateTime>";
            input << "    <date>";
            input << "        <year>2007</year>";
            input << "        <month>7</month>";
            input << "        <day>5</day>";
            input << "    </date>";           
            input << "    <time>";
            input << "        <hour>20</hour>";
            input << "        <minute>7</minute>";
            input << "        <second>5</second>";
            input << "        <millisec>1</millisec>";
            input << "    </time>"; 
            input << "</dateTime>";

            Pt::Xml::XmlReader reader(input);
            Pt::Xml::XmlDeserializer deser(reader);
            Pt::ObjectData data;
            deser.deserialize(data);
            
            const Pt::SerializationData* subdata = data.getData(L"dateTime");
            *subdata >> datetime;
            std::cout<< datetime.toIsoString() <<std::endl;
        }
};

Pt::Unit::RegisterTest<XmlSerializerTest> register_XmlSerializerTest;

