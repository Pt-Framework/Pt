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

#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Rgb32.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>
#include <cstring>

namespace Pt {

namespace Gfx {

class BitmapTest : public Pt::Unit::TestSuite
{
    public:
        BitmapTest()
        : Pt::Unit::TestSuite("Pt::Gfx::BitmapTest")
        {
            registerMethod("ResetSizeReusesBuffer", *this,
                           &BitmapTest::ResetSizeReusesBuffer);
            registerMethod("ResetImageReusesBuffer", *this,
                           &BitmapTest::ResetImageReusesBuffer);
            registerMethod("ResetImageCopiesPixels", *this,
                           &BitmapTest::ResetImageCopiesPixels);
            registerMethod("ResetImageRectView", *this,
                           &BitmapTest::ResetImageRectView);
            registerMethod("ResetImageNegativeStride", *this,
                           &BitmapTest::ResetImageNegativeStride);
        }

        void ResetSizeReusesBuffer()
        {
            Bitmap bmp(SizeF(8, 8));
            const Pt::uint8_t* data = bmp.image().data();

            bmp.reset(SizeF(8, 8));

            PT_UNIT_ASSERT(data == bmp.image().data());
            PT_UNIT_ASSERT_EQUAL(bmp.image().width(), 8);
            PT_UNIT_ASSERT_EQUAL(bmp.image().height(), 8);
        }

        void ResetImageReusesBuffer()
        {
            Bitmap bmp(SizeF(4, 4));
            const Pt::uint8_t* data = bmp.image().data();

            Image src(4, 4, Rgb32());
            bmp.reset(src);

            PT_UNIT_ASSERT(data == bmp.image().data());
        }

        void ResetImageCopiesPixels()
        {
            Image src(3, 2, Rgb32());
            for(Pt::ssize_t y = 0; y < src.height(); ++y)
            {
                for(Pt::ssize_t x = 0; x < src.width(); ++x)
                {
                    Pt::uint8_t* p = src.data() + y * src.stride() + x * 4;
                    p[0] = static_cast<Pt::uint8_t>(x);
                    p[1] = static_cast<Pt::uint8_t>(y);
                    p[2] = 7;
                    p[3] = 255;
                }
            }

            Bitmap bmp;
            bmp.reset(src);

            PT_UNIT_ASSERT_EQUAL(bmp.image().width(), 3);
            PT_UNIT_ASSERT_EQUAL(bmp.image().height(), 2);

            for(Pt::ssize_t y = 0; y < src.height(); ++y)
            {
                const Pt::uint8_t* from = src.data() + y * src.stride();
                const Pt::uint8_t* to = bmp.image().data() + y * bmp.image().stride();
                PT_UNIT_ASSERT(std::memcmp(from, to, 3 * 4) == 0);
            }
        }

        void ResetImageRectView()
        {
            Image src(6, 4, Rgb32());
            for(Pt::ssize_t y = 0; y < src.height(); ++y)
            {
                for(Pt::ssize_t x = 0; x < src.width(); ++x)
                {
                    Pt::uint8_t* p = src.data() + y * src.stride() + x * 4;
                    p[0] = static_cast<Pt::uint8_t>(x);
                    p[1] = static_cast<Pt::uint8_t>(y);
                    p[2] = 9;
                    p[3] = 255;
                }
            }

            const Pt::ssize_t x = 2;
            const Pt::ssize_t y = 1;
            const Pt::ssize_t w = 3;
            const Pt::ssize_t h = 2;
            const Pt::ssize_t padding = src.stride() - w * src.pixelStride();
            Image view(src.data() + y * src.stride() + x * src.pixelStride(),
                       w, h, padding, src.format());

            Bitmap bmp;
            bmp.reset(view);

            PT_UNIT_ASSERT_EQUAL(bmp.image().width(), w);
            PT_UNIT_ASSERT_EQUAL(bmp.image().height(), h);

            for(Pt::ssize_t row = 0; row < h; ++row)
            {
                const Pt::uint8_t* from = src.data() + (y + row) * src.stride()
                                          + x * src.pixelStride();
                const Pt::uint8_t* to = bmp.image().data() + row * bmp.image().stride();
                PT_UNIT_ASSERT(std::memcmp(from, to, w * 4) == 0);
            }
        }

        void ResetImageNegativeStride()
        {
            const Pt::ssize_t w = 3;
            const Pt::ssize_t h = 2;
            const Pt::ssize_t bpr = w * 4;
            Pt::uint8_t buf[3 * 2 * 4];

            for(Pt::ssize_t visY = 0; visY < h; ++visY)
            {
                Pt::uint8_t* row = buf + (h - 1 - visY) * bpr;
                for(Pt::ssize_t x = 0; x < w; ++x)
                {
                    row[x * 4 + 0] = static_cast<Pt::uint8_t>(x);
                    row[x * 4 + 1] = static_cast<Pt::uint8_t>(visY);
                    row[x * 4 + 2] = 11;
                    row[x * 4 + 3] = 255;
                }
            }

            Pt::uint8_t* first = buf + (h - 1) * bpr;
            const Pt::ssize_t padding = -bpr - w * 4;
            Image src(first, w, h, padding, Rgb32());

            Bitmap bmp;
            bmp.reset(src);

            PT_UNIT_ASSERT_EQUAL(bmp.image().width(), w);
            PT_UNIT_ASSERT_EQUAL(bmp.image().height(), h);

            for(Pt::ssize_t y = 0; y < h; ++y)
            {
                const Pt::uint8_t* to = bmp.image().data() + y * bmp.image().stride();
                for(Pt::ssize_t x = 0; x < w; ++x)
                {
                    PT_UNIT_ASSERT_EQUAL(to[x * 4 + 0], static_cast<Pt::uint8_t>(x));
                    PT_UNIT_ASSERT_EQUAL(to[x * 4 + 1], static_cast<Pt::uint8_t>(y));
                    PT_UNIT_ASSERT_EQUAL(to[x * 4 + 2], 11);
                    PT_UNIT_ASSERT_EQUAL(to[x * 4 + 3], 255);
                }
            }
        }
};

} // namespace Gfx

} // namespace Pt

Pt::Unit::RegisterTest<Pt::Gfx::BitmapTest> register_BitmapTest;
