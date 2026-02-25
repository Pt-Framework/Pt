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

#ifndef PT_GFX_BASIC_SPAN_H
#define PT_GFX_BASIC_SPAN_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/BasicPixelView.h>
#include <Pt/Types.h>

#include <cstddef>
#include <cassert>

namespace Pt {

namespace Gfx {

template <typename FormatT, typename TraitsT>
class BasicSpan
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        typedef typename TraitsT::PixelType Pixel;
        typedef typename TraitsT::ConstPixelType ConstPixel;
    
        typedef BasicPixelIterator<Format, Traits> Iterator;
        typedef BasicConstPixelIterator<Format, Traits> ConstIterator;

    public:
        BasicSpan(BasicView<Format>& view, 
                  Pt::ssize_t x, Pt::ssize_t y, std::size_t length)
        : _view(&view)
        , _x(x)
        , _y(y)
        , _p(view, x, y)
        , _length(length)
        { }

        BasicSpan(const BasicSpan& span)
        : _view(span._view)
        , _x(span._x)
        , _y(span._y)
        , _p(span._p)
        , _length(span._length)
        {}

        BasicSpan& operator=(const BasicSpan& span)
        {
            _view = span._view;
            _x = span._x;
            _y = span._y;
            _p.reset(span._p);
            _length = span.length;
        }

        void reset(BasicView<Format>& view, 
                   Pt::ssize_t x, Pt::ssize_t y, std::size_t length)
        {
            _view = &view;
            _x = x;
            _y = y;
            _p.reset(view, x, y);
            _length = length;
        }

        Pt::ssize_t xpos() const
        { return _x; }

        Pt::ssize_t ypos() const
        { return _y; }

        bool empty() const
        { return _length == 0; }

        std::size_t length() const
        { return _length; }

        void setLength(std::size_t length)
        {
            _length = length;
        }

        void advance(std::size_t n)
        {
            _p.advance(n);
            _x + n;
        }

        void advanceLine()
        {
            _p.advanceLine();
            _x = 0;
        }

        void advanceLines(std::size_t n)
        {
            _p.advanceLines(n);
            _y += n;
        }

        Pixel& front()
        { return _p; }

        const Pixel& front() const
        { return _p; }

        Iterator begin()
        { return Iterator(*_view, _x, _y); }

        Iterator end()
        { return Iterator(*_view, _x + _length, _y); }

        ConstIterator begin() const
        { return ConstIterator(*_view, _x, _y); }

        ConstIterator end() const
        { return ConstIterator(*_view, _x + _length, _y); }

    private:
        BasicView<Format>* _view;
        Pt::ssize_t        _x;
        Pt::ssize_t        _y;
        Pixel              _p;
        std::size_t        _length;
};


template <typename FormatT, typename TraitsT>
class BasicConstSpan
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        typedef typename TraitsT::PixelType Pixel;
        typedef typename TraitsT::ConstPixelType ConstPixel;
    
        typedef BasicConstPixelIterator<Format, Traits> Iterator;
        typedef BasicConstPixelIterator<Format, Traits> ConstIterator;

    public:
        BasicConstSpan(const BasicConstView<Format>& view, 
                       Pt::ssize_t x, Pt::ssize_t y, std::size_t length)
        : _view(&view)
        , _x(x)
        , _y(y)
        , _p(view, x, y)
        , _length(length)
        { }

        BasicConstSpan(const BasicConstSpan& span)
        : _view(span._view)
        , _x(span._x)
        , _y(span._y)
        , _p(span._p)
        , _length(span._length)
        { }

        template <typename Fmt, typename Tr>
        BasicConstSpan(const BasicSpan<Fmt, Tr>& span)
        : _view(span._view)
        , _x(span._x)
        , _y(span._y)
        , _p(span._p)
        , _length(span._length)
        { }

        BasicConstSpan& operator=(const BasicConstSpan& span)
        {
            _view = span._view;
            _x = span._x;
            _y = span._y;
            _p.reset(span._p);
            _length = span.length();
        }

        void reset(BasicConstView<Format>& view, 
                   Pt::ssize_t x, Pt::ssize_t y, std::size_t length)
        {
            _view = &view;
            _x = x;
            _y = y;
            _p.reset(view, x, y);
            _length = length;
        }

        Pt::ssize_t xpos() const
        { return _x; }

        Pt::ssize_t ypos() const
        { return _y; }

        bool empty() const
        { return _length == 0; }

        std::size_t length() const
        { return _length; }

        void setLength(std::size_t length)
        {
            _length = length;
        }

        void advance(std::size_t n)
        {
            _p.advance(n);
            _x + n;
        }

        void advanceLine()
        {
            _p.advanceLine();
            _x = 0;
        }

        void advanceLines(std::size_t n)
        {
            _p.advanceLines(n);
            _y += n;
        }

        ConstPixel& front()
        { return _p; }

        const ConstPixel& front() const
        { return _p; }

        ConstIterator begin() const
        { return ConstIterator(*_view, _x, _y); }

        ConstIterator end() const
        { return ConstIterator(*_view, _x + _length, _y); }

    private:
        const BasicConstView<Format>* _view;
        Pt::ssize_t        _x;
        Pt::ssize_t        _y;
        ConstPixel         _p;
        std::size_t        _length;
};


template <typename SpanT, typename FormatT, typename TraitsT>
void copySpan(const SpanT& from, BasicPixelIterator<FormatT, TraitsT>& to)
{
    copyPixels(from.front(), *to, from.length());
}


template <typename SpanT, typename FormatT, typename TraitsT>
void copySpan(const SpanT& from, BasicSpan<FormatT, TraitsT>& to)
{
    copyPixels(from.front(), to.front(), from.length());
}


//template <typename SpanT, typename FormatT, typename TraitsT>
//void copySpan(const SpanT& from, BasicPixelIterator<FormatT, TraitsT>& to);
//
//
//template <typename Fmt, typename Tr1, typename Tr2>
//void copySpan(const BasicSpan<Fmt, Tr1>& from, BasicPixelIterator<Fmt, Tr2>& to)
//{
//    to->assign( from.front(), from.length() );
//}
//
//
//template <typename Fmt, typename Tr1, typename Tr2>
//void copySpan(const BasicConstSpan<Fmt, Tr1>& from, BasicPixelIterator<Fmt, Tr2>& to)
//{
//    to->assign( from.front(), from.length() );
//}


//template <typename SpanT, typename SpanT2>
//void copySpan(const SpanT& from, SpanT2& to)
//{
//    copySpan( from, to.begin() );
//}

} // namespace

} // namespace

#endif
