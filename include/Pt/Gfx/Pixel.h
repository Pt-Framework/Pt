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

#ifndef PT_GFX_PIXEL_H
#define PT_GFX_PIXEL_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Location.h>
#include <Pt/Gfx/PixelBase.h>
#include <Pt/Gfx/BasicView.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Types.h>

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

        ~Pixel();

        Pixel& operator=(const Argb32Color& color);

        Pixel& operator=(const Color& color);

        Pixel& operator=(const Pixel& p);

        Pixel& operator=(const ConstPixel& p);

        void reset(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const Pixel& p);

        const ImageFormat& format() const
        { return *_format; }

        const PixelBase* pixelBase() const
        { return _pixel; }

        ViewBase& view()
        { return *_view; }

        const ViewBase& view() const
        { return *_view; }

        Pt::uint8_t* base()
        { return _pixel->base(); }

        const Pt::uint8_t* base() const
        { return _pixel->base(); }

        Pt::ssize_t xpos() const
        { return _x; }

        Pt::ssize_t ypos() const
        { return _y; }

        Color toColor() const
        {
            return _pixel->toColor();
        }

        Argb32Color toArgb32Color() const
        {
            return _pixel->toArgb32Color();
        }

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

        bool equals(const Pixel& p) const
        {
            return _pixel->base() == p.base();
        }

        bool equals(const ConstPixel& p) const;

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

        ~ConstPixel();

        void reset(const BasicConstView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const ConstPixel& p);

        void reset(const Pixel& p);

        const ImageFormat& format() const
        {
            return *_format;
        }

        const ConstPixelBase* pixelBase() const
        { return _pixel; }

        const ViewBase& view() const
        { return *_view; }

        const Pt::uint8_t* base() const
        { return _pixel->base(); }
        
        Pt::ssize_t xpos() const
        { return _x; }

        Pt::ssize_t ypos() const
        { return _y; }
        
        Color toColor() const
        {
            return _pixel->toColor();
        }

        Argb32Color toArgb32Color() const
        {
            return _pixel->toArgb32Color();
        }

        void advance()
        {
            const ConstLocation& loc =_pixel->advance();
            _x = loc.xpos();
            _y = loc.ypos();
        }

        void advance(Pt::ssize_t n)
        {
            const ConstLocation& loc =_pixel->advance(n);
            _x = loc.xpos();
            _y = loc.ypos();
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
// Span
///////////////////////////////////////////////////////////////////////

class Span
{
    public:
        Span(BasicView<ImageFormat>& view, 
             Pt::ssize_t x, Pt::ssize_t y, std::size_t length)
        : _p(view, x, y)
        , _length(length)
        {
        }

        void setLength(std::size_t length)
        {
            _length = length;
        }

        void advance(std::size_t n)
        {
            _p.advance(n);
        }

        Pixel& front()
        {
            return _p;
        }

    private:
        Pixel       _p;
        std::size_t _length;
};

} // namespace

} // namespace

#include <Pt/Gfx/Pixel.hpp>

#endif
