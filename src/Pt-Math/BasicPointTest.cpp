/*
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

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

#include "Pt/SerializationInfo.h"
//#include <Pt/Xml/XmlSerializer.h>
//#include <Pt/Xml/XmlDeserializer.h>
#include <Pt/Math/MathUtils.h>
#include <Pt/Math/Point.h>
#include <limits>


class BasicPointTest : public Pt::Unit::TestSuite
{
public:
    BasicPointTest()
    : TestSuite("BasicPointTest")
    {
        Pt::Unit::TestSuite::registerMethod("testPointUINT8Serialize", *this, &BasicPointTest::testPointUINT8Serialize);
        Pt::Unit::TestSuite::registerMethod("testPointUINT16Serialize", *this, &BasicPointTest::testPointUINT16Serialize);
        Pt::Unit::TestSuite::registerMethod("testPointUINT32Serialize", *this, &BasicPointTest::testPointUINT32Serialize);
        Pt::Unit::TestSuite::registerMethod("testPointINT8Serialize", *this, &BasicPointTest::testPointINT8Serialize);
        Pt::Unit::TestSuite::registerMethod("testPointINT16Serialize", *this, &BasicPointTest::testPointINT16Serialize);
        Pt::Unit::TestSuite::registerMethod("testPointINT32Serialize", *this, &BasicPointTest::testPointINT32Serialize);
        Pt::Unit::TestSuite::registerMethod("testPointFLOATSerialize", *this, &BasicPointTest::testPointFLOATSerialize);
        Pt::Unit::TestSuite::registerMethod("testPointDOUBLESerialize", *this, &BasicPointTest::testPointDOUBLESerialize);
        
        Pt::Unit::TestSuite::registerMethod("testConstructors", *this, &BasicPointTest::testConstructors);
        Pt::Unit::TestSuite::registerMethod("testAddSubMove", *this, &BasicPointTest::testAddSubMove);
        Pt::Unit::TestSuite::registerMethod("testSet", *this, &BasicPointTest::testSet);
        Pt::Unit::TestSuite::registerMethod("testComparisonOperators", *this, &BasicPointTest::testComparisonOperators);
        Pt::Unit::TestSuite::registerMethod("testOperators", *this, &BasicPointTest::testOperators);
    }

protected:
    void testPointUINT8Serialize();
    void testPointUINT16Serialize();
    void testPointUINT32Serialize();
    void testPointINT8Serialize();
    void testPointINT16Serialize();
    void testPointINT32Serialize();
    void testPointFLOATSerialize();
    void testPointDOUBLESerialize();
    void testConstructors();
    void testAddSubMove();
    void testSet();
    void testComparisonOperators();
    void testOperators();
};

Pt::Unit::RegisterTest<BasicPointTest> register_MathBasicPointTest;


void BasicPointTest::testPointUINT8Serialize()
{
    Pt::SerializationInfo si;
    Pt::Math::BasicPoint<Pt::uint8_t> myPoint(134,178);
    si <<= myPoint;

    Pt::Math::BasicPoint<Pt::uint8_t> myPoint2;
    si >>= myPoint2;
    PT_UNIT_ASSERT(myPoint == myPoint2);
}

void BasicPointTest::testPointUINT16Serialize()
{
    Pt::SerializationInfo si;
    Pt::Math::BasicPoint<Pt::uint16_t> myPoint(134,178);
    si <<= myPoint;

    Pt::Math::BasicPoint<Pt::uint16_t> myPoint2;
    si >>= myPoint2;
    PT_UNIT_ASSERT(myPoint == myPoint2);

    Pt::SerializationInfo si2;
    Pt::Math::BasicPoint<Pt::uint16_t> myPoint3(2332,17521);
    si2 <<= myPoint3;

    Pt::Math::BasicPoint<Pt::uint16_t> myPoint4;
    si2 >>= myPoint4;
    PT_UNIT_ASSERT(myPoint3 == myPoint4);
}

void BasicPointTest::testPointUINT32Serialize()
{
    Pt::SerializationInfo si;
    Pt::Math::BasicPoint<Pt::uint16_t> myPoint(134,178);
    si <<= myPoint;

    Pt::Math::BasicPoint<Pt::uint16_t> myPoint2;
    si >>= myPoint2;
    PT_UNIT_ASSERT(myPoint == myPoint2);

    Pt::SerializationInfo si2;
    Pt::Math::BasicPoint<Pt::uint32_t> myPoint3(2332,17521);
    si2 <<= myPoint3;

    Pt::Math::BasicPoint<Pt::uint32_t> myPoint4;
    si2 >>= myPoint4;
    PT_UNIT_ASSERT(myPoint3 == myPoint4);
}

void BasicPointTest::testPointINT8Serialize()
{
    Pt::SerializationInfo si;
    Pt::Math::BasicPoint<Pt::int8_t> myPoint(-14,34);
    si <<= myPoint;

    Pt::Math::BasicPoint<Pt::int8_t> myPoint2;
    si >>= myPoint2;
    PT_UNIT_ASSERT(myPoint == myPoint2);
}

void BasicPointTest::testPointINT16Serialize()
{
    Pt::SerializationInfo si;
    Pt::Math::BasicPoint<Pt::int16_t> myPoint(-13,-178);
    si <<= myPoint;

    Pt::Math::BasicPoint<Pt::int16_t> myPoint2;
    si >>= myPoint2;
    PT_UNIT_ASSERT(myPoint == myPoint2);

    Pt::SerializationInfo si2;
    Pt::Math::BasicPoint<Pt::int16_t> myPoint3(2332,17521);
    si2 <<= myPoint3;

    Pt::Math::BasicPoint<Pt::int16_t> myPoint4;
    si2 >>= myPoint4;
    PT_UNIT_ASSERT(myPoint3 == myPoint4);
}

void BasicPointTest::testPointINT32Serialize()
{
    Pt::SerializationInfo si;
    Pt::Math::BasicPoint<Pt::int32_t> myPoint(-2343234,235235);
    si <<= myPoint;

    Pt::Math::BasicPoint<Pt::int32_t> myPoint2;
    si >>= myPoint2;
    PT_UNIT_ASSERT(myPoint == myPoint2);
}

void BasicPointTest::testPointFLOATSerialize()
{
    Pt::SerializationInfo si2;
    Pt::Math::BasicPoint<float> myPoint3(-5.3431f,8654.321342f);
    si2 <<= myPoint3;
    Pt::Math::BasicPoint<float> myPoint4;
    si2 >>=myPoint4;

    std::cout << "Float-epsilon:" << std::numeric_limits<float>::epsilon() << std::endl;

    PT_UNIT_ASSERT(Pt::Math::equal(myPoint3.x(), myPoint4.x(), 0.002f));
    PT_UNIT_ASSERT(Pt::Math::equal(myPoint3.y(), myPoint4.y(), 0.002f));
}

void BasicPointTest::testPointDOUBLESerialize()
{
    std::cout << "Double-epsilon:" << std::numeric_limits<double>::epsilon() << std::endl;

    Pt::SerializationInfo si2;
    Pt::Math::BasicPoint<double> myPoint3(-5.3431f,8654.321342f);
    si2 <<= myPoint3;
    Pt::Math::BasicPoint<double> myPoint4;
    si2 >>= myPoint4;

    PT_UNIT_ASSERT(Pt::Math::equal(myPoint3.x(), myPoint4.x(), 0.000001f));
    PT_UNIT_ASSERT(Pt::Math::equal(myPoint3.y(), myPoint4.y(), 0.000001f));
}

void BasicPointTest::testConstructors()
{
    Pt::Math::BasicPoint<double> p1(1.23, 4.56);
    PT_UNIT_ASSERT(p1.x() == 1.23);
    PT_UNIT_ASSERT(p1.y() == 4.56);

    Pt::Math::BasicPoint<int> p2(1, 4);
    PT_UNIT_ASSERT(p2.x() == 1);
    PT_UNIT_ASSERT(p2.y() == 4);

    Pt::Math::BasicPoint<int> p3;
    PT_UNIT_ASSERT(p3.x() == 0);
    PT_UNIT_ASSERT(p3.y() == 0);

    Pt::Math::BasicPoint<double> p4(p1);
    PT_UNIT_ASSERT(p4.x() == 1.23);
    PT_UNIT_ASSERT(p4.y() == 4.56);
}

void BasicPointTest::testAddSubMove()
{
    Pt::Math::BasicPoint<double> p(1.25, 4.75);
    PT_UNIT_ASSERT(p.x() == 1.25);
    PT_UNIT_ASSERT(p.y() == 4.75);
    
    p.addX(15);
    PT_UNIT_ASSERT(p.x() == 16.25);

    p.addY(7);
    PT_UNIT_ASSERT(p.y() == 11.75);

    p.subX(6);
    PT_UNIT_ASSERT(p.x() == 10.25);

    p.subY(3);
    PT_UNIT_ASSERT(p.y() == 8.75);

    p.move(13, -5);
    PT_UNIT_ASSERT(p.x() == 23.25);
    PT_UNIT_ASSERT(p.y() == 3.75);
}

void BasicPointTest::testSet()
{
    Pt::Math::BasicPoint<double> p(1.25, 4.75);
    PT_UNIT_ASSERT(p.x() == 1.25);
    PT_UNIT_ASSERT(p.y() == 4.75);

    p.setX(5.125);
    PT_UNIT_ASSERT(p.x() == 5.125);

    p.setY(-3.5);
    PT_UNIT_ASSERT(p.y() == -3.5);

    p.set(7, 4);
    PT_UNIT_ASSERT(p.x() == 7);
    PT_UNIT_ASSERT(p.y() == 4);
}

void BasicPointTest::testComparisonOperators()
{
    // Test operator==, operator!=
    {
        Pt::Math::BasicPoint<double> p1(1.25, 4.75);
        Pt::Math::BasicPoint<double> p2(1.25, 4.75);
        Pt::Math::BasicPoint<double> p3(1, 4);
        
        PT_UNIT_ASSERT(p1 == p1);
        PT_UNIT_ASSERT(p1 == p2);
        PT_UNIT_ASSERT(p1 != p3);
        PT_UNIT_ASSERT(p2 != p3);
    }

    // Test operator==, operator!=
    {
        Pt::Math::BasicPoint<int> p1(1, 4);
        Pt::Math::BasicPoint<int> p2(1, 4);
        Pt::Math::BasicPoint<int> p3(5, 11);

        PT_UNIT_ASSERT(p1 == p1);
        PT_UNIT_ASSERT(p1 == p2);
        PT_UNIT_ASSERT(p1 != p3);
        PT_UNIT_ASSERT(p2 != p3);
    }

    // Test operator>, operator<
    {
        Pt::Math::BasicPoint<int> p1(1, 4);
        Pt::Math::BasicPoint<int> p2(2, 4);
        Pt::Math::BasicPoint<int> p3(1, 5);
        
        PT_UNIT_ASSERT(p2 > p1);
        PT_UNIT_ASSERT(p3 > p1);
        PT_UNIT_ASSERT(p1 < p2);
        PT_UNIT_ASSERT(p1 < p3);
    }
}


void BasicPointTest::testOperators()
{
    // Test operator+, operator-, operator+=, operator-=
    {
        Pt::Math::BasicPoint<double> p1(1.25, 4.75);
        Pt::Math::BasicPoint<double> p2(7.75, 2.75);
        Pt::Math::BasicPoint<double> p3(-3.5, 1.75);

        Pt::Math::BasicPoint<double> res = p1 + p2;
        PT_UNIT_ASSERT(res.x() == 9);
        PT_UNIT_ASSERT(res.y() == 7.5);

        res = p1 + p3;
        PT_UNIT_ASSERT(res.x() == -2.25);
        PT_UNIT_ASSERT(res.y() == 6.5);
        
        p1 += p2;
        PT_UNIT_ASSERT(p1.x() == 9);
        PT_UNIT_ASSERT(p1.y() == 7.5);
        
        p1 -= p3;
        PT_UNIT_ASSERT(p1.x() == 12.5);
        PT_UNIT_ASSERT(p1.y() == 5.75);
    }
    
    // Test operator=
    {
        Pt::Math::BasicPoint<double> p1(1.25, 4.75);
        Pt::Math::BasicPoint<double> p2;
        
        p2 = p1;
        PT_UNIT_ASSERT(p2.x() == 1.25);
        PT_UNIT_ASSERT(p2.y() == 4.75);
    }    
    
    // Test operator*
    {
        Pt::Math::BasicPoint<double> p1(1.25, 4.75);
        Pt::Math::BasicPoint<double> p2;

        p2 = p1 * 4;
        PT_UNIT_ASSERT(p2.x() == 1.25 * 4);
        PT_UNIT_ASSERT(p2.y() == 4.75 * 4);
    }    
}
