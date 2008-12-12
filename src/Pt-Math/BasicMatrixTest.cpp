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
