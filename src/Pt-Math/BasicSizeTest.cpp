/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/
#undef PT_MATH_API_EXPORT

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

#include "Pt/SerializationInfo.h"
#include <Pt/Math/MathUtils.h>
#include <Pt/Math/Size.h>


class BasicSizeTest : public Pt::Unit::TestSuite
{
public:
    BasicSizeTest()
    : TestSuite("BasicSizeTest")
    {
        Pt::Unit::TestSuite::registerMethod("testConstructors", *this, &BasicSizeTest::testConstructors);
        Pt::Unit::TestSuite::registerMethod("testAddSub", *this, &BasicSizeTest::testAddSub);
        Pt::Unit::TestSuite::registerMethod("testSet", *this, &BasicSizeTest::testSet);
        Pt::Unit::TestSuite::registerMethod("testComparisonOperators", *this, &BasicSizeTest::testComparisonOperators);
        Pt::Unit::TestSuite::registerMethod("testOperators", *this, &BasicSizeTest::testOperators);
    }

protected:
    void testConstructors();
    void testAddSub();
    void testSet();
    void testComparisonOperators();
    void testOperators();
};

Pt::Unit::RegisterTest<BasicSizeTest> register_MathBasicSizeTest;


void BasicSizeTest::testConstructors()
{
    Pt::Math::BasicSize<double> s1;
    PT_UNIT_ASSERT(s1.width()  == 0);
    PT_UNIT_ASSERT(s1.height() == 0);

    Pt::Math::BasicSize<double> s2(15.75);
    PT_UNIT_ASSERT(s2.width()  == 15.75);
    PT_UNIT_ASSERT(s2.height() == 0);

    Pt::Math::BasicSize<double> s3(13.25, 17);
    PT_UNIT_ASSERT(s3.width()  == 13.25);
    PT_UNIT_ASSERT(s3.height() == 17);

    Pt::Math::BasicSize<int> s4(-5, 8);
    PT_UNIT_ASSERT(s4.width()  == -5);
    PT_UNIT_ASSERT(s4.height() == 8);
}

void BasicSizeTest::testAddSub()
{
    Pt::Math::BasicSize<double> s(1.25, 4.75);
    PT_UNIT_ASSERT(s.width() == 1.25);
    PT_UNIT_ASSERT(s.height() == 4.75);
    
    s.addWidth(15);
    PT_UNIT_ASSERT(s.width() == 16.25);

    s.addHeight(7);
    PT_UNIT_ASSERT(s.height() == 11.75);

    s.subWidth(6);
    PT_UNIT_ASSERT(s.width() == 10.25);

    s.subHeight(3);
    PT_UNIT_ASSERT(s.height() == 8.75);
}

void BasicSizeTest::testSet()
{
    Pt::Math::BasicSize<double> s(1.25, 4.75);
    PT_UNIT_ASSERT(s.width() == 1.25);
    PT_UNIT_ASSERT(s.height() == 4.75);

    s.setWidth(5.125);
    PT_UNIT_ASSERT(s.width() == 5.125);

    s.setHeight(-3.5);
    PT_UNIT_ASSERT(s.height() == -3.5);

    s.setWidthHeight(7, 4);
    PT_UNIT_ASSERT(s.width() == 7);
    PT_UNIT_ASSERT(s.height() == 4);
}

void BasicSizeTest::testComparisonOperators()
{
    // Test operator==, operator!=
    {
        Pt::Math::BasicSize<double> s1(1.25, 4.75);
        Pt::Math::BasicSize<double> s2(1.25, 4.75);
        Pt::Math::BasicSize<double> s3(1, 4);
        
        PT_UNIT_ASSERT(s1 == s1);
        PT_UNIT_ASSERT(s1 == s2);
        PT_UNIT_ASSERT(s1 != s3);
        PT_UNIT_ASSERT(s2 != s3);
    }

    // Test operator==, operator!=
    {
        Pt::Math::BasicSize<int> s1(1, 4);
        Pt::Math::BasicSize<int> s2(1, 4);
        Pt::Math::BasicSize<int> s3(5, 11);

        PT_UNIT_ASSERT(s1 == s1);
        PT_UNIT_ASSERT(s1 == s2);
        PT_UNIT_ASSERT(s1 != s3);
        PT_UNIT_ASSERT(s2 != s3);
    }

    // Test operator>, operator<
    {
        Pt::Math::BasicSize<int> s1(1, 4);
        Pt::Math::BasicSize<int> s2(2, 4);
        Pt::Math::BasicSize<int> s3(1, 5);
        
        PT_UNIT_ASSERT(s2 > s1);
        PT_UNIT_ASSERT(s3 > s1);
        PT_UNIT_ASSERT(s1 < s2);
        PT_UNIT_ASSERT(s1 < s3);
    }
}


void BasicSizeTest::testOperators()
{
    // Test operator=
    {
        Pt::Math::BasicSize<double> s1(1.25, 4.75);
        Pt::Math::BasicSize<double> s2;
        
        s2 = s1;
        PT_UNIT_ASSERT(s2.width() == 1.25);
        PT_UNIT_ASSERT(s2.height() == 4.75);
    }    
}
