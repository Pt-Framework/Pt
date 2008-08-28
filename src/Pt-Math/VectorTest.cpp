/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#undef PT_MATH_API_EXPORT

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include <Pt/Math/Vector2d.h>
#include <Pt/Math/Vector3d.h>
#include <Pt/Math/Point.h>
#include <Pt/Math/MathUtils.h>

#include <cmath>

class VectorTest : public Pt::Unit::TestSuite
{
public:
    VectorTest()
    : TestSuite("VectorTest")
    {
        Pt::Unit::TestSuite::registerMethod("testConstructor1", *this, &VectorTest::testConstructor1);
        Pt::Unit::TestSuite::registerMethod("testConstructor2", *this, &VectorTest::testConstructor2);
        Pt::Unit::TestSuite::registerMethod("testConstructor3", *this, &VectorTest::testConstructor3);
        Pt::Unit::TestSuite::registerMethod("testConstructor4", *this, &VectorTest::testConstructor4);
        Pt::Unit::TestSuite::registerMethod("testSetGetValue", *this, &VectorTest::testSetGetValue);
        Pt::Unit::TestSuite::registerMethod("testSetGetValue2", *this, &VectorTest::testSetGetValue2);
        Pt::Unit::TestSuite::registerMethod("testSetGetWithPoint", *this, &VectorTest::testSetGetWithPoint);
        Pt::Unit::TestSuite::registerMethod("testEqualNotEqual", *this, &VectorTest::testEqualNotEqual);
        Pt::Unit::TestSuite::registerMethod("testAddidition", *this, &VectorTest::testAddidition);
        Pt::Unit::TestSuite::registerMethod("testSubtraction", *this, &VectorTest::testSubtraction);
        Pt::Unit::TestSuite::registerMethod("testMultiplyFactor", *this, &VectorTest::testMultiplyFactor);
        Pt::Unit::TestSuite::registerMethod("testScalarProduct", *this, &VectorTest::testScalarProduct);
        Pt::Unit::TestSuite::registerMethod("testLenght", *this, &VectorTest::testLenght);
        Pt::Unit::TestSuite::registerMethod("testSqrtLenght", *this, &VectorTest::testSqrtLenght);
        Pt::Unit::TestSuite::registerMethod("testNormalize", *this, &VectorTest::testNormalize);
        Pt::Unit::TestSuite::registerMethod("testAdditionPoint", *this, &VectorTest::testAdditionPoint);
        Pt::Unit::TestSuite::registerMethod("testSubtractionPoint", *this, &VectorTest::testSubtractionPoint);
        Pt::Unit::TestSuite::registerMethod("testAngleTowardsNorth", *this, &VectorTest::testAngleTowardsNorth);
    }

protected:
    void testConstructor1();
    void testConstructor2();
    void testConstructor3();
    void testConstructor4();
    void testSetGetValue();
    void testSetGetValue2();
    void testSetGetWithPoint();
    void testEqualNotEqual();
    void testAddidition();
    void testSubtraction();
    void testMultiplyFactor();
    void testScalarProduct();
    void testLenght();
    void testSqrtLenght();
    void testNormalize();
    void testAdditionPoint();
    void testSubtractionPoint();
    void testAngleTowardsNorth();
};


Pt::Unit::RegisterTest<VectorTest> register_VectorTest;


void VectorTest::testConstructor1()
{
    // for the 3d vector
    Pt::Math::BasicVector3d<unsigned int> vector;

    PT_UNIT_ASSERT( vector.x() == 0 );
    PT_UNIT_ASSERT( vector.y() == 0 );
    PT_UNIT_ASSERT( vector.z() == 0 );

    // for the 2d vector
    Pt::Math::BasicVector2d<unsigned int> vector2d;

    PT_UNIT_ASSERT( vector2d.x() == 0 );
    PT_UNIT_ASSERT( vector2d.y() == 0 );
}


