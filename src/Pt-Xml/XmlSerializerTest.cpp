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
#include "Pt/Reflex/Reflectable.h"
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
    si.setValue( Pt::String::widen( date.toIsoString() ) );
}


void fromXmlString(const Pt::SerializationInfo& si, Pt::Date& date)
{
    Pt::String str;
    si.getValue(str);
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
            Pt::Unit::TestSuite::registerMethod( "Object", *this, &XmlSerializerTest::Object );
            Pt::Unit::TestSuite::registerMethod( "AdvanceObject", *this, &XmlSerializerTest::AdvanceObject );
            Pt::Unit::TestSuite::registerMethod( "DynamicObject", *this, &XmlSerializerTest::DynamicObject );
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
            Pt::Xml::XmlSerializer ser(output);
            ser.context()->registerSurrogate("date", &fromXmlString, &toXmlString);

            ser.serialize(date1, "date1");
            ser.serialize(dr, "dr");
            ser.serialize(dateptr, "dateptr");
            ser.serialize(datesp, "datesp");
            ser.serialize(datesp2, "datesp2");
            ser.serialize(dateNull, "dateNull");

            ser.finish();
            ser.flush();

            Pt::Date date2(1, 1, 1);
            dr.setDate(0);
            Pt::Date* dateptr2 = 0; // const ?
            DateSmartPtr datesp3;
            DateSmartPtr datesp4;
            DateSmartPtr nullDate( new Pt::Date(1 ,1, 1) );

            std::cerr << "\n--------------------" << std::endl;
            std::cerr << output.str();
            std::cerr << "---------------------\n" << std::endl;

            std::stringstream input( output.str() );
            Pt::Xml::XmlDeserializer deser(input);
            deser.context()->registerSurrogate("date", &fromXmlString, &toXmlString);

            deser.deserialize(date2);
            deser.deserialize(dr);
            deser.deserialize(dateptr2);
            deser.deserialize(datesp3);
            deser.deserialize(datesp4);
            deser.deserialize(nullDate);

            deser.finish();
            //std::cerr << "FIXED POINTER: "<< dr.date << " - " << &date2 << std::endl;
            //std::cerr << "RESULT: "<< dr.date()->toIsoString() << std::endl;
            /*std::cerr << "RESULT: "<< dateptr2->toIsoString() << std::endl;
            std::cerr << "RESULT: "<< datesp3->toIsoString() << std::endl;
            std::cerr << "RESULT: "<< datesp4->toIsoString() << std::endl;*/
            //std::cerr << "========================\n" << std::endl;

            PT_UNIT_ASSERT( date1 == date2);
            PT_UNIT_ASSERT( datesp3.getPointer() == datesp4.getPointer() );
            PT_UNIT_ASSERT( nullDate.getPointer() == 0);
        }

        void MultiSet()
        {
            std::multiset<Pt::Date> dates;
            dates.insert( Pt::Date(2000, 4,18) );
            dates.insert( Pt::Date(2000, 4,17) );
            dates.insert( Pt::Date(2000, 4,19) );
            const Pt::Date* dateptr = &(*dates.begin() );

            std::stringstream output;
            Pt::Xml::XmlSerializer ser(output);

            ser.serialize(dates, "dates");
            ser.serialize(dateptr, "dateptr");

            ser.finish();
            ser.flush();

            std::cerr << "\n--------------------" << std::endl;
            std::cerr << output.str();
            std::cerr << "---------------------\n" << std::endl;

            dates.clear();
            dateptr = 0;

            std::stringstream input( output.str() );
            Pt::TextIStream tis(input, new Pt::Utf8Codec);

            Pt::Xml::XmlReader reader(tis);
            Pt::Xml::XmlDeserializer deser(reader);

            deser.deserialize(dates);
            deser.deserialize(dateptr);
            deser.finish();

            PT_UNIT_ASSERT( dateptr );
            PT_UNIT_ASSERT( dates.size() == 3 );
            PT_UNIT_ASSERT( dateptr == &(*dates.begin()) );
            std::cerr << "dateptr:" << dateptr->toIsoString() << std::endl;
        }

        void Object()
        {
            Pt::DateTime date1(1889, 4, 20, 1, 2, 3, 4);
            Pt::Date date2(2000, 4,18);

            std::stringstream output;
            Pt::Xml::XmlSerializer ser(output);
            ser.context()->enableReferencing(false);

            ser.serialize(date1, "date1a");
            ser.serialize(date2, "date2a");
            ser.serialize(date1, "date1b");
            ser.serialize(date2, "date2b");

            ser.finish();
            ser.flush();

            std::cerr << "\n--------------------" << std::endl;
            std::cerr << output.str();
            std::cerr << "---------------------\n" << std::endl;

            Pt::DateTime date3(1, 1, 1, 1, 1, 1, 1);
            Pt::Date date4(1800, 7, 6);

            std::stringstream input( output.str() );
            Pt::TextIStream tis(input, new Pt::Utf8Codec);

            Pt::Xml::XmlReader reader(tis);
            Pt::Xml::XmlDeserializer deser(reader);
            deser.context()->enableReferencing(false);

            deser.deserialize(date3);
            deser.deserialize(date4);
            deser.finish();

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
            Pt::Date date1a(1889, 4,20);
            Pt::Date date2a(1945, 4,29);
            Pt::Date* dateptr1a = &date1a;

            std::stringstream output;
            Pt::Xml::XmlSerializer ser(output);

            ser.serialize(date1a, "date1");
            ser.serialize(date2a, "date2");
            ser.serialize(dateptr1a, "dateptr1");

            //std::cerr << "\n--------------------" << std::endl;
            ser.begin();
            while( ser.advance() )
            {
                //ser.flush();
                //std::cerr << output.str() << "- ADVANCE -" << std::endl;
                //output.str( std::string() );
            }

            ser.finish();
            ser.flush();

            //std::cerr << output.str();
            //std::cerr << "---------------------\n" << std::endl;

            Pt::Date date1b;
            Pt::Date date2b;
            Pt::Date* dateptr1b = 0;
            std::stringstream input( output.str() );
            Pt::TextIStream tis(input, new Pt::Utf8Codec);

            tis.buffer().import();
            Pt::Xml::XmlReader reader(tis);
            Pt::Xml::XmlDeserializer deser(reader);

            deser.begin(date1b);
            while( tis.buffer().in_avail() && deser.advance() )
            { }

            tis.buffer().import();
            deser.begin(date2b);
            while( tis.buffer().in_avail()  && deser.advance() )
            { }

            tis.buffer().import();
            deser.begin(dateptr1b);
            while( tis.buffer().in_avail()  && deser.advance() )
            { }

            deser.finish();

            //std::cerr << "date1b: " << date1b.toIsoString() << std::endl;
            //std::cerr << "date2b: " << date2b.toIsoString() << std::endl;
            PT_UNIT_ASSERT(date1a == date1b);
            PT_UNIT_ASSERT(date2a == date2b);
            PT_UNIT_ASSERT(dateptr1b == &date1b);
        }

        void DynamicObject();
};

