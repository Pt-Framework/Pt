/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/
#undef PT_MATH_API_EXPORT

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include <Pt/Math/Math.h>
#include <Pt/Math/Matrix.h>


class MatrixTest : public Pt::Unit::TestSuite
{
public:
    MatrixTest()
    : TestSuite("MatrixTest")
    {
        Pt::Unit::TestSuite::registerMethod("testConstructor1", *this, &MatrixTest::testConstructor1);
        Pt::Unit::TestSuite::registerMethod("testConstructor2", *this, &MatrixTest::testConstructor2);
        Pt::Unit::TestSuite::registerMethod("testConstructor3", *this, &MatrixTest::testConstructor3);
        Pt::Unit::TestSuite::registerMethod("testSetGetValue", *this, &MatrixTest::testSetGetValue);
        Pt::Unit::TestSuite::registerMethod("testEqualNotEqual", *this, &MatrixTest::testEqualNotEqual);
        Pt::Unit::TestSuite::registerMethod("testAddidition", *this, &MatrixTest::testAddidition);
        Pt::Unit::TestSuite::registerMethod("testSubtraction", *this, &MatrixTest::testSubtraction);
        Pt::Unit::TestSuite::registerMethod("testMultiplication_1", *this, &MatrixTest::testMultiplication_1);
        Pt::Unit::TestSuite::registerMethod("testMultiplication_2", *this, &MatrixTest::testMultiplication_2);
        Pt::Unit::TestSuite::registerMethod("testMultiplication_3", *this, &MatrixTest::testMultiplication_3);
        Pt::Unit::TestSuite::registerMethod("testMultiplication_4", *this, &MatrixTest::testMultiplication_4);
        Pt::Unit::TestSuite::registerMethod("testAddValue", *this, &MatrixTest::testAddValue);
        Pt::Unit::TestSuite::registerMethod("testSubtractValue", *this, &MatrixTest::testSubtractValue);
        Pt::Unit::TestSuite::registerMethod("testMultiplyValue", *this, &MatrixTest::testMultiplyValue);
        Pt::Unit::TestSuite::registerMethod("testDivideValue", *this, &MatrixTest::testDivideValue);
        Pt::Unit::TestSuite::registerMethod("testGetSetOperator", *this, &MatrixTest::testGetSetOperator);
        Pt::Unit::TestSuite::registerMethod("testTranspose", *this, &MatrixTest::testTranspose);
        Pt::Unit::TestSuite::registerMethod("testSetIdentity", *this, &MatrixTest::testSetIdentity);
        Pt::Unit::TestSuite::registerMethod("testIsIdentity", *this, &MatrixTest::testIsIdentity);
        Pt::Unit::TestSuite::registerMethod("testBracketOperator", *this, &MatrixTest::testBracketOperator);
    }

protected:
    void testConstructor1();
    void testConstructor2();
    void testConstructor3();
    void testSetGetValue();
    void testEqualNotEqual();
    void testAddidition();
    void testSubtraction();
    void testMultiplication_1();
    void testMultiplication_2();
    void testMultiplication_3();
    void testMultiplication_4();
    void testAddValue();
    void testSubtractValue();
    void testMultiplyValue();
    void testDivideValue();
    void testGetSetOperator();
    void testTranspose();
    void testSetIdentity();
    void testIsIdentity();
    void testBracketOperator();
};

Pt::Unit::RegisterTest<MatrixTest> register_MatrixTest;


void MatrixTest::testConstructor1()
{
    // construct matrix with dimension 2x4
    Pt::Math::BasicMatrix<int, 4, 2> matrix;

    // check No. of columns and rows
    PT_UNIT_ASSERT( matrix.colCount() == 2 );
    PT_UNIT_ASSERT( matrix.rowCount() == 4 );
}


