/* Copyright (C) 2026 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include <Pt/Gfx/Rect.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

namespace Pt {

namespace Gfx {

class RectTest : public Pt::Unit::TestSuite
{
    public:
        RectTest()
        : Pt::Unit::TestSuite("Pt::Gfx::RectTest")
        {
            registerMethod("DefaultConstruct", *this, &RectTest::DefaultConstruct);
            registerMethod("ConstructFromPointSize", *this, &RectTest::ConstructFromPointSize);
            registerMethod("ConstructFromSize", *this, &RectTest::ConstructFromSize);
            registerMethod("ConstructFromWidthHeight", *this, &RectTest::ConstructFromWidthHeight);
            registerMethod("ConstructFromTwoPoints", *this, &RectTest::ConstructFromTwoPoints);
            registerMethod("ConstructFromEdges", *this, &RectTest::ConstructFromEdges);
            registerMethod("CopyConstruct", *this, &RectTest::CopyConstruct);
            registerMethod("Accessors", *this, &RectTest::Accessors);
            registerMethod("Corners", *this, &RectTest::Corners);
            registerMethod("SetOriginAndSize", *this, &RectTest::SetOriginAndSize);
            registerMethod("Clear", *this, &RectTest::Clear);
            registerMethod("IsEmpty", *this, &RectTest::IsEmpty);
            registerMethod("Move", *this, &RectTest::Move);
            registerMethod("ExpandShrink", *this, &RectTest::ExpandShrink);
            registerMethod("Unify", *this, &RectTest::Unify);
            registerMethod("Intersect", *this, &RectTest::Intersect);
            registerMethod("Contains", *this, &RectTest::Contains);
            registerMethod("Equality", *this, &RectTest::Equality);
            registerMethod("ConstructFromLTRB", *this, &RectTest::ConstructFromLTRB);
            registerMethod("ConstructFromXYWH", *this, &RectTest::ConstructFromXYWH);
            registerMethod("ContainsRect", *this, &RectTest::ContainsRect);
            registerMethod("Intersects", *this, &RectTest::Intersects);
            registerMethod("Center", *this, &RectTest::Center);
            registerMethod("Normalize", *this, &RectTest::Normalize);
            registerMethod("ToNormalized", *this, &RectTest::ToNormalized);
            registerMethod("Round", *this, &RectTest::Round);
            registerMethod("Floor", *this, &RectTest::Floor);
            registerMethod("Ceil", *this, &RectTest::Ceil);
            registerMethod("RoundOut", *this, &RectTest::RoundOut);
            registerMethod("RoundIn", *this, &RectTest::RoundIn);
            registerMethod("ConstructFromRectI", *this, &RectTest::ConstructFromRectI);
        }

        void DefaultConstruct()
        {
            Rect r;
            PT_UNIT_ASSERT_NEAR(r.x(), 0.0);
            PT_UNIT_ASSERT_NEAR(r.y(), 0.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 0.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 0.0);
            PT_UNIT_ASSERT(r.isEmpty());
        }

        void ConstructFromPointSize()
        {
            Rect r(Point(10.0, 20.0), Size(30.0, 40.0));
            PT_UNIT_ASSERT_NEAR(r.x(), 10.0);
            PT_UNIT_ASSERT_NEAR(r.y(), 20.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 30.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 40.0);
        }

        void ConstructFromSize()
        {
            Rect r(Size(100.0, 200.0));
            PT_UNIT_ASSERT_NEAR(r.x(), 0.0);
            PT_UNIT_ASSERT_NEAR(r.y(), 0.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 100.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 200.0);
        }

        void ConstructFromWidthHeight()
        {
            Rect r(50.0, 75.0);
            PT_UNIT_ASSERT_NEAR(r.x(), 0.0);
            PT_UNIT_ASSERT_NEAR(r.y(), 0.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 50.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 75.0);
        }

        void ConstructFromTwoPoints()
        {
            Rect r(Point(10.0, 20.0), Point(50.0, 60.0));
            PT_UNIT_ASSERT_NEAR(r.x(), 10.0);
            PT_UNIT_ASSERT_NEAR(r.y(), 20.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 40.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 40.0);
        }

        void ConstructFromEdges()
        {
            // fromLTRB(left, top, right, bottom)
            Rect r = Rect::fromLTRB(10.0, 20.0, 50.0, 60.0);
            PT_UNIT_ASSERT_NEAR(r.left(), 10.0);
            PT_UNIT_ASSERT_NEAR(r.right(), 50.0);
            PT_UNIT_ASSERT_NEAR(r.top(), 20.0);
            PT_UNIT_ASSERT_NEAR(r.bottom(), 60.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 40.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 40.0);
        }

        void CopyConstruct()
        {
            Rect r1(Point(1.0, 2.0), Size(3.0, 4.0));
            Rect r2(r1);
            PT_UNIT_ASSERT(r2 == r1);
        }

        void Accessors()
        {
            Rect r(Point(5.0, 10.0), Size(20.0, 30.0));

            PT_UNIT_ASSERT_NEAR(r.x(), 5.0);
            PT_UNIT_ASSERT_NEAR(r.y(), 10.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 20.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 30.0);
            PT_UNIT_ASSERT_NEAR(r.left(), 5.0);
            PT_UNIT_ASSERT_NEAR(r.top(), 10.0);
            PT_UNIT_ASSERT_NEAR(r.right(), 25.0);
            PT_UNIT_ASSERT_NEAR(r.bottom(), 40.0);

            PT_UNIT_ASSERT(r.origin() == Point(5.0, 10.0));
            PT_UNIT_ASSERT(r.size() == Size(20.0, 30.0));
        }

        void Corners()
        {
            Rect r(Point(10.0, 20.0), Size(30.0, 40.0));

            PT_UNIT_ASSERT(r.topLeft() == Point(10.0, 20.0));
            PT_UNIT_ASSERT(r.topRight() == Point(40.0, 20.0));
            PT_UNIT_ASSERT(r.bottomLeft() == Point(10.0, 60.0));
            PT_UNIT_ASSERT(r.bottomRight() == Point(40.0, 60.0));
        }

        void SetOriginAndSize()
        {
            Rect r;

            r.setOrigin(Point(5.0, 10.0));
            PT_UNIT_ASSERT_NEAR(r.x(), 5.0);
            PT_UNIT_ASSERT_NEAR(r.y(), 10.0);

            r.setSize(Size(20.0, 30.0));
            PT_UNIT_ASSERT_NEAR(r.width(), 20.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 30.0);

            r.setWidth(50.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 50.0);

            r.setHeight(60.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 60.0);

            r.set(Point(1.0, 2.0), Size(3.0, 4.0));
            PT_UNIT_ASSERT_NEAR(r.x(), 1.0);
            PT_UNIT_ASSERT_NEAR(r.y(), 2.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 3.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 4.0);
        }

        void Clear()
        {
            Rect r(Point(5.0, 6.0), Size(7.0, 8.0));
            r.clear();
            PT_UNIT_ASSERT_NEAR(r.x(), 0.0);
            PT_UNIT_ASSERT_NEAR(r.y(), 0.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 0.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 0.0);
        }

        void IsEmpty()
        {
            Rect r1;
            PT_UNIT_ASSERT(r1.isEmpty());

            Rect r2(10.0, 0.0);
            PT_UNIT_ASSERT(r2.isEmpty());

            Rect r3(0.0, 10.0);
            PT_UNIT_ASSERT(r3.isEmpty());

            Rect r4(10.0, 10.0);
            PT_UNIT_ASSERT( ! r4.isEmpty() );
        }

        void Move()
        {
            Rect r(Point(10.0, 20.0), Size(30.0, 40.0));
            r.move(5.0, -3.0);

            PT_UNIT_ASSERT_NEAR(r.x(), 15.0);
            PT_UNIT_ASSERT_NEAR(r.y(), 17.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 30.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 40.0);
        }

        void ExpandShrink()
        {
            Rect r(Point(10.0, 20.0), Size(30.0, 40.0));

            r.expand(5.0, 10.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 35.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 50.0);
            PT_UNIT_ASSERT_NEAR(r.x(), 10.0);
            PT_UNIT_ASSERT_NEAR(r.y(), 20.0);

            r.shrink(5.0, 10.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 30.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 40.0);
        }

        void Unify()
        {
            Rect r1(Point(0.0, 0.0), Size(10.0, 10.0));
            Rect r2(Point(5.0, 5.0), Size(10.0, 10.0));

            r1.unify(r2);
            PT_UNIT_ASSERT_NEAR(r1.left(), 0.0);
            PT_UNIT_ASSERT_NEAR(r1.top(), 0.0);
            PT_UNIT_ASSERT_NEAR(r1.right(), 15.0);
            PT_UNIT_ASSERT_NEAR(r1.bottom(), 15.0);

            // unify with null rect does nothing
            Rect r3(Point(1.0, 2.0), Size(3.0, 4.0));
            Rect null;
            r3.unify(null);
            PT_UNIT_ASSERT_NEAR(r3.left(), 1.0);
            PT_UNIT_ASSERT_NEAR(r3.top(), 2.0);
            PT_UNIT_ASSERT_NEAR(r3.width(), 3.0);
            PT_UNIT_ASSERT_NEAR(r3.height(), 4.0);

            // null rect unify with non-null becomes the other rect
            Rect r4;
            r4.unify(r3);
            PT_UNIT_ASSERT(r4 == r3);
        }

        void Intersect()
        {
            Rect r1(Point(0.0, 0.0), Size(10.0, 10.0));
            Rect r2(Point(5.0, 5.0), Size(10.0, 10.0));

            Rect inter = r1.toIntersected(r2);
            PT_UNIT_ASSERT_NEAR(inter.left(), 5.0);
            PT_UNIT_ASSERT_NEAR(inter.top(), 5.0);
            PT_UNIT_ASSERT_NEAR(inter.right(), 10.0);
            PT_UNIT_ASSERT_NEAR(inter.bottom(), 10.0);

            // non-overlapping rects produce null intersection
            Rect r3(Point(0.0, 0.0), Size(5.0, 5.0));
            Rect r4(Point(10.0, 10.0), Size(5.0, 5.0));
            Rect noInter = r3.toIntersected(r4);
            PT_UNIT_ASSERT(noInter.isEmpty());

            // touching at edge
            Rect r5(Point(0.0, 0.0), Size(5.0, 5.0));
            Rect r6(Point(5.0, 0.0), Size(5.0, 5.0));
            Rect edgeInter = r5.toIntersected(r6);
            PT_UNIT_ASSERT_NEAR(edgeInter.width(), 0.0);
        }

        void Contains()
        {
            Rect r(Point(10.0, 20.0), Size(30.0, 40.0));

            // inside
            PT_UNIT_ASSERT(r.contains(Point(15.0, 25.0)));

            // on top-left corner (inclusive)
            PT_UNIT_ASSERT(r.contains(Point(10.0, 20.0)));

            // on bottom-right edge (exclusive)
            PT_UNIT_ASSERT( ! r.contains(Point(40.0, 60.0)) );

            // outside
            PT_UNIT_ASSERT( ! r.contains(Point(5.0, 25.0)) );
            PT_UNIT_ASSERT( ! r.contains(Point(15.0, 65.0)) );
        }

        void Equality()
        {
            Rect r1(Point(1.0, 2.0), Size(3.0, 4.0));
            Rect r2(Point(1.0, 2.0), Size(3.0, 4.0));
            Rect r3(Point(1.0, 2.0), Size(3.0, 5.0));

            PT_UNIT_ASSERT(r1 == r2);
            PT_UNIT_ASSERT(r1 != r3);
            PT_UNIT_ASSERT( ! (r1 != r2) );
            PT_UNIT_ASSERT( ! (r1 == r3) );
        }

        void ConstructFromLTRB()
        {
            Rect r = Rect::fromLTRB(10.0, 20.0, 50.0, 60.0);
            PT_UNIT_ASSERT_NEAR(r.left(), 10.0);
            PT_UNIT_ASSERT_NEAR(r.top(), 20.0);
            PT_UNIT_ASSERT_NEAR(r.right(), 50.0);
            PT_UNIT_ASSERT_NEAR(r.bottom(), 60.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 40.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 40.0);
        }

        void ConstructFromXYWH()
        {
            Rect r = Rect::fromXYWH(10.0, 20.0, 40.0, 30.0);
            PT_UNIT_ASSERT_NEAR(r.x(), 10.0);
            PT_UNIT_ASSERT_NEAR(r.y(), 20.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 40.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 30.0);
        }

        void ContainsRect()
        {
            Rect outer(Point(0.0, 0.0), Size(100.0, 100.0));
            Rect inner(Point(10.0, 10.0), Size(50.0, 50.0));
            PT_UNIT_ASSERT(outer.contains(inner));
            PT_UNIT_ASSERT( ! inner.contains(outer) );

            Rect partial(Point(50.0, 50.0), Size(100.0, 100.0));
            PT_UNIT_ASSERT( ! outer.contains(partial) );
        }

        void Intersects()
        {
            Rect r1(Point(0.0, 0.0), Size(10.0, 10.0));
            Rect r2(Point(5.0, 5.0), Size(10.0, 10.0));
            PT_UNIT_ASSERT(r1.intersects(r2));
            PT_UNIT_ASSERT(r2.intersects(r1));

            Rect r3(Point(20.0, 20.0), Size(10.0, 10.0));
            PT_UNIT_ASSERT( ! r1.intersects(r3) );

            Rect adjacent(Point(10.0, 0.0), Size(10.0, 10.0));
            PT_UNIT_ASSERT( ! r1.intersects(adjacent) );
        }

        void Center()
        {
            Rect r(Point(0.0, 0.0), Size(10.0, 20.0));
            Point c = r.center();
            PT_UNIT_ASSERT_NEAR(c.x(), 5.0);
            PT_UNIT_ASSERT_NEAR(c.y(), 10.0);
        }

        void Normalize()
        {
            Rect r(Point(5.0, 5.0), Size(-3.0, -4.0));
            r.normalize();
            PT_UNIT_ASSERT_NEAR(r.left(), 2.0);
            PT_UNIT_ASSERT_NEAR(r.top(), 1.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 3.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 4.0);
        }

        void ToNormalized()
        {
            Rect r(Point(5.0, 5.0), Size(-3.0, -4.0));
            Rect n = r.toNormalized();
            PT_UNIT_ASSERT_NEAR(n.left(), 2.0);
            PT_UNIT_ASSERT_NEAR(n.top(), 1.0);
            PT_UNIT_ASSERT_NEAR(n.width(), 3.0);
            PT_UNIT_ASSERT_NEAR(n.height(), 4.0);
            PT_UNIT_ASSERT_NEAR(r.width(), -3.0);
        }

        void Round()
        {
            Rect r(Point(1.2, 2.7), Size(3.4, 4.6));
            RectI ri = r.round();
            PT_UNIT_ASSERT_EQUAL(ri.x(), Int(1));
            PT_UNIT_ASSERT_EQUAL(ri.y(), Int(3));
            PT_UNIT_ASSERT_EQUAL(ri.width(), Int(3));
            PT_UNIT_ASSERT_EQUAL(ri.height(), Int(5));
        }

        void Floor()
        {
            Rect r(Point(1.9, 2.1), Size(3.9, 4.1));
            RectI ri = r.floor();
            PT_UNIT_ASSERT_EQUAL(ri.x(), Int(1));
            PT_UNIT_ASSERT_EQUAL(ri.y(), Int(2));
            PT_UNIT_ASSERT_EQUAL(ri.width(), Int(3));
            PT_UNIT_ASSERT_EQUAL(ri.height(), Int(4));
        }

        void Ceil()
        {
            Rect r(Point(1.1, 2.9), Size(3.1, 4.9));
            RectI ri = r.ceil();
            PT_UNIT_ASSERT_EQUAL(ri.x(), Int(2));
            PT_UNIT_ASSERT_EQUAL(ri.y(), Int(3));
            PT_UNIT_ASSERT_EQUAL(ri.width(), Int(4));
            PT_UNIT_ASSERT_EQUAL(ri.height(), Int(5));
        }

        void RoundOut()
        {
            // origin=(1.2, 2.7), size=(3.4, 4.6) -> right=4.6, bottom=7.3
            Rect r(Point(1.2, 2.7), Size(3.4, 4.6));
            RectI ri = r.roundOut();
            PT_UNIT_ASSERT_EQUAL(ri.left(), Int(1));
            PT_UNIT_ASSERT_EQUAL(ri.top(), Int(2));
            PT_UNIT_ASSERT_EQUAL(ri.right(), Int(5));
            PT_UNIT_ASSERT_EQUAL(ri.bottom(), Int(8));
        }

        void RoundIn()
        {
            // origin=(1.2, 2.7), size=(3.4, 4.6) -> right=4.6, bottom=7.3
            Rect r(Point(1.2, 2.7), Size(3.4, 4.6));
            RectI ri = r.roundIn();
            PT_UNIT_ASSERT_EQUAL(ri.left(), Int(2));
            PT_UNIT_ASSERT_EQUAL(ri.top(), Int(3));
            PT_UNIT_ASSERT_EQUAL(ri.right(), Int(4));
            PT_UNIT_ASSERT_EQUAL(ri.bottom(), Int(7));
        }

        void ConstructFromRectI()
        {
            RectI ri(PointI(5, 10), SizeI(30, 40));
            Rect r(ri);
            PT_UNIT_ASSERT_NEAR(r.x(), 5.0);
            PT_UNIT_ASSERT_NEAR(r.y(), 10.0);
            PT_UNIT_ASSERT_NEAR(r.width(), 30.0);
            PT_UNIT_ASSERT_NEAR(r.height(), 40.0);

            Rect r2;
            r2 = ri;
            PT_UNIT_ASSERT_NEAR(r2.x(), 5.0);
            PT_UNIT_ASSERT_NEAR(r2.y(), 10.0);
        }
};


class RectITest : public Pt::Unit::TestSuite
{
    public:
        RectITest()
        : Pt::Unit::TestSuite("Pt::Gfx::RectITest")
        {
            registerMethod("DefaultConstruct", *this, &RectITest::DefaultConstruct);
            registerMethod("ConstructFromPointSize", *this, &RectITest::ConstructFromPointSize);
            registerMethod("ConstructFromSize", *this, &RectITest::ConstructFromSize);
            registerMethod("ConstructFromWidthHeight", *this, &RectITest::ConstructFromWidthHeight);
            registerMethod("ConstructFromTwoPoints", *this, &RectITest::ConstructFromTwoPoints);
            registerMethod("ConstructFromEdges", *this, &RectITest::ConstructFromEdges);
            registerMethod("CopyConstruct", *this, &RectITest::CopyConstruct);
            registerMethod("Accessors", *this, &RectITest::Accessors);
            registerMethod("Corners", *this, &RectITest::Corners);
            registerMethod("SetOriginAndSize", *this, &RectITest::SetOriginAndSize);
            registerMethod("Clear", *this, &RectITest::Clear);
            registerMethod("IsEmpty", *this, &RectITest::IsEmpty);
            registerMethod("Move", *this, &RectITest::Move);
            registerMethod("ExpandShrink", *this, &RectITest::ExpandShrink);
            registerMethod("Unify", *this, &RectITest::Unify);
            registerMethod("Intersect", *this, &RectITest::Intersect);
            registerMethod("Contains", *this, &RectITest::Contains);
            registerMethod("Equality", *this, &RectITest::Equality);
            registerMethod("ConstructFromLTRB", *this, &RectITest::ConstructFromLTRB);
            registerMethod("ConstructFromXYWH", *this, &RectITest::ConstructFromXYWH);
            registerMethod("ContainsRect", *this, &RectITest::ContainsRect);
            registerMethod("Intersects", *this, &RectITest::Intersects);
            registerMethod("Center", *this, &RectITest::Center);
            registerMethod("Normalize", *this, &RectITest::Normalize);
            registerMethod("ToNormalized", *this, &RectITest::ToNormalized);
        }

        void DefaultConstruct()
        {
            RectI r;
            PT_UNIT_ASSERT_EQUAL(r.x(), Int(0));
            PT_UNIT_ASSERT_EQUAL(r.y(), Int(0));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(0));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(0));
            PT_UNIT_ASSERT(r.isEmpty());
        }

        void ConstructFromPointSize()
        {
            RectI r(PointI(10, 20), SizeI(30, 40));
            PT_UNIT_ASSERT_EQUAL(r.x(), Int(10));
            PT_UNIT_ASSERT_EQUAL(r.y(), Int(20));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(30));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(40));
        }

        void ConstructFromSize()
        {
            RectI r(SizeI(100, 200));
            PT_UNIT_ASSERT_EQUAL(r.x(), Int(0));
            PT_UNIT_ASSERT_EQUAL(r.y(), Int(0));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(100));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(200));
        }

        void ConstructFromWidthHeight()
        {
            RectI r(50, 75);
            PT_UNIT_ASSERT_EQUAL(r.x(), Int(0));
            PT_UNIT_ASSERT_EQUAL(r.y(), Int(0));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(50));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(75));
        }

        void ConstructFromTwoPoints()
        {
            RectI r(PointI(10, 20), PointI(50, 60));
            PT_UNIT_ASSERT_EQUAL(r.x(), Int(10));
            PT_UNIT_ASSERT_EQUAL(r.y(), Int(20));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(40));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(40));
        }

        void ConstructFromEdges()
        {
            // fromLTRB(left, top, right, bottom)
            RectI r = RectI::fromLTRB(10, 20, 50, 60);
            PT_UNIT_ASSERT_EQUAL(r.left(), Int(10));
            PT_UNIT_ASSERT_EQUAL(r.right(), Int(50));
            PT_UNIT_ASSERT_EQUAL(r.top(), Int(20));
            PT_UNIT_ASSERT_EQUAL(r.bottom(), Int(60));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(40));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(40));
        }

        void CopyConstruct()
        {
            RectI r1(PointI(1, 2), SizeI(3, 4));
            RectI r2(r1);
            PT_UNIT_ASSERT(r2 == r1);
        }

        void Accessors()
        {
            RectI r(PointI(5, 10), SizeI(20, 30));

            PT_UNIT_ASSERT_EQUAL(r.x(), Int(5));
            PT_UNIT_ASSERT_EQUAL(r.y(), Int(10));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(20));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(30));
            PT_UNIT_ASSERT_EQUAL(r.left(), Int(5));
            PT_UNIT_ASSERT_EQUAL(r.top(), Int(10));
            PT_UNIT_ASSERT_EQUAL(r.right(), Int(25));
            PT_UNIT_ASSERT_EQUAL(r.bottom(), Int(40));

            PT_UNIT_ASSERT(r.origin() == PointI(5, 10));
            PT_UNIT_ASSERT(r.size() == SizeI(20, 30));
        }

        void Corners()
        {
            RectI r(PointI(10, 20), SizeI(30, 40));

            PT_UNIT_ASSERT(r.topLeft() == PointI(10, 20));
            PT_UNIT_ASSERT(r.topRight() == PointI(40, 20));
            PT_UNIT_ASSERT(r.bottomLeft() == PointI(10, 60));
            PT_UNIT_ASSERT(r.bottomRight() == PointI(40, 60));
        }

        void SetOriginAndSize()
        {
            RectI r;

            r.setOrigin(PointI(5, 10));
            PT_UNIT_ASSERT_EQUAL(r.x(), Int(5));
            PT_UNIT_ASSERT_EQUAL(r.y(), Int(10));

            r.setSize(SizeI(20, 30));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(20));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(30));

            r.setWidth(50);
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(50));

            r.setHeight(60);
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(60));

            r.set(PointI(1, 2), SizeI(3, 4));
            PT_UNIT_ASSERT_EQUAL(r.x(), Int(1));
            PT_UNIT_ASSERT_EQUAL(r.y(), Int(2));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(3));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(4));
        }

        void Clear()
        {
            RectI r(PointI(5, 6), SizeI(7, 8));
            r.clear();
            PT_UNIT_ASSERT_EQUAL(r.x(), Int(0));
            PT_UNIT_ASSERT_EQUAL(r.y(), Int(0));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(0));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(0));
        }

        void IsEmpty()
        {
            RectI r1;
            PT_UNIT_ASSERT(r1.isEmpty());

            RectI r2(10, 0);
            PT_UNIT_ASSERT(r2.isEmpty());

            RectI r3(0, 10);
            PT_UNIT_ASSERT(r3.isEmpty());

            RectI r4(10, 10);
            PT_UNIT_ASSERT( ! r4.isEmpty() );
        }

        void Move()
        {
            RectI r(PointI(10, 20), SizeI(30, 40));
            r.move(5, -3);

            PT_UNIT_ASSERT_EQUAL(r.x(), Int(15));
            PT_UNIT_ASSERT_EQUAL(r.y(), Int(17));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(30));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(40));
        }

        void ExpandShrink()
        {
            RectI r(PointI(10, 20), SizeI(30, 40));

            r.expand(5, 10);
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(35));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(50));
            PT_UNIT_ASSERT_EQUAL(r.x(), Int(10));
            PT_UNIT_ASSERT_EQUAL(r.y(), Int(20));

            r.shrink(5, 10);
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(30));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(40));
        }

        void Unify()
        {
            RectI r1(PointI(0, 0), SizeI(10, 10));
            RectI r2(PointI(5, 5), SizeI(10, 10));

            r1.unify(r2);
            PT_UNIT_ASSERT_EQUAL(r1.left(), Int(0));
            PT_UNIT_ASSERT_EQUAL(r1.top(), Int(0));
            PT_UNIT_ASSERT_EQUAL(r1.right(), Int(15));
            PT_UNIT_ASSERT_EQUAL(r1.bottom(), Int(15));

            // unify with null rect does nothing
            RectI r3(PointI(1, 2), SizeI(3, 4));
            RectI null;
            r3.unify(null);
            PT_UNIT_ASSERT_EQUAL(r3.left(), Int(1));
            PT_UNIT_ASSERT_EQUAL(r3.top(), Int(2));
            PT_UNIT_ASSERT_EQUAL(r3.width(), Int(3));
            PT_UNIT_ASSERT_EQUAL(r3.height(), Int(4));

            // null rect unify with non-null becomes the other rect
            RectI r4;
            r4.unify(r3);
            PT_UNIT_ASSERT(r4 == r3);
        }

        void Intersect()
        {
            RectI r1(PointI(0, 0), SizeI(10, 10));
            RectI r2(PointI(5, 5), SizeI(10, 10));

            RectI inter = r1.toIntersected(r2);
            PT_UNIT_ASSERT_EQUAL(inter.left(), Int(5));
            PT_UNIT_ASSERT_EQUAL(inter.top(), Int(5));
            PT_UNIT_ASSERT_EQUAL(inter.right(), Int(10));
            PT_UNIT_ASSERT_EQUAL(inter.bottom(), Int(10));

            // non-overlapping rects produce null intersection
            RectI r3(PointI(0, 0), SizeI(5, 5));
            RectI r4(PointI(10, 10), SizeI(5, 5));
            RectI noInter = r3.toIntersected(r4);
            PT_UNIT_ASSERT(noInter.isEmpty());

            // touching at edge
            RectI r5(PointI(0, 0), SizeI(5, 5));
            RectI r6(PointI(5, 0), SizeI(5, 5));
            RectI edgeInter = r5.toIntersected(r6);
            PT_UNIT_ASSERT_EQUAL(edgeInter.width(), Int(0));
        }

        void Contains()
        {
            RectI r(PointI(10, 20), SizeI(30, 40));

            // inside
            PT_UNIT_ASSERT(r.contains(PointI(15, 25)));

            // on top-left corner (inclusive)
            PT_UNIT_ASSERT(r.contains(PointI(10, 20)));

            // on bottom-right edge (exclusive)
            PT_UNIT_ASSERT( ! r.contains(PointI(40, 60)) );

            // outside
            PT_UNIT_ASSERT( ! r.contains(PointI(5, 25)) );
            PT_UNIT_ASSERT( ! r.contains(PointI(15, 65)) );
        }

        void Equality()
        {
            RectI r1(PointI(1, 2), SizeI(3, 4));
            RectI r2(PointI(1, 2), SizeI(3, 4));
            RectI r3(PointI(1, 2), SizeI(3, 5));

            PT_UNIT_ASSERT(r1 == r2);
            PT_UNIT_ASSERT(r1 != r3);
            PT_UNIT_ASSERT( ! (r1 != r2) );
            PT_UNIT_ASSERT( ! (r1 == r3) );
        }

        void ConstructFromLTRB()
        {
            RectI r = RectI::fromLTRB(10, 20, 50, 60);
            PT_UNIT_ASSERT_EQUAL(r.left(), Int(10));
            PT_UNIT_ASSERT_EQUAL(r.top(), Int(20));
            PT_UNIT_ASSERT_EQUAL(r.right(), Int(50));
            PT_UNIT_ASSERT_EQUAL(r.bottom(), Int(60));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(40));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(40));
        }

        void ConstructFromXYWH()
        {
            RectI r = RectI::fromXYWH(10, 20, 40, 30);
            PT_UNIT_ASSERT_EQUAL(r.x(), Int(10));
            PT_UNIT_ASSERT_EQUAL(r.y(), Int(20));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(40));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(30));
        }

        void ContainsRect()
        {
            RectI outer(PointI(0, 0), SizeI(100, 100));
            RectI inner(PointI(10, 10), SizeI(50, 50));
            PT_UNIT_ASSERT(outer.contains(inner));
            PT_UNIT_ASSERT( ! inner.contains(outer) );

            RectI partial(PointI(50, 50), SizeI(100, 100));
            PT_UNIT_ASSERT( ! outer.contains(partial) );
        }

        void Intersects()
        {
            RectI r1(PointI(0, 0), SizeI(10, 10));
            RectI r2(PointI(5, 5), SizeI(10, 10));
            PT_UNIT_ASSERT(r1.intersects(r2));
            PT_UNIT_ASSERT(r2.intersects(r1));

            RectI r3(PointI(20, 20), SizeI(10, 10));
            PT_UNIT_ASSERT( ! r1.intersects(r3) );

            RectI adjacent(PointI(10, 0), SizeI(10, 10));
            PT_UNIT_ASSERT( ! r1.intersects(adjacent) );
        }

        void Center()
        {
            RectI r(PointI(0, 0), SizeI(10, 20));
            PointI c = r.center();
            PT_UNIT_ASSERT_EQUAL(c.x(), Int(5));
            PT_UNIT_ASSERT_EQUAL(c.y(), Int(10));
        }

        void Normalize()
        {
            RectI r(PointI(5, 5), SizeI(-3, -4));
            r.normalize();
            PT_UNIT_ASSERT_EQUAL(r.left(), Int(2));
            PT_UNIT_ASSERT_EQUAL(r.top(), Int(1));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(3));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(4));
        }

        void ToNormalized()
        {
            RectI r(PointI(5, 5), SizeI(-3, -4));
            RectI n = r.toNormalized();
            PT_UNIT_ASSERT_EQUAL(n.left(), Int(2));
            PT_UNIT_ASSERT_EQUAL(n.top(), Int(1));
            PT_UNIT_ASSERT_EQUAL(n.width(), Int(3));
            PT_UNIT_ASSERT_EQUAL(n.height(), Int(4));
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(-3));
        }
};

} // namespace Gfx

} // namespace Pt

Pt::Unit::RegisterTest<Pt::Gfx::RectTest> register_RectTest;
Pt::Unit::RegisterTest<Pt::Gfx::RectITest> register_RectITest;
