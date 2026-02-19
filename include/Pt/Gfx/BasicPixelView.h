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
#include <Pt/Gfx/PixelTraits.h>
#include <Pt/Types.h>

#include <iterator>
#include <cstddef>

namespace Pt {

namespace Gfx {

template <typename FormatT, typename TraitsT>
class BasicPixelIterator;

template <typename FormatT, typename TraitsT>
class BasicConstPixelIterator;

template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
class BasicPixelView; 

template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
class BasicConstPixelView;


template <typename FormatT, typename TraitsT>
class BasicPixelIterator
{
    template <typename F, typename T>
    friend class BasicConstPixelIterator;

    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        typedef typename Traits::PixelType Pixel;
        typedef typename Traits::ConstPixelType ConstPixel;

        typedef BasicConstPixelIterator<Format, Traits> ConstIterator;
 
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

        BasicPixelIterator(const Pixel& p)
        : _pixel(p)
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

        bool operator!=(const ConstIterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator==(const BasicPixelIterator& it) const
        { return _pixel.equals(it._pixel); }

        bool operator==(const ConstIterator& it) const
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


template <typename FormatT, typename TraitsT>
class BasicConstPixelIterator
{
    template <typename F, typename T>
    friend class BasicPixelIterator;

    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        typedef typename Traits::PixelType Pixel;
        typedef typename Traits::ConstPixelType ConstPixel;

        typedef BasicPixelIterator<Format, Traits> Iterator;

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

        BasicConstPixelIterator(const ConstPixel& p)
        : _pixel(p)
        { }

        BasicConstPixelIterator(const Pixel& p)
        : _pixel(p)
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

        bool operator!=(const Iterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator==(const BasicConstPixelIterator& it) const
        { return _pixel.equals(it._pixel); }

        bool operator==(const Iterator& it) const
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


template <typename FormatT, typename TraitsT>
class BasicPixelView : public BasicView<FormatT>
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;

        typedef typename Traits::PixelType Pixel;
        typedef typename Traits::ConstPixelType ConstPixel;

        typedef PixelTraits<Pixel> PixelTraits;

        typedef BasicPixelIterator<Format, PixelTraits> Iterator;
        typedef BasicConstPixelIterator<Format, PixelTraits> ConstIterator;

    public:
        explicit BasicPixelView(const Format& format)
        : BasicView<FormatT>(format)
        { }

        explicit BasicPixelView(BasicImage<FormatT, TraitsT>& image);

        template <typename OtherFormatT, typename OtherTraitsT>
        explicit BasicPixelView(BasicImage<OtherFormatT, OtherTraitsT>& image);

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

        ConstIterator cpixel(Pt::ssize_t x, Pt::ssize_t y) const
        { return ConstIterator(*this, x, y); }

        ConstIterator cbegin() const
        { return ConstIterator(*this, 0, 0); }

        ConstIterator cend() const
        { return ConstIterator(*this, 0, this->height()); }
};


template <typename FormatT, typename TraitsT>
class BasicConstPixelView : public BasicConstView<FormatT>
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;

        typedef typename Traits::PixelType Pixel;
        typedef typename Traits::ConstPixelType ConstPixel;

        typedef PixelTraits<Pixel> PixelTraits;

        typedef BasicConstPixelIterator<Format, PixelTraits> Iterator;

    public:
        explicit BasicConstPixelView(const Format& format)
        : BasicConstView<FormatT>(format)
        { }

        explicit BasicConstPixelView(const BasicView<FormatT>& view);

        explicit BasicConstPixelView(const BasicImage<FormatT, TraitsT>& image);

        explicit BasicConstPixelView(const BasicConstImage<FormatT, TraitsT>& image);

        template <typename OtherFormatT, typename OtherTraitsT>
        explicit BasicConstPixelView(const BasicImage<OtherFormatT, OtherTraitsT>& image);

        template <typename OtherFormatT, typename OtherTraitsT>
        explicit BasicConstPixelView(const BasicConstImage<OtherFormatT, OtherTraitsT>& image);

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