void MatrixTest::testConstructor2()
{
    // construct a matrix and fill with some values
    Pt::Math::BasicMatrix<int, 2, 2> matrix;
    matrix.setValue(11, 0, 0);
    matrix.setValue(22, 0, 1);
    matrix.setValue(33, 1, 0);
    matrix.setValue(44, 1, 1);

    // construct other matrix with first matrix
    Pt::Math::BasicMatrix<int, 2, 2> otherMatrix(matrix);

    // check No. of columns and rows
    PT_UNIT_ASSERT( otherMatrix.colCount() == matrix.colCount() );
    PT_UNIT_ASSERT( otherMatrix.rowCount() == matrix.rowCount() );

    // check if both matrices have the same values
    PT_UNIT_ASSERT( otherMatrix.getValue(0, 0) == 11 );
    PT_UNIT_ASSERT( otherMatrix.getValue(0, 1) == 22 );
    PT_UNIT_ASSERT( otherMatrix.getValue(1, 0) == 33 );
    PT_UNIT_ASSERT( otherMatrix.getValue(1, 1) == 44 );
}


void MatrixTest::testConstructor3()
{
    // create a std::vector with some values
    std::vector<float> dataArr;
    dataArr.push_back(1.0f);
    dataArr.push_back(2.0f);
    dataArr.push_back(3.0f);
    dataArr.push_back(4.0f);
    dataArr.push_back(5.0f);
    dataArr.push_back(6.0f);
    dataArr.push_back(7.0f);
    dataArr.push_back(8.0f);
    dataArr.push_back(9.0f);

    // construct the matrix with the vector
    Pt::Math::BasicMatrix<float, 3, 3> matrix(dataArr);

    Pt::Math::BasicMatrix<float, 3, 3> matrix2(dataArr);

    PT_UNIT_ASSERT( matrix == matrix2 );

    // check No. of columns and rows
    PT_UNIT_ASSERT( matrix.colCount() == 3 );
    PT_UNIT_ASSERT( matrix.rowCount() == 3 );

    // check if values of vector are on right place in matrix
    for(unsigned int i=0; i<9; ++i)
    {
        PT_UNIT_ASSERT( matrix.getValue(i/3, i%3) == i+1 );
    }
}

void MatrixTest::testSetGetValue()
{
    Pt::Math::BasicMatrix<unsigned int, 4, 4> matrix;

    matrix.setValue( 1, 0, 0);
    matrix.setValue( 2, 0, 1);
    matrix.setValue( 3, 0, 2);
    matrix.setValue( 4, 0, 3);
    matrix.setValue( 5, 1, 0);
    matrix.setValue( 6, 1, 1);
    matrix.setValue( 7, 1, 2);
    matrix.setValue( 8, 1, 3);
    matrix.setValue( 9, 2, 0);
    matrix.setValue(10, 2, 1);
    matrix.setValue(11, 2, 2);
    matrix.setValue(12, 2, 3);
    matrix.setValue(13, 3, 0);
    matrix.setValue(14, 3, 1);
    matrix.setValue(15, 3, 2);
    matrix.setValue(16, 3, 3);

    // test if the get function return the prior set values
    for(unsigned int i=0; i<16; ++i)
    {
        PT_UNIT_ASSERT( matrix.getValue(i/4, i%4) == i+1 );
    }
}


