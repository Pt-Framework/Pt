/* Copyright (C) 2015 Marc Boris Duerner

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef PT_GFX_IMAGEFORMAT_H
#define PT_GFX_IMAGEFORMAT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ViewBase.h>
#include <Pt/Gfx/Color.h>
#include <Pt/TypeInfo.h>
#include <memory>

namespace Pt {

namespace Gfx {

class PixelBase;
class ConstPixelBase;
class PixelStorage;
class Pixel;
class ConstPixel;

/** @brief %Image format.
*/
class ImageFormat
{ 
  friend std::size_t pixelStride(const ImageFormat& format);

  friend std::size_t imageSize(const ImageFormat& format, Pt::ssize_t width, 
                               Pt::ssize_t height, std::size_t padding);
     
  friend std::unique_ptr<ImageFormat> clone(const ImageFormat& format);

    public:
        typedef Pixel Pixel;
        typedef ConstPixel ConstPixel;
        typedef Color Color;

    public:
        PT_GFX_API static const ImageFormat& rgb16();

        PT_GFX_API static const ImageFormat& rgb32();

        PT_GFX_API static const ImageFormat& argb32();

    public:
        explicit ImageFormat(size_t pixelStride)
        : _pixelStride(pixelStride)
        { }

        virtual ~ImageFormat() 
        { }

        PixelBase* createPixel(Pt::uint8_t* data, ViewBase& view, 
                               Pt::ssize_t x, Pt::ssize_t y, 
                               PixelStorage& store) const
        {
            return onCreatePixel(data, view, x, y, store);
        }

        ConstPixelBase* createPixel(const Pt::uint8_t* data, const ViewBase& view, 
                                    Pt::ssize_t x, Pt::ssize_t y, 
                                    PixelStorage& store) const
        {
            return onCreateConstPixel(data, view, x, y, store);
        }

        bool operator==(const ImageFormat& a) const
        {
            return onGetType() == a.onGetType();
        }

        bool operator!=(const ImageFormat& a) const
        {
            return ! (*this == a);
        }

    protected:
        virtual std::unique_ptr<ImageFormat> onClone() const
        {
            return 0;
        }

        virtual const std::type_info& onGetType() const = 0;

        virtual std::size_t onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                        std::size_t padding) const = 0;

        virtual PixelBase* onCreatePixel(Pt::uint8_t* data, ViewBase& view, 
                                         Pt::ssize_t x, Pt::ssize_t y, 
                                         PixelStorage& store) const = 0;

        virtual ConstPixelBase* onCreateConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                                                   Pt::ssize_t x, Pt::ssize_t y, 
                                                   PixelStorage& store) const = 0;

        inline const void* r0() const
        { return _r0.ptr; }
    
        inline const void* r1() const
        { return _r1.ptr; }

        inline const void* r2() const
        { return _r2.ptr; }

    private:
        const std::size_t  _pixelStride;
        const varint_t     _r0;
        const varint_t     _r1;
        const varint_t     _r2;
};


inline std::unique_ptr<ImageFormat> clone(const ImageFormat& format)
{
    return format.onClone();
}


inline std::size_t pixelStride(const ImageFormat& format)
{
    return format._pixelStride;
}


inline std::size_t imageSize(const ImageFormat& format, Pt::ssize_t width, Pt::ssize_t height,
                             std::size_t padding)
{
    return format.onImageSize(width, height, padding);
}

} // namespace

} // namespace

#include <Pt/Gfx/Pixel.h>

#endif
