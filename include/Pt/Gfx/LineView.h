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
#include <Pt/Gfx/BasicView.h>
#include <Pt/Gfx/BasicSpan.h>
#include <Pt/Types.h>

#include <cstddef>
#include <cassert>

namespace Pt {

namespace Gfx {

template <typename FormatT>
class BasicLineIterator
{
    public:
        typedef FormatT Format;
        typedef typename FormatT::Pixel Pixel;
        typedef typename FormatT::ConstPixel ConstPixel;

        using SpanType = BasicSpan<FormatT>;

    public:
        using value_type        = SpanType;
        using difference_type   = std::ptrdiff_t;
        using pointer           = SpanType*;
        using reference         = SpanType&;
        using iterator_category = std::forward_iterator_tag;

    public:
        BasicLineIterator(BasicView<Format>& view, Pt::ssize_t x, Pt::ssize_t y)
        : _view(&view)
        , _span(view, x, y, view.width())
        { }

        SpanType& operator*()
        { return _span; }

        const SpanType& operator*() const
        { return _span; }

        SpanType* operator->()
        { return &_span; }

        const SpanType* operator->() const
        { return &_span; }

        BasicLineIterator& operator++() noexcept
        {
            _span.advance( _view->width() );
            return *this;
        }

        BasicLineIterator operator++(int) noexcept
        {
            BasicLineIterator it(*this);
            ++*this;
            return it;
        }

        BasicLineIterator& operator+=(Pt::ssize_t n)
        {
            _span.advance(_view->width() * n);
            return *this;
        }

        bool operator==(const BasicLineIterator& other) const noexcept
        {
            return _span.front().equals( other->front() );
        }

        bool operator!=(const BasicLineIterator& other) const noexcept
        {
            return ! (*this == other);
        }

    private:
        BasicView<Format>* _view;
        SpanType           _span;
};


template <typename FormatT>
class BasicConstLineIterator
{
    public:
        typedef FormatT Format;
        typedef typename FormatT::Pixel Pixel;
        typedef typename FormatT::ConstPixel ConstPixel;

        using SpanType = BasicConstSpan<FormatT>;

    public:
        using value_type        = SpanType;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const SpanType*;
        using reference         = const SpanType&;
        using iterator_category = std::forward_iterator_tag;

    public:
        BasicConstLineIterator(BasicConstView<Format>& view, Pt::ssize_t x, Pt::ssize_t y)
        : _view(&view)
        , _span(view, x, y, view.width())
        { }

        const SpanType& operator*() const
        { return _span; }

        const SpanType* operator->() const
        { return &_span; }

        BasicConstLineIterator& operator++() noexcept
        {
            _span.advance( _view->width() );
            return *this;
        }

        BasicConstLineIterator operator++(int) noexcept
        {
            BasicConstLineIterator it(*this);
            ++*this;
            return it;
        }

        BasicConstLineIterator& operator+=(Pt::ssize_t n)
        {
            _span.advance( _view->width() * n );
            return *this;
        }

        bool operator==(const BasicConstLineIterator& other) const noexcept
        {
            return _span.front().equals( other->front() );
        }

        bool operator!=(const BasicConstLineIterator& other) const noexcept
        {
            return ! (*this == other);
        }

    private:
        BasicConstView<Format>* _view;
        SpanType           _span;
};


template <typename FormatT>
class BasicLineView : public BasicView<FormatT>
{
    public:
        typedef FormatT Format;
        typedef typename FormatT::Pixel Pixel;
        typedef typename FormatT::ConstPixel ConstPixel;

        typedef BasicLineIterator<Format> Iterator;
        typedef BasicConstLineIterator<Format> ConstIterator;

    public:
        explicit BasicLineView(const Format& format)
        : BasicView<FormatT>(format)
        { }

        explicit BasicLineView(BasicView<FormatT>& view);

        explicit BasicLineView(BasicImage<FormatT>& image);

        template <typename OtherFormatT>
        explicit BasicLineView(BasicImage<OtherFormatT>& image);

        Iterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return Iterator(*this, x, y); }

        Iterator begin()
        { return Iterator(*this, 0, 0); }

        Iterator end()
        { return Iterator(*this, 0, this->height()); }

        ConstIterator pixel(Pt::ssize_t x, Pt::ssize_t y) const
        { return ConstIterator(*this, x, y); }

        ConstIterator begin() const
        { return ConstIterator(*this, 0, 0); }

        ConstIterator end() const
        { return ConstIterator(*this, 0, this->height()); }
};


template <typename FormatT>
class BasicConstLineView : public BasicConstView<FormatT>
{
    public:
        typedef FormatT Format;
        typedef typename FormatT::Pixel Pixel;
        typedef typename FormatT::ConstPixel ConstPixel;

        typedef BasicConstLineIterator<Format> Iterator;

    public:
        explicit BasicConstLineView(const Format& format)
        : BasicConstView<FormatT>(format)
        { }

        explicit BasicConstLineView(const BasicView<FormatT>& view);

        explicit BasicConstLineView(const BasicConstView<FormatT>& view);

        explicit BasicConstLineView(const BasicImage<FormatT>& image);

        explicit BasicConstLineView(const BasicConstImage<FormatT>& image);

        template <typename OtherFormatT>
        explicit BasicConstLineView(const BasicImage<OtherFormatT>& image);

        template <typename OtherFormatT>
        explicit BasicConstLineView(const BasicConstImage<OtherFormatT>& image);

        Iterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return Iterator(*this, x, y); }

        Iterator begin()
        { return Iterator(*this, 0, 0); }

        Iterator end()
        { return Iterator(*this, 0, this->height()); }
};

} // namespace

} // namespace


namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// BasicLineView
///////////////////////////////////////////////////////////////////////

template <typename FormatT>
inline BasicLineView<FormatT>::BasicLineView(BasicView<FormatT>& view)
: BasicView<FormatT>(view)
{ }


template <typename FormatT>
inline BasicLineView<FormatT>::BasicLineView(BasicImage<FormatT>& image)
: BasicView<FormatT>(image)
{ }


template <typename FormatT>
template <typename OtherFormatT>
inline BasicLineView<FormatT>::BasicLineView(BasicImage<OtherFormatT>& image)
: BasicView<FormatT>(image)
{
}

///////////////////////////////////////////////////////////////////////
// BasicConstLineView
///////////////////////////////////////////////////////////////////////

template <typename FormatT>
inline BasicConstLineView<FormatT>::BasicConstLineView(const BasicView<FormatT>& view)
: BasicConstView<FormatT>(view)
{
}


template <typename FormatT>
inline BasicConstLineView<FormatT>::BasicConstLineView(const BasicConstView<FormatT>& view)
: BasicConstView<FormatT>(view)
{
}


template <typename FormatT>
inline BasicConstLineView<FormatT>::BasicConstLineView(const BasicImage<FormatT>& image)
: BasicConstView<FormatT>(image)
{ }


template <typename FormatT>
inline BasicConstLineView<FormatT>::BasicConstLineView(const BasicConstImage<FormatT>& image)
: BasicConstView<FormatT>(image)
{ }


template <typename FormatT>
template <typename OtherFormatT>
inline BasicConstLineView<FormatT>::BasicConstLineView(const BasicImage<OtherFormatT>& image)
: BasicConstView<FormatT>(image)
{
}


template <typename FormatT>
template <typename OtherFormatT>
inline BasicConstLineView<FormatT>::BasicConstLineView(const BasicConstImage<OtherFormatT>& image)
: BasicConstView<FormatT>(image)
{
}

} // namespace

} // namespace

#endif