void MatrixTest::testEqualNotEqual()
{
    Pt::Math::BasicMatrix<unsigned int, 4, 4> matrixI;
    Pt::Math::BasicMatrix<unsigned int, 4, 4> matrixII;

    matrixI.setValue( 1, 0, 0);
    matrixI.setValue( 2, 0, 1);
    matrixI.setValue( 3, 0, 2);
    matrixI.setValue( 4, 0, 3);
    matrixI.setValue( 5, 1, 0);
    matrixI.setValue( 6, 1, 1);
    matrixI.setValue( 7, 1, 2);
    matrixI.setValue( 8, 1, 3);
    matrixI.setValue( 9, 2, 0);
    matrixI.setValue(10, 2, 1);
    matrixI.setValue(11, 2, 2);
    matrixI.setValue(12, 2, 3);
    matrixI.setValue(13, 3, 0);
    matrixI.setValue(14, 3, 1);
    matrixI.setValue(15, 3, 2);
    matrixI.setValue(16, 3, 3);

    matrixII.setValue( 1, 0, 0);
    matrixII.setValue( 2, 0, 1);
    matrixII.setValue( 3, 0, 2);
    matrixII.setValue( 4, 0, 3);
    matrixII.setValue( 5, 1, 0);
    matrixII.setValue( 6, 1, 1);
    matrixII.setValue( 7, 1, 2);
    matrixII.setValue( 8, 1, 3);
    matrixII.setValue( 9, 2, 0);
    matrixII.setValue(10, 2, 1);
    matrixII.setValue(11, 2, 2);
    matrixII.setValue(12, 2, 3);
    matrixII.setValue(13, 3, 0);
    matrixII.setValue(14, 3, 1);
    matrixII.setValue(15, 3, 2);
    matrixII.setValue(16, 3, 3);

    // check equality
    PT_UNIT_ASSERT( matrixI == matrixII );

    // change one value
    matrixII.setValue( 111, 0, 0);

    // should be not equal now
    PT_UNIT_ASSERT( matrixI != matrixII );
}


void MatrixTest::testAddidition()
{
    Pt::Math::BasicMatrix<double, 2, 2> matrixI;
    Pt::Math::BasicMatrix<double, 2, 2> matrixII;
    Pt::Math::BasicMatrix<double, 2, 2> matrixIII;

    matrixI.setValue(1.0, 0, 0);
    matrixI.setValue(2.0, 0, 1);
    matrixI.setValue(3.0, 1, 0);
    matrixI.setValue(4.0, 1, 1);

    matrixII.setValue(5.0, 0, 0);
    matrixII.setValue(6.0, 0, 1);
    matrixII.setValue(7.0, 1, 0);
    matrixII.setValue(8.0, 1, 1);

    // add the matrices
    matrixIII = matrixI + matrixII;

    // check the result
    PT_UNIT_ASSERT( matrixIII.getValue(0, 0) == 6.0 );
    PT_UNIT_ASSERT( matrixIII.getValue(0, 1) == 8.0 );
    PT_UNIT_ASSERT( matrixIII.getValue(1, 0) == 10.0 );
    PT_UNIT_ASSERT( matrixIII.getValue(1, 1) == 12.0 );
}

void MatrixTest::testSubtraction()
{
    Pt::Math::BasicMatrix<int, 2, 2> matrixI;
    Pt::Math::BasicMatrix<int, 2, 2> matrixII;
    Pt::Math::BasicMatrix<int, 2, 2> matrixIII;

    matrixI.setValue(1, 0, 0);
    matrixI.setValue(2, 0, 1);
    matrixI.setValue(3, 1, 0);
    matrixI.setValue(4, 1, 1);

    matrixII.setValue(5, 0, 0);
    matrixII.setValue(6, 0, 1);
    matrixII.setValue(7, 1, 0);
    matrixII.setValue(8, 1, 1);

    // subtract the matrices
    matrixIII = matrixI - matrixII;

    // check the result
    PT_UNIT_ASSERT( matrixIII.getValue(0, 0) == -4 );
    PT_UNIT_ASSERT( matrixIII.getValue(0, 1) == -4 );
    PT_UNIT_ASSERT( matrixIII.getValue(1, 0) == -4 );
    PT_UNIT_ASSERT( matrixIII.getValue(1, 1) == -4 );

}


