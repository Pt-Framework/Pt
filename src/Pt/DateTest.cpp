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
            Pt::Unit::TestSuite::registerMethod( "IsoConvert", *this, &DateTest::IsoConvert );
        }

    protected:
        void Null();
        void Assign();
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

