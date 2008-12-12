/*
 * Copyright (C) 2006-2007 PTV AG
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

#include "Pt/Math/Polygon.h"
#include "Pt/Math/Point.h"

#include <limits>
#include <vector>


using namespace std;

namespace Pt {
namespace Math {

class PolygonTest : public Pt::Unit::TestSuite
{
    public:
        PolygonTest()
        : Pt::Unit::TestSuite("PolygonTest")
        {
            Pt::Unit::TestSuite::registerMethod("testConstructors",          *this, &PolygonTest::testConstructors);
            Pt::Unit::TestSuite::registerMethod("testAddPoint",              *this, &PolygonTest::testAddPoint);
            Pt::Unit::TestSuite::registerMethod("testComparisonOperators",   *this, &PolygonTest::testComparisonOperators);
            Pt::Unit::TestSuite::registerMethod("testTooSmallPolygon",       *this, &PolygonTest::testTooSmallPolygon);
            Pt::Unit::TestSuite::registerMethod("testSimplePolygon",         *this, &PolygonTest::testSimplePolygon);
            Pt::Unit::TestSuite::registerMethod("testComplexPolygon",        *this, &PolygonTest::testComplexPolygon);
            Pt::Unit::TestSuite::registerMethod("testComplexPolygon2",       *this, &PolygonTest::testComplexPolygon2);
            Pt::Unit::TestSuite::registerMethod("testSquareBracketOperator", *this, &PolygonTest::testSquareBracketOperator);
        }


    void testConstructors();
    void testAddPoint();
    void testComparisonOperators();
    void testTooSmallPolygon();
    void testSimplePolygon();
    void testComplexPolygon();
    void testComplexPolygon2();
    void testSquareBracketOperator();
};


Pt::Unit::RegisterTest<PolygonTest> _registerPolygonTest;


void PolygonTest::testConstructors()
{
    {
        Pt::Math::Polygon p;
        PT_UNIT_ASSERT(p.size() == 0);
    }

    {
        Pt::Math::PolygonF p;
        PT_UNIT_ASSERT(p.size() == 0);
    }

    {
        std::vector<Pt::Math::PointF> pList;

        pList.push_back(Pt::Math::PointF(1.2, 2.3));
        pList.push_back(Pt::Math::PointF(3.4, 4.5));
        pList.push_back(Pt::Math::PointF(5.6, 7.8));

        Pt::Math::PolygonF p(pList);
        PT_UNIT_ASSERT(p.size() == 3);
    }

    {
        std::vector<Pt::Math::Point> pList;

        pList.push_back(Pt::Math::Point(1, 2));
        pList.push_back(Pt::Math::Point(3, 4));
        pList.push_back(Pt::Math::Point(5, 6));
        pList.push_back(Pt::Math::Point(7, 8));

        Pt::Math::Polygon p(pList);
        PT_UNIT_ASSERT(p.size() == 4);
    }
}

void PolygonTest::testAddPoint()
{
    Pt::Math::Polygon p;

    PT_UNIT_ASSERT(p.size() == 0);

    p.addPoint(Pt::Math::Point(1, 2));
    PT_UNIT_ASSERT(p.size() == 1);

    p.addPoint(Pt::Math::Point(3, 4));
    PT_UNIT_ASSERT(p.size() == 2);

    p.addPoint(Pt::Math::Point(5, 6));
    PT_UNIT_ASSERT(p.size() == 3);

    std::vector<Pt::Math::Point>::const_iterator it = p.points().begin();

    PT_UNIT_ASSERT(it->x() == 1);
    PT_UNIT_ASSERT(it->y() == 2);
    ++it;
    PT_UNIT_ASSERT(it->x() == 3);
    PT_UNIT_ASSERT(it->y() == 4);
    ++it;
    PT_UNIT_ASSERT(it->x() == 5);
    PT_UNIT_ASSERT(it->y() == 6);
}

void PolygonTest::testComparisonOperators()
{
    std::vector<Pt::Math::PointF> pfList1;

    pfList1.push_back(Pt::Math::PointF(1.2, 2.3));
    pfList1.push_back(Pt::Math::PointF(3.4, 4.5));
    pfList1.push_back(Pt::Math::PointF(5.6, 7.8));


    std::vector<Pt::Math::PointF> pfList2;

    pfList2.push_back(Pt::Math::PointF(1, 2));
    pfList2.push_back(Pt::Math::PointF(3, 4));
    pfList2.push_back(Pt::Math::PointF(5, 6));
    pfList2.push_back(Pt::Math::PointF(7, 8));


    std::vector<Pt::Math::Point> pList;

    pList.push_back(Pt::Math::Point(1, 2));
    pList.push_back(Pt::Math::Point(3, 4));
    pList.push_back(Pt::Math::Point(5, 6));
    pList.push_back(Pt::Math::Point(7, 8));


    {
        Pt::Math::PolygonF p1(pfList1);

        PT_UNIT_ASSERT(p1 == p1);
    }

    {
        Pt::Math::PolygonF p1(pfList1);
        Pt::Math::PolygonF p2(pfList1);

        PT_UNIT_ASSERT(p1 == p2);
    }

    {
        Pt::Math::PolygonF p1(pfList2);
        Pt::Math::Polygon  p2(pList);

        PT_UNIT_ASSERT(p1 == p2);
    }

    {
        Pt::Math::PolygonF p1(pfList1);
        Pt::Math::PolygonF p2(pfList2);

        PT_UNIT_ASSERT(p1 != p2);
    }
}


void PolygonTest::testTooSmallPolygon()
{
    {
        Pt::Math::Polygon p;
        PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(10, 10)) == false);
    }

    {
        Pt::Math::Polygon p;
        p.addPoint(Pt::Math::Point(10, 10));
        p.addPoint(Pt::Math::Point(20, 20));

        PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(15, 15)) == false);
    }
}

void PolygonTest::testSimplePolygon()
{
    Pt::Math::Polygon p;
    p.addPoint(Pt::Math::Point(0,  0));
    p.addPoint(Pt::Math::Point(10, 10));
    p.addPoint(Pt::Math::Point(0,  20));

    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(5, 7)) == true);

    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(1,  0)) == false);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(-1, 0)) == false);

    PT_UNIT_ASSERT(p.contains(Pt::Math::Point(11, 10)) == false);
    PT_UNIT_ASSERT(p.contains(Pt::Math::Point(9 , 10)) == true);

    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(1,  20)) == false);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(-1, 20)) == false);

    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(0.000001,  20)) == false);

    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(5.00001, 15)) == false);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(4.99999, 15)) == true);

    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(5.00001, 5)) == false);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(4.99999, 5)) == true);

    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(-0.00001, 3)) == false);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(0.00001, 3)) == true);
}

void PolygonTest::testComplexPolygon()
{
    Pt::Math::PolygonF p;
    p.addPoint(Pt::Math::PointF(0,  0));
    p.addPoint(Pt::Math::PointF(5,  6));
    p.addPoint(Pt::Math::PointF(10, 0));
    p.addPoint(Pt::Math::PointF(10, 10));
    p.addPoint(Pt::Math::PointF(5,  3));
    p.addPoint(Pt::Math::PointF(0,  10));

    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(1, 2)) == true);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(1, 3)) == true);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(1, 4)) == true);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(1, 6)) == true);

    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(4.5, 3)) == false);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(4.5, 4)) == true);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(4.5, 5)) == true);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(4.5, 6)) == false);

    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(9, 2)) == true);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(9, 3)) == true);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(9, 4)) == true);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(9, 6)) == true);

    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(4, 6)) == false);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(6, 6)) == false);

    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(-0.0001, 6)) == false);
    PT_UNIT_ASSERT(p.contains(Pt::Math::PointF(10.0001, 9)) == false);
}



void PolygonTest::testComplexPolygon2()
{
    Pt::Math::Polygon p;
    p.addPoint(Pt::Math::Point(4275255, 3225477));
    p.addPoint(Pt::Math::Point(4457295, 3157849));
    p.addPoint(Pt::Math::Point(4536120, 3245700));
    p.addPoint(Pt::Math::Point(4354080, 3313328));
    
    PT_UNIT_ASSERT(p.contains(Pt::Math::Point(4356608, 3186176)) == false);
    PT_UNIT_ASSERT(p.contains(Pt::Math::Point(4421632, 3186176)) == true);
    PT_UNIT_ASSERT(p.contains(Pt::Math::Point(4421632, 3251200)) == true);
    PT_UNIT_ASSERT(p.contains(Pt::Math::Point(4356608, 3251200)) == true);
}


void PolygonTest::testSquareBracketOperator()
{
    Pt::Math::Polygon p;
    p.addPoint(Pt::Math::Point(    0,     0));
    p.addPoint(Pt::Math::Point(   35,    36));
    p.addPoint(Pt::Math::Point(   32,  6854));
    p.addPoint(Pt::Math::Point(65431, 10658));
    p.addPoint(Pt::Math::Point(  363,  8816));
    p.addPoint(Pt::Math::Point( 9661,   125));

    PT_UNIT_ASSERT(p[0] == Pt::Math::Point(    0,     0));
    PT_UNIT_ASSERT(p[1] == Pt::Math::Point(   35,    36));
    PT_UNIT_ASSERT(p[2] == Pt::Math::Point(   32,  6854));
    PT_UNIT_ASSERT(p[3] == Pt::Math::Point(65431, 10658));
    PT_UNIT_ASSERT(p[4] == Pt::Math::Point(  363,  8816));
    PT_UNIT_ASSERT(p[5] == Pt::Math::Point( 9661,   125));

}

} // namespace Math
} // namespace Pt
