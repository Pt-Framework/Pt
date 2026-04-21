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

#ifndef PT_GFX_PIXEL_VIEW_H
#define PT_GFX_PIXEL_VIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/View.h>
#include <Pt/Types.h>

#include <iterator>
#include <cstddef>

namespace Pt {

namespace Gfx {

template <typename FormatT, typename TraitsT>
class PixelIterator;

template <typename FormatT, typename TraitsT>
class ConstPixelIterator;


template <typename FormatT, typename TraitsT>
class PixelIterator
{
    template <typename F, typename T>
    friend class ConstPixelIterator;

    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        
        typedef typename Traits::Pixel      Pixel;
        typedef typename Traits::ConstPixel ConstPixel;

        typedef ConstPixelIterator<Format, Traits> ConstIterator;
 
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = Pixel;
        using difference_type   = std::ptrdiff_t;
        using pointer           = Pixel*;
        using reference         = Pixel&;

    public:
        template <typename T>
        PixelIterator(T& view, Pt::ssize_t x, Pt::ssize_t y)
        : _x(x)
        , _y(y)
        , _pixel(view, x, y)
        { }

        PixelIterator(const PixelIterator& it)
        : _x(it._x)
        , _y(it._y)
        , _pixel(it._pixel)
        { }

        PixelIterator& operator=(const PixelIterator& it)
        {
            _pixel.reset(it._pixel);
            _x = it._x;
            _y = it._y;
            return *this;
        }