void MatrixTest::testMultiplication_1()
{

    Pt::Math::BasicMatrix<unsigned int, 2, 3> matrixI;
    Pt::Math::BasicMatrix<unsigned int, 3, 2> matrixII;
    Pt::Math::BasicMatrix<unsigned int, 2, 2> matrixIII;


    matrixI.setValue( 1, 0, 0);
    matrixI.setValue( 2, 0, 1);
    matrixI.setValue( 3, 0, 2);
    matrixI.setValue( 4, 1, 0);
    matrixI.setValue( 5, 1, 1);
    matrixI.setValue( 6, 1, 2);

    matrixII.setValue(  6, 0, 0);
    matrixII.setValue(  3, 1, 0);
    matrixII.setValue(  0, 2, 0);
    matrixII.setValue(  1, 0, 1);
    matrixII.setValue(  2, 1, 1);
    matrixII.setValue(  1, 2, 1);


    // multiply the matrices
    matrixIII =  matrixI * matrixII;

    // check the result
    PT_UNIT_ASSERT( matrixIII.getValue(0, 0) == 12 );
    PT_UNIT_ASSERT( matrixIII.getValue(0, 1) == 8 );
    PT_UNIT_ASSERT( matrixIII.getValue(1, 0) == 39 );
    PT_UNIT_ASSERT( matrixIII.getValue(1, 1) == 20 );
}

void MatrixTest::testMultiplication_2()
{

    Pt::Math::BasicMatrix<unsigned int, 4, 4> matrixI;
    Pt::Math::BasicMatrix<unsigned int, 4, 4> matrixII;
    Pt::Math::BasicMatrix<unsigned int, 4, 4> matrixIII;


    for (int i=0; i < 4; i++)
    {
        matrixI.setValue(1, i,i);
        matrixII.setValue(1, i,i);
    }

    matrixI.setValue(2, 0, 3);
    matrixI.setValue(3, 1, 3);
    matrixI.setValue(4, 2, 3);

    // multiply the matrices
    matrixIII =  matrixII * matrixI;

    // check the result
    PT_UNIT_ASSERT( matrixIII == matrixI );

    // multiply the matrices
    matrixIII =  matrixII * matrixI;

    // check the result
    PT_UNIT_ASSERT( matrixIII == matrixI );

}

void MatrixTest::testMultiplication_3()
{

    Pt::Math::BasicMatrix<unsigned int, 4, 4> matrixI;
    Pt::Math::BasicMatrix<unsigned int, 4, 1> matrixII;
    Pt::Math::BasicMatrix<unsigned int, 4, 1> matrixIII;


    for (int i=0; i < 4; i++)
    {
        matrixI.setValue(1, i,i);
    }

    matrixII.setValue(2, 0, 0);
    matrixII.setValue(3, 1, 0);
    matrixII.setValue(4, 2, 0);
    matrixII.setValue(1, 3, 0);

    // multiply the matrices
    matrixIII =  matrixI * matrixII;

    // check the result
    PT_UNIT_ASSERT( matrixIII == matrixII );

}

void MatrixTest::testMultiplication_4()
{

    Pt::Math::BasicMatrix<unsigned int, 4, 4> matrixI;
    Pt::Math::BasicMatrix<unsigned int, 4, 1> matrixII;
    Pt::Math::BasicMatrix<unsigned int, 4, 1> matrixIII;


    for (int i=0; i < 4; i++)
    {
        matrixI.setValue(1, i,i);
    }

    matrixI.setValue(10, 0, 3);
    matrixI.setValue(10, 1, 3);
    matrixI.setValue(10, 2, 3);


    matrixII.setValue(1, 0, 0);
    matrixII.setValue(1, 1, 0);
    matrixII.setValue(1, 2, 0);
    matrixII.setValue(1, 3, 0);


    // multiply the matrices
    matrixIII =  matrixI * matrixII;


    // check the result
    PT_UNIT_ASSERT( matrixIII.getValue(0, 0) == 11 );
    PT_UNIT_ASSERT( matrixIII.getValue(1, 0) == 11 );
    PT_UNIT_ASSERT( matrixIII.getValue(2, 0) == 11 );



}


void MatrixTest::testAddValue()
{
    Pt::Math::BasicMatrix<int, 2, 2> matrix;
    matrix.setValue(1, 0, 0);
    matrix.setValue(2, 0, 1);
    matrix.setValue(3, 1, 0);
    matrix.setValue(4, 1, 1);

    // add some values to matrix member
    matrix.addValue(  10, 0, 0);
    matrix.addValue(  50, 0, 1);
    matrix.addValue( 100, 1, 0);
    matrix.addValue(1000, 1, 1);

    // check the result
    PT_UNIT_ASSERT( matrix.getValue(0, 0) == 11   );
    PT_UNIT_ASSERT( matrix.getValue(0, 1) == 52   );
    PT_UNIT_ASSERT( matrix.getValue(1, 0) == 103  );
    PT_UNIT_ASSERT( matrix.getValue(1, 1) == 1004 );
}

