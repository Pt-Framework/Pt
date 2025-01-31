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

template <typename TraitsT, typename PixelBaseT>
class BasicPixel;

template <typename TraitsT, typename PixelBaseT>
class BasicConstPixel;


template <typename FormatT, typename PixelBaseT>
class BasicPixel : public PixelBaseT
{
    public:
        typedef PixelBaseT PixelBase;

        typedef FormatT Format;
        typedef typename Format::View View;

        typedef typename Format::ConstPixel        ConstBase;
        typedef BasicConstPixel<Format, ConstBase> ConstPixel;

    public:
        BasicPixel(View& view, Pt::ssize_t x, Pt::ssize_t y)
        : PixelBase(view, x, y)
        , _view(&view)
        {
        }
        
        BasicPixel(const BasicPixel& p)
        : PixelBase(p)
        , _view(p._view)
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
            PixelBase::operator=( PixelBase(view, x, y) );
        }

        void reset(const BasicPixel& p)
        {
             _view = p._view;
             PixelBase::operator=(p);
        }

        Color getColor() const
        {
            return format().getColor(*_view, *this);
        }

        void advance()
        {
            format().advance(*_view, *this);
        }

        void advance( Pt::ssize_t n )
        {
            format().advance(*_view, *this, n);
        }

        void assign(const Color& c, CompositionMode mode)
        {
            switch(mode) 
            {
                default:
                case CompositionMode::SourceCopy:
                    format().sourceCopy(*_view, *this, c);
                    break;

                case CompositionMode::SourceOver:
                    format().sourceOver(*_view, *this, c);
                    break;
            }
        }

        void assign(const BasicPixel& p, CompositionMode mode)
        {
            assign(ConstPixel(p), mode);
        }

        void assign(const ConstPixel& p, CompositionMode mode)
        {
            const bool isCompatible = *_view->format() == *p.view().format();
            if( ! isCompatible )
            {
                assign(p.getColor(), mode);
                return;
            }

            switch(mode) 
            {
                default:
                case CompositionMode::SourceCopy:
                    format().sourceCopy(*_view, *this, p.view(), p);
                    break;

                case CompositionMode::SourceOver:
                    format().sourceOver(*_view, *this, p.view(), p);
                    break;
            }
        }


        const View& view() const
        { return *_view; }

        View& view()
        { return *_view; }


        const Format& format() const
        { return *_view->format(); }


        bool operator!=(const BasicPixel& p) const
        { return ! format().equals(*this, p); }

        bool operator==(const BasicPixel& p) const
        { return format().equals(*this, p); }

    private:
        View*  _view;
};


template <typename FormatT, typename PixelBaseT>
class BasicConstPixel : public PixelBaseT
{
    public:
        typedef PixelBaseT PixelBase;

        typedef FormatT Format;
        typedef typename Format::View View;
        
        typedef typename Format::Pixel          MutableBase;
        typedef BasicPixel<Format, MutableBase> Pixel;

    public:
        BasicConstPixel(const View& view, Pt::ssize_t x, Pt::ssize_t y)
        : PixelBase(view, x, y)
        , _view(&view)
        {
        }
        
        BasicConstPixel(const BasicConstPixel& p)
        : PixelBase(p)
        , _view(p._view)
        {  }

        BasicConstPixel(const Pixel& p)
        : PixelBase(p)
        , _view( &p.view() )
        {  }

        void reset(const View& view, Pt::ssize_t x, Pt::ssize_t y)
        {
            _view = &view;
            PixelBase::operator=( PixelBase(view, x, y) );
        }

        void reset(const BasicConstPixel& p)
        {
             _view = p._view;
             PixelBase::operator=(p);
        }

        void reset(const Pixel& p)
        {
             _view = p._view;
             PixelBase::operator=(p);
        }

        Color getColor() const
        {
            return format().getColor(*_view, *this);
        }

        void advance()
        {
            format().advance(*_view, *this);
        }

        void advance( Pt::ssize_t n )
        {
            format().advance(*_view, *this, n);
        }


        const View& view() const
        { return *_view; }

        const Format& format() const
        { return *_view->format(); }


        bool operator!=(const BasicConstPixel& p) const
        { return ! format().equals(*this, p); }

        bool operator==(const BasicConstPixel& p) const
        { return format().equals(*this, p._idex); }

    private:
        const View*  _view;
};


class ViewBase
{
    public:
        typedef Pt::ssize_t       pos_t;
        typedef BasicPoint<pos_t> Point;
        typedef BasicSize<pos_t>  Size;
        typedef BasicRect<pos_t>  Rect;

    public:
        ViewBase()
        : _data(0)
        , _width(0)
        , _height(0)
        , _pixelStride(0)
        , _padding(0)
        , _stride(0)
        {
        }

        ViewBase(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height, 
                 Pt::ssize_t pixelStride, Pt::ssize_t padding = 0)
        : _data(data)
        , _width(width)
        , _height(height)
        , _pixelStride(pixelStride)
        , _padding(padding)
        , _stride(0)
        {
            _stride = (_width * pixelStride) + _padding;
        }

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
        { return _pixelStride; }

    private:      
        Pt::uint8_t*  _data;
        Pt::ssize_t   _width;
        Pt::ssize_t   _height;
        Pt::ssize_t   _pixelStride;
        Pt::ssize_t   _padding;
        Pt::ssize_t   _stride;
};


template <typename FormatT>
class BasicView : public ViewBase
{
    public:
        typedef FormatT Format;
        
        typedef typename Format::Pixel      PixelBase;
        typedef typename Format::ConstPixel ConstPixelBase;

        typedef BasicPixel<Format, PixelBase>           Pixel;
        typedef BasicConstPixel<Format, ConstPixelBase> ConstPixel;

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
        : ViewBase()
        , _format(0)
        {
            //_format = FormatT::instance() 
        }

        BasicView(const Format& format)
        : ViewBase()
        , _format(&format)
        { }

        BasicView(const Format& format, Pt::uint8_t* data, 
                  Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding = 0)
        : ViewBase(data, width, height, format.pixelStride(), padding)
        , _format(&format)
        {
        }

        virtual ~BasicView()
        { }

        void reset(const Format& format, Pt::uint8_t* data, 
                   Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding = 0)
        {
            _format = &format;

            ViewBase& base = *this;
            base = ViewBase(data, width, height, format.pixelStride(), padding);
        }

        void clear()
        {
            ViewBase& base = *this;
            base = ViewBase();
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

        const Format* format() const
        { return _format; }

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
                    _format->sourceCopy(to.view(), to, p.view(), p, n);
                    break;

                case CompositionMode::SourceOver:
                    _format->sourceOver(to.view(), to, p.view(), p, n);
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
};

} // namespace

} // namespace

#endif
