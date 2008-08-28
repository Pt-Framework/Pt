#undef PT_MATH_API_EXPORT

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

#include <Pt/Math/Matrix.h>


class BasicMatrixTest : public Pt::Unit::TestSuite
{
public:
    BasicMatrixTest()
    : TestSuite("BasicMatrixTest")
    {
        Pt::Unit::TestSuite::registerMethod("testMultiplication", *this, &BasicMatrixTest::testMultiplication);
    }

protected:
    void testMultiplication();
};

Pt::Unit::RegisterTest<BasicMatrixTest> register_MathBasicMatrixTest;


void BasicMatrixTest::testMultiplication()
{

    Pt::Math::BasicMatrix<Pt::uint8_t, 5, 5> identity;
    identity.setToIdentity();
    Pt::Math::BasicMatrix<Pt::uint8_t, 2, 5> matrix2x5(std::vector<Pt::uint8_t>(10, 42));
    Pt::Math::BasicMatrix<Pt::uint8_t, 2, 5> result;
    Pt::Math::BasicMatrix<Pt::uint8_t, 2, 5> expectedResult(std::vector<Pt::uint8_t>(10, 42));
    result = matrix2x5 * identity;
    PT_UNIT_ASSERT(result == expectedResult);


    Pt::Math::BasicMatrix<Pt::uint8_t, 5, 5> nullMatrix;
    PT_UNIT_ASSERT(nullMatrix == identity * nullMatrix);


    Pt::Math::BasicMatrix<Pt::uint8_t, 1, 1> two(std::vector<Pt::uint8_t>(1, 2));
    Pt::Math::BasicMatrix<Pt::uint8_t, 1, 1> three(std::vector<Pt::uint8_t>(1, 3));
    Pt::Math::BasicMatrix<Pt::uint8_t, 1, 1> six(std::vector<Pt::uint8_t>(1, 6));
    PT_UNIT_ASSERT(six == two * three);
    PT_UNIT_ASSERT(six == three * two);
}
