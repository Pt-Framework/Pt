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
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <vector>

namespace Pt {

namespace Gfx {

class Rgb32Test : public Pt::Unit::TestSuite
{
    public:
        Rgb32Test()
        : Pt::Unit::TestSuite("Pt::Gfx::Rgb32Test")
        , inputColors { // colors
                Color(255, 200, 100, 50),  Color(0,   200, 100, 50),
                Color(128, 200, 100, 50),  Color(64,  180,  60, 30),
            }
        , rgb32Data { // rgb32Data
                50, 100, 200, 255,   0,   0,   0,   0,
                25,  50, 100, 128,   8,  15,  45,  64,
            }
        , expectedColors { // outputColors
                Color(255, 200, 100, 50),  Color(0,     0,   0,  0),
                Color(128, 199,  99, 49),  Color(64,  179,  59, 31),
            }
        , rgb32Colors { // rgb32Colors
                Gfx::Rgb32Color(255, 200, 100, 50),  Gfx::Rgb32Color(  0,   0,   0,  0),
                Gfx::Rgb32Color(128, 100,  50, 25),  Gfx::Rgb32Color( 64,  45,  15,  8),
            }
        {
            registerMethod("ImageTraits", *this, &Rgb32Test::ImageTraits);
            registerMethod("Rgb32Color", *this, &Rgb32Test::Rgb32Color);
            registerMethod("GenericSetColor", *this, &Rgb32Test::GenericSetColor);
            registerMethod("GenericGetColor", *this, &Rgb32Test::GenericGetColor);
            registerMethod("DirectSetRgb32Color", *this, &Rgb32Test::DirectSetRgb32Color);
            registerMethod("DirectGetColor", *this, &Rgb32Test::DirectGetColor);
            registerMethod("DirectGetRgb32Color", *this, &Rgb32Test::DirectGetRgb32Color);
        }

