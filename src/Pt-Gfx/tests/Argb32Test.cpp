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

#include <Pt/Gfx/Argb32Image.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/CopyPixel.h>
#include <Pt/System/Clock.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

#include <algorithm>

Pt::uint32_t argb32Data[] = 
{
    0x00010203, 0x04050607, 
    0x08091011, 0x12131415,
};

class Argb32Test : public Pt::Unit::TestSuite
{
    public:
        Argb32Test()
        : Pt::Unit::TestSuite("Argb32Test")
        {
            registerMethod("A_SubView",*this, &Argb32Test::SubView);
            registerMethod("Pixel",*this, &Argb32Test::Pixel);
            registerMethod("Iterator",*this, &Argb32Test::Iterator);
            registerMethod("ColorAssign",*this, &Argb32Test::ColorAssign);
            registerMethod("ColorCopy",*this, &Argb32Test::ColorCopy);

            registerMethod("BenchmarkA_Pixel", *this, &Argb32Test::BenchmarkPixel);
            registerMethod("BenchmarkB_SpanPixel", *this, &Argb32Test::BenchmarkSpanPixel);
            registerMethod("BenchmarkC_CopyColors", *this, &Argb32Test::BenchmarkCopyColors);
            registerMethod("BenchmarkD_CopyView_Generic", *this, &Argb32Test::BenchmarkCopyView);
            registerMethod("BenchmarkE_CopyView_Argb32", *this, &Argb32Test::BenchmarkCopyViewArgb32);
        }

        void Pixel()
        {
            using namespace Pt::Gfx;

            Pt::uint8_t* data = reinterpret_cast<Pt::uint8_t*>(argb32Data);

            Argb32Image image(data, 2, 2);
            Argb32PixelView::Iterator pixel(image, 1, 1);
            
            Pt::uint8_t a = pixel->alpha();
            Pt::uint8_t r = pixel->red();
            Pt::uint8_t g = pixel->green();
            Pt::uint8_t b = pixel->blue();
            
            PT_UNIT_ASSERT_EQUAL(a, 0x12);
            PT_UNIT_ASSERT_EQUAL(r, 0x13);
            PT_UNIT_ASSERT_EQUAL(g, 0x14);
            PT_UNIT_ASSERT_EQUAL(b, 0x15);
        }

        void SubView()
        {
            using namespace Pt::Gfx;

            Pt::uint8_t* data = reinterpret_cast<Pt::uint8_t*>(argb32Data);

            Argb32Image from(data, 2, 2);
            Argb32Image to(8, 8, 4);
            
            Argb32View subView(to, 1, 1, 2, 2);
            copyView(from, subView);

            Argb32PixelView pixelView(to);
            Argb32PixelView::Iterator pixel = pixelView.pixel(2, 2);
            Color color = pixel->toColor();
            PT_UNIT_ASSERT(color.value() == 0x12131415);
        }

        void Iterator()
        {
            using namespace Pt::Gfx;

            Pt::uint8_t* data = reinterpret_cast<Pt::uint8_t*>(argb32Data);

            Argb32Image image(data, 2, 2);
            Argb32PixelView view(image);

            Argb32PixelView::Iterator it = view.begin();
            Argb32PixelView::Iterator end = view.end();

            Pt::uint32_t blue = 0;
            for( ; it != end; ++it)
            {
                blue += (*it).blue();
            }

            PT_UNIT_ASSERT_EQUAL(blue, 48);
        }

        void ColorAssign()
        {
            using namespace Pt::Gfx;

            Pt::uint32_t argb32[] = { 0xaabbccdd };
            Pt::uint8_t* data = reinterpret_cast<Pt::uint8_t*>(argb32Data);

            Argb32Image image(data, 1, 1);
            Argb32PixelView imageView(image);
            Argb32PixelView::Iterator pixel = imageView.pixel(0, 0);

            Color c = pixel->toColor();
            *pixel = c;

            PT_UNIT_ASSERT(argb32[0] == 0xaabbccdd);
        }
        
        void ColorCopy()
        {
            using namespace Pt::Gfx;

            Image image( 2, 2, Argb32() );
            const ConstImage cimage(image);
            PixelView pixelView(image);
            ConstPixelView cpixelView(image);

            Argb32Image argb32Image(2, 2);
            const Argb32ConstImage cargb32Image(argb32Image);
            Argb32PixelView argb32View(argb32Image);
            Argb32ConstPixelView cargb32View(argb32Image);

            std::transform( cpixelView.begin(), cpixelView.end(), argb32View.begin(),
                            [](const ConstPixelView::ConstPixel& p) 
                            { return p.toColor(); });

            std::transform( argb32View.begin(), argb32View.end(), pixelView.begin(),
                            [](const Argb32Pixel& p) 
                            { return p.toColor(); });

            ImageView imageView(image);

            copyView(imageView, imageView);
            copyView(image, imageView);
            copyView(cimage, imageView);
            copyView(cargb32Image, imageView);

            copyPixel(*cpixelView.begin(), *pixelView.begin());
            copyPixel(*cpixelView.begin(), *argb32View.begin());
            copyPixel(*cargb32View.begin(), *pixelView.begin());
            copyPixel(*cargb32View.begin(), *argb32View.begin());
        }

