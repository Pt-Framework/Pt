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

#ifndef PT_GFX_BASIC_VIEW_H
#define PT_GFX_BASIC_VIEW_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ViewBase.h>
#include <Pt/Gfx/ImageTraits.h>
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
        explicit BasicView(const Format& format);

        template <typename F, typename T>
        explicit BasicView(BasicView<F, T>& view);

        template <typename F, typename T>
        BasicView(BasicView<F, T>& view, Int x, Int y, Int w, Int h);

        template <typename F, typename T>
        explicit BasicView(BasicImage<F, T>& image);

        template <typename F, typename T>
        BasicView(BasicImage<F, T>& image, Int x, Int y, Int w, Int h);

        virtual ~BasicView()
        { }

        Pt::uint8_t* data()
        { return _data; }

        const Pt::uint8_t* data() const
        { return _data; }

        const Format& format() const
        { return *_format; }

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
        explicit BasicConstView(const Format& format);

        template <typename F, typename T>
        explicit BasicConstView(const BasicView<F, T>& view);

        template <typename F, typename T>
        BasicConstView(const BasicView<F, T>& view, Int x, Int y, Int w, Int h);

        template <typename F, typename T>
        explicit BasicConstView(const BasicConstView<F, T>& view);

        template <typename F, typename T>
        BasicConstView(const BasicConstView<F, T>& view, Int x, Int y, Int w, Int h);

        template <typename F, typename T>
        explicit BasicConstView(const BasicImage<F, T>& image);
        
        template <typename F, typename T>
        BasicConstView(const BasicImage<F, T>& image, Int x, Int y, Int w, Int h);

        template <typename F, typename T>
        explicit BasicConstView(const BasicConstImage<F, T>& image);

        template <typename F, typename T>
        BasicConstView(const BasicConstImage<F, T>& image, Int x, Int y, Int w, Int h);

        virtual ~BasicConstView()
        { }

        const Pt::uint8_t* data() const
        { return _data; }

        const Format& format() const
        { return *_format; }

    private:
        const Pt::uint8_t*  _data;
        const Format*       _format;
};


template <typename T>
BasicConstView<typename T::Format, typename T::Traits> constView(const T& source)
{ 
    return BasicConstView<typename T::Format, typename T::Traits>(source); 
}


template <typename T>
BasicView<typename T::Format, typename T::Traits> view(T& source)
{ 
    return BasicView<typename T::Format, typename T::Traits>(source); 
}


template <typename From, typename To>
void copyView(const From& from, To& to);

} // namespace

} // namespace

#endif

#include <Pt/Gfx/BasicView.hpp>