void VectorTest::testConstructor2()
{
    // for the 3d vector
    Pt::Math::BasicVector3d<unsigned int> vector(80, 40, 20);
    Pt::Math::BasicVector3d<unsigned int> otherVector(vector);

    PT_UNIT_ASSERT( otherVector.x() == 80 );
    PT_UNIT_ASSERT( otherVector.y() == 40 );
    PT_UNIT_ASSERT( otherVector.z() == 20 );

    // for the 2d vector
    Pt::Math::BasicVector2d<unsigned int> vector2d(80, 40);
    Pt::Math::BasicVector2d<unsigned int> otherVector2d(vector2d);

    PT_UNIT_ASSERT( otherVector.x() == 80 );
    PT_UNIT_ASSERT( otherVector.y() == 40 );
}


void VectorTest::testConstructor3()
{
    // for the 3d vector
    Pt::Math::BasicVector3d<float> vector(2.34f, 3355.54f, 674.17f);

    PT_UNIT_ASSERT( vector.x() == 2.34f );
    PT_UNIT_ASSERT( vector.y() == 3355.54f );
    PT_UNIT_ASSERT( vector.z() == 674.17f );

    // for the 2d vector
    Pt::Math::BasicVector2d<float> vector2d(2.34f, 3355.54f);

    PT_UNIT_ASSERT( vector2d.x() == 2.34f );
    PT_UNIT_ASSERT( vector2d.y() == 3355.54f );
}

void VectorTest::testConstructor4()
{
    //ToDo needed a point3d class here!!!
/*
    // for the 3d vector
    Pt::Math::BasicVector3d<Pt::int32_t> vector(startPt, endPt);

    PT_UNIT_ASSERT( vector.x() == );
    PT_UNIT_ASSERT( vector.y() == );
    PT_UNIT_ASSERT( vector.z() == );
*/
    // for the 2d vector

    // with same template type for vector and points
    Pt::Math::BasicPoint<Pt::int32_t> startPt2d(5, 3);
    Pt::Math::BasicPoint<Pt::int32_t> endPt2d(9, -48);
    Pt::Math::BasicVector2d<Pt::int32_t> vector2d(startPt2d, endPt2d);

    PT_UNIT_ASSERT( vector2d.x() ==   4 );
    PT_UNIT_ASSERT( vector2d.y() == -51 );

    // with different template type for vector and points
    Pt::Math::BasicPoint<Pt::int32_t> startPt2d2(5, 3);
    Pt::Math::BasicPoint<Pt::int32_t> endPt2d2(9, -48);
    Pt::Math::BasicVector2d<float> vector2d2(startPt2d2, endPt2d2);

    PT_UNIT_ASSERT( vector2d2.x() ==   4.0f );
    PT_UNIT_ASSERT( vector2d2.y() == -51.0f );
}

void VectorTest::testSetGetValue()
{
    // for the 3d vector
    Pt::Math::BasicVector3d<float> vector;

    vector.setX(798.446f);
    vector.setY(9874.8416f);
    vector.setZ(87.46f);

    PT_UNIT_ASSERT( vector.x() == 798.446f );
    PT_UNIT_ASSERT( vector.y() == 9874.8416f );
    PT_UNIT_ASSERT( vector.z() == 87.46f );

    // for the 2d vector
    Pt::Math::BasicVector2d<float> vector2d;

    vector2d.setX(798.446f);
    vector2d.setY(9874.8416f);

    PT_UNIT_ASSERT( vector2d.x() == 798.446f );
    PT_UNIT_ASSERT( vector2d.y() == 9874.8416f );
}

