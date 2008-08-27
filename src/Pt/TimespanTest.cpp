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

#include "Pt/Timespan.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>


class TimeSpanTest : public Pt::Unit::TestSuite
{
public:
    TimeSpanTest()
    : Pt::Unit::TestSuite("TimeSpanTest")
    {
        Pt::Unit::TestSuite::registerMethod("testConstructors", *this, &TimeSpanTest::testConstructors);
        Pt::Unit::TestSuite::registerMethod("testEmptyTimespan", *this, &TimeSpanTest::testEmptyTimespan);
        Pt::Unit::TestSuite::registerMethod("testGetters", *this, &TimeSpanTest::testGetters);
        Pt::Unit::TestSuite::registerMethod("testComparisonOperators", *this, &TimeSpanTest::testComparisonOperators);
        Pt::Unit::TestSuite::registerMethod("testOperators", *this, &TimeSpanTest::testOperators);
        Pt::Unit::TestSuite::registerMethod("testSwap", *this, &TimeSpanTest::testSwap);
    }

protected:
    void testConstructors();
    void testEmptyTimespan();
    void testGetters();
    void testComparisonOperators();
    void testOperators();
    void testSwap();
};

Pt::Unit::RegisterTest<TimeSpanTest> register_TimeSpanTest;


void TimeSpanTest::testConstructors()
{
    {
        Pt::Timespan ts((((155 * 24 + 13) * 60 + 34) * 60 + 11) * Pt::int64_t(1000000) + 567891);
        PT_UNIT_ASSERT(ts.days() == 155);
        PT_UNIT_ASSERT(ts.hours() == 13);
        PT_UNIT_ASSERT(ts.minutes() == 34);
        PT_UNIT_ASSERT(ts.seconds() == 11);
        PT_UNIT_ASSERT(ts.useconds() == 567891);
    }

    {
        Pt::Timespan ts(((155 * 24 + 13) * 60 + 34) * 60 + 11, 567891);
        PT_UNIT_ASSERT(ts.days() == 155);
        PT_UNIT_ASSERT(ts.hours() == 13);
        PT_UNIT_ASSERT(ts.minutes() == 34);
        PT_UNIT_ASSERT(ts.seconds() == 11);
        PT_UNIT_ASSERT(ts.useconds() == 567891);
    }

    {
        Pt::Timespan ts(155, 13, 34, 11, 567891);
        PT_UNIT_ASSERT(ts.days() == 155);
        PT_UNIT_ASSERT(ts.hours() == 13);
        PT_UNIT_ASSERT(ts.minutes() == 34);
        PT_UNIT_ASSERT(ts.seconds() == 11);
        PT_UNIT_ASSERT(ts.useconds() == 567891);
    }
    
    {
        Pt::Timespan ts;
        ts.set(((155 * 24 + 13) * 60 + 34) * 60 + 11, 567891);
        PT_UNIT_ASSERT(ts.days() == 155);
        PT_UNIT_ASSERT(ts.hours() == 13);
        PT_UNIT_ASSERT(ts.minutes() == 34);
        PT_UNIT_ASSERT(ts.seconds() == 11);
        PT_UNIT_ASSERT(ts.useconds() == 567891);
    }

    {
        Pt::Timespan ts;
        ts.set(155, 13, 34, 11, 567891);
        PT_UNIT_ASSERT(ts.days() == 155);
        PT_UNIT_ASSERT(ts.hours() == 13);
        PT_UNIT_ASSERT(ts.minutes() == 34);
        PT_UNIT_ASSERT(ts.seconds() == 11);
        PT_UNIT_ASSERT(ts.useconds() == 567891);
    }
}

void TimeSpanTest::testEmptyTimespan()
{
    Pt::Timespan ts;
    
    PT_UNIT_ASSERT(ts.days() == 0);
    PT_UNIT_ASSERT(ts.hours() == 0);
    PT_UNIT_ASSERT(ts.totalHours() == 0);
    PT_UNIT_ASSERT(ts.minutes() == 0);
    PT_UNIT_ASSERT(ts.totalMinutes() == 0);
    PT_UNIT_ASSERT(ts.seconds() == 0);
    PT_UNIT_ASSERT(ts.totalSeconds() == 0);
    PT_UNIT_ASSERT(ts.msecs() == 0);
    PT_UNIT_ASSERT(ts.totalMSecs() == 0);
    PT_UNIT_ASSERT(ts.microseconds() == 0);
    PT_UNIT_ASSERT(ts.useconds() == 0);
    PT_UNIT_ASSERT(ts.toUSecs() == 0);
}


