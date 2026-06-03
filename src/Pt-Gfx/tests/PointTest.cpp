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

#include <Pt/Gfx/Point.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

namespace Pt {

namespace Gfx {

class PointTest : public Pt::Unit::TestSuite
{
    public:
        PointTest()
        : Pt::Unit::TestSuite("Pt::Gfx::PointTest")
        {
            registerMethod("DefaultConstruct", *this, &PointTest::DefaultConstruct);
            registerMethod("ConstructFromValues", *this, &PointTest::ConstructFromValues);
            registerMethod("CopyConstruct", *this, &PointTest::CopyConstruct);
            registerMethod("CopyAssign", *this, &PointTest::CopyAssign);
            registerMethod("SetValues", *this, &PointTest::SetValues);
            registerMethod("Clear", *this, &PointTest::Clear);
            registerMethod("MoveAndAdd", *this, &PointTest::MoveAndAdd);
            registerMethod("CalcDistance", *this, &PointTest::CalcDistance);
            registerMethod("ArithmeticOperators", *this, &PointTest::ArithmeticOperators);
            registerMethod("CompoundAssignment", *this, &PointTest::CompoundAssignment);
            registerMethod("Equality", *this, &PointTest::Equality);
            registerMethod("ScalarOperators", *this, &PointTest::ScalarOperators);
            registerMethod("IsNull", *this, &PointTest::IsNull);
            registerMethod("UnaryNegation", *this, &PointTest::UnaryNegation);
            registerMethod("Length", *this, &PointTest::Length);
            registerMethod("LengthSquared", *this, &PointTest::LengthSquared);
            registerMethod("Normalized", *this, &PointTest::Normalized);
            registerMethod("Round", *this, &PointTest::Round);
            registerMethod("Floor", *this, &PointTest::Floor);
            registerMethod("Ceil", *this, &PointTest::Ceil);
            registerMethod("ConstructFromPointI", *this, &PointTest::ConstructFromPointI);
        }

        void DefaultConstruct()
        {
            Point p;
            PT_UNIT_ASSERT_NEAR(p.x(), 0.0);
            PT_UNIT_ASSERT_NEAR(p.y(), 0.0);
        }

        void ConstructFromValues()
        {
            Point p(3.5, -7.25);
            PT_UNIT_ASSERT_NEAR(p.x(), 3.5);
            PT_UNIT_ASSERT_NEAR(p.y(), -7.25);
        }

        void CopyConstruct()
        {
            Point p1(1.5, 2.5);
            Point p2(p1);
            PT_UNIT_ASSERT_NEAR(p2.x(), 1.5);
            PT_UNIT_ASSERT_NEAR(p2.y(), 2.5);
        }

        void CopyAssign()
        {
            Point p1(4.0, 5.0);
            Point p2;
            p2 = p1;
            PT_UNIT_ASSERT_NEAR(p2.x(), 4.0);
            PT_UNIT_ASSERT_NEAR(p2.y(), 5.0);
        }

        void SetValues()
        {
            Point p;

            p.setX(10.0);
            PT_UNIT_ASSERT_NEAR(p.x(), 10.0);
            PT_UNIT_ASSERT_NEAR(p.y(), 0.0);

            p.setY(20.0);
            PT_UNIT_ASSERT_NEAR(p.x(), 10.0);
            PT_UNIT_ASSERT_NEAR(p.y(), 20.0);

            p.set(30.0, 40.0);
            PT_UNIT_ASSERT_NEAR(p.x(), 30.0);
            PT_UNIT_ASSERT_NEAR(p.y(), 40.0);
        }

        void Clear()
        {
            Point p(5.0, 6.0);
            p.clear();
            PT_UNIT_ASSERT_NEAR(p.x(), 0.0);
            PT_UNIT_ASSERT_NEAR(p.y(), 0.0);
        }

        void MoveAndAdd()
        {
            Point p(1.0, 2.0);

            p.addX(3.0);
            PT_UNIT_ASSERT_NEAR(p.x(), 4.0);

            p.subX(1.0);
            PT_UNIT_ASSERT_NEAR(p.x(), 3.0);

            p.addY(5.0);
            PT_UNIT_ASSERT_NEAR(p.y(), 7.0);

            p.subY(2.0);
            PT_UNIT_ASSERT_NEAR(p.y(), 5.0);

            p.move(10.0, 20.0);
            PT_UNIT_ASSERT_NEAR(p.x(), 13.0);
            PT_UNIT_ASSERT_NEAR(p.y(), 25.0);
        }

