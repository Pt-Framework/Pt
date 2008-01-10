/***************************************************************************
 *   Copyright (C) 2006 by Tommi Maekitalo                                 *
 *   Copyright (C) 2006 by Marc Boris Duerner                               *
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

#include "Pt/Time.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>


class TimeTest : public Pt::Unit::TestSuite
{
public:
    TimeTest()
    : Pt::Unit::TestSuite("TimeTest")
    {
        Pt::Unit::TestSuite::registerMethod("testAssign", *this, &TimeTest::testAssign);
        Pt::Unit::TestSuite::registerMethod("testIsoConvert", *this, &TimeTest::testIsoConvert);
    }

protected:
    void testAssign();
    void testIsoConvert();
};

Pt::Unit::RegisterTest<TimeTest> register_TimeTest;


void TimeTest::testAssign()
{
    Pt::Time time(12, 45, 23, 956);
    PT_UNIT_ASSERT( time.hour() == 12 );
    PT_UNIT_ASSERT( time.minute() == 45 );
    PT_UNIT_ASSERT( time.second() == 23 );
    PT_UNIT_ASSERT( time.msec() == 956 );

    time.set(23, 59, 59, 999);
    PT_UNIT_ASSERT( time.hour() == 23 );
    PT_UNIT_ASSERT( time.minute() == 59 );
    PT_UNIT_ASSERT( time.second() == 59 );
    PT_UNIT_ASSERT( time.msec() == 999 );
}


void TimeTest::testIsoConvert()
{
    Pt::Time time(12, 45, 23, 956);
    std::string isoString = time.toIsoString();
    PT_UNIT_ASSERT( isoString == "12:45:23.956" );

    time = Pt::Time::fromIsoString("23:59:59.999");
    PT_UNIT_ASSERT( time.hour() == 23 );
    PT_UNIT_ASSERT( time.minute() == 59 );
    PT_UNIT_ASSERT( time.second() == 59 );
    PT_UNIT_ASSERT( time.msec() == 999 );
}

