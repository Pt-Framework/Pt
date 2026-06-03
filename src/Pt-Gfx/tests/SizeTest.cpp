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

#include <Pt/Gfx/Size.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

namespace Pt {

namespace Gfx {

class SizeTest : public Pt::Unit::TestSuite
{
    public:
        SizeTest()
        : Pt::Unit::TestSuite("Pt::Gfx::SizeTest")
        {
            registerMethod("DefaultConstruct", *this, &SizeTest::DefaultConstruct);
            registerMethod("ConstructFromValues", *this, &SizeTest::ConstructFromValues);
            registerMethod("CopyConstruct", *this, &SizeTest::CopyConstruct);
            registerMethod("CopyAssign", *this, &SizeTest::CopyAssign);
            registerMethod("SetValues", *this, &SizeTest::SetValues);
            registerMethod("Clear", *this, &SizeTest::Clear);
            registerMethod("IsEmpty", *this, &SizeTest::IsEmpty);
            registerMethod("AddSub", *this, &SizeTest::AddSub);
            registerMethod("ArithmeticOperators", *this, &SizeTest::ArithmeticOperators);
            registerMethod("CompoundAssignment", *this, &SizeTest::CompoundAssignment);
            registerMethod("Equality", *this, &SizeTest::Equality);
            registerMethod("LessThan", *this, &SizeTest::LessThan);
            registerMethod("Area", *this, &SizeTest::Area);
            registerMethod("Transpose", *this, &SizeTest::Transpose);
            registerMethod("ToTransposed", *this, &SizeTest::ToTransposed);
            registerMethod("Round", *this, &SizeTest::Round);
            registerMethod("Floor", *this, &SizeTest::Floor);
            registerMethod("Ceil", *this, &SizeTest::Ceil);
            registerMethod("ConstructFromSizeI", *this, &SizeTest::ConstructFromSizeI);
        }

        void DefaultConstruct()
        {
            Size s;
            PT_UNIT_ASSERT_NEAR(s.width(), 0.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 0.0);
            PT_UNIT_ASSERT(s.isEmpty());
        }

        void ConstructFromValues()
        {
            Size s(10.5, 20.75);
            PT_UNIT_ASSERT_NEAR(s.width(), 10.5);
            PT_UNIT_ASSERT_NEAR(s.height(), 20.75);
        }

        void CopyConstruct()
        {
            Size s1(3.0, 4.0);
            Size s2(s1);
            PT_UNIT_ASSERT_NEAR(s2.width(), 3.0);
            PT_UNIT_ASSERT_NEAR(s2.height(), 4.0);
        }

        void CopyAssign()
        {
            Size s1(5.0, 6.0);
            Size s2;
            s2 = s1;
            PT_UNIT_ASSERT_NEAR(s2.width(), 5.0);
            PT_UNIT_ASSERT_NEAR(s2.height(), 6.0);
        }

        void SetValues()
        {
            Size s;

            s.setWidth(10.0);
            PT_UNIT_ASSERT_NEAR(s.width(), 10.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 0.0);

            s.setHeight(20.0);
            PT_UNIT_ASSERT_NEAR(s.width(), 10.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 20.0);

            s.setWidthHeight(30.0, 40.0);
            PT_UNIT_ASSERT_NEAR(s.width(), 30.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 40.0);

            s.set(50.0, 60.0);
            PT_UNIT_ASSERT_NEAR(s.width(), 50.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 60.0);
        }

        void Clear()
        {
            Size s(5.0, 6.0);
            s.clear();
            PT_UNIT_ASSERT_NEAR(s.width(), 0.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 0.0);
        }

        void IsEmpty()
        {
            Size s1(0.0, 0.0);
            PT_UNIT_ASSERT(s1.isEmpty());

            Size s2(1.0, 0.0);
            PT_UNIT_ASSERT(s2.isEmpty());

            Size s3(0.0, 1.0);
            PT_UNIT_ASSERT(s3.isEmpty());

            Size s4(1.0, 1.0);
            PT_UNIT_ASSERT( ! s4.isEmpty() );
        }

        void AddSub()
        {
            Size s(10.0, 20.0);

            s.addWidth(5.0);
            PT_UNIT_ASSERT_NEAR(s.width(), 15.0);

            s.subWidth(3.0);
            PT_UNIT_ASSERT_NEAR(s.width(), 12.0);

            s.addHeight(8.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 28.0);

            s.subHeight(4.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 24.0);

            Size s2(1.0, 2.0);
            s.add(s2);
            PT_UNIT_ASSERT_NEAR(s.width(), 13.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 26.0);
        }