        void ImageTraits()
        {
            // Rgb32Image
            Gfx::Rgb32Image rgb32Image(10, 5);
            PT_UNIT_ASSERT_EQUAL(rgb32Image.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(rgb32Image.size(), std::size_t(200));
            PT_UNIT_ASSERT(rgb32Image.format().type() == typeid(Rgb32));

            const Gfx::Rgb32ConstImage constRgb32Image(rgb32Image);
            PT_UNIT_ASSERT_EQUAL(constRgb32Image.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(constRgb32Image.size(), std::size_t(200));
            PT_UNIT_ASSERT(constRgb32Image.format().type() == typeid(Rgb32));

            // Image
            Gfx::Image image(10, 5, Rgb32::get());
            PT_UNIT_ASSERT_EQUAL(image.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(image.size(), std::size_t(200));

            const Gfx::ConstImage constImage(image);
            PT_UNIT_ASSERT_EQUAL(constImage.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(constImage.size(), std::size_t(200));

            // ImageF
            Gfx::ImageF imageF(10, 5, Rgb32::get());
            PT_UNIT_ASSERT_EQUAL(imageF.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(imageF.size(), std::size_t(200));

            const Gfx::ConstImageF constImageF(imageF);
            PT_UNIT_ASSERT_EQUAL(constImageF.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(constImageF.size(), std::size_t(200));
        }

        void Rgb32Color()
        {
            // default constructor
            Gfx::Rgb32Color c0;
            PT_UNIT_ASSERT_EQUAL(c0.alpha(), 0);
            PT_UNIT_ASSERT_EQUAL(c0.red(),   0);
            PT_UNIT_ASSERT_EQUAL(c0.green(), 0);
            PT_UNIT_ASSERT_EQUAL(c0.blue(),  0);
            PT_UNIT_ASSERT_EQUAL(c0.value(), Pt::uint32_t(0));

            // construct from channel values (a, pr, pg, pb)
            Gfx::Rgb32Color c1(0xFF, 0x80, 0x40, 0x20);
            PT_UNIT_ASSERT_EQUAL(c1.alpha(), 0xFF);
            PT_UNIT_ASSERT_EQUAL(c1.red(),   0x80);
            PT_UNIT_ASSERT_EQUAL(c1.green(), 0x40);
            PT_UNIT_ASSERT_EQUAL(c1.blue(),  0x20);
            PT_UNIT_ASSERT_EQUAL(c1.value(), Pt::uint32_t(0xFF804020));

            // construct from uint32_t
            Gfx::Rgb32Color c2(Pt::uint32_t(0xAABBCCDD));
            PT_UNIT_ASSERT_EQUAL(c2.alpha(), 0xAA);
            PT_UNIT_ASSERT_EQUAL(c2.red(),   0xBB);
            PT_UNIT_ASSERT_EQUAL(c2.green(), 0xCC);
            PT_UNIT_ASSERT_EQUAL(c2.blue(),  0xDD);

            // construct from byte pointer
            const Pt::uint8_t* p = reinterpret_cast<const Pt::uint8_t*>(&c1.value());
            Gfx::Rgb32Color c3(p);
            PT_UNIT_ASSERT_EQUAL(c3.value(), c1.value());

            // copy constructor
            Gfx::Rgb32Color c4(c2);
            PT_UNIT_ASSERT_EQUAL(c4.value(), c2.value());

            // copy assignment
            c4 = c1;
            PT_UNIT_ASSERT_EQUAL(c4.value(), c1.value());

            // setters change only the target channel
            Gfx::Rgb32Color c5(0x11, 0x22, 0x33, 0x44);

            c5.setAlpha(0xAA);
            PT_UNIT_ASSERT_EQUAL(c5.alpha(), 0xAA);
            PT_UNIT_ASSERT_EQUAL(c5.red(),   0x22);
            PT_UNIT_ASSERT_EQUAL(c5.green(), 0x33);
            PT_UNIT_ASSERT_EQUAL(c5.blue(),  0x44);

            c5.setRed(0xBB);
            PT_UNIT_ASSERT_EQUAL(c5.alpha(), 0xAA);
            PT_UNIT_ASSERT_EQUAL(c5.red(),   0xBB);
            PT_UNIT_ASSERT_EQUAL(c5.green(), 0x33);
            PT_UNIT_ASSERT_EQUAL(c5.blue(),  0x44);

            c5.setGreen(0xCC);
            PT_UNIT_ASSERT_EQUAL(c5.alpha(), 0xAA);
            PT_UNIT_ASSERT_EQUAL(c5.red(),   0xBB);
            PT_UNIT_ASSERT_EQUAL(c5.green(), 0xCC);
            PT_UNIT_ASSERT_EQUAL(c5.blue(),  0x44);

            c5.setBlue(0xDD);
            PT_UNIT_ASSERT_EQUAL(c5.alpha(), 0xAA);
            PT_UNIT_ASSERT_EQUAL(c5.red(),   0xBB);
            PT_UNIT_ASSERT_EQUAL(c5.green(), 0xCC);
            PT_UNIT_ASSERT_EQUAL(c5.blue(),  0xDD);
        }

        void GenericSetColor()
        {
            Gfx::Image image(2, 2, Rgb32::get());
            Gfx::PixelView view(image);

            std::copy(inputColors.begin(), inputColors.end(), view.begin());

            const bool isEqual = std::equal(rgb32Data.begin(), rgb32Data.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void DirectSetRgb32Color()
        {
            Gfx::Rgb32Image image(2, 2);
            Gfx::Rgb32PixelView view(image);

            std::copy(rgb32Colors.begin(), rgb32Colors.end(), view.begin());

            const bool isEqual = std::equal(rgb32Data.begin(), rgb32Data.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void GenericGetColor()
        {
            Gfx::Image image(2, 2, Rgb32::get());
            std::memcpy(image.data(), rgb32Data.data(), rgb32Data.size());

            Gfx::PixelView view(image);

            auto isEqualColor = [](const auto& pixel, const Color& color) {
                return pixel.getColor().value() == color.value();
            };

            const bool isEqual = std::equal(view.begin(), view.end(),
                                        expectedColors.begin(), isEqualColor);
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));

            Gfx::ConstPixelView cview(image);

            const bool isEqualConst = std::equal(cview.begin(), cview.end(),
                                            expectedColors.begin(), isEqualColor);
            PT_UNIT_ASSERT_MSG(isEqualConst,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void DirectGetColor()
        {
            Gfx::Rgb32Image image(2, 2);
            std::memcpy(image.data(), rgb32Data.data(), rgb32Data.size());

            Gfx::Rgb32PixelView view(image);

            auto isEqualColor = [](const auto& pixel, const Color& color) {
                return pixel.getColor().value() == color.value();
            };

            const bool isEqual = std::equal(view.begin(), view.end(),
                                        expectedColors.begin(), isEqualColor);
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));

            Gfx::Rgb32ConstPixelView cview(image);

            const bool isEqualConst = std::equal(cview.begin(), cview.end(),
                                            expectedColors.begin(), isEqualColor);
            PT_UNIT_ASSERT_MSG(isEqualConst,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void DirectGetRgb32Color()
        {
            Gfx::Rgb32Image image(2, 2);
            std::memcpy(image.data(), rgb32Data.data(), rgb32Data.size());

            Gfx::Rgb32PixelView view(image);

            auto isEqualColor = [](const auto& pixel, const Gfx::Rgb32Color& color) {
                return pixel.color().value() == color.value();
            };

            const bool isEqual = std::equal(view.begin(), view.end(),
                                            rgb32Colors.begin(), isEqualColor);
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));

            Gfx::Rgb32ConstPixelView cview(image);

            const bool isEqualConst = std::equal(cview.begin(), cview.end(),
                                                 rgb32Colors.begin(), isEqualColor);
            PT_UNIT_ASSERT_MSG(isEqualConst,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

    private:
        const std::vector<Color> inputColors;
        const std::vector<Pt::uint8_t> rgb32Data;
        const std::vector<Color> expectedColors;
        const std::vector<Gfx::Rgb32Color> rgb32Colors;

        static std::string printBytes(const Pt::uint8_t* data, std::size_t size)
        {
            std::ostringstream out;

            for(std::size_t i = 0; i < size; ++i)
            {
                if(i != 0)
                    out << ' ';

                out << std::hex << std::setw(2) << std::setfill('0')
                    << Pt::uint32_t(data[i]);
            }

            return out.str();
        }
};

} // namespace Gfx

} // namespace Pt

Pt::Unit::RegisterTest<Pt::Gfx::Rgb32Test> register_Rgb32Test;
