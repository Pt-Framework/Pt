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

#include <Pt/Gfx/Rgb32Image.h>
#include <Pt/Gfx/Color.h>

#include <Pt/System/Clock.h>

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

Pt::uint8_t rgb32Data[] = 
{
    0,  1,  2,  3,     4,  5,  6,  7, 
    8,  9,  10, 11,   12, 13, 14, 15,
    //16, 17, 18, 19,   20, 21, 22, 23,
    //24, 25, 26, 27,   28, 29, 30, 31
};

class Rgb32Test : public Pt::Unit::TestSuite
{
    public:
        Rgb32Test()
        : Pt::Unit::TestSuite("Rgb32Test")
        {
            registerMethod("Pixel",*this, &Rgb32Test::Pixel);
            registerMethod("Iterator",*this, &Rgb32Test::Iterator);
            registerMethod("Color",*this, &Rgb32Test::Color);
        }

        void Pixel()
        {
            using namespace Pt::Gfx;

            Rgb32Image image( rgb32Data, Size(2, 2) );
            Rgb32Image::PixelIterator pixel = image.pixel(1, 1);
            
            Pt::uint8_t a = pixel->alpha();
            Pt::uint8_t r = pixel->red();
            Pt::uint8_t g = pixel->green();
            Pt::uint8_t b = pixel->blue();
            
            PT_UNIT_ASSERT_EQUAL(a, 12);
            PT_UNIT_ASSERT_EQUAL(r, 13);
            PT_UNIT_ASSERT_EQUAL(g, 14);
            PT_UNIT_ASSERT_EQUAL(b, 15);
        }

        void Iterator()
        {
            using namespace Pt::Gfx;

            Rgb32Image image( rgb32Data, Size(2, 2) );

            Rgb32Image::PixelIterator it = image.begin();
            Rgb32Image::PixelIterator end = image.end();

            Pt::uint32_t blue = 0;
            for( ; it != end; ++it)
            {
                blue += (*it).blue();
            }

            PT_UNIT_ASSERT_EQUAL(blue, 36);
        }

        void Color()
        {
            using namespace Pt::Gfx;

            Pt::uint8_t rgb32[] = { 100, 100, 100, 100 };

            Rgb32Image image( rgb32, Size(1, 1) );
            Rgb32Image::PixelIterator pixel = image.pixel(0, 0);

            Pt::Gfx::Color c = pixel->toColor();
            pixel->assign(c, CompositionMode::SourceCopy);

            PT_UNIT_ASSERT(rgb32[0] > 99 && rgb32[0] < 101);
            PT_UNIT_ASSERT(rgb32[1] > 99 && rgb32[1] < 101);
            PT_UNIT_ASSERT(rgb32[2] > 99 && rgb32[2] < 101);
            PT_UNIT_ASSERT(rgb32[3] > 99 && rgb32[3] < 101);
        }
};

Pt::Unit::RegisterTest<Rgb32Test> register_Rgb32Test;
