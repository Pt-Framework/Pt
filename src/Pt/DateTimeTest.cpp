/***************************************************************************
 *   Copyright (C) 2006 by Tommi Mäkitalo                                  *
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
 *   Copyright (C) 2006 by Stefan Büder                                    *
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
#include "Pt/DateTime.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>


class DateTimeTest : public Pt::Unit::TestSuite
{
    public:
        DateTimeTest()
        : Pt::Unit::TestSuite("DateTimeTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Null", *this, &DateTimeTest::Null );
            Pt::Unit::TestSuite::registerMethod( "Assign", *this, &DateTimeTest::Assign );
            Pt::Unit::TestSuite::registerMethod( "IsoConvert", *this, &DateTimeTest::IsoConvert );
        }

    protected:
        void Null();
        void Assign();
        void IsoConvert();
};

Pt::Unit::RegisterTest<DateTimeTest> register_DateTimeTest;


void DateTimeTest::Null()
{
    Pt::DateTime dt;
    PT_UNIT_ASSERT( dt.isNull() );

    Pt::Date date;
    Pt::Time time;
    Pt::DateTime dt2(date, time);
    PT_UNIT_ASSERT( dt.isNull() );
}

void DateTimeTest::Assign()
{
    Pt::Date date(2001, 11, 15);
    Pt::Time time(12, 45, 23, 956);
    Pt::DateTime dt(date, time);
    PT_UNIT_ASSERT( !dt.isNull() );
    PT_UNIT_ASSERT( dt.years() == 2001 );
    PT_UNIT_ASSERT( dt.months() == 11 );
    PT_UNIT_ASSERT( dt.days() == 15 );
    PT_UNIT_ASSERT( dt.hours() == 12 );
    PT_UNIT_ASSERT( dt.minutes() == 45 );
    PT_UNIT_ASSERT( dt.seconds() == 23 );
    PT_UNIT_ASSERT( dt.msecs() == 956 );

    dt.set(1789, 5, 12, 23, 59, 59, 999);
    PT_UNIT_ASSERT( !dt.isNull() );
    PT_UNIT_ASSERT( dt.years() == 1789 );
    PT_UNIT_ASSERT( dt.months() == 5 );
    PT_UNIT_ASSERT( dt.days() == 12 );
    PT_UNIT_ASSERT( dt.hours() == 23 );
    PT_UNIT_ASSERT( dt.minutes() == 59 );
    PT_UNIT_ASSERT( dt.seconds() == 59 );
    PT_UNIT_ASSERT( dt.msecs() == 999 );
}


void DateTimeTest::IsoConvert()
{
    Pt::Date date(2001, 11, 15);
    Pt::Time time(12, 45, 23, 956);
    Pt::DateTime dt(date, time);
    std::string isoString = dt.toIsoString();
    PT_UNIT_ASSERT( isoString == "2001-11-15 12:45:23.956" );

    dt = Pt::DateTime::fromIsoString("1789-05-12 23:59:59.999");
    PT_UNIT_ASSERT( !dt.isNull() );
    PT_UNIT_ASSERT( dt.years() == 1789 );
    PT_UNIT_ASSERT( dt.months() == 5 );
    PT_UNIT_ASSERT( dt.days() == 12 );
    PT_UNIT_ASSERT( dt.hours() == 23 );
    PT_UNIT_ASSERT( dt.minutes() == 59 );
    PT_UNIT_ASSERT( dt.seconds() == 59 );
    PT_UNIT_ASSERT( dt.msecs() == 999 );
}

