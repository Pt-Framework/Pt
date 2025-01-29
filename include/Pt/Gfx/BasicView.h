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

#ifndef PT_GFX_BASIC_VIEW_H
#define PT_GFX_BASIC_VIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

template <typename FormatT>
class BasicView;

template <typename TraitsT>
class BasicPixel;

template <typename TraitsT>
class BasicConstPixel;


template <typename TraitsT>
class BasicPixel
{
    public:
        typedef TraitsT Traits;

        typedef typename Traits::Format Format;
        typedef typename Traits::View   View;
        typedef typename Traits::Index  Index;
        
        typedef BasicConstPixel<Traits> ConstPixel;

    public:
        BasicPixel(View& view, Pt::ssize_t x, Pt::ssize_t y)
        : _view(&view)
        , _index(view, x, y)
        {
        }
        
        BasicPixel(const BasicPixel& p)
        : _view(p._view)
        , _index(p._index)
        {  }

        BasicPixel& operator=(const BasicPixel& p)
        {
            assign(p, CompositionMode::SourceCopy);
            return *this;
        }

        BasicPixel& operator=(const ConstPixel& p)
        {
            assign(p, CompositionMode::SourceCopy);
            return *this;
        }

        BasicPixel& operator=(const Color& color)
        {
            assign(color, CompositionMode::SourceCopy);
            return *this;
        }

        void reset(View& view, Pt::ssize_t x, Pt::ssize_t y)
        {
            _view = &view;
            _index = Index(view, x, y);
        }

        void reset(const BasicPixel& p)
        {
             _view = p._view;
             _index = p._index;
        }

        Color getColor() const
        {
            return Traits::getColor(_view, _index);
        }

        void advance()
        {
            Traits::advance(_view, _index);
        }

        void advance( Pt::ssize_t n )
        {
            Traits::advance(_view, _index, n);
        }

        void assign(const Color& c, CompositionMode mode)
        {
            switch(mode) 
            {
                default:
                case CompositionMode::SourceCopy:
                    Traits::sourceCopy(_view, _index, c);
                    break;

                case CompositionMode::SourceOver:
                    Traits::sourceOver(_view, _index, c);
                    break;
            }
        }

        void assign(const BasicPixel& p, CompositionMode mode)
        {
            assign(ConstPixel(p), mode);
        }

        void assign(const ConstPixel& p, CompositionMode mode)
        {
            const bool isCompatible = _view->format() == *p.view().format();
            if( ! isCompatible )
            {
                assign(p.getColor(), mode);
                return;
            }

            switch(mode) 
            {
                default:
                case CompositionMode::SourceCopy:
                    Traits::sourceCopy(_view, _index, p);
                    break;

                case CompositionMode::SourceOver:
                    Traits::sourceOver(_view, _index, p);
                    break;
            }
        }

        const View& view() const
        { return *_view; }

        View& view()
        { return *_view; }


        bool operator!=(const BasicPixel& p) const
        { return Traits::equals(_index, p._index); }

        bool operator==(const BasicPixel& p) const
        { return ! Traits::equals(_index, p._index); }

    private:
        View*  _view;
        Index  _index;
};


template <typename TraitsT>
class BasicConstPixel
{
    public:
        typedef TraitsT Traits;

        typedef typename Traits::Format      Format;
        typedef typename Traits::View        View;
        typedef typename Traits::ConstIndex  Index;
        
        typedef BasicPixel<Traits> Pixel;

    public:
        BasicConstPixel(const View& view, Pt::ssize_t x, Pt::ssize_t y)
        : _view(&view)
        , _index(view, x, y)
        {
        }
        
        BasicConstPixel(const BasicConstPixel& p)
        : _view(p._view)
        , _index(p._index)
        {  }

        BasicConstPixel(const Pixel& p)
        : _view(p._view)
        , _index(p._index)
        {  }

        void reset(const View& view, Pt::ssize_t x, Pt::ssize_t y)
        {
            _view = &view;
            _index = Index(view, x, y);
        }