        void ArithmeticOperators()
        {
            Size s(4.0, 6.0);

            Size scaled = s * 2.0;
            PT_UNIT_ASSERT_NEAR(scaled.width(), 8.0);
            PT_UNIT_ASSERT_NEAR(scaled.height(), 12.0);

            Size divided = s / 2.0;
            PT_UNIT_ASSERT_NEAR(divided.width(), 2.0);
            PT_UNIT_ASSERT_NEAR(divided.height(), 3.0);

            Size added = s + 1.0;
            PT_UNIT_ASSERT_NEAR(added.width(), 5.0);
            PT_UNIT_ASSERT_NEAR(added.height(), 7.0);

            Size subtracted = s - 1.0;
            PT_UNIT_ASSERT_NEAR(subtracted.width(), 3.0);
            PT_UNIT_ASSERT_NEAR(subtracted.height(), 5.0);
        }

        void CompoundAssignment()
        {
            Size s(4.0, 6.0);

            s += Size(1.0, 2.0);
            PT_UNIT_ASSERT_NEAR(s.width(), 5.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 8.0);

            s *= 2.0;
            PT_UNIT_ASSERT_NEAR(s.width(), 10.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 16.0);

            s /= 4.0;
            PT_UNIT_ASSERT_NEAR(s.width(), 2.5);
            PT_UNIT_ASSERT_NEAR(s.height(), 4.0);

            s += 0.5;
            PT_UNIT_ASSERT_NEAR(s.width(), 3.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 4.5);

            s -= 0.5;
            PT_UNIT_ASSERT_NEAR(s.width(), 2.5);
            PT_UNIT_ASSERT_NEAR(s.height(), 4.0);
        }

        void Equality()
        {
            Size s1(3.0, 4.0);
            Size s2(3.0, 4.0);
            Size s3(3.0, 5.0);

            PT_UNIT_ASSERT(s1.isEqual(s2));
            PT_UNIT_ASSERT( ! s1.isEqual(s3));
            PT_UNIT_ASSERT( ! ( ! s1.isEqual(s2)) );
            PT_UNIT_ASSERT( ! s1.isEqual(s3) );
        }

        void LessThan()
        {
            // height-first comparison
            Size s1(10.0, 1.0);
            Size s2(1.0, 2.0);
            PT_UNIT_ASSERT(s1 < s2);

            // same height, compare width
            Size s3(1.0, 5.0);
            Size s4(2.0, 5.0);
            PT_UNIT_ASSERT(s3 < s4);

            // equal
            Size s5(3.0, 3.0);
            Size s6(3.0, 3.0);
            PT_UNIT_ASSERT( ! (s5 < s6) );
        }

        void Area()
        {
            Size s(3.0, 4.0);
            PT_UNIT_ASSERT_NEAR(s.area(), 12.0);

            Size zero(0.0, 5.0);
            PT_UNIT_ASSERT_NEAR(zero.area(), 0.0);
        }

        void Transpose()
        {
            Size s(3.0, 5.0);
            s.transpose();
            PT_UNIT_ASSERT_NEAR(s.width(), 5.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 3.0);
        }

        void ToTransposed()
        {
            Size s(3.0, 5.0);
            Size t = s.toTransposed();
            PT_UNIT_ASSERT_NEAR(t.width(), 5.0);
            PT_UNIT_ASSERT_NEAR(t.height(), 3.0);
            PT_UNIT_ASSERT_NEAR(s.width(), 3.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 5.0);
        }

        void Round()
        {
            Size s(2.4, 3.6);
            SizeI r = s.round();
            PT_UNIT_ASSERT_EQUAL(r.width(), Int(2));
            PT_UNIT_ASSERT_EQUAL(r.height(), Int(4));
        }

        void Floor()
        {
            Size s(2.9, 3.1);
            SizeI f = s.floor();
            PT_UNIT_ASSERT_EQUAL(f.width(), Int(2));
            PT_UNIT_ASSERT_EQUAL(f.height(), Int(3));
        }

        void Ceil()
        {
            Size s(2.1, 3.9);
            SizeI c = s.ceil();
            PT_UNIT_ASSERT_EQUAL(c.width(), Int(3));
            PT_UNIT_ASSERT_EQUAL(c.height(), Int(4));
        }

        void ConstructFromSizeI()
        {
            SizeI si(10, 20);
            Size s(si);
            PT_UNIT_ASSERT_NEAR(s.width(), 10.0);
            PT_UNIT_ASSERT_NEAR(s.height(), 20.0);

            Size s2;
            s2 = si;
            PT_UNIT_ASSERT_NEAR(s2.width(), 10.0);
            PT_UNIT_ASSERT_NEAR(s2.height(), 20.0);
        }
};


class SizeITest : public Pt::Unit::TestSuite
{
    public:
        SizeITest()
        : Pt::Unit::TestSuite("Pt::Gfx::SizeITest")
        {
            registerMethod("DefaultConstruct", *this, &SizeITest::DefaultConstruct);
            registerMethod("ConstructFromValues", *this, &SizeITest::ConstructFromValues);
            registerMethod("CopyConstruct", *this, &SizeITest::CopyConstruct);
            registerMethod("CopyAssign", *this, &SizeITest::CopyAssign);
            registerMethod("SetValues", *this, &SizeITest::SetValues);
            registerMethod("Clear", *this, &SizeITest::Clear);
            registerMethod("IsEmpty", *this, &SizeITest::IsEmpty);
            registerMethod("AddSub", *this, &SizeITest::AddSub);
            registerMethod("ArithmeticOperators", *this, &SizeITest::ArithmeticOperators);
            registerMethod("CompoundAssignment", *this, &SizeITest::CompoundAssignment);
            registerMethod("Equality", *this, &SizeITest::Equality);
            registerMethod("Area", *this, &SizeITest::Area);
            registerMethod("Transpose", *this, &SizeITest::Transpose);
            registerMethod("ToTransposed", *this, &SizeITest::ToTransposed);
        }

