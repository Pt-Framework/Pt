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

#ifndef PT_GFX_IMAGE_FORMAT_H
#define PT_GFX_IMAGE_FORMAT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ViewBase.h>
#include <Pt/Gfx/ImageTraits.h>
#include <Pt/Gfx/PixelBase.h>
#include <Pt/Gfx/Color.h>

#include <typeinfo>
#include <memory>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// ImageFormat
///////////////////////////////////////////////////////////////////////

/** @brief %Image format.
*/
class ImageFormat
{ 
    friend struct ImageTraits<ImageFormat>;
    friend struct ColorImageTraits;

    public:
        explicit ImageFormat(size_t pixelStride)
        : _pixelStride(pixelStride)
        { }

        virtual ~ImageFormat() 
        { }

        PixelBase* createPixel(Pt::uint8_t* data, const ViewBase& view, 
                               Pt::ssize_t x, Pt::ssize_t y, 
                               PixelStorage& store) const
        {
            return onCreatePixel(data, view, x, y, store);
        }

        bool operator==(const ImageFormat& a) const
        {
            return onGetType() == a.onGetType();
        }

        bool operator!=(const ImageFormat& a) const
        {
            return ! (*this == a);
        }

        const std::type_info& type() const
        {
            return onGetType();
        }

    public:
        PT_GFX_API static const ImageFormat& get(int type = 0);

        PT_GFX_API static const ImageFormat& rgb16();

        PT_GFX_API static const ImageFormat& rgb32();

        PT_GFX_API static const ImageFormat& argb32();

    protected:
        virtual std::unique_ptr<ImageFormat> onClone() const
        {
            return 0;
        }

        virtual const std::type_info& onGetType() const = 0;

