/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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

#ifndef PT_GFX_IMAGEFORMAT_H
#define PT_GFX_IMAGEFORMAT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/CompositionMode.h>

namespace Pt {

namespace Gfx {

class Pixel;
class ConstPixel;
class ImageView;

/** @brief %Image format.
*/
class ImageFormat
{
    public:
        ImageFormat(size_t pixelStride)
        : _pixelStride(pixelStride)
        { }

        virtual ~ImageFormat()
        {}

        /** @brief Returns distance in bytes between two pixel base pointers.
        */
        std::size_t pixelStride() const
        {
            return _pixelStride;
        }

        bool operator==(const ImageFormat& a) const
        {
          return _pixelStride == a._pixelStride;
        }

        bool operator!=(const ImageFormat& a) const
        {
          return _pixelStride != a._pixelStride;
        }

        PT_GFX_API static const ImageFormat& rgb16();

        PT_GFX_API static const ImageFormat& rgb32();

        PT_GFX_API static const ImageFormat& argb32();

    public:
        /** @brief Sets the pixel color.
        */
        void setPixel(Pixel& to, const Pixel& from,
                      CompositionMode mode) const
        { onSetPixel(to, from, mode); }

        /** @brief Sets the pixel color.
        */
        void setPixel(Pixel& to, const ConstPixel& from,
                      CompositionMode mode) const
        { onSetPixel(to, from, mode); }

        /** @brief Sets the pixel color.
        */
        void setPixel(Pixel& to, const Color& c,
                      CompositionMode mode) const
        { onSetPixel(to, c, mode); }

        /** @brief Sets the pixel color with additional blending alhpa.
        */
        void setPixel(Pixel& to, const Pixel& from,
                      CompositionMode mode, Pt::uint8_t blendingAlpha) const
        { onSetPixel(to, from, mode, blendingAlpha); }

        /** @brief Sets the pixel color with additional blending alhpa.
        */
        void setPixel(Pixel& to, const ConstPixel& from,
                      CompositionMode mode, Pt::uint8_t blendingAlpha) const
        { onSetPixel(to, from, mode, blendingAlpha); }

        /** @brief Sets the pixel color with additional blending alhpa.
        */
        void setPixel(Pixel& to, const Color& c,
                      CompositionMode mode, Pt::uint8_t blendingAlpha) const
        { onSetPixel(to, c, mode, blendingAlpha); }

        /** @brief Sets the pixels color.
        */
        void setPixels(Pixel& to, const Pixel& from, size_t length,
                      CompositionMode mode) const
        { onSetPixels(to, from, length, mode); }

        /** @brief Sets the pixels color.
        */
        void setPixels(Pixel& to, const ConstPixel& from, size_t length,
                      CompositionMode mode) const
        { onSetPixels(to, from, length, mode); }

        /** @brief Sets the pixels color.
        */
        void setPixels(Pixel& to, const Color& c, size_t length,
                      CompositionMode mode) const
        { onSetPixels(to, c, length, mode); }

        /** @brief Gets the pixel color.
        */
        Color getColor(const Pixel& pixel) const
        { return onGetColor(pixel); }

        /** @brief Gets the pixel color.
        */
        Color getColor(const ConstPixel& pixel) const
        { return onGetColor(pixel); }

        /** @brief Sets the color in a pixel span.
        */
        void copy(Pixel& dst, const Pixel& src, size_t length,
                  CompositionMode mode) const
        { onCopy(dst, src, length, mode); }

        /** @brief Sets the color in a pixel span.
        */
        void copy(Pixel& dst, const ConstPixel& src, size_t length,
                  CompositionMode mode) const
        { onCopy(dst, src, length, mode); }

        /** @brief Copies an area of pixels.
        */
        PT_GFX_API void copy(ImageView& to, const Point& toPoint,
                  const ImageView& from, const Rect& fromRect,
                  CompositionMode mode) const;

        /** @brief Returns the size in bytes for a given image size.
        */
        std::size_t imageSize(const Size& size, Pt::ssize_t padding) const
        { return onImageSize(size, padding); }

    protected:
        virtual void onSetPixel(Pixel& to, const Pixel& from,
                                CompositionMode mode) const = 0;

        virtual void onSetPixel(Pixel& to, const ConstPixel& from,
                                CompositionMode mode) const = 0;

        virtual void onSetPixel(Pixel& pixel, const Color& c,
                                CompositionMode  mode) const = 0;

        virtual void onSetPixel(Pixel& to, const Pixel& from,
                                CompositionMode mode, Pt::uint8_t blendingAlpha) const = 0;

        virtual void onSetPixel(Pixel& to, const ConstPixel& from,
                                CompositionMode mode, Pt::uint8_t blendingAlpha) const = 0;

        virtual void onSetPixel(Pixel& pixel, const Color& c,
                                CompositionMode mode, Pt::uint8_t blendingAlpha) const = 0;

        virtual void onSetPixels(Pixel& to, const Pixel& from, size_t length,
                                CompositionMode mode) const = 0;

        virtual void onSetPixels(Pixel& to, const ConstPixel& from, size_t length,
                                CompositionMode mode) const = 0;

        virtual void onSetPixels(Pixel& pixel, const Color& c, size_t length,
                                CompositionMode  mode) const = 0;

        virtual Color onGetColor(const Pixel& pixel) const = 0;

        virtual Color onGetColor(const ConstPixel& pixel) const = 0;

        virtual void onCopy(Pixel& dst, const Pixel& src, size_t length,
                            CompositionMode mode) const = 0;

        virtual void onCopy(Pixel& dst, const ConstPixel& src, size_t length,
                            CompositionMode mode) const = 0;

        virtual void onCopy(ImageView& to, const Point& toPos,
                            const ImageView& from, const Rect& fromRect,
                            CompositionMode mode) const = 0;

        virtual std::size_t onImageSize(const Size& size, Pt::ssize_t padding) const = 0;

    private:
        std::size_t _pixelStride;
};

} // namespace

} // namespace

#endif
