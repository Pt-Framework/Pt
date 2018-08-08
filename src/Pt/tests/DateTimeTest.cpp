/*
 * Copyright (C) 2006 by Tommi Maekitalo
 * Copyright (C) 2006 by Marc Boris Duerner
 * Copyright (C) 2006 by Stefan Bueder
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
#undef PT_API_EXPORT

#include <Pt/DateTime.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <Pt/System/Clock.h>

#include <string>
#include <iostream>


class DateTimeTest : public Pt::Unit::TestSuite
{
    public:
        DateTimeTest()
        : Pt::Unit::TestSuite("DateTimeTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Assign", *this, &DateTimeTest::Assign );
            Pt::Unit::TestSuite::registerMethod( "Operators", *this, &DateTimeTest::Operators );
            Pt::Unit::TestSuite::registerMethod( "MsecsUnixEpoch", *this, &DateTimeTest::MsecsUnixEpoch );
            Pt::Unit::TestSuite::registerMethod( "Iso8601Convert", *this, &DateTimeTest::Iso8601Convert );
            Pt::Unit::TestSuite::registerMethod( "Iso8601ConvertOffset", *this, &DateTimeTest::Iso8601ConvertOffset );
        }

    protected:
        void Assign();
        void Operators();
        void MsecsUnixEpoch();
        void Iso8601Convert();
        void Iso8601ConvertOffset();
};


void DateTimeTest::Assign()
{
    Pt::DateTime dt(2001, 11, 15, 12, 45, 23, 956);
    PT_UNIT_ASSERT( dt.year()   == 2001 );
    PT_UNIT_ASSERT( dt.month()  == 11 );
    PT_UNIT_ASSERT( dt.day()    == 15 );
    PT_UNIT_ASSERT( dt.hour()   == 12 );
    PT_UNIT_ASSERT( dt.minute() == 45 );
    PT_UNIT_ASSERT( dt.second() == 23 );
    PT_UNIT_ASSERT( dt.msec()   == 956 );

    dt.set(1789, 5, 12, 23, 59, 59, 999);
    PT_UNIT_ASSERT( dt.year()   == 1789 );
    PT_UNIT_ASSERT( dt.month()  == 5 );
    PT_UNIT_ASSERT( dt.day()    == 12 );
    PT_UNIT_ASSERT( dt.hour()   == 23 );
    PT_UNIT_ASSERT( dt.minute() == 59 );
    PT_UNIT_ASSERT( dt.second() == 59 );
    PT_UNIT_ASSERT( dt.msec()   == 999 );
}


void DateTimeTest::Operators()
{
    Pt::DateTime dt(1998, 06, 27, 11, 45, 20, 800);
    Pt::Timespan timespan;

    timespan.set(4, 3, 6, 50, 300*1000); // d, h, min, sec, usec
    dt += timespan;
    PT_UNIT_ASSERT( dt.year()   == 1998 );
    PT_UNIT_ASSERT( dt.month()  == 07 );
    PT_UNIT_ASSERT( dt.day()    == 01 );
    PT_UNIT_ASSERT( dt.hour()   == 14 );
    PT_UNIT_ASSERT( dt.minute() == 52 );
    PT_UNIT_ASSERT( dt.second() == 11 );
    PT_UNIT_ASSERT( dt.msec()   == 100 );

    dt -= timespan;
    PT_UNIT_ASSERT( dt.year()   == 1998 );
    PT_UNIT_ASSERT( dt.month()  == 06 );
    PT_UNIT_ASSERT( dt.day()    == 27 );
    PT_UNIT_ASSERT( dt.hour()   == 11 );
    PT_UNIT_ASSERT( dt.minute() == 45 );
    PT_UNIT_ASSERT( dt.second() == 20 );
    PT_UNIT_ASSERT( dt.msec()   == 800 );

    timespan.set(-4, -3, -6, -50, -300*1000);
    dt -= timespan;
    PT_UNIT_ASSERT( dt.year()   == 1998 );
    PT_UNIT_ASSERT( dt.month()  == 07 );
    PT_UNIT_ASSERT( dt.day()    == 01 );
    PT_UNIT_ASSERT( dt.hour()   == 14 );
    PT_UNIT_ASSERT( dt.minute() == 52 );
    PT_UNIT_ASSERT( dt.second() == 11 );
    PT_UNIT_ASSERT( dt.msec()   == 100 );

    dt += timespan;
    PT_UNIT_ASSERT( dt.year()   == 1998 );
    PT_UNIT_ASSERT( dt.month()  == 06 );
    PT_UNIT_ASSERT( dt.day()    == 27 );
    PT_UNIT_ASSERT( dt.hour()   == 11 );
    PT_UNIT_ASSERT( dt.minute() == 45 );
    PT_UNIT_ASSERT( dt.second() == 20 );
    PT_UNIT_ASSERT( dt.msec()   == 800 );

    Pt::DateTime dt2(1998, 06, 26, 10, 44, 19, 500);
    Pt::int64_t msecsDiff = Pt::Time::MSecsPerDay + 
                            Pt::Time::MSecsPerHour + 
                            Pt::Time::MSecsPerMinute + 
                            Pt::Time::MSecsPerSecond + 300;

    Pt::Timespan ts(dt - dt2);
    PT_UNIT_ASSERT( ts.toMSecs() == msecsDiff );
    
    ts = dt2 - dt;
    PT_UNIT_ASSERT( ts.toMSecs() == -msecsDiff );
}


void DateTimeTest::MsecsUnixEpoch()
{
    //unsigned msecsSinceEpoch = 2000;
    //Pt::DateTime dt(1970, 1, 1);
    //Pt::Timespan ts(msecsSinceEpoch*1000);
    //dt += ts;

	   //Pt::DateTime dt = Pt::System::Clock::getLocalTime();
    //Pt::DateTime dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(1970, 8, 1, 12, 11, 25, 0);
    //PT_UNIT_ASSERT(18360685000LL == dt.msecsSinceEpoch());
    //dt = Pt::DateTime(1970, 1, 1, 0, 0, 0, 0);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(1983, 5, 11, 13, 39, 52, 762);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(2064, 1, 15, 2, 54, 33, 122);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(2099, 12, 31, 23, 59, 59, 999);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(2100, 1, 1, 0, 0, 0, 0);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(2400, 2, 29, 23, 59, 59, 999);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(2400, 3, 1, 0, 0, 0, 0);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(1968, 2, 29, 23, 59, 59, 999);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(1968, 3, 1, 0, 0, 0, 0);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(1969, 12, 31, 23, 59, 59, 999);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(1967, 4, 17, 23, 59, 59, 999);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(1967, 2, 17, 23, 59, 59, 999);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(1969, 1, 1, 0, 0, 0, 0);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(1967, 10, 1, 12, 11, 0, 0);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(1735, 10, 1, 12, 11, 0, 0);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(1969, 1, 1, 0, 0, 0, 1);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);
    //dt = Pt::DateTime(1735, 7, 1, 12, 11, 25, 0);
    //dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    //PT_UNIT_ASSERT(dt == dt2);

    /*dt = Pt::DateTime(1735, 1, 1, 12, 11, 25, 0);
    for(Pt::int64_t i = 0; i > dt.msecsSinceEpoch(); i-=3600)
    {
    std::cout<<Pt::DateTime(i).toIsoString() << std::endl;
    }*/
}


