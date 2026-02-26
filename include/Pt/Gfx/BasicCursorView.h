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

#ifndef PT_GFX_BASIC_CURSOR_VIEW_H
#define PT_GFX_BASIC_CURSOR_VIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/BasicView.h>
#include <Pt/Gfx/PixelTraits.h>
#include <Pt/Types.h>

#include <iterator>
#include <cstddef>

namespace Pt {

namespace Gfx {

template <typename FormatT, typename TraitsT>
class BasicCursorIterator;

template <typename FormatT, typename TraitsT>
class BasicConstCursorIterator;


template <typename FormatT, typename TraitsT>
class BasicCursorIterator
{
    template <typename F, typename T>
    friend class BasicConstCursorIterator;

    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        typedef typename Traits::PixelType Pixel;
        typedef typename Traits::ConstPixelType ConstPixel;

        typedef BasicConstCursorIterator<Format, Traits> ConstIterator;
 
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = Pixel;
        using difference_type   = std::ptrdiff_t;
        using pointer           = Pixel*;
        using reference         = Pixel&;

    public:
        template <typename Tr>
        BasicCursorIterator(BasicView<Format, Tr>& view, Pt::ssize_t x, Pt::ssize_t y)
        : _x(x)
        , _y(y)
        , _pixel(view, x, y)
        { }

        BasicCursorIterator(const BasicCursorIterator& it)
        : _x(it._x)
        , _y(it._y)
        , _pixel(it._pixel)
        { }

        BasicCursorIterator& operator=(const BasicCursorIterator& it)
        {
            _pixel.reset(it._pixel);
            _x = it._x;
            _y = it._y;
            return *this;
        }