        void reset(const BasicConstPixel& p)
        {
             _view = p._view;
             _index = p._index;
        }

        void reset(const Pixel& p)
        {
             _view = p._view;
             _index = Index(p._index);
        }

        Color getColor() const
        {
            return Traits::getColor(_view, _index);
        }

        void advance()
        {
            Traits::advance(_view, _index);
        }

        void advance( Pt::ssize_t n )
        {
            Traits::advance(_view, _index, n);
        }


        const View& view() const
        { return *_view; }


        bool operator!=(const BasicConstPixel& p) const
        { return Traits::equals(_index, p._index); }

        bool operator==(const BasicConstPixel& p) const
        { return ! Traits::equals(_index, p._index); }

    private:
        const View*  _view;
        Index        _index;
};


template <typename FormatT>
class BasicView
{
    public:
        typedef FormatT                      Format;
        typedef typename FormatT::Pixel      Pixel;
        typedef typename FormatT::ConstPixel ConstPixel;

        typedef Pt::ssize_t       pos_t;
        typedef BasicPoint<pos_t> Point;
        typedef BasicSize<pos_t>  Size;
        typedef BasicRect<pos_t>  Rect;

        class PixelIterator
        {
            public:
                PixelIterator(BasicView& view, Pt::ssize_t x, Pt::ssize_t y)
                : _pixel(view, x, y)
                { }

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

            private:
                Pixel _pixel;
        };

        class ConstPixelIterator
        {
            public:
                ConstPixelIterator(const BasicView& view, Pt::ssize_t x, Pt::ssize_t y)
                : _pixel(view, x, y)
                { }

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

                const ConstPixel& operator*() const
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

            private:
                ConstPixel _pixel;
        };

    public:
        BasicView()
        : _format(0)
        , _data(0)
        , _width(0)
        , _height(0)
        , _padding(0)
        , _stride(0)
        {
            //_format = FormatT::instance() 
        }

        BasicView(const Format& format)
        : _format(&format)
        , _data(0)
        , _width(0)
        , _height(0)
        , _padding(0)
        , _stride(0)
        { }

        BasicView(const Format& format, Pt::uint8_t* data, 
                  Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding = 0)
        : _format(&format)
        , _data(data)
        , _width(width)
        , _height(height)
        , _padding(padding)
        , _stride(0)
        {
            _stride = (_width * format.pixelStride()) + _padding;
        }

        virtual ~BasicView()
        { }

        void reset(const Format& format, Pt::uint8_t* data, 
                   Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding = 0)
        {
            _format = &format;
            _data = data;
            _width = width,
            _height = height;
            _padding = padding;
            _stride = _width * format.pixelStride() + _padding;
        }

        void clear()
        {
            _data = 0;
            _width = 0;
            _height = 0;
            _padding = 0;
            _stride = 0;
        }

