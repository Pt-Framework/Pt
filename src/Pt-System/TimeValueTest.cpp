/***************************************************************************
 *   Copyright (C) 2007 by Roman Schnider                                  *
 *   Copyright (C) 2007 by Marc Boris Duerner                              *
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
#include "Pt/System/TimeValue.h"

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

using namespace std;
using namespace Pt;
using namespace Pt::System;


class TimeValueTest : public Pt::Unit::TestSuite
{
public:
    TimeValueTest()
    : TestSuite("TimeValueTest")
    {
        Pt::Unit::TestSuite::registerMethod("testTimeValue", *this, &TimeValueTest::testTimeValue);
    }

protected:
    void testTimeValue();

private:

};


Pt::Unit::RegisterTest<TimeValueTest> register_TimeValueTest;

void TimeValueTest::testTimeValue()
{
    //Constructor
    TimeValue timeValueA( 10, 2 );
    TimeValue timeValueB;
    TimeValue result;

    PT_UNIT_ASSERT( timeValueA.seconds() == 10 && timeValueA.microSeconds() == 2 );

    //Egality and getter setter
    timeValueB.setSeconds( 10 );
    timeValueB.setMicroSeconds( 2 );

    PT_UNIT_ASSERT( timeValueA == timeValueB );

    //Substraction1
    timeValueB.setSeconds( 9 );
    timeValueB.setMicroSeconds( 4 );
    result = timeValueA - timeValueB;

    PT_UNIT_ASSERT( result.seconds() == 0 && result.microSeconds() == 1000000 - 2 );

    //Substraction2
    timeValueA.setSeconds( 9 );
    timeValueA.setMicroSeconds( 4 );

    timeValueB.setSeconds( 9 );
    timeValueB.setMicroSeconds( 4 );
    result = timeValueA - timeValueB;

    PT_UNIT_ASSERT( result.seconds() == 0 && result.microSeconds() == 0 );

    //Substraction3
    timeValueA.setSeconds( 19 );
    timeValueA.setMicroSeconds( 4 );

    timeValueB.setSeconds( 9 );
    timeValueB.setMicroSeconds( 4 );
    result = timeValueA - timeValueB;

    PT_UNIT_ASSERT( result.seconds() == 10 && result.microSeconds() == 0 );

    //Substraction4
    timeValueA.setSeconds( 19 );
    timeValueA.setMicroSeconds( 4 );

    timeValueB.setSeconds( 9 );
    timeValueB.setMicroSeconds( 2 );
    result = timeValueA - timeValueB;

    PT_UNIT_ASSERT( result.seconds() == 10 && result.microSeconds() == 2 );

    //Substraction5
    timeValueA.setSeconds( 19 );
    timeValueA.setMicroSeconds( 2 );

    timeValueB.setSeconds( 9 );
    timeValueB.setMicroSeconds( 4 );
    result = timeValueA - timeValueB;

    PT_UNIT_ASSERT( result.seconds() == 9 && result.microSeconds() == 999998 );

    //Addition1
    timeValueA.setSeconds( 10 );
    timeValueA.setMicroSeconds( 2 );

    timeValueB.setSeconds( 9 );
    timeValueB.setMicroSeconds( 999999 );
    result = timeValueA + timeValueB;

    PT_UNIT_ASSERT( result.seconds() == 20 && result.microSeconds() == 1 );

    //Addition2
    timeValueA.setSeconds( 10 );
    timeValueA.setMicroSeconds( 2 );

    timeValueB.setSeconds( 9 );
    timeValueB.setMicroSeconds( 1000);
    result = timeValueA + timeValueB;

    PT_UNIT_ASSERT( result.seconds() == 19 && result.microSeconds() == 1002 );

    //Addition3
    timeValueA.setSeconds( 2 );
    timeValueA.setMicroSeconds( 800000 );

    timeValueB.setSeconds( 1 );
    timeValueB.setMicroSeconds( 400000);
    result = timeValueA + timeValueB;
    PT_UNIT_ASSERT( result.seconds() == 4 && result.microSeconds() == 200000 );

    //+=
    timeValueA += timeValueB;
    PT_UNIT_ASSERT( timeValueA.seconds() == 4 && timeValueA.microSeconds() == 200000 );

    //-=
    timeValueA.setSeconds( 10 );
    timeValueA.setMicroSeconds( 2 );

    timeValueB.setSeconds( 9 );
    timeValueB.setMicroSeconds( 4 );

    timeValueA -=  timeValueB;
    PT_UNIT_ASSERT( timeValueA.seconds() == 0 && timeValueA.microSeconds() == 1000000 - 2 );
}
