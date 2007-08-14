#undef PT_MATH_API_EXPORT

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/TestMain.h>
#include <Pt/Unit/RegisterTest.h>

#include "Pt/SerializationInfo.h"
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
