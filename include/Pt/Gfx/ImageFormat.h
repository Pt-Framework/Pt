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
#include <Pt/Gfx/BasicView.h>
#include <Pt/Gfx/Location.h>
#include <Pt/Gfx/PixelBase.h>
#include <Pt/Gfx/Color.h>
#include <Pt/TypeInfo.h>

namespace Pt {

namespace Gfx {

class Pixel;
class ConstPixel;
class ImageFormat;

///////////////////////////////////////////////////////////////////////
// Pixel
///////////////////////////////////////////////////////////////////////

class Pixel
{
    friend class ConstPixel;

    public:
        Pixel(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

        Pixel(const Pixel& p);

        ~Pixel()
        {
            if(_pixel)
                _pixel->~PixelBase();
        }

        Pixel& operator=(const Argb32Color& color);

        Pixel& operator=(const Color& color);

        Pixel& operator=(const Pixel& p);

        Pixel& operator=(const ConstPixel& p);

        void reset(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const Pixel& p);

        const ImageFormat& format() const
        { return *_format; }

        Pt::uint8_t* base()
        { return _pixel->base(); }

        const Pt::uint8_t* base() const
        { return _pixel->base(); }

        Pt::ssize_t xpos() const
        { return _x; }

        Pt::ssize_t ypos() const
        { return _y; }

        void advance()
        {
            Location& loc =_pixel->advance();
            _x = loc.xpos();
            _y = loc.ypos();
        }

        void advance(Pt::ssize_t n)
        {
            Location& loc =_pixel->advance(n);
            _x = loc.xpos();
            _y = loc.ypos();
        }

        Color toColor() const
        {
            return _pixel->toColor();
        }

        Argb32Color toArgb32Color() const
        {
            return _pixel->toArgb32Color();
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

        void assign(const ConstPixel& p, std::size_t length);

        void fill(std::size_t n, const Color& color)
        {   
            _pixel->fill(n, color);
        }

        bool equals(const ConstPixel& p) const;

        bool equals(const Pixel& p) const
        {
            return _pixel->base() == p.base();
        }

    private:
        ViewBase*           _view;
        Pt::ssize_t         _x;
        Pt::ssize_t         _y;
        const ImageFormat*  _format;
        PixelStorage        _storage;
        PixelBase*          _pixel;
        Pt::uint8_t*        _data;
};


inline Color toColor(const Pixel& p, const Color* tag = 0)
{
  return p.toColor();
}


inline Argb32Color toColor(const Pixel& p, const Argb32Color* tag = 0)
{
  return p.toArgb32Color();
}

///////////////////////////////////////////////////////////////////////
// Pixel
///////////////////////////////////////////////////////////////////////

class ConstPixel
{
    friend class Pixel;

    public:
        ConstPixel(const BasicConstView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

        ConstPixel(const BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

        ConstPixel(const ConstPixel& p);

        explicit ConstPixel(const Pixel& p);

        ~ConstPixel()
        {
            if(_pixel)
                _pixel->~ConstPixelBase();
        }

        void reset(const BasicConstView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const ConstPixel& p);

        const ImageFormat& format() const
        {
            return *_format;
        }

        const Pt::uint8_t* base() const
        { return _pixel->base(); }
        
        Pt::ssize_t xpos() const
        { return _x; }

        Pt::ssize_t ypos() const
        { return _y; }
        
        void advance()
        {
            _pixel->advance();
        }

        void advance(Pt::ssize_t n)
        {
            _pixel->advance(n);
        }

        Color toColor() const
        {
            return _pixel->toColor();
        }

        Argb32Color toArgb32Color() const
        {
            return _pixel->toArgb32Color();
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

        bool equals(const Pixel& p) const
        { 
            return _pixel->base() == p.base();
        }

    private:
        const ViewBase*     _view;
        Pt::ssize_t         _x;
        Pt::ssize_t         _y;
        const ImageFormat*  _format;
        PixelStorage        _storage;
        ConstPixelBase*     _pixel;
        const Pt::uint8_t*  _data;
};


inline Color toColor(const ConstPixel& p, const Color* tag = 0)
{
  return p.toColor();
}


inline Argb32Color toColor(const ConstPixel& p, const Argb32Color* tag = 0)
{
  return p.toArgb32Color();
}

///////////////////////////////////////////////////////////////////////
// ImageFormat
///////////////////////////////////////////////////////////////////////

/** @brief %Image format.
*/
class ImageFormat
{ 
  friend std::size_t pixelStride(const ImageFormat& format);

  friend std::size_t imageSize(const ImageFormat& format, Pt::ssize_t width, 
                               Pt::ssize_t height, std::size_t padding);
     
  friend ImageFormat* clone(const ImageFormat& format);

  friend void release(const ImageFormat* format);

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

        ImageFormat* clone() const
        {
            return onClone();
        }

        void release() const
        {
            onRelease();
        }

    protected:
        virtual ImageFormat* onClone() const
        {
            return 0;
        }

        virtual void onRelease() const
        {
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


inline ImageFormat* clone(const ImageFormat& format)
{
    return format.onClone();
}


inline void release(const ImageFormat* format)
{
     format->onRelease();
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

///////////////////////////////////////////////////////////////////////
// Pixel
///////////////////////////////////////////////////////////////////////

inline Pixel::Pixel(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
, _format( &view.format() )
, _pixel(0)
, _data( view.data() )
{ 
    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
}


inline Pixel::Pixel(const Pixel& p)
: _view(p._view)
, _x(p._x)
, _y(p._y)
, _format(p._format)
, _pixel(0)
, _data(p._data)
{ 
    if(p._pixel)
    {
        _pixel = _format->createPixel(_data, *_view, p.xpos(), p.ypos(), _storage);
    }
}


inline void Pixel::reset(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
{
    if(_pixel)
    {
        _pixel->~PixelBase();
        _pixel = 0;
    }

    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
    _data = view.data();
    _view = &view;
    _x = x;
    _y = y;
    _format = &view.format();
}


inline void Pixel::reset(const Pixel& p)
{
    if(_pixel)
    {
        _pixel->~PixelBase();
        
        _pixel = 0;
        _data = 0;
        _view = 0;
        _x = 0;
        _y = 0;
        _format = 0;
    }

    _pixel = p._format->createPixel(p._data, *p._view, p.xpos(), p.ypos(), _storage);
    _data = p._data;
    _view = p._view;
    _x = p._x;
    _y = p._y;
    _format = p._format;
}


inline Pixel& Pixel::operator=(const Argb32Color& color)
{
    _pixel->assign(color);
    return *this;
}


inline Pixel& Pixel::operator=(const Color& color)
{
    _pixel->assign(color);
    return *this;
}


inline Pixel& Pixel::operator=(const Pixel& p)
{
    _pixel->assign( p.toColor() );
    return *this;
}


inline Pixel& Pixel::operator=(const ConstPixel& p)
{
    _pixel->assign( p.toColor() );
    return *this;
}


inline void Pixel::assign(const ConstPixel& p, std::size_t length)
{
    bool isCompatible = _pixel->assign(*p._pixel, length);
    if( isCompatible )
        return;

    isCompatible = p._pixel->copy(*_pixel, length);
    if( isCompatible )
        return;

    const std::size_t colorsLength = 64;
    Argb32Color colors[colorsLength];

    std::size_t avail = length;

    while(avail > 0)
    {
        std::size_t n = std::min(colorsLength, avail);
        p.getColors(colors, n);
        this->assign(colors, n);
        avail -= n;
    }
}


inline bool Pixel::equals(const ConstPixel& p) const
{ 
    return _pixel->base() == p.base();
}

///////////////////////////////////////////////////////////////////////
// ConstPixel
///////////////////////////////////////////////////////////////////////

inline ConstPixel::ConstPixel(const BasicConstView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
, _format( &view.format() )
, _pixel(0)
, _data( view.data() )
{ 
    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
}


inline ConstPixel::ConstPixel(const BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
, _format( &view.format() )
, _pixel(0)
, _data( view.data() )
{ 
    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
}


inline ConstPixel::ConstPixel(const ConstPixel& p)
: _view(p._view)
, _x(p._x)
, _y(p._y)
, _format(p._format)
, _pixel(0)
, _data(p._data)
{ 
    if(p._pixel)
    {
        _pixel = _format->createPixel(_data, *_view, p.xpos(), p.ypos(), _storage);
    }
}


inline ConstPixel::ConstPixel(const Pixel& p)
: _view(p._view)
, _x(p._x)
, _y(p._y)
, _format(p._format)
, _pixel(0)
, _data(p._data)
{ 
    _pixel = _format->createPixel(_data, *_view, p.xpos(), p.ypos(), _storage);
}


inline void ConstPixel::reset(const BasicConstView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
{
    if(_pixel)
    {
        _pixel->~ConstPixelBase();
        _pixel = 0;
    }

    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
    _data = view.data();
    _view = &view;
    _x = x;
    _y = y;
    _format = &view.format();
}


inline void ConstPixel::reset(const ConstPixel& p)
{
    if(_pixel)
    {
        _pixel->~ConstPixelBase();
        
        _pixel = 0;
        _data = 0;
        _view = 0;
        _x = 0;
        _y = 0;
        _format = 0;
    }

    if(p._pixel)
    {
        _pixel = p._format->createPixel(p._data, *p._view, p.xpos(), p.ypos(), _storage);
        _data = p._data;
        _view = p._view;
        _x = p._x;
        _y = p._y;
        _format = p._format;
    }
}

} // namespace

} // namespace

#endif
