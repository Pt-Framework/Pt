/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Duerner                                 *
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
#undef PT_MATH_API_EXPORT

#include "Pt/Math/Api.h"

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include "Pt/Math/Line.h"
#include "Pt/Math/Point.h"
#include <limits>


using namespace std;

class LineTest : public Pt::Unit::TestSuite
{
    public:
        LineTest()
        : Pt::Unit::TestSuite("LineTest")
        {
            Pt::Unit::TestSuite::registerMethod( "testConstructor",
                                                 *this, &LineTest::testConstructor );
            Pt::Unit::TestSuite::registerMethod( "testComparisonOperators",
                                                 *this, &LineTest::testComparisonOperators );
            Pt::Unit::TestSuite::registerMethod( "testPerpendicularForHorizontalLine",
                                                 *this, &LineTest::testPerpendicularForHorizontalLine );
            Pt::Unit::TestSuite::registerMethod( "testPerpendicularForVerticalLine",
                                                 *this, &LineTest::testPerpendicularForVerticalLine );
            Pt::Unit::TestSuite::registerMethod( "testPerpendiculaForPointOnLine",
                                                 *this, &LineTest::testPerpendiculaForPointOnLine );
            Pt::Unit::TestSuite::registerMethod( "testPerpendiculaForPointOutsideOfLine",
                                                 *this, &LineTest::testPerpendiculaForPointOutsideOfLine );
            Pt::Unit::TestSuite::registerMethod( "testSlope",
                                                 *this, &LineTest::testSlope );
            Pt::Unit::TestSuite::registerMethod( "testIsVertical",
                                                 *this, &LineTest::testIsVertical );
            Pt::Unit::TestSuite::registerMethod( "testCalcDistance",
                                                 *this, &LineTest::testCalcDistance );
            Pt::Unit::TestSuite::registerMethod( "testAxisIntercept",
                                                 *this, &LineTest::testAxisIntercept );
        }

        void testConstructor();
        void testPointDistance();
        void testComparisonOperators();
        void testPerpendicularForHorizontalLine();
        void testPerpendicularForVerticalLine();
        void testPerpendiculaForPointOnLine();
        void testPerpendiculaForPointOutsideOfLine();
        void testSlope();
        void testIsVertical();
        void testCalcDistance();
        void testAxisIntercept();
};


Pt::Unit::RegisterTest<LineTest> _registerTestLine;


void LineTest::testConstructor()
{
    {
        Pt::Math::LineF l(Pt::Math::PointF(1.5, 3.125), Pt::Math::PointF(0.125, 99.0078125));

        PT_UNIT_ASSERT(l.p1().x() == 1.5);
        PT_UNIT_ASSERT(l.p1().y() == 3.125);
        PT_UNIT_ASSERT(l.p2().x() == 0.125);
        PT_UNIT_ASSERT(l.p2().y() == 99.0078125);
    }

    {
        Pt::Math::Line l(Pt::Math::Point(3, 5), Pt::Math::Point(-8, 6666));

        PT_UNIT_ASSERT(l.p1().x() == 3);
        PT_UNIT_ASSERT(l.p1().y() == 5);
        PT_UNIT_ASSERT(l.p2().x() == -8);
        PT_UNIT_ASSERT(l.p2().y() == 6666);
    }

    {
        // Line with same point. Has to throw an invalid_argument exception.
        try
        {
            Pt::Math::Line l(Pt::Math::Point(3, 5), Pt::Math::Point(3, 5));
            PT_UNIT_ASSERT(false);
        }
        catch (std::invalid_argument)
        {
            PT_UNIT_ASSERT(true);
        }
    }
}


