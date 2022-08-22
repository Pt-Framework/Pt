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

#include "Pt/Date.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>


class DateTest : public Pt::Unit::TestSuite
{
    public:
        DateTest()
        : Pt::Unit::TestSuite("DateTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Assign", *this, &DateTest::Assign );
            Pt::Unit::TestSuite::registerMethod( "Increment", *this, &DateTest::Increment );
            Pt::Unit::TestSuite::registerMethod( "IsoConvert", *this, &DateTest::IsoConvert );
        }

    protected:
        void Assign();
        void Increment();
        void IsoConvert();
};

Pt::Unit::RegisterTest<DateTest> register_DateTest;


void DateTest::Assign()
{
    Pt::Date date2(1);
    //std::cerr << "\n Null Date: "<< date2.day() << " " << date2.month() << " " << date2.year() << std::endl;

    Pt::Date date(2001, 11, 15);
    PT_UNIT_ASSERT( date.year() == 2001 );
    PT_UNIT_ASSERT( date.month() == 11 );
    PT_UNIT_ASSERT( date.day() == 15 );

    date.set(1789, 5, 12);
    PT_UNIT_ASSERT( date.year() == 1789 );
    PT_UNIT_ASSERT( date.month() == 5 );
    PT_UNIT_ASSERT( date.day() == 12 );

    int y;
    unsigned m, d;
    date.get(y, m, d);
    PT_UNIT_ASSERT( y == 1789 );
    PT_UNIT_ASSERT( m == 5 );
    PT_UNIT_ASSERT( d == 12 );
}


void DateTest::Increment()
{
  Pt::Date d1(2000, 3, 15);
  d1.addMonths(13);
  PT_UNIT_ASSERT( d1.day() == 15 );
  PT_UNIT_ASSERT( d1.month() == 4 );
  PT_UNIT_ASSERT( d1.year() == 2001 );

  d1.addDays(21);
  PT_UNIT_ASSERT( d1.day() == 6 );
  PT_UNIT_ASSERT( d1.month() == 5 );
  PT_UNIT_ASSERT( d1.year() == 2001 );
  
  d1.addYears(1000);
  PT_UNIT_ASSERT( d1.day() == 6 );
  PT_UNIT_ASSERT( d1.month() == 5 );
  PT_UNIT_ASSERT( d1.year() == 3001 );

  Pt::Date d2(2001, 1, 31);
  d2.addMonths(1);
  PT_UNIT_ASSERT( d2.day() == 28 );
  PT_UNIT_ASSERT( d2.month() == 2 );
  PT_UNIT_ASSERT( d2.year() == 2001 );

  Pt::Date d3(2001, 4, 30);
  d3.addMonths(-7);
  PT_UNIT_ASSERT( d3.day() == 30 );
  PT_UNIT_ASSERT( d3.month() == 9 );
  PT_UNIT_ASSERT( d3.year() == 2000 );

}

void DateTest::IsoConvert()
{
    Pt::Date date(2001, 11, 15);
    std::string isoString = date.toIsoString();
    PT_UNIT_ASSERT( isoString == "2001-11-15" );

    date = Pt::Date::fromIsoString("1789-05-12");
    PT_UNIT_ASSERT( date.year() == 1789 );
    PT_UNIT_ASSERT( date.month() == 5 );
    PT_UNIT_ASSERT( date.day() == 12 );
}
