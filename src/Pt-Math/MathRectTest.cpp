/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/
#undef PT_MATH_API_EXPORT

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include <string>
#include <iostream>
using namespace std;

#include <Pt/Math/Math.h>
#include <Pt/Math/Rect.h>
#include <Pt/Math/Point.h>
#include <Pt/Math/Size.h>

class MathRectTest : public Pt::Unit::TestSuite
{
public:
    MathRectTest()
    : TestSuite("MathRectTest")
    {
        Pt::Unit::TestSuite::registerMethod("testCtor1", *this, &MathRectTest::testCtor1);
        Pt::Unit::TestSuite::registerMethod("testCtor2", *this, &MathRectTest::testCtor2);
    }

protected:
    void testCtor1();
    void testCtor2();
};


Pt::Unit::RegisterTest<MathRectTest> register_MathRectTest;


void MathRectTest::testCtor1()
{
    Pt::Math::Rect r(Pt::Math::Point(0,0), Pt::Math::Size(10,10));

    PT_UNIT_ASSERT( r.x() == 0 && r.y() == 0 && r.width() == 10 && r.height() == 10 );

    r.setSize(Pt::Math::Size(20,20));
    PT_UNIT_ASSERT( r.width() == 20 );

    PT_UNIT_ASSERT( r.height() == 20 );
}

void MathRectTest::testCtor2()
{
    Pt::Math::Rect r;

    r.setGeometry(Pt::Math::Point(0,0) ,Pt::Math::Size(10,10));

    PT_UNIT_ASSERT( r.x() == 0 && r.y() == 0 && r.width() == 10 && r.height() == 10 );

    r.setOrigin(Pt::Math::Point(5,8));

    PT_UNIT_ASSERT( r.x() == 5 );

    PT_UNIT_ASSERT( r.y() == 8 );

    PT_UNIT_ASSERT( r.width() == 10 );
    PT_UNIT_ASSERT( r.height() == 10 );
}



