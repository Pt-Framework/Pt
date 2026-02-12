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
#include <Pt/Gfx/Argb32.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Algorithm.h>

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
            registerMethod("Pixel",*this, &Argb32Test::Pixel);
            registerMethod("Iterator",*this, &Argb32Test::Iterator);
            registerMethod("Color",*this, &Argb32Test::Color);

            registerMethod("BenchmarkA_Generic", *this, &Argb32Test::Benchmark);
            registerMethod("BenchmarkB_Direct", *this, &Argb32Test::BenchmarkRaw);
            registerMethod("BenchmarkC_CopyColors", *this, &Argb32Test::BenchmarkCopyColors);
            registerMethod("BenchmarkD_CopyColors_Direct", *this, &Argb32Test::BenchmarkCopyColorsRaw);
            registerMethod("BenchmarkE_CopyPixels", *this, &Argb32Test::BenchmarkCopyPixels);
            registerMethod("BenchmarkF_CopyPixels_Direct", *this, &Argb32Test::BenchmarkCopyPixelsRaw);
        }

        void Pixel()
        {
            using namespace Pt::Gfx;

            Pt::uint8_t* data = reinterpret_cast<Pt::uint8_t*>(argb32Data);

            Argb32Image image(data, 2, 2);
            Argb32PixelView imageView(image);
            
            Argb32PixelView::PixelIterator pixel = imageView.pixel(1, 1);
            
            Pt::uint8_t a = pixel->alpha();
            Pt::uint8_t r = pixel->red();
            Pt::uint8_t g = pixel->green();
            Pt::uint8_t b = pixel->blue();
            
            PT_UNIT_ASSERT_EQUAL(a, 0x12);
            PT_UNIT_ASSERT_EQUAL(r, 0x13);
            PT_UNIT_ASSERT_EQUAL(g, 0x14);
            PT_UNIT_ASSERT_EQUAL(b, 0x15);
        }

        void Iterator()
        {
            using namespace Pt::Gfx;

            Pt::uint8_t* data = reinterpret_cast<Pt::uint8_t*>(argb32Data);

            Argb32Image image(data, 2, 2);
            Argb32PixelView view(image);

            Argb32PixelView::PixelIterator it = view.begin();
            Argb32PixelView::PixelIterator end = view.end();

            Pt::uint32_t blue = 0;
            for( ; it != end; ++it)
            {
                blue += (*it).blue();
            }

            PT_UNIT_ASSERT_EQUAL(blue, 48);
        }

        void Color()
        {
            using namespace Pt::Gfx;

            Pt::uint32_t argb32[] = { 0xaabbccdd };

            Pt::uint8_t* data = reinterpret_cast<Pt::uint8_t*>(argb32Data);

            Argb32Image image(data, 1, 1);
            Argb32PixelView imageView(image);
            Argb32PixelView::PixelIterator pixel = imageView.pixel(0, 0);

            Pt::Gfx::Argb32Color c = pixel->toColor();
            *pixel = c;

            PT_UNIT_ASSERT(argb32[0] == 0xaabbccdd);

            Image i(Argb32(), 2, 2);
            ConstColorView<Argb32Color> pixelView(i);

            Argb32Image a(2, 2);
            Argb32PixelView argb32View(a);

            Pt::Gfx::copy( pixelView.begin(), pixelView.end(), argb32View.begin() );
        }
        
        static const std::size_t width = 64;
        static const std::size_t height = 10000;

        void Benchmark()
        {
            using namespace Pt::Gfx;
           
            Pt::uint64_t best = std::numeric_limits<Pt::uint64_t>::max();

            for(int n = 0; n < 10; ++n)
            {
                Argb32 format;
                Image image(format, width, height);
                
                PixelView pixelView(image);
                PixelView::PixelIterator it = pixelView.begin();
                PixelView::PixelIterator end = pixelView.end();

                ConstColorView<Argb32Color>  colorView(image);
                auto from = colorView.begin();

                Pt::System::Clock clock;
                clock.start();

                for( ; it != end; ++it)
                {
                    (*it) = *from;
                }

                Pt::uint64_t time = clock.stop().toUSecs();
                if(time < best)
                    best = time;
            }

            std::clog << "GENERIC: " << (width * height) / double(best) << std::endl;
        }

        void BenchmarkRaw()
        {
            using namespace Pt::Gfx;

            Pt::uint64_t best = std::numeric_limits<Pt::uint64_t>::max();

            for(int n = 0; n < 10; ++n)
            {
                Argb32Image image(width, height);
                
                Argb32PixelView imageView(image);
                Argb32PixelView::PixelIterator it = imageView.begin();
                Argb32PixelView::PixelIterator end = imageView.end();

                Argb32ConstPixelView constView(image);
                Argb32Pixel cpixel = *imageView.begin();

                Pt::System::Clock clock;
                clock.start();
            
                for( ; it != end; ++it)
                {
                    (*it) = cpixel;
                }

                Pt::uint64_t time = clock.stop().toUSecs();
                if(time < best)
                    best = time;
            }

            std::clog << "ARGB32: " << (width * height) / double(best) << std::endl;
        }

        void BenchmarkCopyPixels()
        {
            using namespace Pt::Gfx;
           
            Pt::uint64_t best = std::numeric_limits<Pt::uint64_t>::max();

            for(int n = 0; n < 10; ++n)
            {
                Argb32 format;
                Image fromImage(format, width, height);
                ConstPixelView fromView(fromImage);
                ConstPixelView::Iterator from = fromView.begin();

                Image image(format, width, height);
                PixelView imageView(image);
                PixelView::PixelIterator it = imageView.begin();
                PixelView::PixelIterator end = imageView.end();

                Pt::System::Clock clock;
                clock.start();

                for( ; it != end; it += image.width(), from += fromImage.width() )
                {
                    it->assign( *from, image.width() );
                }

                Pt::uint64_t time = clock.stop().toUSecs();
                if(time < best)
                    best = time;
            }

            std::clog << "GENERIC: " << (width * height) / double(best) << std::endl;
        }

        void BenchmarkCopyPixelsRaw()
        {
            using namespace Pt::Gfx;
           
            Pt::uint64_t best = std::numeric_limits<Pt::uint64_t>::max();
            
            for(int n = 0; n < 10; ++n)
            {
                Argb32Image fromImage(width, height);
                Argb32PixelView fromView(fromImage);
                Argb32PixelView::PixelIterator from = fromView.begin();

                Argb32Image image(width, height);
                Argb32PixelView imageView(image);
                Argb32PixelView::PixelIterator it = imageView.begin();
                Argb32PixelView::PixelIterator end = imageView.end();

                Pt::System::Clock clock;
                clock.start();

                for( ; it != end; it += image.width(), from += fromImage.width() )
                {
                    it->assign( *from, image.width() );
                }

                Pt::uint64_t time = clock.stop().toUSecs();
                if(time < best)
                    best = time;
            }

            std::clog << "ARGB32: " << (width * height) / double(best) << std::endl;
        }

        void BenchmarkCopyColors()
        {
            using namespace Pt::Gfx;
           
            Pt::uint64_t best = std::numeric_limits<Pt::uint64_t>::max();

            for(int n = 0; n < 10; ++n)
            {
                Argb32 format;
                Image fromImage(format, width, height);
                ConstPixelView fromView(fromImage);
                ConstPixelView::Iterator from = fromView.begin();

                Image image(format, width, height);
                PixelView imageView(image);
                PixelView::PixelIterator it = imageView.begin();
                PixelView::PixelIterator end = imageView.end();

                Pt::System::Clock clock;
                clock.start();

                Argb32Color colors[width];

                for( ; it != end; it += image.width(), from += fromImage.width() )
                {
                    it->getColors(colors, width);
                    it->assign(colors, width);
                }

                Pt::uint64_t time = clock.stop().toUSecs();
                if(time < best)
                    best = time;
            }

            std::clog << "GENERIC: " << (width * height) / double(best) << std::endl;
        }

        void BenchmarkCopyColorsRaw()
        {
            using namespace Pt::Gfx;
           
            Pt::uint64_t best = std::numeric_limits<Pt::uint64_t>::max();

            for(int n = 0; n < 10; ++n)
            {
                Argb32Image fromImage(width, height);
                Argb32PixelView fromView(fromImage);
                Argb32PixelView::PixelIterator from = fromView.begin();

                Argb32Image image(width, height);
                Argb32PixelView imageView(image);
                Argb32PixelView::PixelIterator it = imageView.begin();
                Argb32PixelView::PixelIterator end = imageView.end();

                Pt::System::Clock clock;
                clock.start();

                for( ; it != end; it += image.width(), from += fromImage.width() )
                {
                    it->assign( *from, image.width() );
                }

                Pt::uint64_t time = clock.stop().toUSecs();
                if(time < best)
                    best = time;
            }

            std::clog << "ARGB32: " << (width * height) / double(best) << std::endl;
        }

};

Pt::Unit::RegisterTest<Argb32Test> register_Argb32Test;