Pt::Unit::RegisterTest<XmlSerializerTest> register_XmlSerializerTest;

// class Object
// {
//     public:
//         Object()
//         {}

//         virtual ~Object()
//         {}

//         void setProperty(const char* name, const Pt::Any& value)
//         { _properties[name] = value; }

//     private:
//         std::map<std::string, Pt::Any> _properties;
// };

// class Port : public Object
// {
//     public:
//         Port()
//         {}

//         virtual ~Port()
//         {}
// };

// class PortList : public Object
// {
//     public:
//         PortList()
//         {}

//         virtual ~PortList()
//         {}

//     private:
// };

// class Runtime : public Object
// {
//     public:
//         Runtime()
//         {}

//         ~Runtime()
//         {}

//         static Object* createObject(const std::string& typeName)
//         {
//             if(typeName == "Port")
//                 return new Port();
//             if(typeName == "PortList")
//                 return new PortList();

//             return 0;
//         }
// };


// void operator >>=(const Pt::SerializationInfo& si, Object& rt)
// {
//     Pt::SerializationInfo::ConstIterator it;
//     for(it = si.begin(); it != si.end(); ++it)
//     {
//         Object* obj = Runtime::createObject( it->typeName().c_str() );
//         Pt::SmartPtr<Object> ptr(obj);
//         rt.setProperty(it->name().c_str(), Pt::Any(ptr));

//         *it >> Pt::load() >>= ptr; // not ptr, but property !!
//     }
// }

namespace Pt {

namespace Reflex {

void operator >>=(const Pt::SerializationInfo& si, Pt::SmartPtr<Pt::Reflex::Reflectable>& sp)
{
    std::cerr << "DESERIALIZE SMARTPTR<REFLECTABLE> BEGIN " << si.typeName() << std::endl;
    sp = new Reflectable("xxx");
    si >>= *sp;
    std::cerr << "DESERIALIZE SMARTPTR<REFLECTABLE> END" << std::endl;
}

}

}

class MyObject : public Pt::Reflex::Reflectable
{
    public:
        MyObject(const std::string& name)
        : Pt::Reflex::Reflectable(name)
        {
            this->registerProperty("child", *this, _child, &MyObject::setChild);
        }

        void setChild(const Pt::SmartPtr<Pt::Reflex::Reflectable>& child)
        { _child.set(child); }

        const Pt::SmartPtr<Pt::Reflex::Reflectable>& child() const
        { return _child.get(); }

    private:
        Pt::Reflex::PropertyValue<
            Pt::SmartPtr<
                Pt::Reflex::Reflectable> > _child;
};


void XmlSerializerTest::DynamicObject()
{
    std::string data = "<refl1 type=\"Port\"id=\"0\">\n"
                       "    <name>myPort2</name>\n"
                       "</refl1>\n"
                       "<myObj>\n"
                       "    <child ref=\"0\"></child>\n"
                       "</myObj>\n";

    std::string data2 = "<runtime>\n"
                       "  <portList1 type=\"PortList\">\n"
                       "    <port1 type=\"Port\"id=\"0\">\n"
                       "      <name>myPort2</name>\n"
                       "    </port1>\n"
                       "    <port2 type=\"Port\"id=\"1\">\n"
                       "      <name>myPort2</name>\n"
                       "    </port2>\n"
                       "  </portList1>\n"
                       "  <connection1 type=\"Connection\">\n"
                       "    <from ref=\"0\"></from>\n"
                       "    <to ref=\"1\"></to>\n"
                       "  </connection1>\n"
                       "</runtime>\n";

    std::stringstream input( data );
    Pt::TextIStream tis(input, new Pt::Utf8Codec);

    Pt::Xml::XmlReader reader(tis);
    Pt::Xml::XmlDeserializer deser(reader);
    deser.context()->enableReferencing(true);

    Pt::SmartPtr<Pt::Reflex::Reflectable> refl1;
    MyObject refl2("myObj");

    deser.deserialize(refl1);
    deser.deserialize(refl2);
    deser.finish();

    std::cerr << "refl1: " << refl1.getPointer() << std::endl;
    std::cerr << "refl1: " << refl1.getPointer()->objectName() << std::endl;
    std::cerr << "CHILD: " << refl2.child().getPointer() << std::endl;
    std::cerr << "CHILD: " << refl2.child().getPointer()->objectName() << std::endl;
}
