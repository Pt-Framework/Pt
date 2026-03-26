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

#ifndef PT_GFX_VIEW_H
#define PT_GFX_VIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ViewBase.h>
#include <Pt/Gfx/ImageTraits.h>
#include <Pt/TypeTraits.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

template <typename FormatT, typename TraitsT>
class BasicView : public ViewBase
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;

    public:
        explicit BasicView( const Format& format = FormatT::get() );

        BasicView(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
                  Pt::ssize_t padding, const Format& format);

        template <typename T>
        explicit BasicView(T& source);

        template <typename T>
        BasicView(T& source, Int x, Int y, Int w, Int h);

        BasicView& operator=(const BasicView&) = default;

        ~BasicView()
        { }

        Pt::uint8_t* data()
        { return _data; }

        const Pt::uint8_t* data() const
        { return _data; }

        const Format& format() const
        { return *_format; }

        Pt::ssize_t pixelStride() const
        { return TraitsT::pixelStride(*_format); }

        Pt::ssize_t padding() const
        { return stride() - width() * pixelStride(); }

    protected:
        void setData(Pt::uint8_t* data)
        { _data = data; }

        void setFormat(const Format& format)
        { _format = &format; }

    private:
        Pt::uint8_t*   _data;
        const Format*  _format;
};


template <typename FormatT, typename TraitsT>
class BasicConstView : public ViewBase
{
    public:
        typedef FormatT Format;
        typedef TraitsT Traits;

    public:
        explicit BasicConstView( const Format& format = FormatT::get() );

        BasicConstView(const Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
                       Pt::ssize_t padding, const Format& format);

        template <typename T>
        explicit BasicConstView(const T& source);

        template <typename T>
        BasicConstView(const T& source, Int x, Int y, Int w, Int h);

        BasicConstView& operator=(const BasicConstView&) = default;

        ~BasicConstView()
        { }

        const Pt::uint8_t* data() const
        { return _data; }

        const Format& format() const
        { return *_format; }

        Pt::ssize_t pixelStride() const
        { return TraitsT::pixelStride(*_format); }

        Pt::ssize_t padding() const
        { return stride() - width() * pixelStride(); }

    protected:
        void setData(const Pt::uint8_t* data)
        { _data = data; }

        void setFormat(const Format& format)
        { _format = &format; }

    private:
        const Pt::uint8_t*  _data;
        const Format*       _format;
};


template <typename T>
BasicView<typename T::Format, typename T::Traits> view(T& source)
{ 
    return BasicView<typename T::Format, typename T::Traits>(source); 
}


template <typename T>
BasicConstView<typename T::Format, typename T::Traits> view(const T& source)
{ 
    return BasicConstView<typename T::Format, typename T::Traits>(source); 
}


template <typename T>
BasicView<typename T::Format, typename T::Traits> view(T& source, Int x, Int y, Int w, Int h)
{ 
    return BasicView<typename T::Format, typename T::Traits>(source, x, y, w, h); 
}


template <typename T>
BasicConstView<typename T::Format, typename T::Traits> view(const T& source, Int x, Int y, Int w, Int h)
{ 
    return BasicConstView<typename T::Format, typename T::Traits>(source, x, y, w, h); 
}


template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
BasicView<FormatT, TraitsT> view(Pt::uint8_t* data, Pt::ssize_t width,
                                 Pt::ssize_t height, Pt::ssize_t padding = 0)
{
    return BasicView<FormatT, TraitsT>(data, width, height, padding, FormatT::get());
}


template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
BasicConstView<FormatT, TraitsT> view(const Pt::uint8_t* data, Pt::ssize_t width,
                                      Pt::ssize_t height, Pt::ssize_t padding = 0)
{
    return BasicConstView<FormatT, TraitsT>(data, width, height, padding, FormatT::get());
}


/** @brief Copies the pixels of a view to another one.
 */
template <typename From, typename To>
void copyView(const From& from, To& to)
{
    ConstSpan<typename From::Format, 
              typename From::Traits> fromSpan( from, 0, 0, from.width() );

    Span<typename To::Format, 
         typename To::Traits> toSpan( to, 0, 0, to.width() );

    for(Pt::ssize_t y = 0; y < from.height(); ++y)
    {
        copySpan(fromSpan, toSpan.front());
        fromSpan.advanceLines(1);
        toSpan.advanceLines(1);
    }
}

} // namespace

} // namespace

#endif

#include <Pt/Gfx/View.hpp>
