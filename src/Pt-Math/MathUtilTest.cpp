/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/
#undef PT_MATH_API_EXPORT

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include <Pt/Math/MathUtils.h>


class MathUtilTest : public Pt::Unit::TestSuite
{
public:
    MathUtilTest()
    : TestSuite("MathUtilTest")
    {
        Pt::Unit::TestSuite::registerMethod("testFastAccurateSin", *this, &MathUtilTest::testFastAccurateSin);
        Pt::Unit::TestSuite::registerMethod("testFastSin", *this, &MathUtilTest::testFastSin);
        Pt::Unit::TestSuite::registerMethod("testFastAccurateCos", *this, &MathUtilTest::testFastAccurateCos);
        Pt::Unit::TestSuite::registerMethod("testFastCos", *this, &MathUtilTest::testFastCos);
        Pt::Unit::TestSuite::registerMethod("testGetCenterOfLine", *this, &MathUtilTest::testGetCenterOfLine);
        Pt::Unit::TestSuite::registerMethod("testAbs", *this, &MathUtilTest::testAbs);
        Pt::Unit::TestSuite::registerMethod("testRound", *this, &MathUtilTest::testRound);
        Pt::Unit::TestSuite::registerMethod("testRadDegConversion", *this, &MathUtilTest::testRadDegConversion);
    }

protected:
    void testFastAccurateSin();
    void testFastSin();
    void testFastAccurateCos();
    void testFastCos();
    void testGetCenterOfLine();
    void testAbs();
    void testRound();
    void testRadDegConversion();
};

Pt::Unit::RegisterTest<MathUtilTest> register_MathUtilTest;


void MathUtilTest::testFastAccurateSin()
{
    // test fast accurate sine
    double buffer = Pt::Math::fastSin<double, true>(0);
    PT_UNIT_ASSERT( std::fabs(buffer) < 0.0015 );

    buffer = Pt::Math::fastSin<double, true>(Pt::Math::PI_HALF);
    PT_UNIT_ASSERT( std::fabs(buffer - 1) < 0.0015 );

    buffer = Pt::Math::fastSin<double, true>(3*Pt::Math::PI_HALF);
    PT_UNIT_ASSERT( std::fabs(buffer + 1) < 0.0015 );

    buffer = Pt::Math::fastSin<double, true>(Pt::Math::PI);
    PT_UNIT_ASSERT( std::fabs(buffer) < 0.0015 );

    buffer = Pt::Math::fastSin<double, true>(Pt::Math::PI/6);
    PT_UNIT_ASSERT( std::fabs(buffer - 0.5) < 0.0015 );

    //Sin (2*PI) = 0
    buffer = Pt::Math::fastSin<double, true>(2*Pt::Math::PI);
    PT_UNIT_ASSERT( std::fabs(buffer ) < 0.0015 );
}

void MathUtilTest::testFastSin()
{
    // test fast accurate sine
    double buffer = Pt::Math::fastSin<double, false>(0);
    PT_UNIT_ASSERT( buffer == 0 );

    buffer = Pt::Math::fastSin<double, false>(Pt::Math::PI_HALF);
    PT_UNIT_ASSERT( std::fabs(buffer - 1) < 0.06 );

    buffer = Pt::Math::fastSin<double, false>(3*Pt::Math::PI_HALF);
    PT_UNIT_ASSERT( std::fabs(buffer + 1) < 0.06 );

    buffer = Pt::Math::fastSin<double, false>(Pt::Math::PI);
    PT_UNIT_ASSERT( std::fabs(buffer) < 0.06 );

    buffer = Pt::Math::fastSin<double, false>(Pt::Math::PI/6);
    PT_UNIT_ASSERT( std::fabs(buffer - 0.5) < 0.06 );

    //Sin (2*PI) = 0
    buffer = Pt::Math::fastSin<double, false>(2*Pt::Math::PI);
    PT_UNIT_ASSERT( std::fabs(buffer ) < 0.06 );
}


void MathUtilTest::testFastAccurateCos()
{
    // test fast accurate cosine
    //cos(0) = 1
    double buffer = Pt::Math::fastCos<double, true>(0);
    PT_UNIT_ASSERT( std::fabs(buffer - 1) < 0.0015 );

    //cos(PI/2) = 0
    buffer = Pt::Math::fastCos<double, true>(Pt::Math::PI_HALF);
    PT_UNIT_ASSERT( std::fabs(buffer) < 0.0015 );

    //cos(-PI/2) = 0
    buffer = Pt::Math::fastCos<double, true>(3*Pt::Math::PI_HALF);
    PT_UNIT_ASSERT( std::fabs(buffer) < 0.0015 );

    //cos(PI) = -1
    buffer = Pt::Math::fastCos<double, true>(Pt::Math::PI);
    PT_UNIT_ASSERT( std::fabs(buffer + 1) < 0.0015 );

    //cos(PI/3) = 0.5
    buffer = Pt::Math::fastCos<double, true>(Pt::Math::PI/3);
    PT_UNIT_ASSERT( std::fabs(buffer - 0.5) < 0.0015 );

    //cos(2*PI) = 1
    buffer = Pt::Math::fastCos<double, true>(2*Pt::Math::PI);
    PT_UNIT_ASSERT( std::fabs(buffer - 1) < 0.0015 );
}