void MatrixTest::testSubtractValue()
{
    Pt::Math::BasicMatrix<int, 2, 2> matrix;
    matrix.setValue(55, 0, 0);
    matrix.setValue(441, 0, 1);
    matrix.setValue(6433, 1, 0);
    matrix.setValue(14, 1, 1);

    // subtract some values from matrix member
    matrix.subValue(157, 0, 0);
    matrix.subValue( 25, 0, 1);
    matrix.subValue(  7, 1, 0);
    matrix.subValue( 76, 1, 1);

    // check the result
    PT_UNIT_ASSERT( matrix.getValue(0, 0) == -102 );
    PT_UNIT_ASSERT( matrix.getValue(0, 1) ==  416 );
    PT_UNIT_ASSERT( matrix.getValue(1, 0) == 6426 );
    PT_UNIT_ASSERT( matrix.getValue(1, 1) ==  -62 );
}

void MatrixTest::testMultiplyValue()
{
    Pt::Math::BasicMatrix<int, 2, 2> matrix;
    matrix.setValue(55, 0, 0);
    matrix.setValue(441, 0, 1);
    matrix.setValue(6433, 1, 0);
    matrix.setValue(14, 1, 1);

    // multiply some values to matrix member
    matrix.mulValue( 5, 0, 0);
    matrix.mulValue(25, 0, 1);
    matrix.mulValue( 7, 1, 0);
    matrix.mulValue(76, 1, 1);

    // check the result
    PT_UNIT_ASSERT( matrix.getValue(0, 0) ==   275 );
    PT_UNIT_ASSERT( matrix.getValue(0, 1) == 11025 );
    PT_UNIT_ASSERT( matrix.getValue(1, 0) == 45031 );
    PT_UNIT_ASSERT( matrix.getValue(1, 1) ==  1064 );
}

void MatrixTest::testDivideValue()
{
    Pt::Math::BasicMatrix<int, 2, 2> matrix;
    matrix.setValue(  275, 0, 0);
    matrix.setValue(11025, 0, 1);
    matrix.setValue(45031, 1, 0);
    matrix.setValue( 1064, 1, 1);

    // subtract some values from matrix member
    matrix.divValue(  55, 0, 0);
    matrix.divValue( 441, 0, 1);
    matrix.divValue(6433, 1, 0);
    matrix.divValue(  14, 1, 1);

    // check the result
    PT_UNIT_ASSERT( matrix.getValue(0, 0) ==  5);
    PT_UNIT_ASSERT( matrix.getValue(0, 1) == 25 );
    PT_UNIT_ASSERT( matrix.getValue(1, 0) ==  7 );
    PT_UNIT_ASSERT( matrix.getValue(1, 1) == 76 );
}

void MatrixTest::testGetSetOperator()
{
    Pt::Math::BasicMatrix<float, 2, 2> matrix;
    matrix.setValue( 14.45f, 0, 0);
    matrix.setValue(110.25f, 0, 1);
    matrix.setValue(45.031f, 1, 0);
    matrix.setValue( 106.4f, 1, 1);

    // check if bracke operator return the right values
    PT_UNIT_ASSERT( matrix.getValue(0, 0) ==  14.45f );
    PT_UNIT_ASSERT( matrix.getValue(0, 1) == 110.25f );
    PT_UNIT_ASSERT( matrix.getValue(1, 0) == 45.031f );
    PT_UNIT_ASSERT( matrix.getValue(1, 1) ==  106.4f );

    // set some values with the bracket operator
    matrix.setValue(57.654f, 0, 0);
    matrix.setValue(7945.5f, 0, 1);
    matrix.setValue(0.4680f, 1, 0);
    matrix.setValue(984.14f, 1, 1);

    // check if right values were set
    PT_UNIT_ASSERT( matrix.getValue(0, 0) == 57.654f );
    PT_UNIT_ASSERT( matrix.getValue(0, 1) == 7945.5f );
    PT_UNIT_ASSERT( matrix.getValue(1, 0) == 0.4680f );
    PT_UNIT_ASSERT( matrix.getValue(1, 1) == 984.14f );
}