void DateTimeTest::Iso8601Convert()
{
    Pt::DateTime dt(2001, 11, 15, 12, 45, 23, 956);
    std::string isoString = dt.toIsoString();
    PT_UNIT_ASSERT_EQUAL( isoString, "2001-11-15T12:45:23.956" );

    dt = Pt::DateTime::fromIsoString("1789-05-12T23:59:59.999");
    PT_UNIT_ASSERT( dt.year() == 1789 );
    PT_UNIT_ASSERT( dt.month() == 5 );
    PT_UNIT_ASSERT( dt.day() == 12 );
    PT_UNIT_ASSERT( dt.hour() == 23 );
    PT_UNIT_ASSERT( dt.minute() == 59 );
    PT_UNIT_ASSERT( dt.second() == 59 );
    PT_UNIT_ASSERT( dt.msec() == 999 );
}


void DateTimeTest::Iso8601ConvertOffset()
{
    Pt::DateTime dt(2001, 11, 15, 12, 45, 23, 956);

    int utcOffset = 165;
    std::string isoString = dt.toIsoString(&utcOffset);
    PT_UNIT_ASSERT_EQUAL( isoString, "2001-11-15T12:45:23.956+02:45" );

    int utcOffset2 = 0;
    dt = Pt::DateTime::fromIsoString("1789-05-12T23:59:59.999+01:15", &utcOffset2);

    PT_UNIT_ASSERT( dt.year() == 1789 );
    PT_UNIT_ASSERT( dt.month() == 5 );
    PT_UNIT_ASSERT( dt.day() == 12 );
    PT_UNIT_ASSERT( dt.hour() == 23 );
    PT_UNIT_ASSERT( dt.minute() == 59 );
    PT_UNIT_ASSERT( dt.second() == 59 );
    PT_UNIT_ASSERT( dt.msec() == 999 );
    PT_UNIT_ASSERT_EQUAL( utcOffset2, 75 );
}

Pt::Unit::RegisterTest<DateTimeTest> register_DateTimeTest;