        bool operator!=(const BasicCursorIterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator!=(const ConstIterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator==(const BasicCursorIterator& it) const
        { return _pixel.equals(it._pixel); }

        bool operator==(const ConstIterator& it) const
        { return _pixel.equals(it._pixel); }

        Pixel& operator*()
        { return _pixel; }

        Pixel* operator->()
        { return &_pixel; }

        BasicCursorIterator& operator++()
        {
            _pixel.advance();

            if( ++_x >= _pixel.view().width() )
            {
                _x = 0;
                ++_y;

                _pixel.advanceLine();
            }

            return *this;
        }

        BasicCursorIterator& operator+=(Pt::ssize_t n)
        {
            Pt::ssize_t off = _x + n;
            std::size_t dy = off / _pixel.view().width();
            std::size_t dx = off % _pixel.view().width() - _x;
            
            if(dy > 0)
            {
                _pixel.advanceLines(dy);
                _y += dy;
            }

            _pixel.advance(dx);
            _x += dx;
            
            return *this;
        }

    private:
        Pt::ssize_t _x;
        Pt::ssize_t _y;
        Pixel       _pixel;
};


template <typename FormatT, typename TraitsT>
class BasicConstCursorIterator
{
    template <typename F, typename T>
    friend class BasicCursorIterator;

    public:
        typedef FormatT Format;
        typedef TraitsT Traits;
        typedef typename Traits::PixelType Pixel;
        typedef typename Traits::ConstPixelType ConstPixel;

        typedef BasicCursorIterator<Format, Traits> Iterator;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = ConstPixel;
        using difference_type   = std::ptrdiff_t;
        using pointer           = ConstPixel*;
        using reference         = ConstPixel&;

    public:
        template <typename Tr>
        BasicConstCursorIterator(const BasicConstView<Format, Tr>& view, 
                                Pt::ssize_t x, Pt::ssize_t y)
        : _x(x)
        , _y(y)
        , _pixel(view, x, y)
        { }

        template <typename Tr>
        BasicConstCursorIterator(const BasicView<Format, Tr>& view, 
                                Pt::ssize_t x, Pt::ssize_t y)
        : _x(x)
        , _y(y)
        , _pixel(view, x, y)
        { }

        BasicConstCursorIterator(const BasicConstCursorIterator& it)
        : _x(it._x)
        , _y(it._y)
        , _pixel(it._pixel)
        { }

        BasicConstCursorIterator& operator=(const BasicConstCursorIterator& it)
        {
            _pixel.reset(it._pixel);
            _x = it._x;
            _y = it._y;
            return *this;
        }

        bool operator!=(const BasicConstCursorIterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator!=(const Iterator& it) const
        { return ! _pixel.equals(it._pixel); }

        bool operator==(const BasicConstCursorIterator& it) const
        { return _pixel.equals(it._pixel); }

        bool operator==(const Iterator& it) const
        { return _pixel.equals(it._pixel); }

        const ConstPixel& operator*() const
        { return _pixel; }

        const ConstPixel* operator->() const
        { return &_pixel; }

        BasicConstCursorIterator& operator++()
        {
            _pixel.advance();
            
            if( ++_x >= _pixel.view().width() )
            {
                _x = 0;
                ++_y;

                _pixel.advanceLine();
            }

            return *this;
        }

        BasicConstCursorIterator& operator+=(Pt::ssize_t n)
        {
            Pt::ssize_t off = _x + n;
            std::size_t dy = off / _pixel.view().width();
            std::size_t dx = off % _pixel.view().width() - _x;
            
            if(dy > 0)
            {
                _pixel.advanceLines(dy);
                _y += dy;
            }

            _pixel.advance(dx);
            _x += dx;
            
            return *this;
        }

    private:
        Pt::ssize_t _x;
        Pt::ssize_t _y;
        ConstPixel _pixel;
};


template <typename FormatT, typename TraitsT>
class BasicCursorView : public BasicView<FormatT, TraitsT>
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;

        typedef typename Traits::PixelType Pixel;
        typedef typename Traits::ConstPixelType ConstPixel;

        typedef PixelTraits<Pixel> PixelTraits;

        typedef BasicCursorIterator<Format, PixelTraits> Iterator;
        typedef BasicConstCursorIterator<Format, PixelTraits> ConstIterator;

    public:
        explicit BasicCursorView(const Format& format = FormatT::get())
        : BasicView<FormatT>(format)
        { }

        template <typename OtherFormatT, typename OtherTraitsT>
        explicit BasicCursorView(BasicView<OtherFormatT, OtherTraitsT>& view);

        template <typename OtherFormatT, typename OtherTraitsT>
        BasicCursorView(BasicView<OtherFormatT, OtherTraitsT>& view,
                       Int x, Int y, Int w, Int h);

        template <typename OtherFormatT, typename OtherTraitsT>
        explicit BasicCursorView(BasicImage<OtherFormatT, OtherTraitsT>& image);

        template <typename OtherFormatT, typename OtherTraitsT>
        BasicCursorView(BasicImage<OtherFormatT, OtherTraitsT>& image,
                       Int x, Int y, Int w, Int h);

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
class BasicConstCursorView : public BasicConstView<FormatT, TraitsT>
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;

        typedef typename Traits::PixelType Pixel;
        typedef typename Traits::ConstPixelType ConstPixel;

        typedef PixelTraits<Pixel> PixelTraits;

        typedef BasicConstCursorIterator<Format, PixelTraits> Iterator;

    public:
        explicit BasicConstCursorView(const Format& format)
        : BasicConstView<FormatT, TraitsT>(format)
        { }

        template <typename OtherFormatT, typename OtherTraitsT>
        explicit BasicConstCursorView(const BasicView<OtherFormatT, OtherTraitsT>& view);

        template <typename OtherFormatT, typename OtherTraitsT>
        BasicConstCursorView(const BasicView<OtherFormatT, OtherTraitsT>& view,
                            Int x, Int y, Int w, Int h);

        template <typename OtherFormatT, typename OtherTraitsT>
        explicit BasicConstCursorView(const BasicConstView<OtherFormatT, OtherTraitsT>& view);

        template <typename OtherFormatT, typename OtherTraitsT>
        BasicConstCursorView(const BasicConstView<OtherFormatT, OtherTraitsT>& view,
                            Int x, Int y, Int w, Int h);

        template <typename OtherFormatT, typename OtherTraitsT>
        explicit BasicConstCursorView(const BasicImage<OtherFormatT, OtherTraitsT>& image);

        template <typename OtherFormatT, typename OtherTraitsT>
        BasicConstCursorView(const BasicImage<OtherFormatT, OtherTraitsT>& image,
                            Int x, Int y, Int w, Int h);

        template <typename OtherFormatT, typename OtherTraitsT>
        explicit BasicConstCursorView(const BasicConstImage<OtherFormatT, OtherTraitsT>& image);

        template <typename OtherFormatT, typename OtherTraitsT>
        BasicConstCursorView(const BasicConstImage<OtherFormatT, OtherTraitsT>& image,
                            Int x, Int y, Int w, Int h);

        Iterator pixel(Pt::ssize_t x, Pt::ssize_t y)
        { return Iterator(*this, x, y); }

        Iterator begin()
        { return Iterator(*this, 0, 0); }

        Iterator end()
        { return Iterator(*this, 0, this->height()); }
};

} // namespace

} // namespace

#endif

#include <Pt/Gfx/BasicCursorView.hpp>
