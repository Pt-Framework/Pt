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

#ifndef PT_GFX_BASIC_COLOR_VIEW_H
#define PT_GFX_BASIC_COLOR_VIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/BasicView.h>
#include <Pt/Types.h>

#include <iterator>
#include <cstddef>

namespace Pt {

namespace Gfx {

template <typename ColorT, typename PixelT> 
ColorT toColor(const PixelT& p, const ColorT* tag = 0)
{
  return p.toColor();
}


template <typename PixelT, typename ColorT> 
void assign(PixelT& p, const ColorT& color)
{
  p = color;
}


template <typename FormatT,  typename ColorT>
class BasicColorIterator
{
    public:
        typedef FormatT Format;
        typedef ColorT Color;
        typedef typename Format::Pixel Pixel;

    private:
        class PixelRef
        {
            public:
                PixelRef(Pixel& p)
                : _p(p)
                { }

                PixelRef& operator=(const ColorT& color)
                {
                    assign(_pixel, color);
                    return *this;
                }

            private:
                Pixel& _p;
        };

    public:
        using iterator_category = std::output_iterator_tag;
        using value_type        = Pixel;
        using difference_type   = std::ptrdiff_t;
        using pointer           = Pixel*;
        using reference         = PixelRef;

    public:
        BasicColorIterator(BasicView<Format>& view, Pt::ssize_t x, Pt::ssize_t y)
        : _pixel(view, x, y)
        { }

        BasicColorIterator(const BasicColorIterator& it)
        : _pixel(it._pixel)
        { }

        BasicColorIterator& operator=(const BasicColorIterator& it)
        {
            _pixel.reset(it._pixel);
            return *this;
        }

        bool operator!=(const BasicColorIterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator==(const BasicColorIterator& it) const
        { return _pixel.equals(it._pixel); }

        PixelRef operator*()
        { return PixelRef(_pixel); }

        BasicColorIterator& operator++()
        {
            _pixel.advance();
            return *this;
        }

        BasicColorIterator& operator+=(Pt::ssize_t n)
        {
            _pixel.advance(n);
            return *this;
        }

    private:
        Pixel _pixel;
};


template <typename FormatT, typename ColorT>
class BasicConstColorIterator
{
    static const ColorT* colorTag()
    { return 0; }

    public:
        typedef FormatT Format;
        typedef ColorT Color;
        typedef typename Format::ConstPixel Pixel;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = Pixel;
        using difference_type   = std::ptrdiff_t;
        using pointer           = Pixel*;
        using reference         = Pixel&;

    public:
        BasicConstColorIterator(const BasicConstView<Format>& view, Pt::ssize_t x, Pt::ssize_t y)
        : _pixel(view, x, y)
        { }

        BasicConstColorIterator(const BasicView<Format>& view, Pt::ssize_t x, Pt::ssize_t y)
        : _pixel(view, x, y)
        { }

        BasicConstColorIterator(const BasicConstColorIterator& it)
        : _pixel(it._pixel)
        { }

        BasicConstColorIterator& operator=(const BasicConstColorIterator& it)
        {
            _pixel.reset(it._pixel);
            return *this;
        }

        bool operator!=(const BasicConstColorIterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator==(const BasicConstColorIterator& it) const
        { return _pixel.equals(it._pixel); }

        Color operator*() const
        { return toColor( _pixel, colorTag() ); }

        BasicConstColorIterator& operator++()
        {
            _pixel.advance();
            return *this;
        }

        BasicConstColorIterator& operator+=(Pt::ssize_t n)
        {
            _pixel.advance(n);
            return *this;
        }

    private:
        Pixel _pixel;
};


template <typename FormatT,
          typename ColorT = typename FormatT::Color>
class BasicColorView : public BasicView<FormatT>
{
    public:
        typedef FormatT Format;
        typedef typename Format::Pixel Pixel;
        typedef BasicColorIterator<Format, ColorT> Iterator;

    public:
        explicit BasicColorView(const Format& format)
        : BasicView<FormatT>(format)
        { }

        explicit BasicColorView(BasicImage<FormatT>& image);

        template <typename OtherFormatT>
        explicit BasicColorView(BasicImage<OtherFormatT>& image);

        Iterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return Iterator(*this, x, y); }

        Iterator begin()
        { return Iterator(*this, 0, 0); }

        Iterator end()
        { return Iterator(*this, 0, this->height()); }
};


template <typename FormatT,
          typename ColorT = typename FormatT::Color>
class BasicConstColorView : public BasicConstView<FormatT>
{
    public:
        typedef FormatT Format;
        typedef typename Format::ConstPixel Pixel;
        typedef BasicConstColorIterator<Format, ColorT> Iterator;

    public:
        explicit BasicConstColorView(const Format& format)
        : BasicConstView<FormatT>(format)
        { }

        explicit BasicConstColorView(const BasicImage<FormatT>& image);

        explicit BasicConstColorView(const BasicConstImage<FormatT>& image);

        Iterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return Iterator(*this, x, y); }

        Iterator begin()
        { return Iterator(*this, 0, 0); }

        Iterator end()
        { return Iterator(*this, 0, this->height()); }
};

} // namespace

} // namespace

#include <Pt/Gfx/BasicColorView.hpp>

#endif
