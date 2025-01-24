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

#ifndef PT_GFX_IMAGEVIEW_H
#define PT_GFX_IMAGEVIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/BasicImage.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

/** @brief Index to a pixel in an image.
*/
class PixelIndex
{
    public:
        PixelIndex(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
        : _view(&view)
        , _x(x)
        , _y(y)
        { }
    
        const BasicView<ImageFormat>& view() const
        { return *_view; }

        Pt::ssize_t x() const
        { return _x; }

        Pt::ssize_t y() const
        { return _y; }

    private:
        const BasicView<ImageFormat>* _view;
        Pt::ssize_t      _x;
        Pt::ssize_t      _y;
};

/** @brief Pixel in an image.
*/
class Pixel
{
    public:
        Pixel(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

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

        void reset(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

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

        void assign(const Pixel& p, CompositionMode mode);

        void assign(const ConstPixel& p, CompositionMode mode);

        Color toColor() const;

        const BasicView<ImageFormat>& view() const
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

    private:
        const BasicView<ImageFormat>* _view;
        Pt::uint8_t*     _base;
        Pt::ssize_t      _x;
        Pt::ssize_t      _y;
};

/** @brief Const pixel in an image.
*/
class ConstPixel
{
    public:
        ConstPixel(const BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

        ConstPixel(const ConstPixel& p)
        : _view(p._view)
        , _base(p._base)
        , _x(p._x)
        , _y(p._y)
        {  }

        void reset(const BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

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

        const BasicView<ImageFormat>& view() const
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

    private:
        const BasicView<ImageFormat>*   _view;
        const Pt::uint8_t* _base;
        Pt::ssize_t        _x;
        Pt::ssize_t        _y;
};

/** @brief View on image data.
*/
class ImageView : public BasicView<ImageFormat>
{
    public:
        ImageView()
        : BasicView( ImageFormat::argb32() )
        { }

        explicit ImageView(const ImageFormat& format)
        : BasicView(format)
        { }

        ImageView(const ImageFormat& format, Pt::uint8_t* data,
                  Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding)
        : BasicView(format, data, width, height, padding)
        { }

        virtual ~ImageView()
        { }

        const ImageFormat& format() const
        { return *model(); }
};

/////////////////////////////////////////////////////////////////////////////
// Pixel Implementation
/////////////////////////////////////////////////////////////////////////////

inline Pixel::Pixel(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
{
    _base = view.data() + view.stride() * y + x * view.pixelStride();
}


inline void Pixel::reset(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
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

    std::size_t dy = off / _view->width();
    std::size_t dx = off % _view->width() - _x;

    _x += dx;
    _y += dy;
    _base += dy * _view->stride() + dx * _view->pixelStride();
}


inline void Pixel::assign(const Color& color, CompositionMode mode)
{
    _view->model()->setPixel(*this, color, mode);
}


inline void Pixel::assign(const Pixel& p, CompositionMode mode)
{
    assign(p.toColor(), mode);
}


inline void Pixel::assign(const ConstPixel& p, CompositionMode mode)
{
    assign(p.toColor(), mode);
}


inline Color Pixel::toColor() const
{
    return _view->model()->getColor(*this);
}

/////////////////////////////////////////////////////////////////////////////
// ConstPixel Implementation
/////////////////////////////////////////////////////////////////////////////

inline ConstPixel::ConstPixel(const BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
{
    _base = view.data() + view.stride() * y + x * view.pixelStride();
}


inline void ConstPixel::reset(const BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
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

    std::size_t dy = off / _view->width();
    std::size_t dx = off % _view->width() - _x;

    _x += dx;
    _y += dy;
    _base += dy * _view->stride() + dx * _view->pixelStride();
}


inline Color ConstPixel::toColor() const
{
    return _view->model()->getColor(*this);
}

} // namespace

} // namespace

#endif