void LineTest::testComparisonOperators()
{
    Pt::Math::LineF l1(Pt::Math::PointF(1.5, 3.125),        Pt::Math::PointF(0.125, 99.0078125));
    Pt::Math::LineF l2(Pt::Math::PointF(1.5, 3.125),        Pt::Math::PointF(0.125, 99.0078125));
    Pt::Math::LineF l3(Pt::Math::PointF(0.125, 99.0078125), Pt::Math::PointF(1.5, 3.125));
    Pt::Math::LineF l4(Pt::Math::PointF(123, 3.125),        Pt::Math::PointF(1.5, 3.125));
    Pt::Math::LineF l5(Pt::Math::PointF(5, 6),              Pt::Math::PointF(5, 7));
    Pt::Math::LineF l6(Pt::Math::PointF(5, 3),              Pt::Math::PointF(5, 2));
    Pt::Math::LineF l7(Pt::Math::PointF(2, -3),             Pt::Math::PointF(2, 5));
    Pt::Math::LineF l8(Pt::Math::PointF(6, 5),              Pt::Math::PointF(8, 6));
    Pt::Math::LineF l9(Pt::Math::PointF(-2, 1),             Pt::Math::PointF(2, 3));
    Pt::Math::LineF l10(Pt::Math::PointF(-2, 5),            Pt::Math::PointF(2, 7));

    PT_UNIT_ASSERT(l1 == l1);
    PT_UNIT_ASSERT(l1 == l3);
    PT_UNIT_ASSERT(l2 == l3);

    PT_UNIT_ASSERT(l1 != l4);
    PT_UNIT_ASSERT(l2 != l4);
    PT_UNIT_ASSERT(l3 != l4);

    PT_UNIT_ASSERT(l5 == l6);   // Same vertical lines.
    PT_UNIT_ASSERT(l5 != l7);   // Different vertical lines.

    PT_UNIT_ASSERT(l8 == l9);   // Same line.
    PT_UNIT_ASSERT(l9 != l10);  // Same slope, but different line.


    Pt::Math::LineF l11(Pt::Math::PointF(1.5, 5.5), Pt::Math::PointF(3.5, 7.5));
    Pt::Math::Line  l12(Pt::Math::Point (2, 6),     Pt::Math::Point (4,   8));    // Line with integer precision.

    PT_UNIT_ASSERT(l11 == l12); // Floating point vs. integer Line - Same line
    PT_UNIT_ASSERT(l10 != l12); // Floating point vs. integer Line - Different line.
}


void LineTest::testPerpendicularForHorizontalLine()
{
    // Positive area
    Pt::Math::PointF p1(10, 3);
    Pt::Math::LineF line1(Pt::Math::PointF(5, 5), Pt::Math::PointF(50, 5));

    Pt::Math::PointF rp1 = line1.calcPerpendicular(p1);

    PT_UNIT_ASSERT(rp1.x() == 10);
    PT_UNIT_ASSERT(rp1.y() == 5);

    // Negative area
    Pt::Math::PointF p2(-10, -100);
    Pt::Math::LineF line2(Pt::Math::PointF(-50, -5), Pt::Math::PointF(244, -5));

    Pt::Math::PointF rp2 = line2.calcPerpendicular(p2);

    PT_UNIT_ASSERT(rp2.x() == -10);
    PT_UNIT_ASSERT(rp2.y() == -5);

    // P1 right of P2
    Pt::Math::PointF p3(13, 4);
    Pt::Math::LineF line3(Pt::Math::PointF(50, 7), Pt::Math::PointF(5, 7));

    Pt::Math::PointF rp3 = line3.calcPerpendicular(p3);

    PT_UNIT_ASSERT(rp3.x() == 13);
    PT_UNIT_ASSERT(rp3.y() == 7);
}


