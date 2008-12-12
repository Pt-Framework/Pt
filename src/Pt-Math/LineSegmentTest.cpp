/*
 * Copyright (C) 2004 Marc Boris Duerner
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
#undef PT_MATH_API_EXPORT

#include "Pt/Math/Api.h"

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include "Pt/Math/LineSegment.h"
#include "Pt/Math/Point.h"


using namespace std;

class LineSegmentTest : public Pt::Unit::TestSuite
{
    public:
        LineSegmentTest()
        : Pt::Unit::TestSuite("LineSegmentTest")
        {
            Pt::Unit::TestSuite::registerMethod( "testConstructor",
                                                 *this, &LineSegmentTest::testConstructor );
            Pt::Unit::TestSuite::registerMethod( "testComparisonOperators",
                                                 *this, &LineSegmentTest::testComparisonOperators );
            Pt::Unit::TestSuite::registerMethod( "testPerpendicularForHorizontalLine",
                                                 *this, &LineSegmentTest::testPerpendicularForHorizontalLine );
            Pt::Unit::TestSuite::registerMethod( "testPerpendicularForVerticalLine",
                                                 *this, &LineSegmentTest::testPerpendicularForVerticalLine );
            Pt::Unit::TestSuite::registerMethod( "testPerpendiculaForPointOnLine",
                                                 *this, &LineSegmentTest::testPerpendiculaForPointOnLine );
            Pt::Unit::TestSuite::registerMethod( "testPerpendiculaForPointOutsideOfLine",
                                                 *this, &LineSegmentTest::testPerpendiculaForPointOutsideOfLine );
            Pt::Unit::TestSuite::registerMethod( "testCalcNearestPointOnLine",
                                                 *this, &LineSegmentTest::testCalcNearestPointOnLine );
            Pt::Unit::TestSuite::registerMethod( "testInBounds",
                                                 *this, &LineSegmentTest::testInBounds );
            Pt::Unit::TestSuite::registerMethod( "testSetPoints",
                                                 *this, &LineSegmentTest::testSetPoints );
            Pt::Unit::TestSuite::registerMethod( "testSlope",
                                                 *this, &LineSegmentTest::testSlope );
            Pt::Unit::TestSuite::registerMethod( "testIsVertical",
                                                 *this, &LineSegmentTest::testIsVertical );
            Pt::Unit::TestSuite::registerMethod( "testCalcDistance",
                                                 *this, &LineSegmentTest::testCalcDistance );
        }

        void testConstructor();
        void testComparisonOperators();
        void testPerpendicularForHorizontalLine();
        void testPerpendicularForVerticalLine();
        void testPerpendiculaForPointOnLine();
        void testPerpendiculaForPointOutsideOfLine();
        void testCalcNearestPointOnLine();
        void testInBounds();
        void testSlope();
        void testIsVertical();
        void testCalcDistance();
        void testSetPoints();
};


Pt::Unit::RegisterTest<LineSegmentTest> _registerTestLineSegment;


void LineSegmentTest::testConstructor()
{
    {
        Pt::Math::LineSegmentF l(Pt::Math::PointF(1.5, 3.125), Pt::Math::PointF(0.125, 99.0078125));

        PT_UNIT_ASSERT(l.p1().x() == 1.5);
        PT_UNIT_ASSERT(l.p1().y() == 3.125);
        PT_UNIT_ASSERT(l.p2().x() == 0.125);
        PT_UNIT_ASSERT(l.p2().y() == 99.0078125);
    }
    
    {
        Pt::Math::LineSegment l(Pt::Math::Point(3, 5), Pt::Math::Point(-8, 6666));

        PT_UNIT_ASSERT(l.p1().x() == 3);
        PT_UNIT_ASSERT(l.p1().y() == 5);
        PT_UNIT_ASSERT(l.p2().x() == -8);
        PT_UNIT_ASSERT(l.p2().y() == 6666);
    }
}




void LineSegmentTest::testComparisonOperators()
{
    Pt::Math::LineSegmentF l1(Pt::Math::PointF(1.5, 3.125),        Pt::Math::PointF(0.125, 99.0078125));
    Pt::Math::LineSegmentF l2(Pt::Math::PointF(1.5, 3.125),        Pt::Math::PointF(0.125, 99.0078125));
    Pt::Math::LineSegmentF l3(Pt::Math::PointF(0.125, 99.0078125), Pt::Math::PointF(1.5, 3.125));
    Pt::Math::LineSegmentF l4(Pt::Math::PointF(123, 3.125),        Pt::Math::PointF(1.5, 3.125));

    PT_UNIT_ASSERT(l1 == l1);
    PT_UNIT_ASSERT(l1 == l3);
    PT_UNIT_ASSERT(l2 == l3);

    PT_UNIT_ASSERT(l1 != l4);
    PT_UNIT_ASSERT(l2 != l4);
    PT_UNIT_ASSERT(l3 != l4);
}


void LineSegmentTest::testPerpendicularForHorizontalLine()
{
    // Positive area
    Pt::Math::PointF p1(10, 3);
    Pt::Math::LineSegmentF line1(Pt::Math::PointF(5, 5), Pt::Math::PointF(50, 5));

    Pt::Math::PointF rp1 = line1.calcPerpendicular(p1);

    PT_UNIT_ASSERT(rp1.x() == 10);
    PT_UNIT_ASSERT(rp1.y() == 5);

    // Negative area
    Pt::Math::PointF p2(-10, -100);
    Pt::Math::LineSegmentF line2(Pt::Math::PointF(-50, -5), Pt::Math::PointF(244, -5));

    Pt::Math::PointF rp2 = line2.calcPerpendicular(p2);

    PT_UNIT_ASSERT(rp2.x() == -10);
    PT_UNIT_ASSERT(rp2.y() == -5);

    // P1 right of P2
    Pt::Math::PointF p3(13, 4);
    Pt::Math::LineSegmentF line3(Pt::Math::PointF(50, 7), Pt::Math::PointF(5, 7));

    Pt::Math::PointF rp3 = line3.calcPerpendicular(p3);

    PT_UNIT_ASSERT(rp3.x() == 13);
    PT_UNIT_ASSERT(rp3.y() == 7);
}


void LineSegmentTest::testPerpendicularForVerticalLine()
{
    // Positive area
    Pt::Math::PointF p1(3, 10);
    Pt::Math::LineSegmentF line1(Pt::Math::PointF(5, 5), Pt::Math::PointF(5, 50));

    Pt::Math::PointF rp1 = line1.calcPerpendicular(p1);

    PT_UNIT_ASSERT(rp1.x() == 5);
    PT_UNIT_ASSERT(rp1.y() == 10);

    // Negative area
    Pt::Math::PointF p2(-100, -10);
    Pt::Math::LineSegmentF line2(Pt::Math::PointF(-5, -50), Pt::Math::PointF(-5, 244));

    Pt::Math::PointF rp2 = line2.calcPerpendicular(p2);

    PT_UNIT_ASSERT(rp2.x() == -5);
    PT_UNIT_ASSERT(rp2.y() == -10);

    // P1 below P2
    Pt::Math::PointF p3(4, 14);
    Pt::Math::LineSegmentF line3(Pt::Math::PointF(7, 50), Pt::Math::PointF(7, 5));

    Pt::Math::PointF rp3 = line3.calcPerpendicular(p3);

    PT_UNIT_ASSERT(rp3.x() == 7);
    PT_UNIT_ASSERT(rp3.y() == 14);
}


void LineSegmentTest::testPerpendiculaForPointOnLine()
{
    {
        // Positive area, point left of line
        Pt::Math::PointF p(-3, 13);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(3, 9));

        Pt::Math::PointF rp = line.calcPerpendicular(p);

        PT_UNIT_ASSERT(rp.x() == 2);
        PT_UNIT_ASSERT(rp.y() == 8);
    }

    {
        // Positive area, point left of line
        Pt::Math::PointF p(32, -10);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(13, 7), Pt::Math::PointF(15, 9));

        Pt::Math::PointF rp = line.calcPerpendicular(p);

        PT_UNIT_ASSERT(rp.x() == 14);
        PT_UNIT_ASSERT(rp.y() == 8);
    }

    {
        // Negative area
        Pt::Math::PointF p(7, -42);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(-5, -50), Pt::Math::PointF(1, -56));

        Pt::Math::PointF rp = line.calcPerpendicular(p);

        PT_UNIT_ASSERT(rp.x() == -3);
        PT_UNIT_ASSERT(rp.y() == -52);
    }

    {
        // P1 right of P2
        Pt::Math::PointF p(4, 14);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(7, 50), Pt::Math::PointF(7, 5));

        Pt::Math::PointF rp = line.calcPerpendicular(p);

        PT_UNIT_ASSERT(rp.x() == 7);
        PT_UNIT_ASSERT(rp.y() == 14);
    }

    {
        // Point is same as P1
        Pt::Math::PointF p(13, 17.125);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(13, 17.125), Pt::Math::PointF(167.51, -22.8945));

        Pt::Math::PointF rp = line.calcPerpendicular(p);

        PT_UNIT_ASSERT(rp.x() == 13);
        PT_UNIT_ASSERT(rp.y() == 17.125);
    }
    
    {
        // Point is same as P2
        Pt::Math::PointF p(894, -22.125);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(78.18946, -8714.849841), Pt::Math::PointF(894, -22.125));

        Pt::Math::PointF rp = line.calcPerpendicular(p);

        PT_UNIT_ASSERT(rp.x() == 894);
        PT_UNIT_ASSERT(rp.y() == -22.125);
    }
}


void LineSegmentTest::testPerpendiculaForPointOutsideOfLine()
{
    Pt::Math::PointF p(-2, 18);
    Pt::Math::LineSegmentF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(3, 9));

    Pt::Math::PointF rp = line.calcPerpendicular(p);

    PT_UNIT_ASSERT(rp.x() == 5);
    PT_UNIT_ASSERT(rp.y() == 11);
}

void LineSegmentTest::testCalcNearestPointOnLine()
{
    {
        // Positive area, point left of line. (Still in range of line.)
        Pt::Math::PointF p(-3, 13);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(3, 9));

        Pt::Math::PointF rp = line.calcNearestPointOnLine(p);

        PT_UNIT_ASSERT(rp.x() == 2);
        PT_UNIT_ASSERT(rp.y() == 8);
    }

    {
        // P1 right of P2. (Still in range of line.)
        Pt::Math::PointF p(4, 14);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(7, 50), Pt::Math::PointF(7, 5));

        Pt::Math::PointF rp = line.calcNearestPointOnLine(p);

        PT_UNIT_ASSERT(rp.x() == 7);
        PT_UNIT_ASSERT(rp.y() == 14);
    }

    {
        // Point is same as P1. (Still in range of line.)
        Pt::Math::PointF p(13, 17.125);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(13, 17.125), Pt::Math::PointF(167.51, -22.8945));

        Pt::Math::PointF rp = line.calcNearestPointOnLine(p);

        PT_UNIT_ASSERT(rp.x() == 13);
        PT_UNIT_ASSERT(rp.y() == 17.125);
    }

    {
        // Point is same as P2. (Still in range of line.)
        Pt::Math::PointF p(894, -22.125);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(78.18946, -8714.849841), Pt::Math::PointF(894, -22.125));

        Pt::Math::PointF rp = line.calcNearestPointOnLine(p);

        PT_UNIT_ASSERT(rp.x() == 894);
        PT_UNIT_ASSERT(rp.y() == -22.125);
    }
    
    {
        // Calculated point is outside of line. Right of it. The resulting line thus should
        // be the right point of the line.
        Pt::Math::PointF p(-2, 18);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(3, 9));

        Pt::Math::PointF rp = line.calcNearestPointOnLine(p);

        PT_UNIT_ASSERT(rp.x() == 3);
        PT_UNIT_ASSERT(rp.y() == 9);
    }

    {
        // Calculated point is outside of line. Left of it. The resulting line thus should
        // be the left point of the line.
        Pt::Math::PointF p(3, 12);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(-2, 7), Pt::Math::PointF(8, -3));

        Pt::Math::PointF rp = line.calcNearestPointOnLine(p);

        PT_UNIT_ASSERT(rp.x() == -2);
        PT_UNIT_ASSERT(rp.y() ==  7);
    }
    
    {
        // Calculated point is outside of line. Right of it. The resulting line thus should
        // be the right point of the line. 
        // P1 is right of P2.
        Pt::Math::PointF p(18, 15);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(5, 4), Pt::Math::PointF(15, 9));

        Pt::Math::PointF rp = line.calcNearestPointOnLine(p);

        PT_UNIT_ASSERT(rp.x() == 15);
        PT_UNIT_ASSERT(rp.y() ==  9);
    }
}

void LineSegmentTest::testInBounds()
{
    {
        // On line.
        Pt::Math::LineSegmentF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(3, 9));
        PT_UNIT_ASSERT(line.inBounds(Pt::Math::PointF(2, 8)));
    }

    {
        // In line bounds: x from left point, y from right point
        Pt::Math::LineSegmentF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(3, 9));
        PT_UNIT_ASSERT(line.inBounds(Pt::Math::PointF(1, 9)));
    }

    {
        // In line bounds: x from right point, y from left point.
        Pt::Math::LineSegmentF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(3, 9));
        PT_UNIT_ASSERT(line.inBounds(Pt::Math::PointF(3, 7)));
    }

    {
        // In line bounds: On line, negative area.
        Pt::Math::LineSegmentF line(Pt::Math::PointF(-1, -7), Pt::Math::PointF(-3, -9));
        PT_UNIT_ASSERT(line.inBounds(Pt::Math::PointF(-2, -8)));
    }

    {
        // In line bounds: P1 is right of P2.
        Pt::Math::LineSegmentF line(Pt::Math::PointF(10, 3), Pt::Math::PointF(8, 1));
        PT_UNIT_ASSERT(line.inBounds(Pt::Math::PointF(9, 2)));
    }

    {
        // In line bounds: P1 and P2 have same x value. Point on line
        Pt::Math::LineSegmentF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(1, 9));
        PT_UNIT_ASSERT(line.inBounds(Pt::Math::PointF(1, 8)));
    }

    {
        // Out of line bounds: Left of P1.
        Pt::Math::LineSegmentF line(Pt::Math::PointF(3, 7), Pt::Math::PointF(5, 9));
        PT_UNIT_ASSERT(!line.inBounds(Pt::Math::PointF(1, 8)));
    }

    {
        // Out of line bounds: Right of P2.
        Pt::Math::LineSegmentF line(Pt::Math::PointF(3, 7), Pt::Math::PointF(5, 9));
        PT_UNIT_ASSERT(!line.inBounds(Pt::Math::PointF(6, 8)));
    }

    {
        // Out of line bounds: Above of P2.
        Pt::Math::LineSegmentF line(Pt::Math::PointF(3, 7), Pt::Math::PointF(5, 9));
        PT_UNIT_ASSERT(!line.inBounds(Pt::Math::PointF(4, 10)));
    }

    {
        // Out of line bounds: Below of P1.
        Pt::Math::LineSegmentF line(Pt::Math::PointF(3, 7), Pt::Math::PointF(5, 9));
        PT_UNIT_ASSERT(!line.inBounds(Pt::Math::PointF(2, 6)));
    }
}


void LineSegmentTest::testSlope()
{
    // 3 tests with floating point precision.
    {
        Pt::Math::LineSegmentF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(9, 13));
        PT_UNIT_ASSERT(line.slope() == 6 / 8.0);
    }

    {
        Pt::Math::LineSegmentF line(Pt::Math::PointF(5, 7), Pt::Math::PointF(10, -3));
        PT_UNIT_ASSERT(line.slope() == -2);
    }

    {
        Pt::Math::LineSegmentF line(Pt::Math::PointF(10, -3), Pt::Math::PointF(5, 7));
        PT_UNIT_ASSERT(line.slope() == -2);
    }

    // Tests with horizontal lines.
    {
        Pt::Math::LineSegmentF line(Pt::Math::PointF(10, -3), Pt::Math::PointF(11, -3));
        PT_UNIT_ASSERT(line.slope() == 0);
    }

    {
        Pt::Math::LineSegmentF line(Pt::Math::PointF(-5, 7), Pt::Math::PointF(7, 7));
        PT_UNIT_ASSERT(line.slope() == 0);
    }

    // Tests with vertical lines.
    {
        Pt::Math::LineSegmentF line(Pt::Math::PointF(10, -3), Pt::Math::PointF(10, 5));
        PT_UNIT_ASSERT(line.slope() == numeric_limits<double>::infinity());
    }

    {
        Pt::Math::LineSegmentF line(Pt::Math::PointF(-5, 7), Pt::Math::PointF(-5, 10));
        PT_UNIT_ASSERT(line.slope() == numeric_limits<double>::infinity());
    }

    // Test with integer values/points.
    {
        Pt::Math::LineSegment line(Pt::Math::Point(3, 4), Pt::Math::Point(7, 5));
        PT_UNIT_ASSERT(line.slope() == 0.25);
    }
}


void LineSegmentTest::testIsVertical()
{
    {
        Pt::Math::LineSegment line(Pt::Math::Point(3, 4), Pt::Math::Point(3, 8));
        PT_UNIT_ASSERT(line.isVertical());
    }

    {
        Pt::Math::LineSegment line(Pt::Math::Point(-3, -4), Pt::Math::Point(-3, -8));
        PT_UNIT_ASSERT(line.isVertical());
    }

    {
        Pt::Math::LineSegmentF line(Pt::Math::PointF(-3.25, -4.25), Pt::Math::PointF(-3.25, -8.25));
        PT_UNIT_ASSERT(line.isVertical());
    }

    {
        Pt::Math::LineSegment line(Pt::Math::Point(3, 3), Pt::Math::Point(4, 4));
        PT_UNIT_ASSERT(!line.isVertical());
    }

    {
        Pt::Math::LineSegment line(Pt::Math::Point(1, 4), Pt::Math::Point(10, 4));
        PT_UNIT_ASSERT(!line.isVertical());
    }

    {
        Pt::Math::LineSegmentF line(Pt::Math::PointF(1.25, 4.75), Pt::Math::PointF(10.25, 4.75));
        PT_UNIT_ASSERT(!line.isVertical());
    }
}


void LineSegmentTest::testCalcDistance()
{
    {
        // Point not on line
        Pt::Math::PointF p(-3, 13);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(1, 7), Pt::Math::PointF(3, 9));

        PT_UNIT_ASSERT(fabs(line.calcDistance(p) - 5 * sqrt(2.0)) < 0.2);
    }

    {
        // Point on line
        Pt::Math::PointF p(14, 8);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(13, 7), Pt::Math::PointF(15, 9));

        PT_UNIT_ASSERT(fabs(line.calcDistance(p)) < 0.2);
    }

    {
        // Point is on the imaginary extended line, but some distance away from the right point.
        Pt::Math::PointF p(17, 11);
        Pt::Math::LineSegmentF line(Pt::Math::PointF(13, 7), Pt::Math::PointF(15, 9));

        PT_UNIT_ASSERT(fabs(line.calcDistance(p) - 2.82) < 0.2);
    }
}


void LineSegmentTest::testSetPoints()
{
    Pt::Math::LineSegmentF line(Pt::Math::PointF(1, 2), Pt::Math::PointF(3, 4));

    try
    {
        line.setPoints(Pt::Math::PointF(5, 6), Pt::Math::PointF(6, 5));
    }
    catch (const std::invalid_argument&)
    {
        PT_UNIT_ASSERT_MSG(false, "An execption occured when settings the points of a LineSegment. "
                                  "As the points are different no exception should occur.");
    }
    
    try
    {
        line.setPoints(Pt::Math::PointF(7, 9), Pt::Math::PointF(7, 9));
        PT_UNIT_ASSERT_MSG(false, "No exception occured although a LineSegment was created with "
                                  "2 identical lines which should lead to an exception.");
    }
    catch (const std::invalid_argument&)
    { }
}
