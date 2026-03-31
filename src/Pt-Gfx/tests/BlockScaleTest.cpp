/* Copyright (C) 2016 Marc Boris Duerner 
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Argb32.h>
#include <Pt/Gfx/BlockScale.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

namespace Pt {

namespace Gfx {

class BlockScaleTest : public Pt::Unit::TestSuite
{
    public:
        BlockScaleTest()
        : Pt::Unit::TestSuite("Pt::Gfx::BlockScaleTest")
        {
            registerMethod("ScaleUp", *this, &BlockScaleTest::ScaleUp);
            registerMethod("ScaleDown", *this, &BlockScaleTest::ScaleDown);
        }

    protected:
        void ScaleUp()
        {
            const Color A(255,  10,  20,  30);
            const Color B(255,  40,  50,  60);
            const Color C(255,  70,  80,  90);
            const Color D(255, 100, 110, 120);

            // 2x2 source
            Argb32Image from(2, 2);
            Argb32PixelView fromView(from);
            *fromView.pixel(0, 0) = A;
            *fromView.pixel(1, 0) = B;
            *fromView.pixel(0, 1) = C;
            *fromView.pixel(1, 1) = D;

            // Scale to 4x6
            Argb32Image to(4, 6);
            blockScale(from, to);

            // Row-major expected result
            const Color expected[] = {
                A, A, B, B,
                A, A, B, B,
                A, A, B, B,
                C, C, D, D,
                C, C, D, D,
                C, C, D, D,
            };

            Argb32PixelView toView(to);
            const Color* exp = expected;
            for(auto it = toView.begin(); it != toView.end(); ++it, ++exp)
            {
                PT_UNIT_ASSERT_EQUAL(it->toColor().value(), exp->value());
            }
        }

        void ScaleDown()
        {
            const Color A(255,  10,  20,  30);
            const Color B(255,  40,  50,  60);
            const Color C(255,  70,  80,  90);
            const Color D(255, 100, 110, 120);
            const Color E(255, 130, 140, 150);
            const Color F(255, 160, 170, 180);
            const Color G(255,  11,  21,  31);
            const Color H(255,  41,  51,  61);

            // 4x2 source
            const Color src[] = {
                A, B, C, D,
                E, F, G, H,
            };

            Argb32Image from(4, 2);
            Argb32PixelView fromView(from);
            const Color* s = src;
            for(auto it = fromView.begin(); it != fromView.end(); ++it, ++s)
                *it = *s;

            // Scale to 2x1
            Argb32Image to(2, 1);
            blockScale(from, to);

            const Color expected[] = {
                A, C,
            };

            Argb32PixelView toView(to);
            const Color* exp = expected;
            for(auto it = toView.begin(); it != toView.end(); ++it, ++exp)
            {
                PT_UNIT_ASSERT_EQUAL(it->toColor().value(), exp->value());
            }
        }
};

} // namespace

} // namespace

Pt::Unit::RegisterTest<Pt::Gfx::BlockScaleTest> register_BlockScaleTest;