void VectorTest::testSetGetValue2()
{
    // for the 3d vector
    Pt::Math::BasicVector3d<float> vector;

    vector.set(798.446f, 9874.8416f, 87.46f);

    PT_UNIT_ASSERT( vector.x() == 798.446f );
    PT_UNIT_ASSERT( vector.y() == 9874.8416f );
    PT_UNIT_ASSERT( vector.z() == 87.46f );

    // for the 2d vector
    Pt::Math::BasicVector2d<float> vector2d;

    vector2d.set(798.446f, 9874.8416f);

    PT_UNIT_ASSERT( vector2d.x() == 798.446f );
    PT_UNIT_ASSERT( vector2d.y() == 9874.8416f );
}

void VectorTest::testSetGetWithPoint()
{
    //ToDo needed a point3d class here!!!
/*
    // for the 3d vector
    Pt::Math::BasicVector3d<double>  vector3d;
    Pt::Math::BasicPoint3d<double>   startPoint3d( 984.654, 47.57, -98.1);
    Pt::Math::BasicPoint3d<double>   endPoint3d( -54.47, 794.1, 476.47);

    vector3d.set(startPoint3d, endPoint3d);

    PT_UNIT_ASSERT( Pt::Math::equal( vector3d.x(), -1039.124, Pt::Math::Eps9 ) );
    PT_UNIT_ASSERT( Pt::Math::equal( vector3d.y(),   746.530, Pt::Math::Eps9 ) );
    PT_UNIT_ASSERT( Pt::Math::equal( vector3d.z(),   574.570, Pt::Math::Eps9 ) );

*/
    // for the 2d vector

    // with same template type for vector and points
    Pt::Math::BasicVector2d<double>  vector2d;
    Pt::Math::BasicPoint<double>     startPoint2d(74.57, 98.45);
    Pt::Math::BasicPoint<double>     endPoint2d(-987.47, 9.47);

    vector2d.set(startPoint2d, endPoint2d);

    PT_UNIT_ASSERT( Pt::Math::equal( vector2d.x(), -1062.04, Pt::Math::Eps9 ) );
    PT_UNIT_ASSERT( Pt::Math::equal( vector2d.y(),   -88.98, Pt::Math::Eps9 ) );

    // with different template type for vector and points
    Pt::Math::BasicVector2d<double>  vector2d2;
    Pt::Math::BasicPoint<Pt::int32_t>     startPoint2d2(74, 98);
    Pt::Math::BasicPoint<Pt::int32_t>     endPoint2d2(987, 9);

    vector2d2.set(startPoint2d2, endPoint2d2);

    PT_UNIT_ASSERT( Pt::Math::equal( vector2d2.x(), 913.0, Pt::Math::Eps9 ) );
    PT_UNIT_ASSERT( Pt::Math::equal( vector2d2.y(), -89.0, Pt::Math::Eps9 ) );
}

void VectorTest::testEqualNotEqual()
{
    // for the 3d vector
    Pt::Math::BasicVector3d<int> vector(-2, 345, 73);
    Pt::Math::BasicVector3d<int> otherVector(-2, 345, 73);

    PT_UNIT_ASSERT( vector == otherVector );

    otherVector.setY(478);

    PT_UNIT_ASSERT( vector != otherVector );

    // for the 2d vector
    Pt::Math::BasicVector2d<int> vector2d(-2, 345);
    Pt::Math::BasicVector2d<int> otherVector2d(-2, 345);

    PT_UNIT_ASSERT( vector2d == otherVector2d );

    otherVector2d.setY(478);

    PT_UNIT_ASSERT( vector2d != otherVector2d );
}


void VectorTest::testAddidition()
{
    // for the 3d vector
    Pt::Math::BasicVector3d<int> vectorI(-2, 345, 73);
    Pt::Math::BasicVector3d<int> vectorII(76, -8741, 636);
    Pt::Math::BasicVector3d<int> vectorIII;

    vectorIII = vectorI + vectorII;

    PT_UNIT_ASSERT( vectorIII.x() == 74 );
    PT_UNIT_ASSERT( vectorIII.y() == -8396 );
    PT_UNIT_ASSERT( vectorIII.z() == 709 );

    // for the 2d vector
    Pt::Math::BasicVector2d<int> vectorI2d(-2, 345);
    Pt::Math::BasicVector2d<int> vectorII2d(76, -8741);
    Pt::Math::BasicVector2d<int> vectorIII2d;

    vectorIII2d = vectorI2d + vectorII2d;

    PT_UNIT_ASSERT( vectorIII2d.x() == 74 );
    PT_UNIT_ASSERT( vectorIII2d.y() == -8396 );
}


