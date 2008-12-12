/*
 * Copyright (C) 2006 PTV AG
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