void LineTest::testPerpendicularForVerticalLine()
{
    // Positive area
    Pt::Math::PointF p1(3, 10);
    Pt::Math::LineF line1(Pt::Math::PointF(5, 5), Pt::Math::PointF(5, 50));

    Pt::Math::PointF rp1 = line1.calcPerpendicular(p1);

    PT_UNIT_ASSERT(rp1.x() == 5);
    PT_UNIT_ASSERT(rp1.y() == 10);

    // Negative area
    Pt::Math::PointF p2(-100, -10);
    Pt::Math::LineF line2(Pt::Math::PointF(-5, -50), Pt::Math::PointF(-5, 244));

    Pt::Math::PointF rp2 = line2.calcPerpendicular(p2);

    PT_UNIT_ASSERT(rp2.x() == -5);
    PT_UNIT_ASSERT(rp2.y() == -10);

    // P1 below P2
    Pt::Math::PointF p3(4, 14);
    Pt::Math::LineF line3(Pt::Math::PointF(7, 50), Pt::Math::PointF(7, 5));

    Pt::Math::PointF rp3 = line3.calcPerpendicular(p3);

    PT_UNIT_ASSERT(rp3.x() == 7);
    PT_UNIT_ASSERT(rp3.y() == 14);
}


void LineTest::testPerpendiculaForPointOnLine()
{
    {
        // Positive area, point left of line
        Pt::Math::PointF p(-3, 13);
        Pt::Math::LineF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(3, 9));

        Pt::Math::PointF rp = line.calcPerpendicular(p);

        PT_UNIT_ASSERT(rp.x() == 2);
        PT_UNIT_ASSERT(rp.y() == 8);
    }

    {
        // Positive area, point left of line
        Pt::Math::PointF p(32, -10);
        Pt::Math::LineF line(Pt::Math::PointF(13, 7), Pt::Math::PointF(15, 9));

        Pt::Math::PointF rp = line.calcPerpendicular(p);

        PT_UNIT_ASSERT(rp.x() == 14);
        PT_UNIT_ASSERT(rp.y() == 8);
    }

    {
        // Negative area
        Pt::Math::PointF p(7, -42);
        Pt::Math::LineF line(Pt::Math::PointF(-5, -50), Pt::Math::PointF(1, -56));

        Pt::Math::PointF rp = line.calcPerpendicular(p);

        PT_UNIT_ASSERT(rp.x() == -3);
        PT_UNIT_ASSERT(rp.y() == -52);
    }

    {
        // P1 right of P2
        Pt::Math::PointF p(4, 14);
        Pt::Math::LineF line(Pt::Math::PointF(7, 50), Pt::Math::PointF(7, 5));

        Pt::Math::PointF rp = line.calcPerpendicular(p);

        PT_UNIT_ASSERT(rp.x() == 7);
        PT_UNIT_ASSERT(rp.y() == 14);
    }

    {
        // Point is same as P1
        Pt::Math::PointF p(13, 17.125);
        Pt::Math::LineF line(Pt::Math::PointF(13, 17.125), Pt::Math::PointF(167.51, -22.8945));

        Pt::Math::PointF rp = line.calcPerpendicular(p);

        PT_UNIT_ASSERT(rp.x() == 13);
        PT_UNIT_ASSERT(rp.y() == 17.125);
    }

    {
        // Point is same as P2
        Pt::Math::PointF p(894, -22.125);
        Pt::Math::LineF line(Pt::Math::PointF(78.18946, -8714.849841), Pt::Math::PointF(894, -22.125));

        Pt::Math::PointF rp = line.calcPerpendicular(p);

        PT_UNIT_ASSERT(rp.x() == 894);
        PT_UNIT_ASSERT(rp.y() == -22.125);
    }
}


void LineTest::testPerpendiculaForPointOutsideOfLine()
{
    Pt::Math::PointF p(-2, 18);
    Pt::Math::LineF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(3, 9));

    Pt::Math::PointF rp = line.calcPerpendicular(p);

    PT_UNIT_ASSERT(rp.x() == 5);
    PT_UNIT_ASSERT(rp.y() == 11);
}


