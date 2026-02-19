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

#ifndef PT_GFX_BASIC_VIEW_HPP
#define PT_GFX_BASIC_VIEW_HPP

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/BasicImage.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// BasicView
///////////////////////////////////////////////////////////////////////

template <typename FormatT>
inline BasicView<FormatT>::BasicView(const Format& format)
: ViewBase()
, _data(0)
, _format(&format)
{ 
}


template <typename FormatT>
template <typename F, typename T>
inline BasicView<FormatT>::BasicView(BasicImage<F, T>& image)
: ViewBase(image.width(), image.height(), 
           image.stride(), image.padding())
, _data( image.data() )
, _format( &image.format() )
{
}


template <typename FormatT>
inline void BasicView<FormatT>::reset()
{
    _data = 0;
    ViewBase::reset(0, 0, 0, 0);
}


template <typename FormatT>
template <typename F, typename T>
inline void BasicView<FormatT>::reset(BasicImage<F, T>& image)
{
    _data = image.data();
    _format = &image.format();
    ViewBase::reset(image.width(), image.height(),
                    image.stride(), image.padding() );
}

///////////////////////////////////////////////////////////////////////
// BasicConstView
///////////////////////////////////////////////////////////////////////

template <typename FormatT>
inline BasicConstView<FormatT>::BasicConstView(const Format& format)
: ViewBase()
, _data(0)
, _format(&format)
{
}


template <typename FormatT>
template <typename F>
inline BasicConstView<FormatT>::BasicConstView(const BasicView<F>& view)
: ViewBase(view.width(), view.height(), 
           view.stride(), view.padding())
, _data( view.data() )
, _format( &view.format() )
{
}


template <typename FormatT>
template <typename F, typename T>
inline BasicConstView<FormatT>::BasicConstView(const BasicImage<F, T>& image)
: ViewBase(image.width(), image.height(), 
           image.stride(), image.padding())
, _data( image.data() )
, _format( &image.format() )
{
}


template <typename FormatT>
template <typename F, typename T>
inline BasicConstView<FormatT>::BasicConstView(const BasicConstImage<F, T>& image)
: ViewBase(image.width(), image.height(), 
           image.stride(), image.padding())
, _data( image.data() )
, _format( &image.format() )
{
}


template <typename FormatT>
template <typename F, typename T>
inline void BasicConstView<FormatT>::reset(const BasicImage<F, T>& image)
{
    _data = image.data();
    _format = &image.format();
    ViewBase::reset(image.width(), image.height(),
                    image.stride(), image.padding() );
}


template <typename FormatT>
template <typename F, typename T>
inline void BasicConstView<FormatT>::reset(const BasicConstImage<F, T>& image)
{
    _data = image.data();
    _format = &image.format();
    ViewBase::reset(image.width(), image.height(),
                    image.stride(), image.padding() );
}

} // namespace

} // namespace

#endif
