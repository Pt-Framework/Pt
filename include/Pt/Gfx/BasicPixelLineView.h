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

#ifndef PT_GFX_BASIC_PIXEL_LINE_VIEW_H
#define PT_GFX_BASIC_PIXEL_LINE_VIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/BasicView.h>
#include <Pt/Gfx/BasicPixelSpan.h>
#include <Pt/Types.h>

#include <cstddef>
#include <cassert>

namespace Pt {

namespace Gfx {

template <typename FormatT>
class BasicPixelSpanIterator
{
    public:
        typedef FormatT Format;
        typedef typename FormatT::Pixel Pixel;
        typedef typename FormatT::ConstPixel ConstPixel;

        using SpanType = BasicPixelSpan<FormatT>;

    public:
        using value_type        = SpanType;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const SpanType*;
        using reference         = SpanType&;
        using iterator_category = std::forward_iterator_tag;

    public:
        BasicPixelSpanIterator(BasicView<Format>& view, Pt::ssize_t x, Pt::ssize_t y, 
                               std::size_t length)
        : _span(view, x, y, length)
        { }

        SpanType& operator*() const
        { return _span; }

        BasicPixelSpanIterator& operator++() noexcept
        {
            ++_span;
            return *this;
        }

        BasicPixelSpanIterator operator++(int) noexcept
        {
            BasicPixelSpanIterator it(*this);
            ++*this;
            return it;
        }

        BasicPixelSpanIterator& operator+=(Pt::ssize_t n)
        {
            _span += n;
            return *this;
        }

        bool operator==(const BasicPixelSpanIterator& other) const noexcept
        {
            return _span.front().equals( other.front() );
        }

        bool operator!=(const BasicPixelSpanIterator& other) const noexcept
        {
            return ! (*this == other);
        }

    private:
        SpanType _span;
};


template <typename FormatT>
class BasicPixelLineIterator
{
    public:
        typedef FormatT Format;
        typedef typename FormatT::Pixel Pixel;
        typedef typename FormatT::ConstPixel ConstPixel;

        using SpanType = BasicPixelSpan<FormatT>;

    public:
        using value_type        = SpanType;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const SpanType*;
        using reference         = SpanType&;
        using iterator_category = std::forward_iterator_tag;

    public:
        BasicPixelLineIterator(BasicView<Format>& view, Pt::ssize_t x, Pt::ssize_t y, 
                               std::size_t length)
        : _view(&view)
        , _span(view, x, y, length)
        { }

        SpanType& operator*() const
        { return _span; }

        BasicPixelLineIterator& operator++() noexcept
        {
            _span += _view->width();
            return *this;
        }

        BasicPixelLineIterator operator++(int) noexcept
        {
            BasicPixelLineIterator it(*this);
            ++*this;
            return it;
        }

        BasicPixelLineIterator& operator+=(Pt::ssize_t n)
        {
            _span += (_view->width() * n);
            return *this;
        }

        bool operator==(const BasicPixelLineIterator& other) const noexcept
        {
            return _span.front().equals( other.front() );
        }

        bool operator!=(const BasicPixelLineIterator& other) const noexcept
        {
            return ! (*this == other);
        }

    private:
        BasicView<Format>* _view;
        SpanType           _span;
};

} // namespace

} // namespace

#endif