void MathUtilTest::testFastCos()
{
    // test fast cosine
    //cos(0) = 1
    double buffer = Pt::Math::fastCos<double, false>(0);
    PT_UNIT_ASSERT( std::fabs(buffer - 1) < 0.06 );

    //cos(PI/2) = 0
    buffer = Pt::Math::fastCos<double, false>(Pt::Math::PI_HALF);
    PT_UNIT_ASSERT( std::fabs(buffer) < 0.06 );

    //cos(-PI/2) = 0
    buffer = Pt::Math::fastCos<double, false>(3*Pt::Math::PI_HALF);
    PT_UNIT_ASSERT( std::fabs(buffer) < 0.06 );

    //cos(PI) = -1
    buffer = Pt::Math::fastCos<double, false>(Pt::Math::PI);
    PT_UNIT_ASSERT( std::fabs(buffer + 1) < 0.06 );

    //cos(PI/3) = 0.5
    buffer = Pt::Math::fastCos<double, false>(2*Pt::Math::PI/3);
    PT_UNIT_ASSERT( std::fabs(buffer + 0.5) < 0.06 );

    //cos(-PI/3) = 0.5
    buffer = Pt::Math::fastCos<double, false>(Pt::Math::PI/3);
    PT_UNIT_ASSERT( std::fabs(buffer - 0.5) < 0.06 );

    //cos(2*PI) = 1
    buffer = Pt::Math::fastCos<double, false>(2*Pt::Math::PI);
    PT_UNIT_ASSERT( std::fabs(buffer - 1) < 0.06 );
}

void MathUtilTest::testGetCenterOfLine()
{
    Pt::Math::Point p1(100, 100);
    Pt::Math::Point p2(200, 200);


    Pt::Math::Point result = Pt::Math::getCenterOfLine(p1, p2);

    PT_UNIT_ASSERT( result == Pt::Math::Point(150, 150) );
}


void MathUtilTest::testAbs()
{
    PT_UNIT_ASSERT(Pt::Math::abs(-5) == 5);
    PT_UNIT_ASSERT(Pt::Math::abs(7) == 7);
}

void MathUtilTest::testRound()
{
    PT_UNIT_ASSERT(Pt::Math::round(5) == 5);
    PT_UNIT_ASSERT(Pt::Math::round(6.5) == 7);
    PT_UNIT_ASSERT(Pt::Math::round(7.99999999999) == 8);
    PT_UNIT_ASSERT(Pt::Math::round(8.00000000001) == 8);
    PT_UNIT_ASSERT(Pt::Math::round(4.49999999999) == 4);

    PT_UNIT_ASSERT(Pt::Math::round(-5) == -5);
    PT_UNIT_ASSERT(Pt::Math::round(-6.5) == -7);
    PT_UNIT_ASSERT(Pt::Math::round(-7.99999999999) == -8);
    PT_UNIT_ASSERT(Pt::Math::round(-8.00000000001) == -8);
    PT_UNIT_ASSERT(Pt::Math::round(-4.49999999999) == -4);
}


void MathUtilTest::testRadDegConversion()
{
    PT_UNIT_ASSERT(Pt::Math::degToRad(-90.0) == -Pt::Math::PI_HALF);
    PT_UNIT_ASSERT(Pt::Math::degToRad(0.0) == 0);
    PT_UNIT_ASSERT(Pt::Math::degToRad(90.0) == Pt::Math::PI_HALF);
    PT_UNIT_ASSERT(Pt::Math::degToRad(180.0) == Pt::Math::PI);
    PT_UNIT_ASSERT(Pt::Math::degToRad(270.0) == Pt::Math::PI + Pt::Math::PI_HALF);
    PT_UNIT_ASSERT(Pt::Math::degToRad(360.0) == Pt::Math::PI * 2);

    PT_UNIT_ASSERT(Pt::Math::radToDeg(-Pt::Math::PI_HALF) == -90);
    PT_UNIT_ASSERT(Pt::Math::radToDeg(0.0) == 0);
    PT_UNIT_ASSERT(Pt::Math::radToDeg(Pt::Math::PI_HALF) == 90);
    PT_UNIT_ASSERT(Pt::Math::radToDeg(Pt::Math::PI) == 180);
    PT_UNIT_ASSERT(Pt::Math::radToDeg(Pt::Math::PI + Pt::Math::PI_HALF) == 270);
    PT_UNIT_ASSERT(Pt::Math::radToDeg(Pt::Math::PI * 2) == 360);
}
