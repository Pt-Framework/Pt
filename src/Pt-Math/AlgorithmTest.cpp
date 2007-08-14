#undef PT_MATH_API_EXPORT

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"


#include <Pt/Math/Algorithm.h>



class AlgorithmTest : public Pt::Unit::TestSuite
{
public:
    AlgorithmTest()
    : TestSuite("AlgorithmTest")
    {
        Pt::Unit::TestSuite::registerMethod("testIntersect", *this, &AlgorithmTest::testIntersect);
    }

protected:
    void testIntersect();
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
