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

#ifndef PT_GFX_YUV12FORMAT_H
#define PT_GFX_YUV12FORMAT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ImageFormat.h>

namespace Pt {

namespace Gfx {

class PT_GFX_API Yuv12Format : public ImageFormat
{
    public:
        Yuv12Format();

    protected:
        virtual void onSetPixel(Pixel& to, const Pixel& from,
                                CompositionMode mode) const;

        virtual void onSetPixel(Pixel& to, const ConstPixel& from,
                                CompositionMode mode) const;

        virtual void onSetPixel(Pixel& pixel, const Color& c,
                                CompositionMode mode) const;

        virtual void onSetPixel(Pixel& to, const Pixel& from,
                                CompositionMode mode, Pt::uint8_t blendingAlpha) const;

        virtual void onSetPixel(Pixel& to, const ConstPixel& from,
                                CompositionMode mode, Pt::uint8_t blendingAlpha) const;

        virtual void onSetPixel(Pixel& pixel, const Color& c,
                                CompositionMode mode, Pt::uint8_t blendingAlpha) const;

        virtual void onSetPixels(Pixel& to, const Pixel& from, size_t length,
                                CompositionMode mode) const;

        virtual void onSetPixels(Pixel& to, const ConstPixel& from, size_t length,
                                CompositionMode mode) const;

        virtual void onSetPixels(Pixel& pixel, const Color& c, size_t length,
                                CompositionMode  mode) const;

        virtual Color onGetColor(const Pixel& pixel) const;

        virtual Color onGetColor(const ConstPixel& pixel) const;

        virtual void onCopy(Pixel& dst, const Pixel& src, size_t length,
                            CompositionMode mode) const;

        virtual void onCopy(Pixel& dst, const ConstPixel& src, size_t length,
                            CompositionMode mode) const;

        virtual void onCopy(ImageView& to, const Point& toPos,
                            const ImageView& from, const Rect& fromRect,
                            CompositionMode mode) const;

        virtual std::size_t onImageSize(const Size& size, Pt::ssize_t padding) const;
};

} // namespace

} // namespace

#endif