void TimeSpanTest::testGetters()
{
    Pt::Timespan ts(12, 23, 34, 45, 999999);

    PT_UNIT_ASSERT(ts.days() == 12);
    PT_UNIT_ASSERT(ts.hours() == 23);
    PT_UNIT_ASSERT(ts.totalHours() == 12 * 24 + 23);
    PT_UNIT_ASSERT(ts.minutes() == 34);
    PT_UNIT_ASSERT(ts.totalMinutes() == (12 * 24 + 23) * 60 + 34);
    PT_UNIT_ASSERT(ts.seconds() == 45);
    PT_UNIT_ASSERT(ts.totalSeconds() == ((12 * 24 + 23) * 60 + 34) * 60 + 45);
    PT_UNIT_ASSERT(ts.msecs() == 999);
    PT_UNIT_ASSERT(ts.totalMSecs() == (((12 * 24 + 23) * 60 + 34) * 60 + 45) * 1000 + 999);
    PT_UNIT_ASSERT(ts.microseconds() == 999);
    PT_UNIT_ASSERT(ts.useconds() == 999999);
    PT_UNIT_ASSERT(ts.toUSecs() == (((12 * 24 + 23) * 60 + 34) * 60 + 45) * Pt::uint64_t(1000000) + 999999);
}


void TimeSpanTest::testComparisonOperators()
{
    Pt::Timespan ts99(99);
    Pt::Timespan ts100(100);
    Pt::Timespan ts101(101);
    
    PT_UNIT_ASSERT(ts99  == ts99);
    PT_UNIT_ASSERT(ts100 == ts100);
    PT_UNIT_ASSERT(ts101 == ts101);
    PT_UNIT_ASSERT(ts99  == 99);
    PT_UNIT_ASSERT(ts100 == 100);
    PT_UNIT_ASSERT(ts101 == 101);

    PT_UNIT_ASSERT(ts99   != ts100);
    PT_UNIT_ASSERT(ts100  != ts101);
    PT_UNIT_ASSERT(ts101  != ts99);
    PT_UNIT_ASSERT(!(ts99 != ts99));
    PT_UNIT_ASSERT(ts99   != 100);
    PT_UNIT_ASSERT(ts100  != 101);
    PT_UNIT_ASSERT(ts101  != 99);
    PT_UNIT_ASSERT(!(ts99 != 99));

    PT_UNIT_ASSERT(ts99  < ts100);
    PT_UNIT_ASSERT(ts100 < ts101);
    PT_UNIT_ASSERT(ts99  < 100);
    PT_UNIT_ASSERT(ts100 < 101);

    PT_UNIT_ASSERT(ts99  <= ts100);
    PT_UNIT_ASSERT(ts100 <= ts100);
    PT_UNIT_ASSERT(ts99  <= 100);
    PT_UNIT_ASSERT(ts100 <= 100);

    PT_UNIT_ASSERT(ts100 > ts99);
    PT_UNIT_ASSERT(ts101 > ts100);
    PT_UNIT_ASSERT(ts100 > 99);
    PT_UNIT_ASSERT(ts101 > 100);

    PT_UNIT_ASSERT(ts100 >= ts99);
    PT_UNIT_ASSERT(ts100 >= ts100);
    PT_UNIT_ASSERT(ts100 >= 99);
    PT_UNIT_ASSERT(ts100 >= 100);
}


void TimeSpanTest::testOperators()
{
    Pt::Timespan ts5(5);
    Pt::Timespan ts6(6);
    Pt::Timespan ts7(7);

    PT_UNIT_ASSERT(ts5 + ts6 == 11);
    PT_UNIT_ASSERT(ts6 - ts5 == 1);

    PT_UNIT_ASSERT(ts7 + 2 == 9);
    PT_UNIT_ASSERT(ts7 - 3 == 4);
    
    ts5 += ts7;
    PT_UNIT_ASSERT(ts5 == 12);
    
    ts5 -= ts6;
    PT_UNIT_ASSERT(ts5 == 6);

    ts5 = Pt::Timespan(5);
    ts5 += 7;
    PT_UNIT_ASSERT(ts5 == 12);

    ts5 -= 6;
    PT_UNIT_ASSERT(ts5 == 6);
}

void TimeSpanTest::testSwap()
{
    Pt::Timespan ts99(99);
    Pt::Timespan ts100(100);

    PT_UNIT_ASSERT(ts99  == 99);
    PT_UNIT_ASSERT(ts100 == 100);

    swap(ts99, ts100);
    PT_UNIT_ASSERT(ts99  == 100);
    PT_UNIT_ASSERT(ts100 == 99);
}

