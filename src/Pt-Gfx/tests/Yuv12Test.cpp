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

#include <Pt/Gfx/Yuv12Image.h>
#include <Pt/Gfx/Argb32Image.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Color.h>

#include <Pt/System/Clock.h>

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

Pt::uint8_t yuv12Data[] = 
{
    0,  1,  2,  3, // y plane
    4,  5,  6,  7, 
    8,  9,  10, 11,
    10, 13, 14, 15,

    100, 101, // u plane
    102, 103,

    200, 201, // v plane
    202, 203
};

class Yuv12Test : public Pt::Unit::TestSuite
{
    public:
        Yuv12Test()
        : Pt::Unit::TestSuite("Yuv12Test")
        {
            registerMethod("Pixel",*this, &Yuv12Test::Pixel);
            registerMethod("Iterator", *this, &Yuv12Test::Iterator);
            registerMethod("Color", *this, &Yuv12Test::Color);
            registerMethod("ConvertGenericArgb32ToYuv12", *this, &Yuv12Test::ConvertGenericArgb32ToYuv12);
            registerMethod("ConvertYuv12ToGenericYuv12", *this, &Yuv12Test::ConvertYuv12ToGenericYuv12);
            registerMethod("ConvertGenericYuv12ToGenericYuv12", *this, &Yuv12Test::ConvertGenericYuv12ToGenericYuv12);
            registerMethod("ConvertArgb32ToYuv12", *this, &Yuv12Test::ConvertArgb32ToYuv12);
            
            //registerMethod("BenchmarkA_Generic", *this, &Yuv12Test::Benchmark);
            //registerMethod("BenchmarkB_Direct", *this, &Yuv12Test::BenchmarkRaw);
        }

    protected:
        void Pixel()
        {
            using namespace Pt::Gfx;

            Yuv12Image image(yuv12Data, 4, 4);
            Yuv12PixelView imageView(image);

            Yuv12PixelView::Iterator pixel = imageView.pixel(1, 3);
            
            Pt::uint8_t y = pixel->y();
            Pt::uint8_t u = pixel->u();
            Pt::uint8_t v = pixel->v();
            
            PT_UNIT_ASSERT_EQUAL(y, 13);
            PT_UNIT_ASSERT_EQUAL(u, 102);
            PT_UNIT_ASSERT_EQUAL(v, 202);

            ++pixel;

            Pt::uint8_t y2 = pixel->y();
            Pt::uint8_t u2 = pixel->u();
            Pt::uint8_t v2 = pixel->v();
            
            PT_UNIT_ASSERT_EQUAL(y2, 14);
            PT_UNIT_ASSERT_EQUAL(u2, 103);
            PT_UNIT_ASSERT_EQUAL(v2, 203);
        }

        void Iterator()
        {
            using namespace Pt::Gfx;

            Yuv12Image image(yuv12Data, 4, 4);
            Yuv12PixelView imageView(image);

            Yuv12PixelView::Iterator it = imageView.begin();
            Yuv12PixelView::Iterator end = imageView.end();

            Pt::uint32_t u = 0;
            Pt::uint32_t v = 0;
            for( ; it != end; ++it)
            {
                v += (*it).v();
                u += (*it).u();
            }

            PT_UNIT_ASSERT_EQUAL(u, 406 * 4);
            PT_UNIT_ASSERT_EQUAL(v, 806 * 4);
        }

        void Color()
        {
            using namespace Pt::Gfx;

            Pt::uint8_t yuv12[] = { 100, 100, 100 };

            Yuv12Image image(yuv12, 1, 1);
            Yuv12PixelView imageView(image);

            Yuv12PixelView::Iterator pixel = imageView.pixel(0, 0);

            Pt::Gfx::ColorF c = pixel->toColor();
            *pixel = c;

            PT_UNIT_ASSERT(yuv12[0] > 99 && yuv12[0] < 101);
            PT_UNIT_ASSERT(yuv12[1] > 99 && yuv12[1] < 101);
            PT_UNIT_ASSERT(yuv12[2] > 99 && yuv12[2] < 101);
        }

        void ConvertGenericArgb32ToYuv12()
        {
            using namespace Pt::Gfx;

            Pt::uint8_t yuv12[] = { 100, 100, 100 };

            // copy an image in unknown format...
            Image image(1, 1, ImageFormat::argb32());

            // ...to a concrete YUV-12 image
            Yuv12Image yuv12Image(yuv12, 1, 1);
            ImageView yuv12View(yuv12Image);

            copyView(image, yuv12View);
        }

        void ConvertYuv12ToGenericYuv12()
        {
            using namespace Pt::Gfx;

            Pt::uint8_t yuv12[] = { 100, 100, 100 };

            // copy a concrete YUV-12 image...
            Yuv12Image image(1, 1);
            ConstImageView imageView(image);

            // ...to an image in YUV-12 image
            Image yuv12Image(yuv12, 1, 1, Yuv12());

            copyView(imageView, yuv12Image);
        }

        void ConvertGenericYuv12ToGenericYuv12()
        {
            using namespace Pt::Gfx;

            Pt::uint8_t yuv12[] = { 100, 100, 100 };

            // copy an image in yuv12 format...
            Image image(1, 1, Yuv12());

            // ...to an image in YUV-12 image
            Image yuv12Image(yuv12, 1, 1, Yuv12());

            copyView(image, yuv12Image);
        }

        void ConvertArgb32ToYuv12()
        {
            using namespace Pt::Gfx;

            Pt::uint8_t argb32[] = { 100, 100, 100, 100 };
            Pt::uint8_t yuv12[] = { 0, 0, 0 };

            // copy a concrete ARGB-32 image...
            Argb32Image image(argb32, 1, 1);

            // ...to a concrete YUV-12 image
            Yuv12Image yuv12Image(yuv12, 1, 1);

            //copyView(image, yuv12Image);
        }

        void Benchmark()
        {
            using namespace Pt::Gfx;

            Pt::uint64_t best = std::numeric_limits<Pt::uint64_t>::max();

            for(int n = 0; n < 10; ++n)
            {
                Yuv12 format;
                Image image(1000, 1000, format);
                PixelView pixels = pixelView(image);

                PixelView::Iterator it = pixels.begin();
                PixelView::Iterator end = pixels.end();
            
                Pt::Gfx::Color color(255, 100, 100, 100);
            
                Pt::System::Clock clock;
                clock.start();
            
                for( ; it != end; ++it)
                {
                    color = it->toColor();
                    color.setRed(99);

                    (*it) = color;
                }

                Pt::uint64_t time = clock.stop().toUSecs();
                if(time < best)
                    best = time;
            }

            std::clog << "GENERIC: " << best << std::endl;
        }

        void BenchmarkRaw()
        {
            using namespace Pt::Gfx;
            
            Pt::uint64_t best = std::numeric_limits<Pt::uint64_t>::max();

            for(int n = 0; n < 10; ++n)
            {
                Yuv12Image image(1000, 1000);
                Yuv12PixelView imageView(image);

                Yuv12PixelView::Iterator it = imageView.begin();
                Yuv12PixelView::Iterator end = imageView.end();
            
                Pt::Gfx::ColorF color(100, 100, 100);
            
                Pt::System::Clock clock;
                clock.start();
            
                for( ; it != end; ++it)
                {
                    color = it->toColor();
                    color.setRed(99);

                    (*it) = color;
                }

                Pt::uint64_t time = clock.stop().toUSecs();
                if(time < best)
                    best = time;
            }

            std::clog << "Yv12: " << best << std::endl;
        }
};

Pt::Unit::RegisterTest<Yuv12Test> register_Yuv12Test;
