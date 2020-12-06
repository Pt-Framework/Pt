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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_GFX_IMAGEVIEW_H
#define PT_GFX_IMAGEVIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Types.h>

namespace Pt {
namespace Gfx {


class ImageView;


/** @brief Pixel in an image.
*/
class Pixel
{
    public:
        Pixel()
        : _view(0)
        , _base(0)
        , _x(0)
        , _y(0)
        {  
        }

        Pixel(ImageView& view, Pt::ssize_t x, Pt::ssize_t y);

        Pixel(const Pixel& p)
        : _view(p._view)
        , _base(p._base)
        , _x(p._x)
        , _y(p._y)
        {  }

        Pixel& operator=(const Pixel& p)
        {
            assign(p, CompositionMode::SourceCopy);
            return *this;
        }

        Pixel& operator=(const ConstPixel& p)
        {
            assign(p, CompositionMode::SourceCopy);
            return *this;
        }

        Pixel& operator=(const Color& color)
        {
            assign(color, CompositionMode::SourceCopy);
            return *this;
        }

        void reset(ImageView& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const Pixel& p)
        {
             _view = p._view;
             _base = p._base;
             _x = p._x;
             _y = p._y;
        }

        void advance();

        void advance( Pt::ssize_t n );

        void assign(const Color& color, CompositionMode mode);

        void assign(const Pixel& p, CompositionMode mode)
        {
            assign(p.toColor(), mode);
        }

        void assign(const ConstPixel& p, CompositionMode mode);

        Color toColor() const;

        ImageView& view() const
        { return *_view; }

        Pt::ssize_t x() const
        { return _x; }

        Pt::ssize_t y() const
        { return _y; }

        Pt::uint8_t* base()
        { return _base; }

        const Pt::uint8_t* base() const
        { return _base; }

        bool operator!=(const Pixel& p) const
        {  return _base != p._base; }

        bool operator==(const Pixel& p) const
        { return _base == p._base; }

        std::size_t pixelStride() const;

    private:
        ImageView* _view;
        Pt::uint8_t* _base;
        Pt::ssize_t _x;
        Pt::ssize_t _y;
};

/** @brief Const pixel in an image.
*/
class ConstPixel
{
    public:
        ConstPixel()
        : _view(0)
        , _base(0)
        , _x(0)
        , _y(0)
        {
        }

        ConstPixel(const ImageView& view, Pt::ssize_t x, Pt::ssize_t y);

        ConstPixel(const ConstPixel& p)
        : _view(p._view)
        , _base(p._base)
        , _x(p._x)
        , _y(p._y)
        {  }

        void reset(const ImageView& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const ConstPixel& p)
        {
             _view = p._view;
             _base = p._base;
             _x = p._x;
             _y = p._y;
        }

        void advance();

        void advance( Pt::ssize_t n );

        Color toColor() const;

        const ImageView& view() const
        { return *_view; }

        Pt::ssize_t x() const
        { return _x; }

        Pt::ssize_t y() const
        { return _y; }

        const Pt::uint8_t* base() const
        { return _base; }

        bool operator!=(const ConstPixel& p) const
        {  return _base != p._base; }

        bool operator==(const ConstPixel& p) const
        { return _base == p._base; }

        std::size_t pixelStride() const;

    private:
        const ImageView*   _view;
        const Pt::uint8_t* _base;
        Pt::ssize_t        _x;
        Pt::ssize_t        _y;
};

/** @brief View on image data.
*/
class ImageView
{
    public:
        typedef Gfx::Pixel Pixel;
        typedef Gfx::ConstPixel ConstPixel;

        class PixelIterator
        {
            public:
                PixelIterator(ImageView& image, Pt::ssize_t x, Pt::ssize_t y)
                : _pixel(image, x, y)
                {}

                PixelIterator(const PixelIterator& it)
                : _pixel(it._pixel)
                {}

                PixelIterator& operator=(const PixelIterator& it)
                {
                    _pixel.reset(it._pixel);
                    return *this;
                }

                bool operator!=(const PixelIterator& it) const
                { return _pixel != it._pixel; }

                bool operator==(const PixelIterator& it) const
                { return _pixel == it._pixel; }

                Pixel& operator*()
                { return _pixel; }

                Pixel* operator->()
                { return &_pixel; }

                PixelIterator& operator++()
                {
                    _pixel.advance();
                    return *this;
                }

                PixelIterator& operator+=(Pt::ssize_t n)
                {
                    _pixel.advance(n);
                    return *this;
                }

                std::size_t pixelStride() const
                { return _pixel.pixelStride(); }

            private:
                Pixel _pixel;
        };

        class ConstPixelIterator
        {
            public:
                ConstPixelIterator(const ImageView& image, Pt::ssize_t x, Pt::ssize_t y)
                : _pixel(image, x, y)
                {}

                ConstPixelIterator(const ConstPixelIterator& it)
                : _pixel(it._pixel)
                {}

                ConstPixelIterator& operator=(const ConstPixelIterator& it)
                {
                    _pixel.reset(it._pixel);
                    return *this;
                }

                bool operator!=(const ConstPixelIterator& it) const
                { return _pixel != it._pixel; }

                bool operator==(const ConstPixelIterator& it) const
                { return _pixel == it._pixel; }

