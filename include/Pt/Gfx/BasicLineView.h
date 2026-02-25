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
#include <Pt/Gfx/PixelTraits.h>
#include <Pt/Types.h>

#include <cstddef>
#include <cassert>

namespace Pt {

namespace Gfx {

template <typename FormatT, typename TraitsT>
class BasicLineIterator;

template <typename FormatT, typename TraitsT>
class BasicConstLineIterator;


template <typename FormatT, typename TraitsT>
class BasicLineIterator
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        typedef typename TraitsT::PixelType Pixel;
        typedef typename TraitsT::ConstPixelType ConstPixel;

        using SpanType = BasicSpan<Format, Traits>;

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
            _span.advanceLines(1);
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
            _span.advanceLines(n);
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


template <typename FormatT, typename TraitsT>
class BasicConstLineIterator
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        typedef typename TraitsT::PixelType Pixel;
        typedef typename TraitsT::ConstPixelType ConstPixel;

        using SpanType = BasicConstSpan<Format, Traits>;

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
            _span.advanceLines(1);
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
            _span.advanceLines(n);
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
        SpanType                _span;
};


template <typename FormatT, typename TraitsT>
class BasicLineView : public BasicView<FormatT>
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;

        typedef typename Traits::PixelType Pixel;
        typedef typename Traits::ConstPixelType ConstPixel;

        typedef PixelTraits<Pixel> PixelTraits;

        typedef BasicLineIterator<Format, PixelTraits> Iterator;
        typedef BasicConstLineIterator<Format, PixelTraits> ConstIterator;

        typedef BasicSpan<Format, PixelTraits> Span;
        typedef BasicConstSpan<Format, PixelTraits> ConstSpan;

    public:
        explicit BasicLineView(const Format& format)
        : BasicView<FormatT>(format)
        { }

        explicit BasicLineView(BasicView<FormatT>& view);

        BasicLineView(BasicView<FormatT>& view,
                      Int x, Int y, Int w, Int h);

        template <typename OtherFmt, typename OtherTr>
        explicit BasicLineView(BasicImage<OtherFmt, OtherTr>& image);

        template <typename OtherFmt, typename OtherTr>
        BasicLineView(BasicImage<OtherFmt, OtherTr>& image,
                      Int x, Int y, Int w, Int h);

        Iterator line(Pt::ssize_t y)
        { return Iterator(*this, 0, y); }

        Iterator begin()
        { return Iterator(*this, 0, 0); }

        Iterator end()
        { return Iterator(*this, 0, this->height()); }

        ConstIterator line(Pt::ssize_t y) const
        { return ConstIterator(*this, 0, y); }

        ConstIterator begin() const
        { return ConstIterator(*this, 0, 0); }

        ConstIterator end() const
        { return ConstIterator(*this, 0, this->height()); }
};


template <typename FormatT, typename TraitsT>
class BasicConstLineView : public BasicConstView<FormatT>
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;

        typedef typename Traits::PixelType Pixel;
        typedef typename Traits::ConstPixelType ConstPixel;

        typedef PixelTraits<Pixel> PixelTraits;

        typedef BasicConstLineIterator<Format, PixelTraits> Iterator;

    public:
        explicit BasicConstLineView(const Format& format)
        : BasicConstView<FormatT>(format)
        { }

        template <typename OtherFormatT>
        explicit BasicConstLineView(const BasicView<OtherFormatT>& view);

        template <typename OtherFormatT>
        BasicConstLineView(const BasicView<OtherFormatT>& view,
                           Int x, Int y, Int w, Int h);

        template <typename OtherFormatT>
        explicit BasicConstLineView(const BasicConstView<OtherFormatT>& view);

        template <typename OtherFormatT>
        BasicConstLineView(const BasicConstView<OtherFormatT>& view,
                           Int x, Int y, Int w, Int h);

        template <typename OtherFormatT, typename OtherTraitsT>
        explicit BasicConstLineView(const BasicImage<OtherFormatT, OtherTraitsT>& image);

        template <typename OtherFormatT, typename OtherTraitsT>
        BasicConstLineView(const BasicImage<OtherFormatT, OtherTraitsT>& image,
                           Int x, Int y, Int w, Int h);

        template <typename OtherFormatT, typename OtherTraitsT>
        explicit BasicConstLineView(const BasicConstImage<OtherFormatT, OtherTraitsT>& image);

        template <typename OtherFormatT, typename OtherTraitsT>
        BasicConstLineView(const BasicConstImage<OtherFormatT, OtherTraitsT>& image,
                           Int x, Int y, Int w, Int h);

        Iterator line(Pt::ssize_t y)
        { return Iterator(*this, 0, y); }

        Iterator begin()
        { return Iterator(*this, 0, 0); }

        Iterator end()
        { return Iterator(*this, 0, this->height()); }
};

} // namespace

} // namespace

#endif

#include <Pt/Gfx/BasicLineView.hpp>