void MatrixTest::testTranspose()
{
    Pt::Math::BasicMatrix<float, 2, 4> matrix;
    matrix.setValue(0.0f, 0, 0);
    matrix.setValue(1.0f, 1, 0);
    matrix.setValue(2.0f, 0, 1);
    matrix.setValue(3.0f, 1, 1);
    matrix.setValue(4.0f, 0, 2);
    matrix.setValue(5.0f, 1, 2);
    matrix.setValue(6.0f, 0, 3);
    matrix.setValue(7.0f, 1, 3);

    Pt::Math::BasicMatrix<float, 4, 2> matrixT;

    // get the transpose matrix (columns <-> rows)
    matrixT = matrix.transpose();


    // check the transpose matrix
    PT_UNIT_ASSERT( matrixT.getValue(0, 0) ==  0.0f );
    PT_UNIT_ASSERT( matrixT.getValue(1, 0) ==  2.0f );
    PT_UNIT_ASSERT( matrixT.getValue(2, 0) ==  4.0f );
    PT_UNIT_ASSERT( matrixT.getValue(3, 0) ==  6.0f );
    PT_UNIT_ASSERT( matrixT.getValue(0, 1) ==  1.0f );
    PT_UNIT_ASSERT( matrixT.getValue(1, 1) ==  3.0f );
    PT_UNIT_ASSERT( matrixT.getValue(2, 1) ==  5.0f );
    PT_UNIT_ASSERT( matrixT.getValue(3, 1) ==  7.0f );
}


void MatrixTest::testSetIdentity()
{
    Pt::Math::BasicMatrix<float, 4, 4> matrix;

    for(size_t r = 0; r < matrix.rowCount(); ++r)
    {
        for(size_t c = 0; c < matrix.colCount(); ++c)
        {
            matrix.setValue((float)matrix.rowCount()*r + c, r, c);
        }
    }

    // create a identity matrix
    matrix.setToIdentity();

    for(size_t j=0; j<matrix.colCount(); ++j)
    {
        for(size_t k=0; k<matrix.rowCount(); ++k)
        {
            if(j==k)
            {
                // diagonal elments should have the value 1
                PT_UNIT_ASSERT( matrix.getValue(j, k) ==  1.0f );
            }else
            {
                // all other elements should have value 0
                PT_UNIT_ASSERT( matrix.getValue(j, k) ==  0.0f );
            }
        }
    }
}


void MatrixTest::testIsIdentity()
{
    Pt::Math::BasicMatrix<float, 4, 4> matrix;
    matrix.setToNull();

    for(size_t i=0; i<matrix.colCount(); ++i)
    {
        matrix.setValue(1.0, i, i);
    }

    PT_UNIT_ASSERT( matrix.isIdentity() == true );

    matrix.setValue(42.0, 0, 0);
    PT_UNIT_ASSERT( matrix.isIdentity() == false );
}


void MatrixTest::testBracketOperator()
{
    Pt::Math::BasicMatrix<Pt::uint32_t, 8, 8> matrix;

    // fill the matrix with values
    for(size_t r = 0; r < matrix.rowCount(); ++r)
    {
        for(size_t c = 0; c < matrix.colCount(); ++c)
        {
            matrix.setValue((Pt::uint32_t)matrix.rowCount()*r + c, r, c);
        }
    }


    // check for the values with bracket operator
    for(size_t r = 0; r < matrix.rowCount(); ++r)
    {
        for(size_t c = 0; c < matrix.colCount(); ++c)
        {
            PT_UNIT_ASSERT( matrix(r, c) == (matrix.rowCount() * r + c) );
        }
    }
}