void LineTest::testSlope()
{
    // 3 tests with floating point precision.
    {
        Pt::Math::LineF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(9, 13));
        PT_UNIT_ASSERT(line.slope() == 6 / 8.0);
    }

    {
        Pt::Math::LineF line(Pt::Math::PointF(5, 7), Pt::Math::PointF(10, -3));
        PT_UNIT_ASSERT(line.slope() == -2);
    }

    {
        Pt::Math::LineF line(Pt::Math::PointF(10, -3), Pt::Math::PointF(5, 7));
        PT_UNIT_ASSERT(line.slope() == -2);
    }

    // Tests with horizontal lines.
    {
        Pt::Math::LineF line(Pt::Math::PointF(10, -3), Pt::Math::PointF(11, -3));
        PT_UNIT_ASSERT(line.slope() == 0);
    }

    {
        Pt::Math::LineF line(Pt::Math::PointF(-5, 7), Pt::Math::PointF(7, 7));
        PT_UNIT_ASSERT(line.slope() == 0);
    }

    // Tests with vertical lines.
    {
        Pt::Math::LineF line(Pt::Math::PointF(10, -3), Pt::Math::PointF(10, 5));
        PT_UNIT_ASSERT(line.slope() == numeric_limits<double>::infinity());
    }

    {
        Pt::Math::LineF line(Pt::Math::PointF(-5, 7), Pt::Math::PointF(-5, 10));
        PT_UNIT_ASSERT(line.slope() == numeric_limits<double>::infinity());
    }

    // Test with integer values/points.
    {
        Pt::Math::Line line(Pt::Math::Point(3, 4), Pt::Math::Point(7, 5));
        PT_UNIT_ASSERT(line.slope() == 0.25);
    }
}


void LineTest::testIsVertical()
{
    {
        Pt::Math::Line line(Pt::Math::Point(3, 4), Pt::Math::Point(3, 8));
        PT_UNIT_ASSERT(line.isVertical());
    }

    {
        Pt::Math::Line line(Pt::Math::Point(-3, -4), Pt::Math::Point(-3, -8));
        PT_UNIT_ASSERT(line.isVertical());
    }

    {
        Pt::Math::LineF line(Pt::Math::PointF(-3.25, -4.25), Pt::Math::PointF(-3.25, -8.25));
        PT_UNIT_ASSERT(line.isVertical());
    }

    {
        Pt::Math::Line line(Pt::Math::Point(3, 3), Pt::Math::Point(4, 4));
        PT_UNIT_ASSERT(!line.isVertical());
    }

    {
        Pt::Math::Line line(Pt::Math::Point(1, 4), Pt::Math::Point(10, 4));
        PT_UNIT_ASSERT(!line.isVertical());
    }

    {
        Pt::Math::LineF line(Pt::Math::PointF(1.25, 4.75), Pt::Math::PointF(10.25, 4.75));
        PT_UNIT_ASSERT(!line.isVertical());
    }
}


void LineTest::testCalcDistance()
{
    {
        // Point not on line
        Pt::Math::PointF p(-3, 13);
        Pt::Math::LineF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(3, 9));

        PT_UNIT_ASSERT(fabs(line.calcDistance(p) - 5 * sqrt(2.0)) < 0.2);
    }

    {
        // Point on line
        Pt::Math::PointF p(17, 11);
        Pt::Math::LineF line(Pt::Math::PointF(13, 7), Pt::Math::PointF(15, 9));

        PT_UNIT_ASSERT(fabs(line.calcDistance(p)) < 0.2);
    }
}

void LineTest::testAxisIntercept()
{
    {
        Pt::Math::LineF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(3, 9));
        PT_UNIT_ASSERT(line.axisIntercept() == 6);
    }

    {
        Pt::Math::LineF line(Pt::Math::PointF(-10, 7), Pt::Math::PointF(-6, 9));
        PT_UNIT_ASSERT(line.axisIntercept() == 12);
    }

    {
        Pt::Math::Line line(Pt::Math::Point(-10, 7), Pt::Math::Point(-6, 8));
        PT_UNIT_ASSERT(line.axisIntercept() == 9.5);
    }

    {
        Pt::Math::Line line(Pt::Math::Point(-2, 7), Pt::Math::Point(2, 8));
        PT_UNIT_ASSERT(line.axisIntercept() == 7.5);
    }
}