        /** @brief Returns an iterator to the pixel at the given position.
        */
        PixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return PixelIterator(*this, x, y); }

        /** @brief Returns an iterator to the first pixel.
        */
        PixelIterator begin()
        { return PixelIterator(*this, 0, 0); }

        /** @brief Returns an iterator to the end of the pixels.
        */
        PixelIterator end()
        { return PixelIterator(*this, 0, height()); }

        /** @brief Returns a const iterator to the pixel at the given position.
        */
        ConstPixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y) const
        { return ConstPixelIterator(*this, x, y); }

        /** @brief Returns a const iterator to the first pixel.
        */
        ConstPixelIterator begin() const
        { return ConstPixelIterator(*this, 0, 0); }

        /** @brief Returns a const iterator to the end of the pixels.
        */
        ConstPixelIterator end() const
        { return ConstPixelIterator(*this, 0, height()); }

        Pt::ssize_t width() const
        { return _width; }

        Pt::ssize_t height() const
        { return _height; }

        bool empty() const
        { return _width == 0 || _height == 0; }

        Pt::uint8_t* data()
        { return _data; }

        const Pt::uint8_t* data() const
        { return _data; }

        Pt::ssize_t stride() const
        { return _stride; }

        Pt::ssize_t padding() const
        { return _padding; }

        std::size_t pixelStride() const
        { return _format ? _format->pixelStride() : 0; }

        const Format* format() const
        { return _format; }

        void assign(Pixel& to, const Color& c, CompositionMode mode)
        {
            switch(mode) 
            {
                default:
                case CompositionMode::SourceCopy:
                    _format->sourceCopy(to, c);
                    break;

                case CompositionMode::SourceOver:
                    _format->sourceOver(to, c);
                    break;
            }
        }

        void assign(Pixel& to, const Pixel& p, CompositionMode mode)
        {
            assign(to, ConstPixel(p), mode);
        }

        void assign(Pixel& to, const ConstPixel& p, CompositionMode mode)
        {
            const bool isCompatible = *to.view().format() == *p.view().format();
            if( ! isCompatible )
            {
                assign(to, p.getColor(), mode);
                return;
            }

            switch(mode) 
            {
                default:
                case CompositionMode::SourceCopy:
                    _format->sourceCopy(to, p);
                    break;

                case CompositionMode::SourceOver:
                    _format->sourceOver(to, p);
                    break;
            }
        }

        void fill(Pixel& to, std::size_t n, const Color& c, CompositionMode mode)
        {
            switch(mode) 
            {
                default:
                case CompositionMode::SourceCopy:
                    _format->sourceCopy(to, n, c);
                    break;

                case CompositionMode::SourceOver:
                    _format->sourceOver(to, n, c);
                    break;
            }
        }

        void fill(Pixel& to, std::size_t n, const Pixel& p, CompositionMode mode)
        {
            fill(to, n, ConstPixel(p), mode);
        }

        void fill(Pixel& to, std::size_t n, const ConstPixel& p, CompositionMode mode)
        {
            const bool isCompatible = *to.view().format() == *p.view().format();
            if( ! isCompatible )
            {
                fill(to, n, p.getColor(), mode);
                return;
            }

            switch(mode) 
            {
                default:
                case CompositionMode::SourceCopy:
                    _format->sourceCopy(to, n, p);
                    break;

                case CompositionMode::SourceOver:
                    _format->sourceOver(to, n, p);
                    break;
            }
        }

        void copy(Pixel& to, const ConstPixel& p, std::size_t n, CompositionMode mode)
        {
            const bool isCompatible = *to.view().format() == *p.view().format();
            if( ! isCompatible )
                return;

            switch(mode) 
            {
                default:
                case CompositionMode::SourceCopy:
                    _format->sourceCopy(to, p, n);
                    break;

                case CompositionMode::SourceOver:
                    _format->sourceOver(to, p, n);
                    break;
            }
        }

        void copy(Pt::ssize_t toX, Pt::ssize_t toY,
                  const BasicView& from, Pt::ssize_t fromX, Pt::ssize_t fromY, 
                  Pt::ssize_t width, Pt::ssize_t height, CompositionMode mode)
        {
            const bool isCompatible = *this->format() == *from.format();
            if( ! isCompatible )
                return;

            switch(mode) 
            {
                default:
                case CompositionMode::SourceCopy:
                    _format->sourceCopy(*this, toX, toY,
                                        from, fromX, fromY,
                                        width, height);
                    break;

                case CompositionMode::SourceOver:
                    _format->sourceOver(*this, toX, toY,
                                        from, fromX, fromY,
                                        width, height);
                    break;
            }
        }

    private:
        // TODO: make sure _forrmat is never NULL or handle it
        const Format* _format;
        
        Pt::uint8_t*  _data;
        Pt::ssize_t   _width;
        Pt::ssize_t   _height;
        Pt::ssize_t   _padding;
        Pt::ssize_t   _stride;
};

} // namespace

} // namespace

#endif