        bool operator!=(const PixelIterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator!=(const ConstIterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator==(const PixelIterator& it) const
        { return _pixel.equals(it._pixel); }

        bool operator==(const ConstIterator& it) const
        { return _pixel.equals(it._pixel); }

        Pixel& operator*()
        { return _pixel; }

        Pixel* operator->()
        { return &_pixel; }

        PixelIterator& operator++()
        {
            _pixel.advance();

            if( ++_x >= _pixel.view().width() )
            {
                _x = 0;
                ++_y;

                _pixel.skipPadding();
            }

            return *this;
        }

        PixelIterator& operator+=(Pt::ssize_t n)
        {
            Pt::ssize_t off = _x + n;
            std::size_t dy = off / _pixel.view().width();
            std::size_t dx = off % _pixel.view().width() - _x;
            
            if(dy > 0)
            {
                _pixel.advanceLines(dy);
                _y += dy;
            }

            _pixel.advance(dx);
            _x += dx;
            
            return *this;
        }

    private:
        Pt::ssize_t _x;
        Pt::ssize_t _y;
        Pixel       _pixel;
};


template <typename FormatT, typename TraitsT>
class ConstPixelIterator
{
    template <typename F, typename T>
    friend class PixelIterator;

    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        
        typedef typename Traits::Pixel      Pixel;
        typedef typename Traits::ConstPixel ConstPixel;

        typedef PixelIterator<Format, Traits> Iterator;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = ConstPixel;
        using difference_type   = std::ptrdiff_t;
        using pointer           = ConstPixel*;
        using reference         = ConstPixel&;

    public:
        template <typename T>
        ConstPixelIterator(const T& view, 
                           Pt::ssize_t x, Pt::ssize_t y)
        : _x(x)
        , _y(y)
        , _pixel(view, x, y)
        { }

        template <typename T>
        ConstPixelIterator(T& view, 
                           Pt::ssize_t x, Pt::ssize_t y)
        : _x(x)
        , _y(y)
        , _pixel(view, x, y)
        { }

        ConstPixelIterator(const ConstPixelIterator& it)
        : _x(it._x)
        , _y(it._y)
        , _pixel(it._pixel)
        { }

        ConstPixelIterator& operator=(const ConstPixelIterator& it)
        {
            _pixel.reset(it._pixel);
            _x = it._x;
            _y = it._y;
            return *this;
        }

        bool operator!=(const ConstPixelIterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator!=(const Iterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator==(const ConstPixelIterator& it) const
        { return _pixel.equals(it._pixel); }

        bool operator==(const Iterator& it) const
        { return _pixel.equals(it._pixel); }

        const ConstPixel& operator*() const
        { return _pixel; }

        const ConstPixel* operator->() const
        { return &_pixel; }

        ConstPixelIterator& operator++()
        {
            _pixel.advance();
            
            if( ++_x >= _pixel.view().width() )
            {
                _x = 0;
                ++_y;

                _pixel.skipPadding();
            }

            return *this;
        }

        ConstPixelIterator& operator+=(Pt::ssize_t n)
        {
            Pt::ssize_t off = _x + n;
            std::size_t dy = off / _pixel.view().width();
            std::size_t dx = off % _pixel.view().width() - _x;
            
            if(dy > 0)
            {
                _pixel.advanceLines(dy);
                _y += dy;
            }

            _pixel.advance(dx);
            _x += dx;
            
            return *this;
        }

    private:
        Pt::ssize_t _x;
        Pt::ssize_t _y;
        ConstPixel _pixel;
};


template <typename FormatT, typename TraitsT>
class BasicPixelView
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;

        typedef typename Traits::Pixel      Pixel;
        typedef typename Traits::ConstPixel ConstPixel;

        typedef PixelIterator<Format, Traits> Iterator;

    public:
        explicit BasicPixelView( const Format& format = FormatT::get() );

        BasicPixelView(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
                       Pt::ssize_t padding = 0, const Format& format = FormatT::get());

        template <typename T>
        explicit BasicPixelView(T& source);

        template <typename T>
        BasicPixelView(T& source, Int x, Int y, Int w, Int h);

        BasicPixelView& operator=(const BasicPixelView&) = default;

        Pt::uint8_t* data()
        { return _view.data(); }

        const Pt::uint8_t* data() const
        { return _view.data(); }

        Pt::ssize_t width() const
        { return _view.width(); }

        Pt::ssize_t height() const
        { return _view.height(); }

        Pt::ssize_t stride() const
        { return _view.stride(); }

        const Format& format() const
        { return _view.format(); }

        Pt::ssize_t pixelStride() const
        { return _view.pixelStride(); }

        Pt::ssize_t padding() const
        { return _view.padding(); }

        Iterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return Iterator(_view, x, y); }

        Iterator begin()
        { return Iterator(_view, 0, 0); }

        Iterator end()
        { return Iterator(_view, 0, _view.height()); }

    private:
        BasicView<Format, Traits> _view;
};


template <typename FormatT, typename TraitsT>
class BasicConstPixelView
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;

        typedef typename Traits::Pixel      Pixel;
        typedef typename Traits::ConstPixel ConstPixel;

        typedef ConstPixelIterator<Format, Traits> Iterator;

    public:
        explicit BasicConstPixelView( const Format& format = FormatT::get() );

        BasicConstPixelView(const Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
                            Pt::ssize_t padding, const Format& format = FormatT::get());

        template <typename T>
        explicit BasicConstPixelView(const T& source);

        template <typename T>
        BasicConstPixelView(const T& source, Int x, Int y, Int w, Int h);

        BasicConstPixelView& operator=(const BasicConstPixelView&) = default;

        const Pt::uint8_t* data() const
        { return _view.data(); }

        Pt::ssize_t width() const
        { return _view.width(); }

        Pt::ssize_t height() const
        { return _view.height(); }

        Pt::ssize_t stride() const
        { return _view.stride(); }

        const Format& format() const
        { return _view.format(); }

        Pt::ssize_t pixelStride() const
        { return _view.pixelStride(); }

        Pt::ssize_t padding() const
        { return _view.padding(); }

        Iterator pixel(Pt::ssize_t x, Pt::ssize_t y) const
        { return Iterator(_view, x, y); }

        Iterator begin() const
        { return Iterator(_view, 0, 0); }

        Iterator end() const
        { return Iterator(_view, 0, _view.height()); }

    private:
        BasicConstView<Format, Traits> _view;
};


template <typename T>
BasicPixelView<typename T::Format, 
               typename T::Traits> pixelView(T& source)
{ 
    return BasicPixelView<typename T::Format, typename T::Traits>(source); 
}

template <typename T>
BasicConstPixelView<typename T::Format, 
                    typename T::Traits> pixelView(const T& source) 
{
    return BasicConstPixelView<typename T::Format, typename T::Traits>(source);
}


template <typename T>
BasicPixelView<typename T::Format,
               typename T::Traits> pixelView(T& source, Int x, Int y, Int w, Int h)
{ 
    return BasicPixelView<typename T::Format, typename T::Traits>(source, x, y, w, h); 
}


template <typename T>
BasicConstPixelView<typename T::Format,
                    typename T::Traits> pixelView(const T& source, Int x, Int y, Int w, Int h) 
{
    return BasicConstPixelView<typename T::Format, typename T::Traits>(source, x, y, w, h);
}


template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
BasicPixelView<FormatT,
               TraitsT> pixelView(Pt::uint8_t* data, Pt::ssize_t width,
                                  Pt::ssize_t height, Pt::ssize_t padding = 0)
{
    return BasicPixelView<FormatT, TraitsT>(data, width, height, padding, FormatT::get());
}


template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
BasicConstPixelView<FormatT,
                    TraitsT> pixelView(const Pt::uint8_t* data, Pt::ssize_t width,
                                       Pt::ssize_t height, Pt::ssize_t padding = 0)
{
    return BasicConstPixelView<FormatT, TraitsT>(data, width, height, padding, FormatT::get());
}


template <typename T>
BasicPixelView<typename T::Format,
               ImageTraitsF> pixelViewF(T& source)
{
    return BasicPixelView<typename T::Format, ImageTraitsF>(source);
}


template <typename T>
BasicConstPixelView<typename T::Format,
                    ImageTraitsF> pixelViewF(const T& source)
{
    return BasicConstPixelView<typename T::Format, ImageTraitsF>(source);
}


template <typename T>
BasicPixelView<typename T::Format,
               ImageTraitsF> pixelViewF(T& source, Int x, Int y, Int w, Int h)
{
    return BasicPixelView<typename T::Format, ImageTraitsF>(source, x, y, w, h);
}


template <typename T>
BasicConstPixelView<typename T::Format,
                    ImageTraitsF> pixelViewF(const T& source, Int x, Int y, Int w, Int h)
{
    return BasicConstPixelView<typename T::Format, ImageTraitsF>(source, x, y, w, h);
}


inline PixelViewF pixelViewF(Pt::uint8_t* data, Pt::ssize_t width,
                              Pt::ssize_t height, Pt::ssize_t padding,
                              const ImageFormat& format)
{
    return PixelViewF(data, width, height, padding, format);
}


inline ConstPixelViewF pixelViewF(const Pt::uint8_t* data, Pt::ssize_t width,
                                   Pt::ssize_t height, Pt::ssize_t padding,
                                   const ImageFormat& format)
{
    return ConstPixelViewF(data, width, height, padding, format);
}

} // namespace

} // namespace

#endif

#include <Pt/Gfx/PixelView.hpp>
