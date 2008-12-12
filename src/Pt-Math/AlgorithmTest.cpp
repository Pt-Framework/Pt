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

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"


#include <Pt/Math/Algorithm.h>



class AlgorithmTest : public Pt::Unit::TestSuite
{
public:
    AlgorithmTest()
    : TestSuite("AlgorithmTest")
    {
        Pt::Unit::TestSuite::registerMethod("testIntersect", *this, &AlgorithmTest::testIntersect);
        Pt::Unit::TestSuite::registerMethod("testQuadrant", *this, &AlgorithmTest::testQuadrant);
    }

protected:
    void testIntersect();
    void testQuadrant();
};


Pt::Unit::RegisterTest<AlgorithmTest> register_AlgorithmTest;



void AlgorithmTest::testIntersect()
{
    using namespace Pt::Math;
    
    //bool ret = intersect(T x1, T y1, T x2, T y2, U xmin, U xmax, U ymin, U ymax);
    
    bool ret = intersect(3, 4, 9, 7, 4, 10, 3, 6);
    PT_UNIT_ASSERT( ret );

    ret = intersect(5, 4, 7, 5, 4, 10, 3, 6);
    PT_UNIT_ASSERT( ret );

    ret = intersect(9, 2, 9, 5, 4, 10, 3, 6);
    PT_UNIT_ASSERT( ret );

    ret = intersect(5, 7, 7, 7, 4, 10, 3, 6);
    PT_UNIT_ASSERT( !ret );

    ret = intersect(3, 5, 3, 6, 4, 10, 3, 6);
    PT_UNIT_ASSERT( !ret );

    ret = intersect(9, 2, 11, 4, 4, 10, 3, 6);
    PT_UNIT_ASSERT( ret );
}


void AlgorithmTest::testQuadrant()
{
    //Top    = 1 << 0, 
    //    Bottom = 1 << 1, 
    //    Right  = 1 << 2, 
    //    Left   = 1 << 3 

    using namespace Pt::Math;

    // Left
    int ret = quadrant(1, 12, 3, 7, 10, 14);
    PT_UNIT_ASSERT(ret == Left);

    // Right
    ret = quadrant(9, 13, 3, 7, 10, 14);
    PT_UNIT_ASSERT(ret == Right);

    // Bottom
    ret = quadrant(4, 5, 3, 7, 10, 14);
    PT_UNIT_ASSERT(ret == Bottom);

    // Top
    ret = quadrant(4, 15, 3, 7, 10, 14);
    PT_UNIT_ASSERT(ret == Top);

    // Bottom-Left
    ret = quadrant(0, 0, 3, 7, 10, 14);
    PT_UNIT_ASSERT(ret == (Bottom | Left));

    // Bottom-Right
    ret = quadrant(10, 0, 3, 7, 10, 14);
    PT_UNIT_ASSERT(ret == (Bottom | Right));

    // Top-Left
    ret = quadrant(0, 16, 3, 7, 10, 14);
    PT_UNIT_ASSERT(ret == (Top | Left));

    // Top-Right
    ret = quadrant(9, 17, 3, 7, 10, 14);
    PT_UNIT_ASSERT(ret == (Top | Right));

    // Inside
    ret = quadrant(4, 11, 3, 7, 10, 14);
    PT_UNIT_ASSERT(ret == 0);
}