void VectorTest::testSubtraction()
{
    // for the 3d vector
    Pt::Math::BasicVector3d<int> vectorI(-2, 345, 73);
    Pt::Math::BasicVector3d<int> vectorII(76, -8741, 636);
    Pt::Math::BasicVector3d<int> vectorIII;

    vectorIII = vectorI - vectorII;

    PT_UNIT_ASSERT( vectorIII.x() == -78 );
    PT_UNIT_ASSERT( vectorIII.y() == 9086 );
    PT_UNIT_ASSERT( vectorIII.z() == -563 );

    // for the 2d vector
    Pt::Math::BasicVector2d<int> vectorI2d(-2, 345);
    Pt::Math::BasicVector2d<int> vectorII2d(76, -8741);
    Pt::Math::BasicVector2d<int> vectorIII2d;

    vectorIII2d = vectorI2d - vectorII2d;

    PT_UNIT_ASSERT( vectorIII2d.x() == -78 );
    PT_UNIT_ASSERT( vectorIII2d.y() == 9086 );
}


void VectorTest::testMultiplyFactor()
{
    // for the 3d vector
    Pt::Math::BasicVector3d<unsigned int> vector(488, 334, 76);
    Pt::Math::BasicVector3d<unsigned int> otherVector;

    otherVector = vector * 65;

    PT_UNIT_ASSERT( otherVector.x() == 31720 );
    PT_UNIT_ASSERT( otherVector.y() == 21710 );
    PT_UNIT_ASSERT( otherVector.z() == 4940 );

    // for the 2d vector
    Pt::Math::BasicVector2d<unsigned int> vector2d(488, 334);
    Pt::Math::BasicVector2d<unsigned int> otherVector2d;

    otherVector2d = vector2d * 65;

    PT_UNIT_ASSERT( otherVector2d.x() == 31720 );
    PT_UNIT_ASSERT( otherVector2d.y() == 21710 );
}


void VectorTest::testScalarProduct()
{
    // for the 3d vector
    Pt::Math::BasicVector3d<unsigned int> vector1(2, 4, 6);
    Pt::Math::BasicVector3d<unsigned int> vector2(5, 7, 9);

    float scalar = (float)(vector1 * vector2);

    PT_UNIT_ASSERT( scalar == 92.0f );

    // for the 2d vector
    Pt::Math::BasicVector2d<unsigned int> vector2d1(2, 4);
    Pt::Math::BasicVector2d<unsigned int> vector2d2(5, 7);

    float scalar2 = (float)(vector2d1 * vector2d2);

    PT_UNIT_ASSERT( scalar2 == 38.0f );
}


void VectorTest::testLenght()
{
    // for the 3d vector
    Pt::Math::BasicVector3d<unsigned int> vector(4, 3, 0);

    double length = vector.length();

    PT_UNIT_ASSERT(length == 5);

    // for the 2d vector
    Pt::Math::BasicVector2d<unsigned int> vector2d(4, 3);

    double length2 = vector2d.length();

    PT_UNIT_ASSERT(length2 == 5);
}


void VectorTest::testSqrtLenght()
{
    // for the 3d vector
    Pt::Math::BasicVector3d<unsigned int> vector(4, 3, 0);

    double length = vector.sqrtLength();

    PT_UNIT_ASSERT(length == 25);

    // for the 2d vector
    Pt::Math::BasicVector2d<unsigned int> vector2d(4, 3);

    double length2 = vector2d.sqrtLength();

    PT_UNIT_ASSERT(length2 == 25);
}


