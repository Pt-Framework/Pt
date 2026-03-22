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
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Types.h>

#include <cstddef>
#include <cassert>

namespace Pt {

namespace Gfx {

template <typename FormatT, typename TraitsT>
class Span
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        
        typedef typename TraitsT::PixelType Pixel;
        typedef typename TraitsT::ConstPixelType ConstPixel;
    
        typedef PixelIterator<Format, Traits> Iterator;
        typedef ConstPixelIterator<Format, Traits> ConstIterator;

    public:
        Span(BasicView<Format>& view, 
             Pt::ssize_t x, Pt::ssize_t y, std::size_t length)
        : _p(view, x, y)
        , _length(length)
        { }

        Span(const Span& span)
        : _p(span._p)
        , _length(span._length)
        { }

        Span& operator=(const Span& span)
        {
            _p.reset(span._p);
            _length = span._length;
            return *this;
        }

        bool empty() const
        { return _length == 0; }

        std::size_t length() const
        { return _length; }

        void setLength(std::size_t length)
        { _length = length; }

        void advance(std::size_t n)
        { _p.advance(n); }

        void skipPadding()
        { _p.skipPadding(); }

        void advanceLines(std::size_t n)
        { _p.advanceLines(n); }

        Pixel& front()
        { return _p; }

        const Pixel& front() const
        { return _p; }

        Iterator begin()
        { return Iterator(_p); }

        Iterator end()
        {
            Iterator it(_p);
            it += _length;
            return it;
        }

        ConstIterator begin() const
        { return ConstIterator(_p); }

        ConstIterator end() const
        {
            ConstIterator it(_p);
            it += _length;
            return it;
        }

    private:
        Pixel       _p;
        std::size_t _length;
};


template <typename FormatT, typename TraitsT>
class ConstSpan
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        
        typedef typename TraitsT::PixelType Pixel;
        typedef typename TraitsT::ConstPixelType ConstPixel;
    
        typedef ConstPixelIterator<Format, Traits> Iterator;
        typedef ConstPixelIterator<Format, Traits> ConstIterator;

    public:
        ConstSpan(const BasicConstView<Format>& view, 
                  Pt::ssize_t x, Pt::ssize_t y, std::size_t length)
        : _p(view, x, y)
        , _length(length)
        { }

        ConstSpan(const BasicView<Format>& view, 
                  Pt::ssize_t x, Pt::ssize_t y, std::size_t length)
        : _p(view, x, y)
        , _length(length)
        { }

        ConstSpan(const ConstSpan& span)
        : _p(span._p)
        , _length(span._length)
        { }

        ConstSpan(const Span<Format, Traits>& span)
        : _p(span.front())
        , _length(span.length())
        { }

        ConstSpan& operator=(const ConstSpan& span)
        {
            _p.reset(span._p);
            _length = span._length;
            return *this;
        }

        bool empty() const
        { return _length == 0; }

        std::size_t length() const
        { return _length; }

        void setLength(std::size_t length)
        { _length = length; }

        void advance(std::size_t n)
        { _p.advance(n); }

        void skipPadding()
        { _p.skipPadding(); }

        void advanceLines(std::size_t n)
        { _p.advanceLines(n); }

        ConstPixel& front()
        { return _p; }

        const ConstPixel& front() const
        { return _p; }

        ConstIterator begin() const
        { return ConstIterator(_p); }

        ConstIterator end() const
        {
            ConstIterator it(_p);
            it += _length;
            return it;
        }

    private:
        ConstPixel  _p;
        std::size_t _length;
};

/** @brief Copies the pixels of a span.
 */
template <typename SpanT, typename P>
void copySpan(const SpanT& from, P& to)
{
    copyPixel(from.front(), to, from.length());
}

} // namespace Gfx

} // namespace Pt

#endif
