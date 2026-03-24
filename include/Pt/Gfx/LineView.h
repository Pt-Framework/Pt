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

#ifndef PT_GFX_LINE_VIEW_H
#define PT_GFX_LINE_VIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ImageView.h>
#include <Pt/Gfx/Span.h>
#include <Pt/Types.h>

#include <cstddef>
#include <cassert>

namespace Pt {

namespace Gfx {

template <typename FormatT, typename TraitsT>
class LineIterator;

template <typename FormatT, typename TraitsT>
class ConstLineIterator;


template <typename FormatT, typename TraitsT>
class LineIterator
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        typedef typename TraitsT::PixelType Pixel;
        typedef typename TraitsT::ConstPixelType ConstPixel;

    public:
        using value_type        = Span<Format, Traits>;
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::forward_iterator_tag;

    public:
        LineIterator(BasicImageView<Format>& view, Pt::ssize_t x, Pt::ssize_t y)
        : _span(view, x, y, view.width())
        { }

        reference operator*()
        { return _span; }

        const reference operator*() const
        { return _span; }

        pointer operator->()
        { return &_span; }

        const value_type* operator->() const
        { return &_span; }

        LineIterator& operator++() noexcept
        {
            _span.advanceLines(1);
            return *this;
        }

        LineIterator operator++(int) noexcept
        {
            LineIterator it(*this);
            ++*this;
            return it;
        }

        LineIterator& operator+=(Pt::ssize_t n)
        {
            _span.advanceLines(n);
            return *this;
        }

        bool operator==(const LineIterator& other) const noexcept
        {
            return _span.front().equals( other->front() );
        }

        bool operator!=(const LineIterator& other) const noexcept
        {
            return ! (*this == other);
        }

    private:
        Span<Format, Traits> _span;
};


template <typename FormatT, typename TraitsT>
class ConstLineIterator
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        
        typedef typename TraitsT::PixelType Pixel;
        typedef typename TraitsT::ConstPixelType ConstPixel;

    public:
        using value_type        = ConstSpan<Format, Traits>;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const value_type*;
        using reference         = const value_type&;
        using iterator_category = std::forward_iterator_tag;

    public:
        ConstLineIterator(const BasicConstImageView<Format>& view, Pt::ssize_t x, Pt::ssize_t y)
        : _span(view, x, y, view.width())
        { }

        const reference operator*() const
        { return _span; }

        const pointer operator->() const
        { return &_span; }

        ConstLineIterator& operator++() noexcept
        {
            _span.advanceLines(1);
            return *this;
        }

        ConstLineIterator operator++(int) noexcept
        {
            ConstLineIterator it(*this);
            ++*this;
            return it;
        }

        ConstLineIterator& operator+=(Pt::ssize_t n)
        {
            _span.advanceLines(n);
            return *this;
        }

        bool operator==(const ConstLineIterator& other) const noexcept
        {
            return _span.front().equals( other->front() );
        }

        bool operator!=(const ConstLineIterator& other) const noexcept
        {
            return ! (*this == other);
        }

    private:
        ConstSpan<Format, Traits> _span;
};


template <typename FormatT, typename TraitsT>
class BasicLineView
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;

        typedef typename Traits::PixelType Pixel;
        typedef typename Traits::ConstPixelType ConstPixel;

        typedef LineIterator<Format, Traits> Iterator;

    public:
        explicit BasicLineView(BasicImageView<FormatT, TraitsT>& view)
        : _view(view)
        { }

        Iterator line(Pt::ssize_t y)
        { return Iterator(_view, 0, y); }

        Iterator begin()
        { return Iterator(_view, 0, 0); }

        Iterator end()
        { return Iterator(_view, 0, _view.height()); }

    private:
        BasicImageView<FormatT, TraitsT> _view;
};


template <typename FormatT, typename TraitsT>
class BasicConstLineView
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;

        typedef typename Traits::PixelType Pixel;
        typedef typename Traits::ConstPixelType ConstPixel;

        typedef ConstLineIterator<Format, Traits> Iterator;
        typedef ConstLineIterator<Format, Traits> ConstIterator;

    public:
        explicit BasicConstLineView(const BasicImageView<FormatT, TraitsT>& view)
        : _view(view)
        { }

        explicit BasicConstLineView(const BasicConstImageView<FormatT, TraitsT>& view)
        : _view(view)
        { }

        Iterator line(Pt::ssize_t y) const
        { return Iterator(_view, 0, y); }

        Iterator begin() const
        { return Iterator(_view, 0, 0); }

        Iterator end() const
        { return Iterator(_view, 0, _view.height()); }

    private:
        const BasicConstImageView<FormatT, TraitsT> _view;
};


template <typename T>
BasicLineView<typename T::Format, typename T::Traits> lineView(T& source) 
{
    return BasicLineView<typename T::Format, typename T::Traits>(source);
}


template <typename T>
BasicConstLineView<typename T::Format, typename T::Traits> lineView(const T& source) 
{
    return BasicConstLineView<typename T::Format, typename T::Traits>(source);
}

} // namespace

} // namespace

#endif

#include <Pt/Gfx/LineView.hpp>