void VectorTest::testNormalize()
{
    // for the 3d vector
    Pt::Math::BasicVector3d<float> vector(0.0f, 0.0f, 4.0f);

    vector.normalize();

    PT_UNIT_ASSERT( vector.x() == 0.0f );
    PT_UNIT_ASSERT( vector.y() == 0.0f );
    PT_UNIT_ASSERT( vector.z() == 1.0f );

    // for the 2d vector
    Pt::Math::BasicVector2d<float> vector2d(0.0f, 4.0f);

    vector2d.normalize();

    PT_UNIT_ASSERT( vector2d.x() == 0.0f );
    PT_UNIT_ASSERT( vector2d.y() == 1.0f );
}

void VectorTest::testAdditionPoint()
{
    // for the 3d vector
    Pt::Math::BasicVector2d<Pt::int32_t>   vector2d(6, 87);
    Pt::Math::BasicVector2d<Pt::int32_t>   vector2d2(116, -9977);
    Pt::Math::BasicPoint<Pt::int32_t>      point2d(45, 53);

    vector2d = vector2d + point2d;

    PT_UNIT_ASSERT( vector2d.x() == 51 );
    PT_UNIT_ASSERT( vector2d.y() == 140 );

    vector2d2 += point2d;

    PT_UNIT_ASSERT( vector2d2.x() == 161 );
    PT_UNIT_ASSERT( vector2d2.y() == -9924 );
}

void VectorTest::testSubtractionPoint()
{
    // for the 3d vector
    Pt::Math::BasicVector2d<Pt::int32_t>   vector2d(874, 185);
    Pt::Math::BasicVector2d<Pt::int32_t>   vector2d2(55, 77);
    Pt::Math::BasicPoint<Pt::int32_t>      point2d(-415, 783);

    vector2d = vector2d - point2d;

    PT_UNIT_ASSERT( vector2d.x() == 1289 );
    PT_UNIT_ASSERT( vector2d.y() == -598 );

    vector2d2 -= point2d;

    PT_UNIT_ASSERT( vector2d2.x() ==  470 );
    PT_UNIT_ASSERT( vector2d2.y() == -706 );
}

void VectorTest::testAngleTowardsNorth()
{
    Pt::Math::BasicVector2d<Pt::int32_t> v1(0, 1); // N
    PT_UNIT_ASSERT(v1.angleTowardsNorth() == 0);

    Pt::Math::BasicVector2d<Pt::int32_t> v2(1, 1); // NE
    PT_UNIT_ASSERT(std::fabs(v2.angleTowardsNorth() - 45) < 0.00001);

    Pt::Math::BasicVector2d<Pt::int32_t> v3(1, 0); // E
    PT_UNIT_ASSERT(v3.angleTowardsNorth() == 90);

    Pt::Math::BasicVector2d<Pt::int32_t> v4(1, -1); // SE
    PT_UNIT_ASSERT(std::fabs(v4.angleTowardsNorth() - 135) < 0.00001);

    Pt::Math::BasicVector2d<Pt::int32_t> v5(0, -1); // S
    PT_UNIT_ASSERT(v5.angleTowardsNorth() == 180);

    Pt::Math::BasicVector2d<Pt::int32_t> v6(-1, -1); // SW
    PT_UNIT_ASSERT(std::fabs(v6.angleTowardsNorth() - 225) < 0.00001);

    Pt::Math::BasicVector2d<Pt::int32_t> v7(-1, 0); // W
    PT_UNIT_ASSERT(v7.angleTowardsNorth() == 270);

    Pt::Math::BasicVector2d<Pt::int32_t> v8(-1, 1); // NW
    PT_UNIT_ASSERT(std::fabs(v8.angleTowardsNorth() - 315) < 0.00001);;
}