        virtual std::size_t onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                        std::size_t padding) const = 0;

        virtual PixelBase* onCreatePixel(Pt::uint8_t* data, const ViewBase& view, 
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

///////////////////////////////////////////////////////////////////////
// ImageTraits
///////////////////////////////////////////////////////////////////////

template <>
struct ImageTraits<ImageFormat>
{
    typedef ViewTraits<ImageFormat> ViewTraitsType;

    static std::size_t pixelStride(const ImageFormat& format)
    {
        return format._pixelStride;
    }

    static std::size_t imageSize(const ImageFormat& format, Pt::ssize_t width, Pt::ssize_t height,
                                 std::size_t padding)
    {
        return format.onImageSize(width, height, padding);
    }

    static std::unique_ptr<ImageFormat> clone(const ImageFormat& format)
    {
        return format.onClone();
    }
};


struct ColorImageTraits
{
    typedef ColorFormatTraits ViewTraitsType;

    static std::size_t pixelStride(const ImageFormat& format)
    {
        return format._pixelStride;
    }

    static std::size_t imageSize(const ImageFormat& format, Pt::ssize_t width, Pt::ssize_t height,
                                 std::size_t padding)
    {
        return format.onImageSize(width, height, padding);
    }

    static std::unique_ptr<ImageFormat> clone(const ImageFormat& format)
    {
        return format.onClone();
    }
};


template <>
struct ViewTraits<ImageFormat>
{
    typedef Argb32Color ColorType;
    typedef Pixel<Argb32Color> PixelType;
    typedef ConstPixel<Argb32Color> ConstPixelType;
};


struct ColorFormatTraits
{
    typedef Color ColorType;
    typedef Pixel<Color> PixelType;
    typedef ConstPixel<Color> ConstPixelType;
};

///////////////////////////////////////////////////////////////////////
// Pixel
///////////////////////////////////////////////////////////////////////

template <typename ColorT>
class Pixel
{
    template <typename C>
    friend class ConstPixel;
        
    public:
        typedef ImageFormat FormatType;
        typedef ColorT ColorType;

    public:
        template <typename Tr>
        Pixel(BasicView<ImageFormat, Tr>& view, Pt::ssize_t x, Pt::ssize_t y);

        Pixel(const Pixel& p);

        ~Pixel();

        //
        // TODO GFX: operator= assignment or shalwow copy?
        //

        Pixel& operator=(const Color& color);

        Pixel& operator=(const Argb32Color& color);

        Pixel& operator=(const Pixel<Color>& p);

        Pixel& operator=(const Pixel<Argb32Color>& p);

        Pixel& operator=(const ConstPixel<Color>& p);

        Pixel& operator=(const ConstPixel<Argb32Color>& p);

        template <typename Tr>
        void reset(BasicView<ImageFormat, Tr>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const Pixel& p);

        const PixelBase* pixelBase() const
        { return _pixel; }

        PixelBase* pixelBase()
        { return _pixel; }

        const ViewBase& view() const
        { return _pixel->view(); }

        const ImageFormat& format() const
        { return *_format; }

        Pt::uint8_t* base()
        { return _pixel->base(); }

        const Pt::uint8_t* base() const
        { return _pixel->base(); }

        ColorT toColor() const
        {
            return _pixel->toColor<ColorT>();
        }

        void advance()
        {
            _pixel->advance();
        }

        void advanceLine()
        {
            _pixel->advanceLine();
        }

        void advance(Pt::ssize_t n)
        {
            _pixel->advance(n);
        }

        void advanceLines(Pt::ssize_t n)
        {
            _pixel->advanceLines(n);
        }

        void getColors(Color* colors, std::size_t length) const
        { 
            _pixel->getColors(colors, length); 
        }

        void getColors(Argb32Color* colors, std::size_t length) const
        { 
            _pixel->getColors(colors, length); 
        }

        void assign(const Argb32Color* colors, std::size_t length)
        { 
            _pixel->assign(colors, length); 
        }

        void assign(const Color* colors, std::size_t length)
        { 
            _pixel->assign(colors, length); 
        }

        void assign(const Pixel<Color>& p, std::size_t length);

        void assign(const ConstPixel<Color>& p, std::size_t length);

        void assign(const Pixel<Argb32Color>& p, std::size_t length);

        void assign(const ConstPixel<Argb32Color>& p, std::size_t length);

        void fill(std::size_t n, const Color& color)
        {   
            _pixel->fill(n, color);
        }

        bool equals(const Pixel& p) const
        {
            return _pixel->base() == p.base();
        }

        bool equals(const ConstPixel<ColorT>& p) const;

    private:
        template <typename PixelT>
        void assignPixels(const PixelT& p, std::size_t length);

    private:
        PixelBase*          _pixel;
        PixelStorage        _storage;
        const ImageFormat*  _format;
};

///////////////////////////////////////////////////////////////////////
// ConstPixel
///////////////////////////////////////////////////////////////////////

template <typename ColorT>
class ConstPixel
{
    template <typename C>
    friend class Pixel;

    public:
        typedef ImageFormat FormatType;
        typedef ColorT ColorType;

    public:
        template <typename Tr>
        ConstPixel(const BasicConstView<ImageFormat, Tr>& view, Pt::ssize_t x, Pt::ssize_t y);

        template <typename Tr>
        ConstPixel(const BasicView<ImageFormat, Tr>& view, Pt::ssize_t x, Pt::ssize_t y);

        ConstPixel(const ConstPixel& p);

        ConstPixel(const Pixel<ColorT>& p);

        ~ConstPixel();

        template <typename Tr>
        void reset(const BasicConstView<ImageFormat, Tr>& view, Pt::ssize_t x, Pt::ssize_t y);

        template <typename Tr>
        void reset(const BasicView<ImageFormat, Tr>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const ConstPixel& p);

        void reset(const Pixel<ColorT>& p);

        const PixelBase* pixelBase() const
        { return _pixel; }

        const ImageFormat& format() const
        { return *_format; }

        const ViewBase& view() const
        { return _pixel->view(); }

        const Pt::uint8_t* base() const
        { return _pixel->base(); }

        ColorT toColor() const
        {
            return _pixel->toColor<ColorT>();
        }

        void advance()
        {
            _pixel->advance();
        }

        void advanceLine()
        {
            _pixel->advanceLine();
        }

        void advance(Pt::ssize_t n)
        {
            _pixel->advance(n);
        }

        void advanceLines(Pt::ssize_t n)
        {
            _pixel->advanceLines(n);
        }

        void getColors(Color* colors, std::size_t length) const
        { 
            _pixel->getColors(colors, length); 
        }

        void getColors(Argb32Color* colors, std::size_t length) const
        { 
            _pixel->getColors(colors, length); 
        }

        bool equals(const ConstPixel& p) const
        {
            return _pixel->base() == p.base();
        }

        bool equals(const Pixel<ColorT>& p) const
        { 
            return _pixel->base() == p.base();
        }

    private:
        PixelBase*          _pixel;
        PixelStorage        _storage;
        const ImageFormat*  _format;
};

} // namespace

} // namespace

#include  <Pt/Gfx/ImageFormat.hpp>

#endif
