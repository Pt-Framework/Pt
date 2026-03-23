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

#include <Pt/Gfx/BasicView.h>
#include <Pt/Gfx/Span.h>
#include <Pt/Gfx/Algorithm.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// BasicView
///////////////////////////////////////////////////////////////////////

template <typename FormatT, typename TraitsT>
inline BasicView<FormatT, TraitsT>::BasicView(const Format& format)
: ViewBase()
, _data(0)
, _format(&format)
{ 
}


template <typename FormatT, typename TraitsT>
template <typename F, typename T>
inline BasicView<FormatT, TraitsT>::BasicView(BasicView<F, T>& view)
: ViewBase(view)
, _data( view.data() )
, _format( &view.format() )
{
}


template <typename FormatT, typename TraitsT>
template <typename F, typename T>
inline BasicView<FormatT, TraitsT>::BasicView(BasicView<F, T>& view, 
                                              Int x, Int y, Int w, Int h)
: ViewBase( view.xpos() + x, view.ypos() + y, w, h, view.stride(), view.padding() )
, _data( view.data() )
, _format( &view.format() )
{
}

///////////////////////////////////////////////////////////////////////
// BasicConstView
///////////////////////////////////////////////////////////////////////

template <typename FormatT, typename TraitsT>
inline BasicConstView<FormatT, TraitsT>::BasicConstView(const Format& format)
: ViewBase()
, _data(0)
, _format(&format)
{
}


template <typename FormatT, typename TraitsT>
template <typename F, typename T>
inline BasicConstView<FormatT, TraitsT>::BasicConstView(const BasicView<F, T>& view)
: ViewBase(view.width(), view.height(), 
           view.stride(), view.padding())
, _data( view.data() )
, _format( &view.format() )
{
}


template <typename FormatT, typename TraitsT>
template <typename F, typename T>
inline BasicConstView<FormatT, TraitsT>::BasicConstView(const BasicView<F, T>& view,
                                                        Int x, Int y, Int w, Int h)
: ViewBase( view.xpos() + x, view.ypos() + y, w, h, view.stride(), view.padding() )
, _data( view.data() )
, _format( &view.format() )
{
}


template <typename FormatT, typename TraitsT>
template <typename F, typename T>
inline BasicConstView<FormatT, TraitsT>::BasicConstView(const BasicConstView<F, T>& view)
: ViewBase(view.width(), view.height(), 
           view.stride(), view.padding())
, _data( view.data() )
, _format( &view.format() )
{
}


template <typename FormatT, typename TraitsT>
template <typename F, typename T>
inline BasicConstView<FormatT, TraitsT>::BasicConstView(const BasicConstView<F, T>& view,
                                                        Int x, Int y, Int w, Int h)
: ViewBase( view.xpos() + x, view.ypos() + y, w, h, view.stride(), view.padding() )
, _data( view.data() )
, _format( &view.format() )
{
}

} // namespace

} // namespace

#endif
