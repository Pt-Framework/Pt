/***************************************************************************
 *   Copyright (C) 2006 by Tommi Maekitalo                                 *
 *   Copyright (C) 2006 by Marc Boris Duerner                              *
 *   Copyright (C) 2006 by Stefan Bueder                                   *
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
            Pt::Unit::TestSuite::registerMethod( "testAssign", *this, &DateTimeTest::testAssign );
            Pt::Unit::TestSuite::registerMethod( "testOperators", *this, &DateTimeTest::testOperators );
            Pt::Unit::TestSuite::registerMethod( "testMsecsUnixEpoch", *this, &DateTimeTest::testMsecsUnixEpoch );
            Pt::Unit::TestSuite::registerMethod( "testIsoConvert", *this, &DateTimeTest::testIsoConvert );
        }

    protected:
        void testAssign();
        void testOperators();
        void testMsecsUnixEpoch();
        void testIsoConvert();
};


void DateTimeTest::testAssign()
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


void DateTimeTest::testOperators()
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

}


void DateTimeTest::testMsecsUnixEpoch()
{
	   Pt::DateTime dt = Pt::System::Clock::getCurrentTime();
    Pt::DateTime dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(1970, 8, 1, 12, 11, 25, 0);
    PT_UNIT_ASSERT(18360685000LL == dt.msecsSinceEpoch());
    dt = Pt::DateTime(1970, 1, 1, 0, 0, 0, 0);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(1983, 5, 11, 13, 39, 52, 762);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(2064, 1, 15, 2, 54, 33, 122);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(2099, 12, 31, 23, 59, 59, 999);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(2100, 1, 1, 0, 0, 0, 0);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(2400, 2, 29, 23, 59, 59, 999);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(2400, 3, 1, 0, 0, 0, 0);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(1968, 2, 29, 23, 59, 59, 999);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(1968, 3, 1, 0, 0, 0, 0);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(1969, 12, 31, 23, 59, 59, 999);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(1967, 4, 17, 23, 59, 59, 999);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(1967, 2, 17, 23, 59, 59, 999);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(1969, 1, 1, 0, 0, 0, 0);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(1967, 10, 1, 12, 11, 0, 0);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(1735, 10, 1, 12, 11, 0, 0);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(1969, 1, 1, 0, 0, 0, 1);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);
    dt = Pt::DateTime(1735, 7, 1, 12, 11, 25, 0);
    dt2 = Pt::DateTime::fromMSecsSinceEpoch(dt.msecsSinceEpoch());
    PT_UNIT_ASSERT(dt == dt2);

    /*dt = Pt::DateTime(1735, 1, 1, 12, 11, 25, 0);
    for(Pt::int64_t i = 0; i > dt.msecsSinceEpoch(); i-=3600)
    {
    std::cout<<Pt::DateTime(i).toIsoString() << std::endl;
    }*/
}


void DateTimeTest::testIsoConvert()
{
    Pt::DateTime dt(2001, 11, 15, 12, 45, 23, 956);
    std::string isoString = dt.toIsoString();
    PT_UNIT_ASSERT( isoString == "2001-11-15 12:45:23.956" );

    dt = Pt::DateTime::fromIsoString("1789-05-12 23:59:59.999");
    PT_UNIT_ASSERT( dt.year() == 1789 );
    PT_UNIT_ASSERT( dt.month() == 5 );
    PT_UNIT_ASSERT( dt.day() == 12 );
    PT_UNIT_ASSERT( dt.hour() == 23 );
    PT_UNIT_ASSERT( dt.minute() == 59 );
    PT_UNIT_ASSERT( dt.second() == 59 );
    PT_UNIT_ASSERT( dt.msec() == 999 );
}

Pt::Unit::RegisterTest<DateTimeTest> register_DateTimeTest;
