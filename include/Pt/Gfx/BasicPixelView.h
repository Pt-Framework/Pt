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

#ifndef PT_GFX_BASIC_PIXEL_VIEW_H
#define PT_GFX_BASIC_PIXEL_VIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/BasicView.h>
#include <Pt/Types.h>

#include <iterator>
#include <cstddef>

namespace Pt {

namespace Gfx {

template <typename FormatT>
class BasicImage;

template <typename FormatT>
class BasicConstImage;

template <typename FormatT>
class BasicConstPixelView;

template <typename FormatT>
class BasicConstPixelIterator;


template <typename FormatT>
class BasicPixelIterator
{
    template <typename F>
    friend class BasicConstPixelIterator;

    public:
        typedef FormatT Format;
        typedef typename FormatT::Pixel Pixel;
        typedef typename FormatT::ConstPixel ConstPixel;
 
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = Pixel;
        using difference_type   = std::ptrdiff_t;
        using pointer           = Pixel*;
        using reference         = Pixel&;

    public:
        BasicPixelIterator(BasicView<Format>& view, Pt::ssize_t x, Pt::ssize_t y)
        : _pixel(view, x, y)
        { }

        BasicPixelIterator(const BasicPixelIterator& it)
        : _pixel(it._pixel)
        { }

        BasicPixelIterator& operator=(const BasicPixelIterator& it)
        {
            _pixel.reset(it._pixel);
            return *this;
        }

        bool operator!=(const BasicPixelIterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator!=(const BasicConstPixelIterator<Format>& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator==(const BasicPixelIterator& it) const
        { return _pixel.equals(it._pixel); }

        bool operator==(const BasicConstPixelIterator<Format>& it) const
        { return _pixel.equals(it._pixel); }

        Pixel& operator*()
        { return _pixel; }

        Pixel* operator->()
        { return &_pixel; }

        BasicPixelIterator& operator++()
        {
            _pixel.advance();
            return *this;
        }

        BasicPixelIterator& operator+=(Pt::ssize_t n)
        {
            _pixel.advance(n);
            return *this;
        }

    private:
        Pixel _pixel;
};


template <typename FormatT>
class BasicConstPixelIterator
{
    template <typename F>
    friend class BasicPixelIterator;

    public:
        typedef FormatT Format;
        typedef typename FormatT::Pixel Pixel;
        typedef typename FormatT::ConstPixel ConstPixel;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = ConstPixel;
        using difference_type   = std::ptrdiff_t;
        using pointer           = ConstPixel*;
        using reference         = ConstPixel&;

    public:
        BasicConstPixelIterator(const BasicConstView<Format>& view, 
                                Pt::ssize_t x, Pt::ssize_t y)
        : _pixel(view, x, y)
        { }

        BasicConstPixelIterator(const BasicView<Format>& view, 
                                Pt::ssize_t x, Pt::ssize_t y)
        : _pixel(view, x, y)
        { }

        BasicConstPixelIterator(const BasicConstPixelIterator& it)
        : _pixel(it._pixel)
        { }

        BasicConstPixelIterator& operator=(const BasicConstPixelIterator& it)
        {
            _pixel.reset(it._pixel);
            return *this;
        }

        bool operator!=(const BasicConstPixelIterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator!=(const BasicPixelIterator<Format>& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator==(const BasicConstPixelIterator& it) const
        { return _pixel.equals(it._pixel); }

        bool operator==(const BasicPixelIterator<Format>& it) const
        { return _pixel.equals(it._pixel); }

        const ConstPixel& operator*() const
        { return _pixel; }

        const ConstPixel* operator->() const
        { return &_pixel; }

        BasicConstPixelIterator& operator++()
        {
            _pixel.advance();
            return *this;
        }

        BasicConstPixelIterator& operator+=(Pt::ssize_t n)
        {
            _pixel.advance(n);
            return *this;
        }

    private:
        ConstPixel _pixel;
};


template <typename FormatT>
class BasicPixelView : public BasicView<FormatT>
{
    public:
        typedef FormatT Format;
        typedef typename FormatT::Pixel Pixel;
        typedef typename FormatT::ConstPixel ConstPixel;

        typedef BasicPixelIterator<Format> PixelIterator;
        typedef BasicConstPixelIterator<Format> ConstPixelIterator;

        typedef BasicPixelIterator<Format> Iterator;
        typedef BasicConstPixelIterator<Format> ConstIterator;

    public:
        explicit BasicPixelView(const Format& format)
        : BasicView<FormatT>(format)
        { }

        explicit BasicPixelView(BasicImage<FormatT>& image);

        template <typename OtherFormatT>
        explicit BasicPixelView(BasicImage<OtherFormatT>& image);

        PixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return PixelIterator(*this, x, y); }

        PixelIterator begin()
        { return PixelIterator(*this, 0, 0); }

        PixelIterator end()
        { return PixelIterator(*this, 0, this->height()); }

        ConstPixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y) const
        { return ConstPixelIterator(*this, x, y); }

        ConstPixelIterator begin() const
        { return ConstPixelIterator(*this, 0, 0); }

        ConstPixelIterator end() const
        { return ConstPixelIterator(*this, 0, this->height()); }

        ConstPixelIterator cpixel(Pt::ssize_t x, Pt::ssize_t y) const
        { return ConstPixelIterator(*this, x, y); }

        ConstPixelIterator cbegin() const
        { return ConstPixelIterator(*this, 0, 0); }

        ConstPixelIterator cend() const
        { return ConstPixelIterator(*this, 0, this->height()); }

    public:
        void assign(Pt::ssize_t x, Pt::ssize_t y, 
                    const BasicConstPixelView<FormatT>& view, 
                    Pt::ssize_t fromX, Pt::ssize_t fromY, 
                    Pt::ssize_t width, Pt::ssize_t height);

        template <typename FormatT2>
        void convert(Pt::ssize_t x, Pt::ssize_t y, 
                     const BasicConstPixelView<FormatT2>& view, 
                     Pt::ssize_t fromX, Pt::ssize_t fromY, Pt::ssize_t width, Pt::ssize_t height);
};


template <typename FormatT>
class BasicConstPixelView : public BasicConstView<FormatT>
{
    public:
        typedef FormatT Format;
        typedef typename FormatT::Pixel Pixel;
        typedef typename FormatT::ConstPixel ConstPixel;

        typedef BasicConstPixelIterator<Format> Iterator;

    public:
        explicit BasicConstPixelView(const Format& format)
        : BasicConstView<FormatT>(format)
        { }

        explicit BasicConstPixelView(const BasicImage<FormatT>& image);

        explicit BasicConstPixelView(const BasicConstImage<FormatT>& image);

        template <typename OtherFormatT>
        explicit BasicConstPixelView(const BasicImage<OtherFormatT>& image);

        template <typename OtherFormatT>
        explicit BasicConstPixelView(const BasicConstImage<OtherFormatT>& image);

        Iterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return Iterator(*this, x, y); }

        Iterator begin()
        { return Iterator(*this, 0, 0); }

        Iterator end()
        { return Iterator(*this, 0, this->height()); }
};

} // namespace

} // namespace

#include <Pt/Gfx/BasicPixelView.hpp>

#endif
