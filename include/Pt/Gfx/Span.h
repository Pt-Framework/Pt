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
#include <Pt/Gfx/View.h>
#include <Pt/Gfx/CopyPixel.h>
#include <Pt/Types.h>

#include <iterator>
#include <cstddef>
#include <cassert>

namespace Pt {

namespace Gfx {

template <typename FormatT, typename TraitsT>
class SpanIterator;

template <typename FormatT, typename TraitsT>
class ConstSpanIterator;


template <typename FormatT, typename TraitsT>
class SpanIterator
{
    template <typename F, typename T>
    friend class ConstSpanIterator;

    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        typedef typename Traits::PixelType Pixel;
        typedef typename Traits::ConstPixelType ConstPixel;

        typedef ConstSpanIterator<Format, Traits> ConstIterator;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = Pixel;
        using difference_type   = std::ptrdiff_t;
        using pointer           = Pixel*;
        using reference         = Pixel&;

    public:
        explicit SpanIterator(const Pixel& pixel)
        : _pixel(pixel)
        { }

        SpanIterator(const SpanIterator& it)
        : _pixel(it._pixel)
        { }

        SpanIterator& operator=(const SpanIterator& it)
        {
            _pixel.reset(it._pixel);
            return *this;
        }

        bool operator!=(const SpanIterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator!=(const ConstIterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator==(const SpanIterator& it) const
        { return _pixel.equals(it._pixel); }

        bool operator==(const ConstIterator& it) const
        { return _pixel.equals(it._pixel); }

        Pixel& operator*()
        { return _pixel; }

        Pixel* operator->()
        { return &_pixel; }

        SpanIterator& operator++()
        {
            _pixel.advance();
            return *this;
        }

        SpanIterator& operator+=(Pt::ssize_t n)
        {
            _pixel.advance(n);
            return *this;
        }

    private:
        Pixel _pixel;
};


template <typename FormatT, typename TraitsT>
class ConstSpanIterator
{
    template <typename F, typename T>
    friend class SpanIterator;

    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        typedef typename Traits::PixelType      Pixel;
        typedef typename Traits::ConstPixelType ConstPixel;

        typedef SpanIterator<Format, Traits> Iterator;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = ConstPixel;
        using difference_type   = std::ptrdiff_t;
        using pointer           = ConstPixel*;
        using reference         = ConstPixel&;

    public:
        explicit ConstSpanIterator(const ConstPixel& pixel)
        : _pixel(pixel)
        { }

        explicit ConstSpanIterator(const Pixel& pixel)
        : _pixel(pixel)
        { }

        ConstSpanIterator(const ConstSpanIterator& it)
        : _pixel(it._pixel)
        { }

        ConstSpanIterator& operator=(const ConstSpanIterator& it)
        {
            _pixel.reset(it._pixel);
            return *this;
        }

        bool operator!=(const ConstSpanIterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator!=(const Iterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator==(const ConstSpanIterator& it) const
        { return _pixel.equals(it._pixel); }

        bool operator==(const Iterator& it) const
        { return _pixel.equals(it._pixel); }

        const ConstPixel& operator*() const
        { return _pixel; }

        const ConstPixel* operator->() const
        { return &_pixel; }

        ConstSpanIterator& operator++()
        {
            _pixel.advance();
            return *this;
        }

        ConstSpanIterator& operator+=(Pt::ssize_t n)
        {
            _pixel.advance(n);
            return *this;
        }

    private:
        ConstPixel _pixel;
};


template <typename FormatT, typename TraitsT>
class Span
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        
        typedef typename TraitsT::PixelType      Pixel;
        typedef typename TraitsT::ConstPixelType ConstPixel;
    
        typedef SpanIterator<Format, Traits> Iterator;

    public:
        template <typename T>
        Span(T& view, Pt::ssize_t x, Pt::ssize_t y, std::size_t length)
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
    
        typedef ConstSpanIterator<Format, Traits> Iterator;

    public:
        template <typename T>
        ConstSpan(const T& view, Pt::ssize_t x, Pt::ssize_t y, std::size_t length)
        : _p(view, x, y)
        , _length(length)
        { }

        template <typename T>
        ConstSpan(T& view, Pt::ssize_t x, Pt::ssize_t y, std::size_t length)
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

        Iterator begin() const
        { return Iterator(_p); }

        Iterator end() const
        {
            Iterator it(_p);
            it += _length;
            return it;
        }

    private:
        ConstPixel  _p;
        std::size_t _length;
};


template <typename T>
Span<typename T::Format,
     typename T::Traits> span(T& source,
                              Pt::ssize_t x,
                              Pt::ssize_t y,
                              std::size_t length)
{
    return Span<typename T::Format, typename T::Traits>(source, x, y, length);
}


template <typename T>
ConstSpan<typename T::Format,
          typename T::Traits> span(const T& source,
                                   Pt::ssize_t x,
                                   Pt::ssize_t y,
                                   std::size_t length)
{
    return ConstSpan<typename T::Format, typename T::Traits>(source, x, y, length);
}

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