        void CalcDistance()
        {
            Point p1(0.0, 0.0);
            Point p2(3.0, 4.0);
            PT_UNIT_ASSERT_NEAR(p1.calcDistance(p2), 5.0);

            // same point
            PT_UNIT_ASSERT_NEAR(p1.calcDistance(p1), 0.0);

            // negative coordinates
            Point p3(-3.0, -4.0);
            PT_UNIT_ASSERT_NEAR(p1.calcDistance(p3), 5.0);
        }

        void ArithmeticOperators()
        {
            Point p1(2.0, 3.0);
            Point p2(4.0, 5.0);

            Point sum = p1 + p2;
            PT_UNIT_ASSERT_NEAR(sum.x(), 6.0);
            PT_UNIT_ASSERT_NEAR(sum.y(), 8.0);

            Point diff = p2 - p1;
            PT_UNIT_ASSERT_NEAR(diff.x(), 2.0);
            PT_UNIT_ASSERT_NEAR(diff.y(), 2.0);

            Point scaled = p1 * 2.0;
            PT_UNIT_ASSERT_NEAR(scaled.x(), 4.0);
            PT_UNIT_ASSERT_NEAR(scaled.y(), 6.0);

            Point divided = p2 / 2.0;
            PT_UNIT_ASSERT_NEAR(divided.x(), 2.0);
            PT_UNIT_ASSERT_NEAR(divided.y(), 2.5);
        }

        void CompoundAssignment()
        {
            Point p(1.0, 2.0);

            p += Point(3.0, 4.0);
            PT_UNIT_ASSERT_NEAR(p.x(), 4.0);
            PT_UNIT_ASSERT_NEAR(p.y(), 6.0);

            p -= Point(1.0, 1.0);
            PT_UNIT_ASSERT_NEAR(p.x(), 3.0);
            PT_UNIT_ASSERT_NEAR(p.y(), 5.0);

            p *= 2.0;
            PT_UNIT_ASSERT_NEAR(p.x(), 6.0);
            PT_UNIT_ASSERT_NEAR(p.y(), 10.0);

            p /= 2.0;
            PT_UNIT_ASSERT_NEAR(p.x(), 3.0);
            PT_UNIT_ASSERT_NEAR(p.y(), 5.0);
        }

        void Equality()
        {
            Point p1(1.0, 2.0);
            Point p2(1.0, 2.0);
            Point p3(1.0, 3.0);

            PT_UNIT_ASSERT(p1 == p2);
            PT_UNIT_ASSERT(p1 != p3);
            PT_UNIT_ASSERT( ! (p1 != p2) );
            PT_UNIT_ASSERT( ! (p1 == p3) );
        }

        void ScalarOperators()
        {
            Point p(2.0, 4.0);

            Point added = p + 1.0;
            PT_UNIT_ASSERT_NEAR(added.x(), 3.0);
            PT_UNIT_ASSERT_NEAR(added.y(), 5.0);

            Point subtracted = p - 1.0;
            PT_UNIT_ASSERT_NEAR(subtracted.x(), 1.0);
            PT_UNIT_ASSERT_NEAR(subtracted.y(), 3.0);
        }

        void IsNull()
        {
            Point p1;
            PT_UNIT_ASSERT(p1.isOrigin());

            Point p2(1.0, 0.0);
            PT_UNIT_ASSERT( ! p2.isOrigin() );

            Point p3(0.0, 1.0);
            PT_UNIT_ASSERT( ! p3.isOrigin() );
        }

        void UnaryNegation()
        {
            Point p(3.0, -4.0);
            Point n = -p;
            PT_UNIT_ASSERT_NEAR(n.x(), -3.0);
            PT_UNIT_ASSERT_NEAR(n.y(), 4.0);
        }

        void Length()
        {
            Point p(3.0, 4.0);
            PT_UNIT_ASSERT_NEAR(p.length(), 5.0);

            Point zero;
            PT_UNIT_ASSERT_NEAR(zero.length(), 0.0);
        }

        void LengthSquared()
        {
            Point p(3.0, 4.0);
            PT_UNIT_ASSERT_NEAR(p.lengthSquared(), 25.0);

            Point zero;
            PT_UNIT_ASSERT_NEAR(zero.lengthSquared(), 0.0);
        }

        void Normalized()
        {
            Point p(3.0, 4.0);
            Point n = p.toNormalized();
            PT_UNIT_ASSERT_NEAR(n.length(), 1.0);
            PT_UNIT_ASSERT_NEAR(n.x(), 3.0 / 5.0);
            PT_UNIT_ASSERT_NEAR(n.y(), 4.0 / 5.0);

            Point zero;
            Point nz = zero.toNormalized();
            PT_UNIT_ASSERT_NEAR(nz.x(), 0.0);
            PT_UNIT_ASSERT_NEAR(nz.y(), 0.0);
        }

