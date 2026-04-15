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
        , inputColors {
            Color(255, 200, 100, 50),  Color(0,   200, 100, 50),
            Color(128, 200, 100, 50),  Color(64,  180,  60, 30),
        }
        , rgb32Colors {
            Gfx::Rgb32Color(255, 200, 100, 50),  Gfx::Rgb32Color(  0,   0,   0,  0),
            Gfx::Rgb32Color(128, 100,  50, 25),  Gfx::Rgb32Color( 64,  45,  15,  8),
        }
        , rgb32Data {
            50, 100, 200, 255,   0,   0,   0,   0,
            25,  50, 100, 128,   8,  15,  45,  64,
        }
        , expectedColors {
            Color(255, 200, 100, 50),  Color(0,     0,   0,  0),
            Color(128, 199,  99, 49),  Color(64,  179,  59, 31),
        }
        , inputColorsF {
            ColorF(65535, 51400, 25700, 12850),  ColorF(0,     51400, 25700, 12850),
            ColorF(32896, 51400, 25700, 12850), ColorF(16448, 46260, 15420,  8224),
        }
        , expectedColorsF {
            ColorF(65535, 51400, 25700, 12850),  ColorF(0,         0,     0,     0),
            ColorF(32896, 51199, 25599, 12799), ColorF(16448, 46079, 15359,  8191),
        }
        {
            registerMethod("ImageTraits", *this, &Rgb32Test::ImageTraits);
            registerMethod("GetSetChannels", *this, &Rgb32Test::GetSetChannels);
            registerMethod("ComparePixel", *this, &Rgb32Test::ComparePixel);
            registerMethod("MovePixel", *this, &Rgb32Test::MovePixel);
            registerMethod("GenericGetColor", *this, &Rgb32Test::GenericGetColor);
            registerMethod("GenericGetColorF", *this, &Rgb32Test::GenericGetColorF);
            registerMethod("DirectGetColor", *this, &Rgb32Test::DirectGetColor);
            registerMethod("DirectGetRgb32Color", *this, &Rgb32Test::DirectGetRgb32Color);
            registerMethod("GenericSetColor", *this, &Rgb32Test::GenericSetColor);
            registerMethod("GenericSetColorF", *this, &Rgb32Test::GenericSetColorF);
            registerMethod("DirectSetColor", *this, &Rgb32Test::DirectSetColor);
            registerMethod("DirectSetRgb32Color", *this, &Rgb32Test::DirectSetRgb32Color);
            registerMethod("GenericFillColor", *this, &Rgb32Test::GenericFillColor);
            registerMethod("GenericFillColorF", *this, &Rgb32Test::GenericFillColorF);
            registerMethod("DirectFillColor", *this, &Rgb32Test::DirectFillColor);
            registerMethod("DirectFillRgb32Color", *this, &Rgb32Test::DirectFillRgb32Color);
            registerMethod("GenericGetColors", *this, &Rgb32Test::GenericGetColors);
            registerMethod("GenericGetColorsF", *this, &Rgb32Test::GenericGetColorsF);
            registerMethod("DirectGetColors", *this, &Rgb32Test::DirectGetColors);
            registerMethod("DirectGetRgb32Colors", *this, &Rgb32Test::DirectGetRgb32Colors);
            registerMethod("GenericAssignPixel", *this, &Rgb32Test::GenericAssignPixel);
            registerMethod("DirectAssignPixel", *this, &Rgb32Test::DirectAssignPixel);
            registerMethod("GenericAssignPixels", *this, &Rgb32Test::GenericAssignPixels);
            registerMethod("DirectAssignPixels", *this, &Rgb32Test::DirectAssignPixels);
            registerMethod("GenericAssignColors", *this, &Rgb32Test::GenericAssignColors);
            registerMethod("GenericAssignColorsF", *this, &Rgb32Test::GenericAssignColorsF);
            registerMethod("DirectAssignColors", *this, &Rgb32Test::DirectAssignColors);
            registerMethod("DirectAssignRgb32Colors", *this, &Rgb32Test::DirectAssignRgb32Colors);
            registerMethod("SourceCopyRgb32Color", *this, &Rgb32Test::SourceCopyRgb32Color);
            registerMethod("SourceCopyPixels", *this, &Rgb32Test::SourceCopyPixels);
            registerMethod("SourceOverRgb32Color", *this, &Rgb32Test::SourceOverRgb32Color);
            registerMethod("SourceOverPixels", *this, &Rgb32Test::SourceOverPixels);
        }

        void ImageTraits()
        {
            // Rgb32Image
            Gfx::Rgb32Image rgb32Image(10, 5);
            PT_UNIT_ASSERT_EQUAL(rgb32Image.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(rgb32Image.size(), Rgb32::get().imageSize(10, 5, 0));
            PT_UNIT_ASSERT(rgb32Image.format().type() == typeid(Rgb32));

            Gfx::Rgb32Image rgb32PaddedImage(10, 5, 4);
            const Gfx::Rgb32ConstImage constRgb32Image(rgb32PaddedImage);
            PT_UNIT_ASSERT_EQUAL(constRgb32Image.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(constRgb32Image.size(), Rgb32::get().imageSize(10, 5, 4));
            PT_UNIT_ASSERT(constRgb32Image.format().type() == typeid(Rgb32));

            // Image
            Gfx::Image image(10, 5, Rgb32::get());
            PT_UNIT_ASSERT_EQUAL(image.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(image.size(), Rgb32::get().imageSize(10, 5, 0));

            Gfx::Image paddedImage(10, 5, 4, Rgb32::get());
            const Gfx::ConstImage constImage(paddedImage);
            PT_UNIT_ASSERT_EQUAL(constImage.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(constImage.size(), Rgb32::get().imageSize(10, 5, 4));

            // ImageF
            Gfx::ImageF imageF(10, 5, Rgb32::get());
            PT_UNIT_ASSERT_EQUAL(imageF.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(imageF.size(), Rgb32::get().imageSize(10, 5, 0));

            Gfx::ImageF paddedImageF(10, 5, 4, Rgb32::get());
            const Gfx::ConstImageF constImageF(paddedImageF);
            PT_UNIT_ASSERT_EQUAL(constImageF.pixelStride(), Pt::ssize_t(4));
            PT_UNIT_ASSERT_EQUAL(constImageF.size(), Rgb32::get().imageSize(10, 5, 4));
        }

        void GetSetChannels()
        {
            const Gfx::Rgb32Color inputColor(0x11, 0x22, 0x33, 0x44);
            const Gfx::Rgb32Color expectedColor(0xAA, 0xBB, 0xCC, 0xDD);

            Gfx::Rgb32Image image(1, 1);
            Gfx::Rgb32Pixel pixel(image, 0, 0);
            pixel = inputColor;

            PT_UNIT_ASSERT_EQUAL(pixel.alpha(), inputColor.alpha());
            PT_UNIT_ASSERT_EQUAL(pixel.red(), inputColor.red());
            PT_UNIT_ASSERT_EQUAL(pixel.green(), inputColor.green());
            PT_UNIT_ASSERT_EQUAL(pixel.blue(), inputColor.blue());

            Gfx::Rgb32ConstPixel constPixel(image, 0, 0);
            PT_UNIT_ASSERT_EQUAL(constPixel.alpha(), inputColor.alpha());
            PT_UNIT_ASSERT_EQUAL(constPixel.red(), inputColor.red());
            PT_UNIT_ASSERT_EQUAL(constPixel.green(), inputColor.green());
            PT_UNIT_ASSERT_EQUAL(constPixel.blue(), inputColor.blue());

            pixel.setAlpha(expectedColor.alpha());
            PT_UNIT_ASSERT_EQUAL(pixel.alpha(), expectedColor.alpha());
            PT_UNIT_ASSERT_EQUAL(pixel.red(), inputColor.red());
            PT_UNIT_ASSERT_EQUAL(pixel.green(), inputColor.green());
            PT_UNIT_ASSERT_EQUAL(pixel.blue(), inputColor.blue());

            pixel.setRed(expectedColor.red());
            PT_UNIT_ASSERT_EQUAL(pixel.alpha(), expectedColor.alpha());
            PT_UNIT_ASSERT_EQUAL(pixel.red(), expectedColor.red());
            PT_UNIT_ASSERT_EQUAL(pixel.green(), inputColor.green());
            PT_UNIT_ASSERT_EQUAL(pixel.blue(), inputColor.blue());

            pixel.setGreen(expectedColor.green());
            PT_UNIT_ASSERT_EQUAL(pixel.alpha(), expectedColor.alpha());
            PT_UNIT_ASSERT_EQUAL(pixel.red(), expectedColor.red());
            PT_UNIT_ASSERT_EQUAL(pixel.green(), expectedColor.green());
            PT_UNIT_ASSERT_EQUAL(pixel.blue(), inputColor.blue());

            pixel.setBlue(expectedColor.blue());
            PT_UNIT_ASSERT_EQUAL(pixel.alpha(), expectedColor.alpha());
            PT_UNIT_ASSERT_EQUAL(pixel.red(), expectedColor.red());
            PT_UNIT_ASSERT_EQUAL(pixel.green(), expectedColor.green());
            PT_UNIT_ASSERT_EQUAL(pixel.blue(), expectedColor.blue());
            PT_UNIT_ASSERT_EQUAL(pixel.color().value(), expectedColor.value());

            PT_UNIT_ASSERT_EQUAL(constPixel.alpha(), expectedColor.alpha());
            PT_UNIT_ASSERT_EQUAL(constPixel.red(), expectedColor.red());
            PT_UNIT_ASSERT_EQUAL(constPixel.green(), expectedColor.green());
            PT_UNIT_ASSERT_EQUAL(constPixel.blue(), expectedColor.blue());
            PT_UNIT_ASSERT_EQUAL(constPixel.color().value(), expectedColor.value());
        }

        void ComparePixel()
        {
            const std::vector<Pt::uint8_t> data {
                1,  2,  3,  4,   5,  6,  7,  8,  0xEE, 0xEE, 0xEE, 0xEE,
                9, 10, 11, 12,  13, 14, 15, 16,  0xEE, 0xEE, 0xEE, 0xEE,
            };

            Gfx::Rgb32Image image(2, 2, 4);
            std::memcpy(image.data(), data.data(), data.size());

            Gfx::Rgb32Pixel pixel(image, 0, 1);
            Gfx::Rgb32Pixel equalPixel(image, 0, 1);
            Gfx::Rgb32Pixel differentPixel(image, 1, 1);
            PT_UNIT_ASSERT(pixel.equals(equalPixel));
            PT_UNIT_ASSERT(!pixel.equals(differentPixel));

            Gfx::Rgb32ConstPixel constPixel(image, 0, 1);
            Gfx::Rgb32ConstPixel equalConstPixel(image, 0, 1);
            Gfx::Rgb32ConstPixel differentConstPixel(image, 1, 0);
            PT_UNIT_ASSERT(pixel.equals(equalConstPixel));
            PT_UNIT_ASSERT(!pixel.equals(differentConstPixel));
            PT_UNIT_ASSERT(constPixel.equals(equalConstPixel));
            PT_UNIT_ASSERT(!constPixel.equals(differentConstPixel));
            PT_UNIT_ASSERT(constPixel.equals(pixel));
            PT_UNIT_ASSERT(!constPixel.equals(differentPixel));
        }

        void MovePixel()
        {
            const std::vector<Pt::uint8_t> data {
                1,  2,  3,  4,   5,  6,  7,  8,  0xEE, 0xEE, 0xEE, 0xEE,
                9, 10, 11, 12,  13, 14, 15, 16,  0xEE, 0xEE, 0xEE, 0xEE,
            };
            const Gfx::Rgb32Color color00(4,  3,  2,  1);
            const Gfx::Rgb32Color color10(8,  7,  6,  5);
            const Gfx::Rgb32Color color01(12, 11, 10, 9);
            const Gfx::Rgb32Color color11(16, 15, 14, 13);

            Gfx::Rgb32Image image(2, 2, 4);
            std::memcpy(image.data(), data.data(), data.size());

            Gfx::Rgb32Pixel pixel(image, 0, 0);
            PT_UNIT_ASSERT(pixel.base() == image.data());
            PT_UNIT_ASSERT_EQUAL(pixel.color().value(), color00.value());

            pixel.advance();
            PT_UNIT_ASSERT(pixel.base() == image.data() + 4);
            PT_UNIT_ASSERT_EQUAL(pixel.color().value(), color10.value());

            pixel.reset(image, 0, 0);
            pixel.advance(2);
            PT_UNIT_ASSERT(pixel.base() == image.data() + 8);
            pixel.skipPadding();
            PT_UNIT_ASSERT(pixel.base() == image.data() + 12);
            PT_UNIT_ASSERT_EQUAL(pixel.color().value(), color01.value());

            pixel.reset(image, 0, 0);
            pixel.advanceLines(1);
            PT_UNIT_ASSERT(pixel.base() == image.data() + 12);
            PT_UNIT_ASSERT_EQUAL(pixel.color().value(), color01.value());

            Gfx::Rgb32Pixel other(image, 1, 1);
            pixel.reset(other);
            PT_UNIT_ASSERT(pixel.base() == image.data() + 16);
            PT_UNIT_ASSERT_EQUAL(pixel.color().value(), color11.value());

            Gfx::Rgb32ConstPixel constPixel(image, 0, 0);
            PT_UNIT_ASSERT(constPixel.base() == image.data());
            PT_UNIT_ASSERT_EQUAL(constPixel.color().value(), color00.value());

            constPixel.advance();
            PT_UNIT_ASSERT(constPixel.base() == image.data() + 4);
            PT_UNIT_ASSERT_EQUAL(constPixel.color().value(), color10.value());

            constPixel.reset(image, 0, 0);
            constPixel.advance(2);
            PT_UNIT_ASSERT(constPixel.base() == image.data() + 8);
            constPixel.skipPadding();
            PT_UNIT_ASSERT(constPixel.base() == image.data() + 12);
            PT_UNIT_ASSERT_EQUAL(constPixel.color().value(), color01.value());

            constPixel.reset(image, 0, 0);
            constPixel.advanceLines(1);
            PT_UNIT_ASSERT(constPixel.base() == image.data() + 12);
            PT_UNIT_ASSERT_EQUAL(constPixel.color().value(), color01.value());

            Gfx::Rgb32ConstPixel otherConst(image, 1, 0);
            constPixel.reset(otherConst);
            PT_UNIT_ASSERT(constPixel.base() == image.data() + 4);
            PT_UNIT_ASSERT_EQUAL(constPixel.color().value(), color10.value());

            pixel.reset(image, 1, 1);
            constPixel.reset(pixel);
            PT_UNIT_ASSERT(constPixel.base() == image.data() + 16);
            PT_UNIT_ASSERT_EQUAL(constPixel.color().value(), color11.value());
        }

        void GenericGetColor()
        {
            Gfx::Image image(2, 2, Rgb32::get());
            std::memcpy(image.data(), rgb32Data.data(), rgb32Data.size());

            Gfx::PixelView view(image);

            const bool isEqual = std::equal(view.begin(), view.end(), 
                                            expectedColors.begin(), IsEqualColor() );
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));

            Gfx::ConstPixelView cview(image);

            const bool isEqualConst = std::equal(cview.begin(), cview.end(), 
                                                 expectedColors.begin(), IsEqualColor() );
            PT_UNIT_ASSERT_MSG(isEqualConst,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void GenericGetColorF()
        {
            Gfx::ImageF image(2, 2, Rgb32::get());
            std::memcpy(image.data(), rgb32Data.data(), rgb32Data.size());

            auto view = Gfx::pixelView(image);

            const bool isEqual = std::equal(view.begin(), view.end(), expectedColorsF.begin(), 
                                            IsEqualColor() );
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));

            const Gfx::ConstImageF constImage(image);
            auto cview = Gfx::pixelView(constImage);

            const bool isEqualConst = std::equal(cview.begin(), cview.end(),expectedColorsF.begin(), 
                                                 IsEqualColor() );
            PT_UNIT_ASSERT_MSG(isEqualConst,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void DirectGetColor()
        {
            Gfx::Rgb32Image image(2, 2);
            std::memcpy(image.data(), rgb32Data.data(), rgb32Data.size());

            Gfx::Rgb32PixelView view(image);

            const bool isEqual = std::equal(view.begin(), view.end(), expectedColors.begin(), 
                                            IsEqualColor() );
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));

            Gfx::Rgb32ConstPixelView cview(image);

            const bool isEqualConst = std::equal(cview.begin(), cview.end(), expectedColors.begin(), 
                                                 IsEqualColor() );
            PT_UNIT_ASSERT_MSG(isEqualConst,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void DirectGetRgb32Color()
        {
            Gfx::Rgb32Image image(2, 2);
            std::memcpy(image.data(), rgb32Data.data(), rgb32Data.size());

            Gfx::Rgb32PixelView view(image);

            const bool isEqual = std::equal(view.begin(), view.end(), rgb32Colors.begin(), 
                                            IsEqualColor() );
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));

            Gfx::Rgb32ConstPixelView cview(image);

            const bool isEqualConst = std::equal(cview.begin(), cview.end(), rgb32Colors.begin(), 
                                                 IsEqualColor() );
            PT_UNIT_ASSERT_MSG(isEqualConst,
                               "unexpected: " << printBytes(image.data(), image.size()));
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

        void GenericSetColorF()
        {
            Gfx::ImageF image(2, 2, Rgb32::get());
            auto view = Gfx::pixelView(image);

            std::copy(inputColorsF.begin(), inputColorsF.end(), view.begin());

            const bool isEqual = std::equal(rgb32Data.begin(), rgb32Data.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void DirectSetColor()
        {
            Gfx::Rgb32Image image(2, 2);
            Gfx::Rgb32PixelView view(image);

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

        void GenericFillColor()
        {
            Gfx::Image image(2, 2, Rgb32::get());
            auto lines = Gfx::lineView(image);

            for(auto& line : lines)
                line.front().fill(line.length(), inputColors.front());

            const std::vector<Pt::uint8_t> expected = toRgb32Data(inputColors.front(), 4);
            const bool isEqual = std::equal(expected.begin(), expected.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void GenericFillColorF()
        {
            Gfx::ImageF image(2, 2, Rgb32::get());
            Gfx::Pixel<ColorF> pixel(image, 0, 0);

            pixel.fill(expectedColorsF.size(), inputColorsF.front());

            const std::vector<Pt::uint8_t> expected = toRgb32Data(inputColorsF.front(), 4);
            const bool isEqual = std::equal(expected.begin(), expected.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void DirectFillColor()
        {
            Gfx::Rgb32Image image(2, 2);
            auto lines = Gfx::lineView(image);

            for(auto& line : lines)
                line.front().fill(line.length(), inputColors.front());

            const std::vector<Pt::uint8_t> expected = toRgb32Data(inputColors.front(), 4);
            const bool isEqual = std::equal(expected.begin(), expected.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void DirectFillRgb32Color()
        {
            Gfx::Rgb32Image image(2, 2);
            auto lines = Gfx::lineView(image);

            for(auto& line : lines)
                line.front().fill(line.length(), rgb32Colors.front());

            const std::vector<Pt::uint8_t> expected = toRgb32Data(rgb32Colors.front(), 4);
            const bool isEqual = std::equal(expected.begin(), expected.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void GenericGetColors()
        {
            Gfx::Image image(2, 2, 4, Rgb32::get());
            setRgb32Data(image, rgb32Data);

            auto isEqualColor = [](const Color& left, const Color& right) {
                return left.value() == right.value();
            };

            std::vector<Color> colors;
            auto lines = Gfx::lineView(image);

            for(auto& line : lines)
            {
                const std::size_t size = colors.size();
                colors.resize(size + line.length());
                line.front().getColors(colors.data() + size, line.length());
            }

            const bool isEqual = std::equal(colors.begin(), colors.end(),
                                            expectedColors.begin(), isEqualColor);
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));

            std::vector<Color> constColors;
            const Gfx::ConstImage constImage(image);
            auto constLines = Gfx::lineView(constImage);

            for(const auto& line : constLines)
            {
                const std::size_t size = constColors.size();
                constColors.resize(size + line.length());
                line.front().getColors(constColors.data() + size, line.length());
            }

            const bool isEqualConst = std::equal(constColors.begin(), constColors.end(),
                                                 expectedColors.begin(), isEqualColor);
            PT_UNIT_ASSERT_MSG(isEqualConst,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void GenericGetColorsF()
        {
            Gfx::ImageF image(2, 2, 4, Rgb32::get());
            setRgb32Data(image, rgb32Data);

            auto isEqualColor = [](const ColorF& left, const ColorF& right) {
                return isEqualColorF(left, right);
            };

            std::vector<ColorF> colors;
            auto lines = Gfx::lineView(image);

            for(auto& line : lines)
            {
                const std::size_t size = colors.size();
                colors.resize(size + line.length());
                line.front().getColors(colors.data() + size, line.length());
            }

            const bool isEqual = std::equal(colors.begin(), colors.end(),
                                            expectedColorsF.begin(), isEqualColor);
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));

            std::vector<ColorF> constColors;
            const Gfx::ConstImageF constImage(image);
            auto constLines = Gfx::lineView(constImage);

            for(const auto& line : constLines)
            {
                const std::size_t size = constColors.size();
                constColors.resize(size + line.length());
                line.front().getColors(constColors.data() + size, line.length());
            }

            const bool isEqualConst = std::equal(constColors.begin(), constColors.end(),
                                                 expectedColorsF.begin(), isEqualColor);
            PT_UNIT_ASSERT_MSG(isEqualConst,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void DirectGetColors()
        {
            Gfx::Rgb32Image image(2, 2);
            std::memcpy(image.data(), rgb32Data.data(), rgb32Data.size());

            auto isEqualColor = [](const Color& left, const Color& right) {
                return left.value() == right.value();
            };

            std::vector<Color> colors(expectedColors.size());
            Gfx::Rgb32Pixel pixel(image, 0, 0);
            pixel.getColors(colors.data(), colors.size());

            const bool isEqual = std::equal(colors.begin(), colors.end(),
                                            expectedColors.begin(), isEqualColor);
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));

            std::vector<Color> constColors(expectedColors.size());
            Gfx::Rgb32ConstPixel constPixel(image, 0, 0);
            constPixel.getColors(constColors.data(), constColors.size());

            const bool isEqualConst = std::equal(constColors.begin(), constColors.end(),
                                                 expectedColors.begin(), isEqualColor);
            PT_UNIT_ASSERT_MSG(isEqualConst,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void DirectGetRgb32Colors()
        {
            Gfx::Rgb32Image image(2, 2);
            std::memcpy(image.data(), rgb32Data.data(), rgb32Data.size());

            auto isEqualColor = [](const Gfx::Rgb32Color& left, const Gfx::Rgb32Color& right) {
                return left.value() == right.value();
            };

            std::vector<Gfx::Rgb32Color> colors(rgb32Colors.size());
            Gfx::Rgb32Pixel pixel(image, 0, 0);
            pixel.getColors(colors.data(), colors.size());

            const bool isEqual = std::equal(colors.begin(), colors.end(),
                                            rgb32Colors.begin(), isEqualColor);
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));

            std::vector<Gfx::Rgb32Color> constColors(rgb32Colors.size());
            Gfx::Rgb32ConstPixel constPixel(image, 0, 0);
            constPixel.getColors(constColors.data(), constColors.size());

            const bool isEqualConst = std::equal(constColors.begin(), constColors.end(),
                                                 rgb32Colors.begin(), isEqualColor);
            PT_UNIT_ASSERT_MSG(isEqualConst,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void GenericAssignPixel()
        {
            Gfx::Image sourceImage(2, 2, Rgb32::get());
            std::memcpy(sourceImage.data(), rgb32Data.data(), rgb32Data.size());

            Gfx::Image image(2, 2, Rgb32::get());
            Gfx::ConstPixelView sourceView(sourceImage);
            Gfx::PixelView view(image);
            
            auto target = view.begin();
            for(const auto& sourcePixel : sourceView)
            {
                target->assign(sourcePixel);
                ++target;
            }

            const bool isEqual = std::equal(rgb32Data.begin(), rgb32Data.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void DirectAssignPixel()
        {
            Gfx::Rgb32Image sourceImage(2, 2);
            std::memcpy(sourceImage.data(), rgb32Data.data(), rgb32Data.size());

            Gfx::Rgb32Image image(2, 2);
            Gfx::Rgb32ConstPixelView sourceView(sourceImage);
            Gfx::Rgb32PixelView view(image);
            
            auto target = view.begin();
            for(const auto& sourcePixel : sourceView)
            {
                target->assign(sourcePixel);
                ++target;
            }

            const bool isEqual = std::equal(rgb32Data.begin(), rgb32Data.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void GenericAssignPixels()
        {
            Gfx::Image sourceImage(2, 2, Rgb32::get());
            std::memcpy(sourceImage.data(), rgb32Data.data(), rgb32Data.size());

            Gfx::Image image(2, 2, Rgb32::get());
            Gfx::ConstPixelView sourceView(sourceImage);
            Gfx::PixelView view(image);

            auto source = sourceView.begin();
            auto target = view.begin();
            target->assign(*source, rgb32Colors.size());

            const bool isEqual = std::equal(rgb32Data.begin(), rgb32Data.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void DirectAssignPixels()
        {
            Gfx::Rgb32Image sourceImage(2, 2);
            std::memcpy(sourceImage.data(), rgb32Data.data(), rgb32Data.size());

            Gfx::Rgb32Image image(2, 2);
            Gfx::Rgb32ConstPixelView sourceView(sourceImage);
            Gfx::Rgb32PixelView view(image);

            auto source = sourceView.begin();
            auto target = view.begin();
            target->assign(*source, rgb32Colors.size());

            const bool isEqual = std::equal(rgb32Data.begin(), rgb32Data.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void GenericAssignColors()
        {
            Gfx::Image image(2, 2, Rgb32::get());
            Gfx::Pixel<Color> pixel(image, 0, 0);

            pixel.assign(inputColors.data(), inputColors.size());

            const bool isEqual = std::equal(rgb32Data.begin(), rgb32Data.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void GenericAssignColorsF()
        {
            Gfx::ImageF image(2, 2, Rgb32::get());
            Gfx::Pixel<ColorF> pixel(image, 0, 0);

            pixel.assign(inputColorsF.data(), inputColorsF.size());

            const bool isEqual = std::equal(rgb32Data.begin(), rgb32Data.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void DirectAssignColors()
        {
            Gfx::Rgb32Image image(2, 2);
            Gfx::Rgb32Pixel pixel(image, 0, 0);

            pixel.assign(inputColors.data(), inputColors.size());

            const bool isEqual = std::equal(rgb32Data.begin(), rgb32Data.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void DirectAssignRgb32Colors()
        {
            Gfx::Rgb32Image image(2, 2);
            Gfx::Rgb32Pixel pixel(image, 0, 0);

            pixel.assign(rgb32Colors.data(), rgb32Colors.size());

            const bool isEqual = std::equal(rgb32Data.begin(), rgb32Data.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void SourceCopyRgb32Color()
        {
            const Gfx::Rgb32Color sourceColor(64, 45, 15, 8);

            Gfx::Rgb32Image image(2, 2);
            std::memset(image.data(), 0, image.size());

            Gfx::Rgb32Pixel to(image, 0, 0);
            Gfx::sourceCopy(to, image.width() * image.height(), sourceColor);

            const std::vector<Pt::uint8_t> expected = toRgb32Data(sourceColor, rgb32Colors.size());
            const bool isEqual = std::equal(expected.begin(), expected.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void SourceCopyPixels()
        {
            Gfx::Rgb32Image sourceImage(2, 2);
            std::memcpy(sourceImage.data(), rgb32Data.data(), rgb32Data.size());

            Gfx::Rgb32Image image(2, 2);
            std::memset(image.data(), 0, image.size());

            Gfx::Rgb32ConstPixel from(sourceImage, 0, 0);
            Gfx::Rgb32Pixel to(image, 0, 0);
            Gfx::sourceCopy(to, from, image.width() * image.height());

            const bool isEqual = std::equal(rgb32Data.begin(), rgb32Data.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void SourceOverRgb32Color()
        {
            const Gfx::Rgb32Color color(64, 45, 15, 8);
            const std::vector<Pt::uint8_t> data {
                50, 100, 200, 255,   0,   0,   0,   0,
                25,  50, 100, 128,   8,  15,  45,  64,
            };
            const std::vector<Pt::uint8_t> expected {
                45,  89, 194, 254,   8,  15,  45,  64,
                26,  52, 119, 159,  13,  26,  78, 111,
            };

            Gfx::Rgb32Image image(2, 2);
            std::memcpy(image.data(), data.data(), data.size());

            Gfx::Rgb32Pixel to(image, 0, 0);
            Gfx::sourceOver(to, image.width() * image.height(), color);

            const bool isEqual = std::equal(expected.begin(), expected.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

        void SourceOverPixels()
        {
            const std::vector<Pt::uint8_t> data1 {
                50, 100, 200, 255,   0,   0,   0,   0,
                25,  50, 100, 128,   8,  15,  45,  64,
            };
            const std::vector<Pt::uint8_t> data2 {
                8,  15,  45,  64,   8,  15,  45,  64,
                8,  15,  45,  64,   8,  15,  45,  64,
            };
            const std::vector<Pt::uint8_t> expected {
                50, 100, 200, 255,   7,  14,  44,  63,
                28,  57, 122, 159,  13,  26,  78, 111,
            };

            Gfx::Rgb32Image sourceImage(2, 2);
            std::memcpy(sourceImage.data(), data1.data(), data1.size());

            Gfx::Rgb32Image image(2, 2);
            std::memcpy(image.data(), data2.data(), data2.size());

            Gfx::Rgb32ConstPixel from(sourceImage, 0, 0);
            Gfx::Rgb32Pixel to(image, 0, 0);
            Gfx::sourceOver(to, from, image.width() * image.height());

            const bool isEqual = std::equal(expected.begin(), expected.end(), image.data());
            PT_UNIT_ASSERT_MSG(isEqual,
                               "unexpected: " << printBytes(image.data(), image.size()));
        }

    private:
        const std::vector<Color> inputColors;
        const std::vector<Pt::uint8_t> rgb32Data;
        const std::vector<Color> expectedColors;
        const std::vector<ColorF> inputColorsF;
        const std::vector<ColorF> expectedColorsF;
        const std::vector<Gfx::Rgb32Color> rgb32Colors;

        static std::vector<Pt::uint8_t> toRgb32Data(const Gfx::Rgb32Color& color,
                                                    std::size_t count)
        {
            std::vector<Pt::uint8_t> bytes(count * 4);

            for(std::size_t i = 0; i < count; ++i)
            {
                bytes[(i * 4) + 0] = color.blue();
                bytes[(i * 4) + 1] = color.green();
                bytes[(i * 4) + 2] = color.red();
                bytes[(i * 4) + 3] = color.alpha();
            }

            return bytes;
        }

        static std::vector<Pt::uint8_t> toRgb32Data(const Color& color,
                                                    std::size_t count)
        {
            const Pt::uint32_t alpha = color.alpha();
            const Pt::uint8_t red   = Pt::uint8_t((color.red() * alpha + 127) / 255);
            const Pt::uint8_t green = Pt::uint8_t((color.green() * alpha + 127) / 255);
            const Pt::uint8_t blue  = Pt::uint8_t((color.blue() * alpha + 127) / 255);

            return toRgb32Data( Gfx::Rgb32Color(color.alpha(), red, green, blue), count);
        }

        static std::vector<Pt::uint8_t> toRgb32Data(const ColorF& color,
                                                    std::size_t count)
        {
            const Pt::uint32_t alpha = color.alpha() >> 8;
            const Pt::uint8_t red = Pt::uint8_t((Pt::uint32_t(color.red() >> 8) * alpha) / 255);
            const Pt::uint8_t green = Pt::uint8_t((Pt::uint32_t(color.green() >> 8) * alpha) / 255);
            const Pt::uint8_t blue = Pt::uint8_t((Pt::uint32_t(color.blue() >> 8) * alpha) / 255);

            return toRgb32Data(Gfx::Rgb32Color(Pt::uint8_t(alpha), red, green, blue), count);
        }

        template <typename ImageT>
        static void setRgb32Data(ImageT& image, const std::vector<Pt::uint8_t>& data)
        {
            std::fill(image.data(), image.data() + image.size(), Pt::uint8_t(0xEE));

            const std::size_t lineSize = std::size_t(image.width() * image.pixelStride());
            for(Pt::ssize_t y = 0; y < image.height(); ++y)
            {
                std::memcpy(image.data() + (y * image.stride()),
                            data.data() + (std::size_t(y) * lineSize),
                            lineSize);
            }
        }

        struct IsEqualColor {
            template <typename PixelT>
            bool operator()(const PixelT& pixel, const Color& color) const
            {
                return pixel.getColor().value() == color.value();
            }

            template <typename PixelT>
            bool operator()(const PixelT& pixel, const ColorF& color) const
            {
                return isEqualColorF(pixel.getColor(), color);
            }

            template <typename PixelT>
            bool operator()(const PixelT& pixel, const Gfx::Rgb32Color& color) const
            {
                return pixel.color().value() == color.value();
            }

            bool isEqualColorF(const ColorF& left, const ColorF& right) const
            {
                return left.alpha() == right.alpha()
                    && left.red() == right.red()
                    && left.green() == right.green()
                    && left.blue() == right.blue();
            }
        };

        static bool isEqualColorF(const ColorF& left, const ColorF& right)
        {
            return left.alpha() == right.alpha()
                && left.red() == right.red()
                && left.green() == right.green()
                && left.blue() == right.blue();
        }

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
