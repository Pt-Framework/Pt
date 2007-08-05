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


            Pt::System::Clock c;
            c.start();

                std::stringstream ss("         hallo         1234567890         ");
                std::string token;

                Pt::SerializationInfo si1;
                Pt::SerializationInfo si2;
                ss >> token;
                si1.setValue(token);

                ss >> token;
                si2.setValue(token);
                Pt::Any a1 = std::string("hallo");
                Pt::Any a2 = 5;
            for(int n = 0; n < 100000; ++n)
            {


                //std::string s = si1.toValue<std::string>();
                //int n = si2.toValue<int>();


                //const std::string& sx = Pt::any_cast<std::string>(aa2);
                //int nx = Pt::any_cast<int>(aa1);
                //int mx = Pt::any_cast<int>(aa1);
                //Pt::TypeFactory::instance().assertBuilder<std::string>();
                //Pt::TypeFactory::instance().assertBuilder<int>();

                a1 = Pt::TypeFactory::instance().create(si1, "std::string");
                a2 = Pt::TypeFactory::instance().create(si2, "int");
                const std::string& s = Pt::any_cast<const std::string&>(a1);
                int n = Pt::any_cast<int>(a2);
            }

            Pt::System::TimeValue tv = c.stop();
            std::cerr << "Duration: " << tv.seconds() << ":" << tv.microSeconds()/1000 << std::endl;
        }
        
        void StdVector()
        {
            std::vector<int> iv, iv2;
            iv.push_back(1);
            iv.push_back(2);
            iv.push_back(3);

            Pt::SerializationInfo si;
            put(si, iv);

            /*for(Pt::SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
            {
                std::cerr << "elem: " << it->getValue<std::string>() << std::endl;
            }*/
            get(si, iv2);
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
            put(si, date);

            /*for(Pt::SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
            {
                std::cerr << "iter: " << it->name() <<  " " << it->getValue<std::string>() << std::endl;
            }*/

            Pt::Date date2(1,1,1);
            get(si, date2);

            PT_UNIT_ASSERT(date == date2);
        }

        void Time()
        {
            Pt::Time time(18, 40, 5, 1);
            Pt::SerializationInfo si;
            put(si, time);

            Pt::Time time2;
            get( si, time2 );

            PT_UNIT_ASSERT(time == time2);
        }

        void DateTime()
        {
            Pt::DateTime datetime(2000, 10, 20, 18, 40, 5, 1);
            Pt::SerializationInfo si;
            put(si, datetime);

            Pt::DateTime datetime2;
            get( si, datetime2 );

            const Pt::Date date = datetime.date();
            const Pt::Date date2 = datetime2.date();
            PT_UNIT_ASSERT(date == date2);

            const Pt::Time time = datetime.time();
            const Pt::Time time2 = datetime2.time();

            PT_UNIT_ASSERT(time == time2);
        }
};

Pt::Unit::RegisterTest<SerializationTest> register_SerializationTest;