        void Round()
        {
            Point p(2.4, 3.6);
            PointI r = p.round();
            PT_UNIT_ASSERT_EQUAL(r.x(), Int(2));
            PT_UNIT_ASSERT_EQUAL(r.y(), Int(4));

            Point neg(-2.4, -3.6);
            PointI rn = neg.round();
            PT_UNIT_ASSERT_EQUAL(rn.x(), Int(-2));
            PT_UNIT_ASSERT_EQUAL(rn.y(), Int(-4));
        }

        void Floor()
        {
            Point p(2.9, 3.1);
            PointI f = p.floor();
            PT_UNIT_ASSERT_EQUAL(f.x(), Int(2));
            PT_UNIT_ASSERT_EQUAL(f.y(), Int(3));

            Point neg(-2.1, -3.9);
            PointI fn = neg.floor();
            PT_UNIT_ASSERT_EQUAL(fn.x(), Int(-3));
            PT_UNIT_ASSERT_EQUAL(fn.y(), Int(-4));
        }

        void Ceil()
        {
            Point p(2.1, 3.9);
            PointI c = p.ceil();
            PT_UNIT_ASSERT_EQUAL(c.x(), Int(3));
            PT_UNIT_ASSERT_EQUAL(c.y(), Int(4));

            Point neg(-2.9, -3.1);
            PointI cn = neg.ceil();
            PT_UNIT_ASSERT_EQUAL(cn.x(), Int(-2));
            PT_UNIT_ASSERT_EQUAL(cn.y(), Int(-3));
        }

        void ConstructFromPointI()
        {
            PointI pi(3, -7);
            Point p(pi);
            PT_UNIT_ASSERT_NEAR(p.x(), 3.0);
            PT_UNIT_ASSERT_NEAR(p.y(), -7.0);

            Point p2;
            p2 = pi;
            PT_UNIT_ASSERT_NEAR(p2.x(), 3.0);
            PT_UNIT_ASSERT_NEAR(p2.y(), -7.0);
        }
};


class PointITest : public Pt::Unit::TestSuite
{
    public:
        PointITest()
        : Pt::Unit::TestSuite("Pt::Gfx::PointITest")
        {
            registerMethod("DefaultConstruct", *this, &PointITest::DefaultConstruct);
            registerMethod("ConstructFromValues", *this, &PointITest::ConstructFromValues);
            registerMethod("CopyConstruct", *this, &PointITest::CopyConstruct);
            registerMethod("CopyAssign", *this, &PointITest::CopyAssign);
            registerMethod("SetValues", *this, &PointITest::SetValues);
            registerMethod("Clear", *this, &PointITest::Clear);
            registerMethod("MoveAndAdd", *this, &PointITest::MoveAndAdd);
            registerMethod("ArithmeticOperators", *this, &PointITest::ArithmeticOperators);
            registerMethod("CompoundAssignment", *this, &PointITest::CompoundAssignment);
            registerMethod("Equality", *this, &PointITest::Equality);
            registerMethod("ScalarOperators", *this, &PointITest::ScalarOperators);
            registerMethod("IsNull", *this, &PointITest::IsNull);
            registerMethod("UnaryNegation", *this, &PointITest::UnaryNegation);
        }

        void DefaultConstruct()
        {
            PointI p;
            PT_UNIT_ASSERT_EQUAL(p.x(), Int(0));
            PT_UNIT_ASSERT_EQUAL(p.y(), Int(0));
        }

        void ConstructFromValues()
        {
            PointI p(3, -7);
            PT_UNIT_ASSERT_EQUAL(p.x(), Int(3));
            PT_UNIT_ASSERT_EQUAL(p.y(), Int(-7));
        }

        void CopyConstruct()
        {
            PointI p1(1, 2);
            PointI p2(p1);
            PT_UNIT_ASSERT_EQUAL(p2.x(), Int(1));
            PT_UNIT_ASSERT_EQUAL(p2.y(), Int(2));
        }

        void CopyAssign()
        {
            PointI p1(4, 5);
            PointI p2;
            p2 = p1;
            PT_UNIT_ASSERT_EQUAL(p2.x(), Int(4));
            PT_UNIT_ASSERT_EQUAL(p2.y(), Int(5));
        }

