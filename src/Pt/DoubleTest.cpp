/***************************************************************************
*   Copyright (C) 2005-2007 by Marc Boris Duerner                         *
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

#include "Pt/Double.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <sstream>


class DoubleTest : public Pt::Unit::TestSuite
{
public:
    DoubleTest()
        : Pt::Unit::TestSuite("DoubleTest")
    {
        this->registerMethod( "Test", *this, &DoubleTest::Test );
    }

protected:

    void Test()
    {
        Pt::uint64_t bd = Pt::Double::toIEEE754Double(0.085);
        double d = Pt::Double::fromIEEE754Double(bd);
        double diff = d - 0.085;
        PT_UNIT_ASSERT(diff < 0.00000001);

        bd = Pt::Double::toIEEE754Double(18.4);
        d = Pt::Double::fromIEEE754Double(bd);
        diff = d - 18.4;
        PT_UNIT_ASSERT(diff < 0.00000001);

        bd = Pt::Double::toIEEE754Double(-5.0);
        d = Pt::Double::fromIEEE754Double(bd);
        diff = d - (-5.0);
        PT_UNIT_ASSERT(diff < 0.00000001);

        bd = Pt::Double::toIEEE754Double(1.0);
        d = Pt::Double::fromIEEE754Double(bd);
        diff = d - 1.0;
        PT_UNIT_ASSERT(diff < 0.00000001);

        bd = Pt::Double::toIEEE754Double(-1.0);
        d = Pt::Double::fromIEEE754Double(bd);
        diff = d - (-1.0);
        PT_UNIT_ASSERT(diff < 0.00000001);

        bd = Pt::Double::toIEEE754Double(0.0);
        d = Pt::Double::fromIEEE754Double(bd);
        diff = d - 0.0;
        PT_UNIT_ASSERT(diff < 0.00000001);

        bd = Pt::Double::toIEEE754Double(-0.0);
        d = Pt::Double::fromIEEE754Double(bd);
        diff = d - (-0.0);
        PT_UNIT_ASSERT(diff < 0.00000001);

        double maxIEEE754Double = (1.0 - pow(2.0, -53)) * pow(2.0, 1024);   
        bd = Pt::Double::toIEEE754Double(maxIEEE754Double);
        d = Pt::Double::fromIEEE754Double(bd);
        diff = d - maxIEEE754Double;
        PT_UNIT_ASSERT(diff < 0.00000001);

        double minIEEE754Double = pow(2.0, -1022);
        bd = Pt::Double::toIEEE754Double(minIEEE754Double);
        d = Pt::Double::fromIEEE754Double(bd);
        diff = d - minIEEE754Double;
        PT_UNIT_ASSERT(diff < 0.00000001);

        double minDouble = std::numeric_limits<double>::min();
        bd = Pt::Double::toIEEE754Double(minDouble);
        d = Pt::Double::fromIEEE754Double(bd);
        diff = d - minDouble;
        PT_UNIT_ASSERT(diff < 0.00000001);

        double maxDouble = std::numeric_limits<double>::max();
        bd = Pt::Double::toIEEE754Double(maxDouble);
        d = Pt::Double::fromIEEE754Double(bd);
        diff = d - maxDouble;
        PT_UNIT_ASSERT(diff < 0.00000001);
    }
};

static Pt::Unit::RegisterTest<DoubleTest> registerDoubleTest;
