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
#include <Pt/Gfx/Image.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

namespace Pt {

namespace Gfx {

class Rgb32Test : public Pt::Unit::TestSuite
{
    public:
        Rgb32Test()
        : Pt::Unit::TestSuite("Pt::Gfx::Rgb32Test")
        {
            registerMethod("ImageTraits", *this, &Rgb32Test::ImageTraits);
        }

        void ImageTraits()
        {
            // Rgb32Image — ImageTraits<Rgb32>
            Gfx::Rgb32Image rgb32Image(10, 5);
            PT_UNIT_ASSERT_EQUAL(rgb32Image.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(rgb32Image.size(), std::size_t(200));
            PT_UNIT_ASSERT(rgb32Image.format().type() == typeid(Rgb32));

            const Gfx::Rgb32ConstImage constRgb32Image(rgb32Image);
            PT_UNIT_ASSERT_EQUAL(constRgb32Image.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(constRgb32Image.size(), std::size_t(200));
            PT_UNIT_ASSERT(constRgb32Image.format().type() == typeid(Rgb32));

            // Image — ImageTraits<ImageFormat>
            Gfx::Image image(10, 5, Rgb32::get());
            PT_UNIT_ASSERT_EQUAL(image.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(image.size(), std::size_t(200));

            const Gfx::ConstImage constImage(image);
            PT_UNIT_ASSERT_EQUAL(constImage.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(constImage.size(), std::size_t(200));

            // ImageF — ImageTraitsF
            Gfx::ImageF imageF(10, 5, Rgb32::get());
            PT_UNIT_ASSERT_EQUAL(imageF.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(imageF.size(), std::size_t(200));

            const Gfx::ConstImageF constImageF(imageF);
            PT_UNIT_ASSERT_EQUAL(constImageF.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(constImageF.size(), std::size_t(200));
        }
};

} // namespace Gfx

} // namespace Pt

Pt::Unit::RegisterTest<Pt::Gfx::Rgb32Test> register_Rgb32Test;