        void SetValues()
        {
            PointI p;

            p.setX(10);
            PT_UNIT_ASSERT_EQUAL(p.x(), Int(10));
            PT_UNIT_ASSERT_EQUAL(p.y(), Int(0));

            p.setY(20);
            PT_UNIT_ASSERT_EQUAL(p.x(), Int(10));
            PT_UNIT_ASSERT_EQUAL(p.y(), Int(20));

            p.set(30, 40);
            PT_UNIT_ASSERT_EQUAL(p.x(), Int(30));
            PT_UNIT_ASSERT_EQUAL(p.y(), Int(40));
        }

        void Clear()
        {
            PointI p(5, 6);
            p.clear();
            PT_UNIT_ASSERT_EQUAL(p.x(), Int(0));
            PT_UNIT_ASSERT_EQUAL(p.y(), Int(0));
        }

        void MoveAndAdd()
        {
            PointI p(1, 2);

            p.addX(3);
            PT_UNIT_ASSERT_EQUAL(p.x(), Int(4));

            p.subX(1);
            PT_UNIT_ASSERT_EQUAL(p.x(), Int(3));

            p.addY(5);
            PT_UNIT_ASSERT_EQUAL(p.y(), Int(7));

            p.subY(2);
            PT_UNIT_ASSERT_EQUAL(p.y(), Int(5));

            p.move(10, 20);
            PT_UNIT_ASSERT_EQUAL(p.x(), Int(13));
            PT_UNIT_ASSERT_EQUAL(p.y(), Int(25));
        }

        void ArithmeticOperators()
        {
            PointI p1(2, 3);
            PointI p2(4, 5);

            PointI sum = p1 + p2;
            PT_UNIT_ASSERT_EQUAL(sum.x(), Int(6));
            PT_UNIT_ASSERT_EQUAL(sum.y(), Int(8));

            PointI diff = p2 - p1;
            PT_UNIT_ASSERT_EQUAL(diff.x(), Int(2));
            PT_UNIT_ASSERT_EQUAL(diff.y(), Int(2));

            PointI scaled = p1 * 3;
            PT_UNIT_ASSERT_EQUAL(scaled.x(), Int(6));
            PT_UNIT_ASSERT_EQUAL(scaled.y(), Int(9));
        }

        void CompoundAssignment()
        {
            PointI p(1, 2);

            p += PointI(3, 4);
            PT_UNIT_ASSERT_EQUAL(p.x(), Int(4));
            PT_UNIT_ASSERT_EQUAL(p.y(), Int(6));

            p -= PointI(1, 1);
            PT_UNIT_ASSERT_EQUAL(p.x(), Int(3));
            PT_UNIT_ASSERT_EQUAL(p.y(), Int(5));

            p *= 2;
            PT_UNIT_ASSERT_EQUAL(p.x(), Int(6));
            PT_UNIT_ASSERT_EQUAL(p.y(), Int(10));
        }

        void Equality()
        {
            PointI p1(1, 2);
            PointI p2(1, 2);
            PointI p3(1, 3);

            PT_UNIT_ASSERT(p1 == p2);
            PT_UNIT_ASSERT(p1 != p3);
            PT_UNIT_ASSERT( ! (p1 != p2) );
            PT_UNIT_ASSERT( ! (p1 == p3) );
        }

        void ScalarOperators()
        {
            PointI p(2, 4);

            PointI added = p + 1;
            PT_UNIT_ASSERT_EQUAL(added.x(), Int(3));
            PT_UNIT_ASSERT_EQUAL(added.y(), Int(5));

            PointI subtracted = p - 1;
            PT_UNIT_ASSERT_EQUAL(subtracted.x(), Int(1));
            PT_UNIT_ASSERT_EQUAL(subtracted.y(), Int(3));
        }

        void IsNull()
        {
            PointI p1;
            PT_UNIT_ASSERT(p1.isOrigin());

            PointI p2(1, 0);
            PT_UNIT_ASSERT( ! p2.isOrigin() );

            PointI p3(0, 1);
            PT_UNIT_ASSERT( ! p3.isOrigin() );
        }

        void UnaryNegation()
        {
            PointI p(3, -4);
            PointI n = -p;
            PT_UNIT_ASSERT_EQUAL(n.x(), Int(-3));
            PT_UNIT_ASSERT_EQUAL(n.y(), Int(4));
        }
};

} // namespace Gfx

} // namespace Pt

Pt::Unit::RegisterTest<Pt::Gfx::PointTest> register_PointTest;
Pt::Unit::RegisterTest<Pt::Gfx::PointITest> register_PointITest;
