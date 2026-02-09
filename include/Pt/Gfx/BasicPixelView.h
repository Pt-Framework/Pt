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

namespace Pt {

namespace Gfx {

template <typename FormatT>
class BasicImage;

template <typename FormatT>
class BasicConstImage;

template <typename FormatT, typename PixelT>
class BasicConstPixelIterator;

template <typename FormatT,typename PixelT, typename ConstPixelT>
class BasicConstPixelView;


template <typename FormatT, typename PixelT = typename FormatT::Pixel>
class BasicPixelIterator
{
    template <typename F, typename P>
    friend class BasicConstPixelIterator;

    public:
        typedef FormatT Format;
        typedef PixelT Pixel;
 
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

        template <typename ConstPixelT>
        bool operator!=(const BasicConstPixelIterator<Format, ConstPixelT>& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator==(const BasicPixelIterator& it) const
        { return _pixel.equals(it._pixel); }

        template <typename ConstPixelT>
        bool operator==(const BasicConstPixelIterator<Format, ConstPixelT>& it) const
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


template <typename FormatT, typename PixelT = typename FormatT::ConstPixel>
class BasicConstPixelIterator
{
    template <typename F, typename P>
    friend class BasicPixelIterator;

    public:
        typedef FormatT Format;
        typedef PixelT ConstPixel;

    public:
        BasicConstPixelIterator(const BasicConstView<Format>& view, Pt::ssize_t x, Pt::ssize_t y)
        : _pixel(view, x, y)
        { }

        BasicConstPixelIterator(const BasicView<Format>& view, Pt::ssize_t x, Pt::ssize_t y)
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

        template <typename P>
        bool operator!=(const BasicPixelIterator<Format, P>& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator==(const BasicConstPixelIterator& it) const
        { return _pixel.equals(it._pixel); }

        template <typename P>
        bool operator==(const BasicPixelIterator<Format, P>& it) const
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


template <typename FormatT, 
          typename PixelT = typename FormatT::Pixel, 
          typename ConstPixelT = typename FormatT::ConstPixel>
class BasicPixelView : public BasicView<FormatT>
{
    public:
        typedef FormatT Format;
        typedef PixelT Pixel;
        typedef ConstPixelT ConstPixel;

        typedef BasicPixelIterator<Format, Pixel> PixelIterator;
        typedef BasicConstPixelIterator<Format, ConstPixel> ConstPixelIterator;

        typedef BasicPixelIterator<Format, Pixel> Iterator;
        typedef BasicConstPixelIterator<Format, ConstPixel> ConstIterator;

    public:
        explicit BasicPixelView(const Format& format)
        : BasicView<FormatT>(format)
        { }

        explicit BasicPixelView(BasicImage<FormatT>& image);

        template <typename OtherT>
        explicit BasicPixelView(BasicImage<OtherT>& image);

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
                    const BasicConstPixelView<FormatT, PixelT, ConstPixelT>& view, 
                    Pt::ssize_t fromX, Pt::ssize_t fromY, Pt::ssize_t width, Pt::ssize_t height);
};


template <typename FormatT,
          typename PixelT = typename FormatT::Pixel, 
          typename ConstPixelT = typename FormatT::ConstPixel>
class BasicConstPixelView : public BasicConstView<FormatT>
{
    public:
        typedef FormatT Format;
        typedef PixelT Pixel;
        typedef ConstPixelT ConstPixel;

        typedef BasicConstPixelIterator<FormatT, ConstPixel> ConstPixelIterator;

        typedef BasicConstPixelIterator<Format, ConstPixel> Iterator;
        typedef BasicConstPixelIterator<Format, ConstPixel> ConstIterator;

    public:
        explicit BasicConstPixelView(const Format& format)
        : BasicConstView<FormatT>(format)
        { }

        explicit BasicConstPixelView(const BasicImage<FormatT>& image);

        explicit BasicConstPixelView(const BasicConstImage<FormatT>& image);

        ConstPixelIterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return ConstPixelIterator(*this, x, y); }

        ConstPixelIterator begin()
        { return ConstPixelIterator(*this, 0, 0); }

        ConstPixelIterator end()
        { return ConstPixelIterator(*this, 0, this->height()); }
};

} // namespace

} // namespace

#include <Pt/Gfx/BasicPixelView.hpp>

#endif
