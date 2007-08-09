/***************************************************************************
 *   Copyright (C) 2006 by Tommi Maekitalo                                  *
 *   Copyright (C) 2006 by Marc Boris Duerner                               *
 *   Copyright (C) 2006 by Stefan Bueder                                    *
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

#include "Pt/SerializationInfo.h"
#include "Pt/Date.h"
#include "Pt/Time.h"
#include "Pt/DateTime.h"
#include "Pt/System/Clock.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>


class Serializer
{
    public:
        Serializer()
        {}

        template <typename T>
        void serialize(const T& type, const std::string& name)
        {
            Pt::SerializationInfo& si = _root.addMember(name);
            si <<= type;
            this->addObject(&type, si);
        }

        void addObject(const void* ref, Pt::SerializationInfo& si)
        {
            _ids[ref] = &si;
        }

        void fixup(const Pt::SerializationInfo& si)
        {
            Pt::SerializationInfo::ConstIterator it;
            for(it = si.begin(); it != si.end(); ++it)
            {
                if(it->category() == Pt::SerializationInfo::Reference)
                {
                    void* p = it->toValue<void*>();
                    Pt::SerializationInfo* pointee = _ids[p];

                    std::stringstream id;
                    id << pointee;
                    pointee->setId( id.str() );
                }

                if(it->category() == Pt::SerializationInfo::Object)
                {
                    this->fixup(*it);
                }
            }
        }

        void fixup()
        {
            this->fixup(_root);
        }

    private:
        Pt::SerializationInfo _root;
        std::map<const void*, Pt::SerializationInfo*> _ids;
};


class Deserializer
{
    public:
        Deserializer()
        {}

        template <typename T>
        void deserialize(const Pt::SerializationInfo& si, T& type)
        {
            si >>= type;
            _types[ si.id() ] = &type;
            _fixups[ si.id() ] = &Deserializer::do_fixup<T>;
        }

        void fixup(const Pt::SerializationInfo& si)
        {
            Pt::SerializationInfo::ConstIterator it;
            for(it = si.begin(); it != si.end(); ++it)
            {
                if(it->category() == Pt::SerializationInfo::Reference)
                {
                    void* target = _types[ it->id() ];

                    void* d = it->toValue<void*>();
                    void** destination = (void**)d;
                    _fixups[ it->id() ]( destination, target);
                }

                if(it->category() == Pt::SerializationInfo::Object)
                {
                    this->fixup(*it);
                }
            }
        }

        template <typename T>
        static void do_fixup(void** ref , void* val)
        {
            *( (T**)(ref) ) = (T*)(val);
        }

    private:
        std::map<std::string, void*> _types;

        typedef void (*Fixup)(void**, void*);
        std::map<std::string, Fixup> _fixups;
};


struct DateRef
{
    Pt::Date* date;
};



void operator >>=(const Pt::SerializationInfo& si, DateRef& dr)
{
    const Pt::SerializationInfo& dptr = si.getMember("date");
    dptr.resolve( dr.date );
}


void operator <<=(Pt::SerializationInfo& si, const DateRef& dr)
{
    Pt::SerializationInfo& d = si.addMember("date");
    d.setReference(dr.date);
}



class SerializationTest : public Pt::Unit::TestSuite
{
    public:
        SerializationTest()
        : Pt::Unit::TestSuite("SerializationTest")
        {
            Pt::Unit::TestSuite::registerMethod( "perf", *this, &SerializationTest::perf );
            Pt::Unit::TestSuite::registerMethod( "StdVector", *this, &SerializationTest::StdVector );
            Pt::Unit::TestSuite::registerMethod( "Date", *this, &SerializationTest::Date );
            Pt::Unit::TestSuite::registerMethod( "Time", *this, &SerializationTest::Time );
            Pt::Unit::TestSuite::registerMethod( "DateTime", *this, &SerializationTest::DateTime );
        }

    protected:
        void perf()
        {
            std::cerr << std::endl;
            Pt::Date date;
            std::cerr << "Address of date: " << &date << std::endl;

            DateRef dref;
            dref.date = &date;

            Serializer ser;
            ser.serialize(date, "myDate");
            ser.serialize(dref, "MyDateRef");
            ser.fixup();

            sleep(1);

            Pt::SerializationInfo si1;
            si1.setName("myDate");
            si1 <<= date;
            si1.setId("11223344");

            Pt::Date* pdate = &date;
            Pt::SerializationInfo si2;
            si2.setName("myDateRef");

            Pt::SerializationInfo& pd = si2.addMember("date");
            pd.setCategory( Pt::SerializationInfo::Reference );
            pd.setId("11223344");

            Deserializer dser;
            dser.deserialize(si1, date);
            dser.deserialize(si2, dref);
            dser.fixup(si2);

            std::cerr << "dref.date is: " << dref.date << std::endl;
            /*
            Pt::System::Clock c;
            c.start();

            Pt::System::TimeValue tv = c.stop();
            std::cerr << "Duration: " << tv.seconds() << ":" << tv.microSeconds()/1000 << std::endl;
            */
        }

        void StdVector()
        {
            std::vector<int> iv, iv2;
            iv.push_back(1);
            iv.push_back(2);
            iv.push_back(3);

            Pt::SerializationInfo si;
            si <<= iv;

            /*for(Pt::SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
            {
                std::cerr << "elem: " << it->getValue<std::string>() << std::endl;
            }*/
            si >>= iv2;
/*
            Pt::ObjectManager manager;

            Pt::SerializationInfo info;
            Pt::SerializationInfo& object = info.addValue("object", 5);
            info.addValue("pointerToObject", "object");

            // read from source and fill object map
            manager._objects["object"] =  &object;

            int n = info.getValue<int>("object");
            manager._objects["object"] =  &n;

            std::string id = info.getValue<std::string>("pointerToObject");
            int* px = 0;
            manager._pointers[(void**)&px] = id;

            // read done, now fixup
            std::map<void**, std::string>::iterator it;
            for(it = manager._pointers.begin(); it != manager._pointers.end(); ++it)
            {
                std::cerr << "\nfixing up to " << it->second << std::endl;
                void* object = manager._objects[ it->second ];
                void** p = it->first;
                int** i = (int**)p;
                std::cerr << "NEW VALUE: " << *( (int*)(object) ) << std::endl;
                *i = &(*( (int*)(object) ));
            }

            std::cerr << "FIXED: " << px << " " << *px << std::endl;*/
        }

        void Date()
        {
            Pt::Date date(2000, 10, 20);
            Pt::SerializationInfo si;
            si <<=  date;

            /*for(Pt::SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
            {
                std::cerr << "iter: " << it->name() <<  " " << it->getValue<std::string>() << std::endl;
            }*/

            Pt::Date date2(1,1,1);
            si >>= date2;

            PT_UNIT_ASSERT(date == date2);
        }

        void Time()
        {
            Pt::Time time(18, 40, 5, 1);
            Pt::SerializationInfo si;
            si <<= time;

            Pt::Time time2;
            si >>= time2;

            PT_UNIT_ASSERT(time == time2);
        }

        void DateTime()
        {
            Pt::DateTime datetime(2000, 10, 20, 18, 40, 5, 1);
            Pt::SerializationInfo si;
            si <<= datetime;

            Pt::DateTime datetime2;
            si >>= datetime2;

            const Pt::Date date = datetime.date();
            const Pt::Date date2 = datetime2.date();
            PT_UNIT_ASSERT(date == date2);

            const Pt::Time time = datetime.time();
            const Pt::Time time2 = datetime2.time();

            PT_UNIT_ASSERT(time == time2);
        }
};

Pt::Unit::RegisterTest<SerializationTest> register_SerializationTest;