        void DefaultConstruct()
        {
            SizeI s;
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(0));
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(0));
            PT_UNIT_ASSERT(s.isEmpty());
        }

        void ConstructFromValues()
        {
            SizeI s(10, 20);
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(10));
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(20));
        }

        void CopyConstruct()
        {
            SizeI s1(3, 4);
            SizeI s2(s1);
            PT_UNIT_ASSERT_EQUAL(s2.width(), Int(3));
            PT_UNIT_ASSERT_EQUAL(s2.height(), Int(4));
        }

        void CopyAssign()
        {
            SizeI s1(5, 6);
            SizeI s2;
            s2 = s1;
            PT_UNIT_ASSERT_EQUAL(s2.width(), Int(5));
            PT_UNIT_ASSERT_EQUAL(s2.height(), Int(6));
        }

        void SetValues()
        {
            SizeI s;

            s.setWidth(10);
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(10));
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(0));

            s.setHeight(20);
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(10));
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(20));

            s.setWidthHeight(30, 40);
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(30));
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(40));

            s.set(50, 60);
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(50));
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(60));
        }

        void Clear()
        {
            SizeI s(5, 6);
            s.clear();
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(0));
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(0));
        }

        void IsEmpty()
        {
            SizeI s1(0, 0);
            PT_UNIT_ASSERT(s1.isEmpty());

            SizeI s2(1, 0);
            PT_UNIT_ASSERT(s2.isEmpty());

            SizeI s3(0, 1);
            PT_UNIT_ASSERT(s3.isEmpty());

            SizeI s4(1, 1);
            PT_UNIT_ASSERT( ! s4.isEmpty() );
        }

        void AddSub()
        {
            SizeI s(10, 20);

            s.addWidth(5);
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(15));

            s.subWidth(3);
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(12));

            s.addHeight(8);
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(28));

            s.subHeight(4);
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(24));

            SizeI s2(1, 2);
            s.add(s2);
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(13));
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(26));
        }

        void ArithmeticOperators()
        {
            SizeI s(4, 6);

            SizeI scaled = s * 2;
            PT_UNIT_ASSERT_EQUAL(scaled.width(), Int(8));
            PT_UNIT_ASSERT_EQUAL(scaled.height(), Int(12));

            SizeI added = s + 1;
            PT_UNIT_ASSERT_EQUAL(added.width(), Int(5));
            PT_UNIT_ASSERT_EQUAL(added.height(), Int(7));

            SizeI subtracted = s - 1;
            PT_UNIT_ASSERT_EQUAL(subtracted.width(), Int(3));
            PT_UNIT_ASSERT_EQUAL(subtracted.height(), Int(5));
        }

        void CompoundAssignment()
        {
            SizeI s(4, 6);

            s += SizeI(1, 2);
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(5));
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(8));

            s *= 2;
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(10));
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(16));

            s += 1;
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(11));
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(17));

            s -= 1;
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(10));
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(16));
        }

        void Equality()
        {
            SizeI s1(3, 4);
            SizeI s2(3, 4);
            SizeI s3(3, 5);

            PT_UNIT_ASSERT(s1 == s2);
            PT_UNIT_ASSERT(s1 != s3);
            PT_UNIT_ASSERT( ! (s1 != s2) );
            PT_UNIT_ASSERT( ! (s1 == s3) );
        }

        void Area()
        {
            SizeI s(3, 4);
            PT_UNIT_ASSERT_EQUAL(s.area(), Int(12));
        }

        void Transpose()
        {
            SizeI s(3, 5);
            s.transpose();
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(5));
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(3));
        }

        void ToTransposed()
        {
            SizeI s(3, 5);
            SizeI t = s.toTransposed();
            PT_UNIT_ASSERT_EQUAL(t.width(), Int(5));
            PT_UNIT_ASSERT_EQUAL(t.height(), Int(3));
            PT_UNIT_ASSERT_EQUAL(s.width(), Int(3));
            PT_UNIT_ASSERT_EQUAL(s.height(), Int(5));
        }
};

} // namespace Gfx

} // namespace Pt

Pt::Unit::RegisterTest<Pt::Gfx::SizeTest> register_SizeTest;
Pt::Unit::RegisterTest<Pt::Gfx::SizeITest> register_SizeITest;
