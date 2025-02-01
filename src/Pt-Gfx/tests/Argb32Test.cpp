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
#include <Pt/Gfx/Argb32Format.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Color.h>

#include <Pt/System/Clock.h>

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

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
        }

        void Pixel()
        {
            using namespace Pt::Gfx;

            Pt::uint8_t* data = reinterpret_cast<Pt::uint8_t*>(argb32Data);

            Argb32Image image(data, 2, 2);
            Argb32Image::PixelIterator pixel = image.pixel(1, 1);
            
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

            Argb32Image::PixelIterator it = image.begin();
            Argb32Image::PixelIterator end = image.end();

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
            Argb32Image::PixelIterator pixel = image.pixel(0, 0);

            Pt::Gfx::Color c = pixel->getColor();
            pixel->assign(c, CompositionMode::SourceCopy);

            PT_UNIT_ASSERT(argb32[0] == 0xaabbccdd);
        }

        void Benchmark()
        {
            using namespace Pt::Gfx;
           
            Pt::uint64_t best = std::numeric_limits<Pt::uint64_t>::max();

            for(int n = 0; n < 10; ++n)
            {
                Argb32Format format;
                Image image( format, 1000, 1000 );
                Image::PixelIterator it = image.begin();
                Image::PixelIterator end = image.end();
            
                Pt::Gfx::Color color(100, 100, 100);
            
                Image::Pixel pixel = *image.begin();
                Image::ConstPixel cpixel(pixel);

                Pt::System::Clock clock;
                clock.start();
            
                for( ; it != end; ++it)
                {
                    Pt::Gfx::Color c = it->getColor();
                    c.setRed(99);
                    
                    //(*it) = c;
                    (*it) = cpixel;
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
                Argb32Format format;
                Image image( format, 1000, 1000 );

                Argb32View argbView( image.data(), image.width(), image.height() );
                Argb32View::PixelIterator it = argbView.begin();
                Argb32View::PixelIterator end = argbView.end();
            
                Pt::Gfx::Color color(100, 100, 100);
            
                Argb32View::Pixel pixel = *argbView.begin();
                Argb32View::ConstPixel cpixel(pixel);

                Pt::System::Clock clock;
                clock.start();
            
                for( ; it != end; ++it)
                {
                    Pt::Gfx::Color c = it->getColor();
                    c.setRed(99);
                    
                    //(*it) = c;
                    (*it) = cpixel;
                }

                Pt::uint64_t time = clock.stop().toUSecs();
                if(time < best)
                    best = time;
            }

            std::clog << "ARGB32: " << best << std::endl;
        }
};

Pt::Unit::RegisterTest<Argb32Test> register_Argb32Test;
