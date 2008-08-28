/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/
#undef PT_MATH_API_EXPORT

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

#include "Pt/SerializationInfo.h"
#include <Pt/Math/MathUtils.h>
#include <Pt/Math/Rect.h>


class BasicRectTest : public Pt::Unit::TestSuite
{
public:
    BasicRectTest()
    : TestSuite("BasicRectTest")
    {
        Pt::Unit::TestSuite::registerMethod("testConstructors", *this, &BasicRectTest::testConstructors);
        Pt::Unit::TestSuite::registerMethod("testGetters", *this, &BasicRectTest::testGetters);
        Pt::Unit::TestSuite::registerMethod("testSet", *this, &BasicRectTest::testSet);
        Pt::Unit::TestSuite::registerMethod("testComparisonOperators", *this, &BasicRectTest::testComparisonOperators);
    }

protected:
    void testConstructors();
    void testGetters();
    void testSet();
    void testComparisonOperators();
};

Pt::Unit::RegisterTest<BasicRectTest> register_MathBasicRectTest;


void BasicRectTest::testConstructors()
{
    using namespace Pt::Math;

    BasicRect<double, double> r1;
    PT_UNIT_ASSERT(r1.x()      == 0);
    PT_UNIT_ASSERT(r1.y()      == 0);
    PT_UNIT_ASSERT(r1.width()  == 0);
    PT_UNIT_ASSERT(r1.height() == 0);

    BasicRect<double, double> r2(BasicPoint<double>(2.25, 3.75));
    PT_UNIT_ASSERT(r2.x()      == 2.25);
    PT_UNIT_ASSERT(r2.y()      == 3.75);
    PT_UNIT_ASSERT(r2.width()  == 0);
    PT_UNIT_ASSERT(r2.height() == 0);

    BasicRect<double, double> r3(BasicPoint<double>(12.25, 13.75), BasicSize<double>(4.125, -1.5));
    PT_UNIT_ASSERT(r3.x()      == 12.25);
    PT_UNIT_ASSERT(r3.y()      == 13.75);
    PT_UNIT_ASSERT(r3.width()  == 4.125);
    PT_UNIT_ASSERT(r3.height() == -1.5);

    BasicRect<int, int> r4(BasicPoint<int>(12, 13), BasicSize<int>(4, -1));
    PT_UNIT_ASSERT(r4.x()      == 12);
    PT_UNIT_ASSERT(r4.y()      == 13);
    PT_UNIT_ASSERT(r4.width()  == 4);
    PT_UNIT_ASSERT(r4.height() == -1);
}


void BasicRectTest::testGetters()
{
    using namespace Pt::Math;

    BasicRect<double, double> r1;
    PT_UNIT_ASSERT(r1.origin() == BasicPoint<double>(0, 0));
    PT_UNIT_ASSERT(r1.size()   == BasicSize<double>(0, 0));
    PT_UNIT_ASSERT(r1.isNull());

    BasicRect<double, double> r2(BasicPoint<double>(2.25, 3.75));
    PT_UNIT_ASSERT(r2.origin() == BasicPoint<double>(2.25, 3.75));
    PT_UNIT_ASSERT(r2.size()   == BasicSize<double>(0, 0));
    PT_UNIT_ASSERT(r2.isNull());

    BasicRect<double, double> r3(BasicPoint<double>(12.25, 13.75), BasicSize<double>(4.125, -1.5));
    PT_UNIT_ASSERT(r3.origin() == BasicPoint<double>(12.25, 13.75));
    PT_UNIT_ASSERT(r3.size()   == BasicSize<double>(4.125, -1.5));
    PT_UNIT_ASSERT(!r3.isNull());

    BasicRect<int, int> r4(BasicPoint<int>(12, 13), BasicSize<int>(4, -1));
    PT_UNIT_ASSERT(r4.origin() == BasicPoint<int>(12, 13));
    PT_UNIT_ASSERT(r4.size()   == BasicSize<int>(4, -1));
}

void BasicRectTest::testSet()
{
    using namespace Pt::Math;

    BasicRect<double, double> r1;
    PT_UNIT_ASSERT(r1.origin() == BasicPoint<double>(0, 0));
    PT_UNIT_ASSERT(r1.size()   == BasicSize<double>(0, 0));

    // Test setGeometry()
    r1.setGeometry(BasicPoint<double>(12.25, 13.75), BasicSize<double>(4.125, -1.5));
    PT_UNIT_ASSERT(r1.x()      == 12.25);
    PT_UNIT_ASSERT(r1.y()      == 13.75);
    PT_UNIT_ASSERT(r1.width()  == 4.125);
    PT_UNIT_ASSERT(r1.height() == -1.5);
    
    // Test setOrigin()
    r1.setOrigin(BasicPoint<double>(2.25, 3.75));
    PT_UNIT_ASSERT(r1.x()      == 2.25);
    PT_UNIT_ASSERT(r1.y()      == 3.75);
    PT_UNIT_ASSERT(r1.width()  == 4.125);
    PT_UNIT_ASSERT(r1.height() == -1.5);

    // Test setOrigin()
    r1.setOrigin(4, 5);
    PT_UNIT_ASSERT(r1.x()      == 4);
    PT_UNIT_ASSERT(r1.y()      == 5);
    PT_UNIT_ASSERT(r1.width()  == 4.125);
    PT_UNIT_ASSERT(r1.height() == -1.5);

    // Test setX()
    r1.setX(77);
    PT_UNIT_ASSERT(r1.x()      == 77);
    PT_UNIT_ASSERT(r1.y()      == 5);

    // Test setY()
    r1.setY(99);
    PT_UNIT_ASSERT(r1.x()      == 77);
    PT_UNIT_ASSERT(r1.y()      == 99);

    // Test setSize()
    r1.setSize(BasicSize<double>(3.25, 23.75));
    PT_UNIT_ASSERT(r1.x()      == 77);
    PT_UNIT_ASSERT(r1.y()      == 99);
    PT_UNIT_ASSERT(r1.width()  == 3.25);
    PT_UNIT_ASSERT(r1.height() == 23.75);

    // Test setSize()
    r1.setSize(-5.75, 99.75);
    PT_UNIT_ASSERT(r1.x()      == 77);
    PT_UNIT_ASSERT(r1.y()      == 99);
    PT_UNIT_ASSERT(r1.width()  == -5.75);
    PT_UNIT_ASSERT(r1.height() == 99.75);

    // Test setWidth()
    r1.setWidth(88);
    PT_UNIT_ASSERT(r1.width()  == 88);
    PT_UNIT_ASSERT(r1.height() == 99.75);

    // Test setHeight()
    r1.setHeight(111);
    PT_UNIT_ASSERT(r1.width()  == 88);
    PT_UNIT_ASSERT(r1.height() == 111);
}

void BasicRectTest::testComparisonOperators()
{
    using namespace Pt::Math;

    // Test operator==, operator!=
    BasicRect<double, double> r1(BasicPoint<double>(12.25, 13.75), BasicSize<double>(4.125, -1.5));
    BasicRect<double, double> r2(BasicPoint<double>(12.25, 13.75), BasicSize<double>(4.125, -1.5));
    BasicRect<double, double> r3(BasicPoint<double>(112.75, 13.75), BasicSize<double>(4.125, -1.5));

    PT_UNIT_ASSERT(r1 == r1);
    PT_UNIT_ASSERT(r1 == r2);
    PT_UNIT_ASSERT(r1 != r3);
    PT_UNIT_ASSERT(r1 != r3);
}