        static const std::size_t width = 64;
        static const std::size_t height = 10000;
        static const std::size_t runs = 2000;

        void BenchmarkPixel()
        {
            using namespace Pt::Gfx;
           
            Pt::uint64_t best = std::numeric_limits<Pt::uint64_t>::max();

            for(int n = 0; n < 100; ++n)
            {
                Argb32 format;
                Image from(width, height, format);
                Image to(width, height, format);

                ConstPixelView fromView(from);
                ConstPixelView::Iterator it = fromView.begin();
                ConstPixelView::Iterator end = fromView.end();

                PixelView toView(to);
                PixelView::Iterator toIt = toView.begin();

                Pt::System::Clock clock;
                clock.start();

                for( ; it != end; ++it, ++toIt)
                {
                    copyPixel(*it, *toIt);
                }

                Pt::uint64_t time = clock.stop().toUSecs();
                if(time < best)
                    best = time;
            }

            std::clog << "GENERIC pixel-wise: " << (width * height) / double(best) << std::endl;
        }

        void BenchmarkSpanPixel()
        {
            using namespace Pt::Gfx;
           
            Pt::uint64_t best = std::numeric_limits<Pt::uint64_t>::max();

            for(int n = 0; n < 100; ++n)
            {
                Argb32 format;
                Image from(width, height, format);
                Image to(width, height, format);

                LineView fromLineView(from);
                auto fromLine = fromLineView.begin();
                auto fromEnd = fromLineView.end();

                LineView toLineView(to);
                auto toLine = toLineView.begin();

                Pt::System::Clock clock;
                clock.start();
                
                for( ; fromLine != fromEnd; ++fromLine, ++toLine)
                {
                    auto fromIt = fromLine->begin();
                    auto fromPixelEnd = fromLine->end();
                    auto toIt = toLine->begin();

                    for( ; fromIt != fromPixelEnd; ++fromIt, ++toIt)
                    {
                        copyPixel(*fromIt, *toIt);
                    }
                }

                Pt::uint64_t time = clock.stop().toUSecs();
                if(time < best)
                    best = time;
            }

            std::clog << "GENERIC line-wise: " << (width * height) / double(best) << std::endl;
        }

        void BenchmarkCopyViewArgb32()
        {
            using namespace Pt::Gfx;

            Pt::uint64_t best = std::numeric_limits<Pt::uint64_t>::max();

            for(int n = 0; n < runs; ++n)
            {
                Argb32Image fromImage(width, height);
                Argb32Image toImage(width, height);

                Pt::System::Clock clock;
                clock.start();

                copyView(fromImage, toImage);

                Pt::uint64_t time = clock.stop().toUSecs();
                if(time < best)
                    best = time;
            }

            std::clog << "ARGB32 copy view: " << (width * height) / double(best) << std::endl;
        }

        void BenchmarkCopyView()
        {
            using namespace Pt::Gfx;
           
            Pt::uint64_t best = std::numeric_limits<Pt::uint64_t>::max();

            for(int n = 0; n < runs; ++n)
            {
                Argb32 format;
                Image fromImage(width, height, format);
                Image toImage(width, height, format);

                Pt::System::Clock clock;
                clock.start();

                copyView(fromImage, toImage);

                Pt::uint64_t time = clock.stop().toUSecs();
                if(time < best)
                    best = time;
            }

            std::clog << "GENERIC copy view: " << (width * height) / double(best) << std::endl;
        }

        void BenchmarkCopyColors()
        {
            using namespace Pt::Gfx;
           
            Pt::uint64_t best = std::numeric_limits<Pt::uint64_t>::max();

            for(int n = 0; n < runs; ++n)
            {
                Argb32 format;
                Image fromImage(width, height, format);
                Image toImage(width, height, format);

                LineView toLineView(toImage);
                auto toLine = toLineView.begin();

                Pt::System::Clock clock;
                clock.start();

                Color colors[width];

                for(const auto& fromSpan : lineView(fromImage))
                {
                    fromSpan.front().getColors(colors, width);
                    toLine->front().assign(colors, width);
                    ++toLine;
                }

                Pt::uint64_t time = clock.stop().toUSecs();
                if(time < best)
                    best = time;
            }

            std::clog << "GENERIC copy colors: " << (width * height) / double(best) << std::endl;
        }
};

Pt::Unit::RegisterTest<Argb32Test> register_Argb32Test;