                const ConstPixel& operator*()
                { return _pixel; }

                const ConstPixel* operator->() const
                { return &_pixel; }

                ConstPixelIterator& operator++()
                {
                    _pixel.advance();
                    return *this;
                }

                ConstPixelIterator& operator+=(Pt::ssize_t n)
                {
                    _pixel.advance(n);
                    return *this;
                }

                std::size_t pixelStride() const
                { return _pixel.pixelStride(); }

            private:
                ConstPixel _pixel;
        };

    public:
        ImageView()
        : _format( &ImageFormat::argb32() )
        , _data(0)
        , _size(0, 0)
        , _padding(0)
        , _stride(0)
        { }

        explicit ImageView(const ImageFormat& format)
        : _format(&format)
        , _data(0)
        , _size()
        , _padding(0)
        , _stride(0)
        { }

        ImageView(const ImageFormat& format, Pt::uint8_t* data,
                  const Size& size, Pt::ssize_t padding)
        : _format(&format)
        , _data(data)
        , _size(size)
        , _padding(padding)
        {
            _stride = (_size.width() * _format->pixelStride()) + _padding;
        }

        virtual ~ImageView()
        {}

        void reset(const ImageFormat& format, Pt::uint8_t* data,
                  const Size& size, Pt::ssize_t padding)
        {
            _format = &format;
            _data = data;
            _size = size;
            _padding = padding;
            _stride = (_size.width() * _format->pixelStride()) + _padding;
        }

        PixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return PixelIterator(*this, x, y); }

        PixelIterator begin()
        { return PixelIterator(*this, 0, 0); }

        PixelIterator end()
        { return PixelIterator(*this, 0, height()); }

        ConstPixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y) const
        { return ConstPixelIterator(*this, x, y); }

        ConstPixelIterator begin() const
        { return ConstPixelIterator(*this, 0, 0); }

        ConstPixelIterator end() const
        { return ConstPixelIterator(*this, 0, height()); }

        const ImageFormat& format() const
        { return *_format; }

        const Size& size() const
        { return _size; }

        Pt::ssize_t width() const
        { return _size.width(); }

        Pt::ssize_t height() const
        { return _size.height(); }

        bool empty() const
        { return _size.width() == 0 || _size.height() == 0; }

        Pt::uint8_t* data()
        { return _data; }

        const Pt::uint8_t* data() const
        { return _data; }

        std::size_t pixelStride() const
        { return _format->pixelStride(); }

        Pt::ssize_t stride() const
        { return _stride; }

        Pt::ssize_t padding() const
        { return _padding; }

        void clear()
        {
            _data = 0;
            _size.set(0, 0);
            _padding = 0;
            _stride = 0;
        }

    private:
        const ImageFormat* _format;

        Pt::uint8_t* _data;
        Size         _size;
        Pt::ssize_t  _padding;
        Pt::ssize_t  _stride;
};


/////////////////////////////////////////////////////////////////////////////
// Pixel Implementation
/////////////////////////////////////////////////////////////////////////////

inline Pixel::Pixel(ImageView& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
{
    _base = view.data() + view.stride() * y + x * view.pixelStride();
}


inline void Pixel::reset(ImageView& view, Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _x = x;
    _y = y;

    _base = view.data() + view.stride() * _y + _x * view.pixelStride();
}


inline void Pixel::advance()
{
    if( ++_x >= _view->width() )
    {
        _x = 0;
        ++_y;

        _base += _view->padding();
    }

    _base += _view->pixelStride();
}


inline void Pixel::advance(Pt::ssize_t n)
{
    Pt::ssize_t off = _x + n;
    _y += off / _view->width();
    _x  = off % _view->width();

    _base = _view->data() + _view->stride() * _y + _x * _view->pixelStride();
}


inline void Pixel::assign(const Color& color, CompositionMode mode)
{
    _view->format().setPixel(*this, color, mode);
}


inline void Pixel::assign(const ConstPixel& p, CompositionMode mode)
{
    assign(p.toColor(), mode);
}


inline Color Pixel::toColor() const
{
    return _view->format().getColor(*this);
}


inline std::size_t Pixel::pixelStride() const
{
    return _view->pixelStride();
}


/////////////////////////////////////////////////////////////////////////////
// ConstPixel Implementation
/////////////////////////////////////////////////////////////////////////////

inline ConstPixel::ConstPixel(const ImageView& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
{
    _base = view.data() + view.stride() * y + x * view.pixelStride();
}


inline void ConstPixel::reset(const ImageView& view, Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _x = x;
    _y = y;

    _base = view.data() + view.stride() * _y + _x * view.pixelStride();
}


inline void ConstPixel::advance()
{
    if( ++_x >= _view->width() )
    {
        _x = 0;
        ++_y;

        _base += _view->padding();
    }

    _base += _view->pixelStride();
}


inline void ConstPixel::advance(Pt::ssize_t n)
{
    Pt::ssize_t off = _x + n;
    _y += off / _view->width();
    _x += off % _view->width();

    _base = _view->data() + _view->stride() * _y + _x * _view->pixelStride();
}


inline Color ConstPixel::toColor() const
{
    return _view->format().getColor(*this);
}


inline std::size_t ConstPixel::pixelStride() const
{
    return _view->pixelStride();
}


} // namespace
} // namespace

#endif
