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

#include <Pt/Gfx/PngReader.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <sstream>

namespace Pt {

namespace Gfx {

// 2x2 RGB PNG: row 0 = red(255,0,0) + green(0,255,0), row 1 = blue(0,0,255) + white(255,255,255)
// Generated with System.Drawing (GDI+), 129 bytes
static const unsigned char sg_png2x2Rgb[] =
{
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x08, 0x02, 0x00, 0x00, 0x00, 0xfd, 0xd4, 0x9a,
    0x73, 0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47, 0x42, 0x00, 0xae, 0xce, 0x1c, 0xe9, 0x00, 0x00,
    0x00, 0x04, 0x67, 0x41, 0x4d, 0x41, 0x00, 0x00, 0xb1, 0x8f, 0x0b, 0xfc, 0x61, 0x05, 0x00, 0x00,
    0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0e, 0xc3, 0x00, 0x00, 0x0e, 0xc3, 0x01, 0xc7,
    0x6f, 0xa8, 0x64, 0x00, 0x00, 0x00, 0x16, 0x49, 0x44, 0x41, 0x54, 0x18, 0x57, 0x63, 0xf8, 0xcf,
    0xc0, 0xc0, 0xd0, 0xc0, 0xc0, 0xc0, 0xc0, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1b, 0x77, 0x05,
    0x7c, 0xf5, 0x3c, 0x85, 0x79, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60,
    0x82
};


class PngTest : public Pt::Unit::TestSuite
{
    public:
        PngTest()
        : Pt::Unit::TestSuite("Pt::Gfx::PngTest")
        {
            registerMethod("GetWholeImage", *this, &PngTest::GetWholeImage);
            registerMethod("AdvanceInOneCall", *this, &PngTest::AdvanceInOneCall);
            registerMethod("AdvanceIncremental", *this, &PngTest::AdvanceIncremental);
        }

        void GetWholeImage()
        {
            std::string data(reinterpret_cast<const char*>(sg_png2x2Rgb), sizeof(sg_png2x2Rgb));
            std::istringstream stream(data, std::ios::binary);
            Pt::Gfx::Image image;

            PngReader reader(stream, image);
            Pt::Gfx::Image& result = reader.get();

            PT_UNIT_ASSERT_EQUAL(result.width(), 2);
            PT_UNIT_ASSERT_EQUAL(result.height(), 2);
        }

        void AdvanceInOneCall()
        {
            std::string data(reinterpret_cast<const char*>(sg_png2x2Rgb), sizeof(sg_png2x2Rgb));
            std::istringstream stream(data, std::ios::binary);
            Pt::Gfx::Image image;

            PngReader reader(stream, image);
            const Pt::Gfx::Image* result = reader.advance( static_cast<std::streamsize>(sizeof(sg_png2x2Rgb)) );

            PT_UNIT_ASSERT( result != 0 );
            PT_UNIT_ASSERT_EQUAL(result->width(), 2);
            PT_UNIT_ASSERT_EQUAL(result->height(), 2);
        }

        void AdvanceIncremental()
        {
            std::string data(reinterpret_cast<const char*>(sg_png2x2Rgb), sizeof(sg_png2x2Rgb));
            std::istringstream stream(data, std::ios::binary);
            Pt::Gfx::Image image;

            PngReader reader(stream, image);
            const Pt::Gfx::Image* result = 0;

            for(std::size_t i = 0; i < sizeof(sg_png2x2Rgb) && ! result; ++i)
                result = reader.advance(1);

            PT_UNIT_ASSERT( result != 0 );
            PT_UNIT_ASSERT_EQUAL(result->width(), 2);
            PT_UNIT_ASSERT_EQUAL(result->height(), 2);
        }
};

} // namespace

} // namespace

Pt::Unit::RegisterTest<Pt::Gfx::PngTest> register_PngTest;
