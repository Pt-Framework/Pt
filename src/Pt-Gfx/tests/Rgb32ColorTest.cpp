/* Copyright (C) 2026 Marc Boris Duerner
  
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

#include <Pt/Gfx/Rgb32.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

namespace Pt {

namespace Gfx {

class Rgb32ColorTest : public Pt::Unit::TestSuite
{
    public:
        Rgb32ColorTest()
        : Pt::Unit::TestSuite("Pt::Gfx::Rgb32ColorTest")
        {
            registerMethod("DefaultConstruct", *this, &Rgb32ColorTest::DefaultConstruct);
            registerMethod("ConstructFromChannels", *this, &Rgb32ColorTest::ConstructFromChannels);
            registerMethod("ConstructFromValue", *this, &Rgb32ColorTest::ConstructFromValue);
            registerMethod("ConstructFromBytes", *this, &Rgb32ColorTest::ConstructFromBytes);
            registerMethod("CopyConstruct", *this, &Rgb32ColorTest::CopyConstruct);
            registerMethod("CopyAssign", *this, &Rgb32ColorTest::CopyAssign);
            registerMethod("SetChannels", *this, &Rgb32ColorTest::SetChannels);
        }

        void DefaultConstruct()
        {
            Gfx::Rgb32Color c0;
            assertColor(c0, 0, 0, 0, 0);
            PT_UNIT_ASSERT_EQUAL(c0.value(), Pt::uint32_t(0));
        }

        void ConstructFromChannels()
        {
            Gfx::Rgb32Color c1(0xFF, 0x80, 0x40, 0x20);
            assertColor(c1, 0xFF, 0x80, 0x40, 0x20);
            PT_UNIT_ASSERT_EQUAL(c1.value(), Pt::uint32_t(0xFF804020));
        }

        void ConstructFromValue()
        {
            Gfx::Rgb32Color c2(Pt::uint32_t(0xAABBCCDD));
            assertColor(c2, 0xAA, 0xBB, 0xCC, 0xDD);
        }

        void ConstructFromBytes()
        {
            const Pt::uint8_t bytes[] = { 0x20, 0x40, 0x80, 0xFF };

            Gfx::Rgb32Color c3(bytes);
            assertColor(c3, 0xFF, 0x80, 0x40, 0x20);
            PT_UNIT_ASSERT_EQUAL(c3.value(), Pt::uint32_t(0xFF804020));
        }

        void CopyConstruct()
        {
            Gfx::Rgb32Color c2(Pt::uint32_t(0xAABBCCDD));
            Gfx::Rgb32Color c4(c2);

            assertColor(c4, 0xAA, 0xBB, 0xCC, 0xDD);
            PT_UNIT_ASSERT_EQUAL(c4.value(), c2.value());
        }

        void CopyAssign()
        {
            Gfx::Rgb32Color c1(0xFF, 0x80, 0x40, 0x20);
            Gfx::Rgb32Color c4;

            c4 = c1;
            assertColor(c4, 0xFF, 0x80, 0x40, 0x20);
            PT_UNIT_ASSERT_EQUAL(c4.value(), c1.value());
        }

        void SetChannels()
        {
            Gfx::Rgb32Color c5(0x11, 0x22, 0x33, 0x44);

            c5.setAlpha(0xAA);
            assertColor(c5, 0xAA, 0x22, 0x33, 0x44);

            c5.setRed(0xBB);
            assertColor(c5, 0xAA, 0xBB, 0x33, 0x44);

            c5.setGreen(0xCC);
            assertColor(c5, 0xAA, 0xBB, 0xCC, 0x44);

            c5.setBlue(0xDD);
            assertColor(c5, 0xAA, 0xBB, 0xCC, 0xDD);
        }

    private:
        static void assertColor(const Gfx::Rgb32Color& color,
                                Pt::uint8_t alpha,
                                Pt::uint8_t red,
                                Pt::uint8_t green,
                                Pt::uint8_t blue)
        {
            PT_UNIT_ASSERT_EQUAL(color.alpha(), alpha);
            PT_UNIT_ASSERT_EQUAL(color.red(),   red);
            PT_UNIT_ASSERT_EQUAL(color.green(), green);
            PT_UNIT_ASSERT_EQUAL(color.blue(),  blue);
        }
};

} // namespace Gfx

} // namespace Pt

Pt::Unit::RegisterTest<Pt::Gfx::Rgb32ColorTest> register_Rgb32ColorTest;