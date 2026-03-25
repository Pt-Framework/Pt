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

#ifndef PT_GFX_LINE_VIEW_HPP
#define PT_GFX_LINE_VIEW_HPP

#include <Pt/Gfx/LineView.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// BasicLineView
///////////////////////////////////////////////////////////////////////

template <typename FormatT, typename TraitsT>
inline BasicLineView<FormatT, TraitsT>::BasicLineView(const Format& format)
: BasicImageView<FormatT, TraitsT>(format)
{ }


template <typename FormatT, typename TraitsT>
inline BasicLineView<FormatT, TraitsT>::BasicLineView(Pt::uint8_t* data, Pt::ssize_t width,
                                                      Pt::ssize_t height, Pt::ssize_t padding,
                                                      const Format& format)
: BasicImageView<FormatT, TraitsT>(data, width, height, padding, format)
{ }


template <typename FormatT, typename TraitsT>
template <typename F, typename T>
inline BasicLineView<FormatT, TraitsT>::BasicLineView(BasicImageView<F, T>& view)
: BasicImageView<FormatT, TraitsT>(view)
{ }


template <typename FormatT, typename TraitsT>
template <typename F, typename T>
inline BasicLineView<FormatT, TraitsT>::BasicLineView(BasicImageView<F, T>& view,
                                                      Int x, Int y, Int w, Int h)
: BasicImageView<FormatT, TraitsT>(view, x, y, w, h)
{ }

///////////////////////////////////////////////////////////////////////
// BasicConstLineView
///////////////////////////////////////////////////////////////////////

template <typename FormatT, typename TraitsT>
inline BasicConstLineView<FormatT, TraitsT>::BasicConstLineView(const Format& format)
: BasicConstImageView<FormatT, TraitsT>(format)
{ }


template <typename FormatT, typename TraitsT>
inline BasicConstLineView<FormatT, TraitsT>::BasicConstLineView(const Pt::uint8_t* data,
                                                                Pt::ssize_t width,
                                                                Pt::ssize_t height,
                                                                Pt::ssize_t padding,
                                                                const Format& format)
: BasicConstImageView<FormatT, TraitsT>(data, width, height, padding, format)
{ }


template <typename FormatT, typename TraitsT>
template <typename F, typename T>
inline BasicConstLineView<FormatT, TraitsT>::BasicConstLineView(const BasicImageView<F, T>& view)
: BasicConstImageView<FormatT, TraitsT>(view)
{ }


template <typename FormatT, typename TraitsT>
template <typename F, typename T>
inline BasicConstLineView<FormatT, TraitsT>::BasicConstLineView(const BasicImageView<F, T>& view,
                                                                Int x, Int y, Int w, Int h)
: BasicConstImageView<FormatT, TraitsT>(view, x, y, w, h)
{ }


template <typename FormatT, typename TraitsT>
template <typename F, typename T>
inline BasicConstLineView<FormatT, TraitsT>::BasicConstLineView(const BasicConstImageView<F, T>& view)
: BasicConstImageView<FormatT, TraitsT>(view)
{ }


template <typename FormatT, typename TraitsT>
template <typename F, typename T>
inline BasicConstLineView<FormatT, TraitsT>::BasicConstLineView(const BasicConstImageView<F, T>& view,
                                                                Int x, Int y, Int w, Int h)
: BasicConstImageView<FormatT, TraitsT>(view, x, y, w, h)
{ }


} // namespace

} // namespace

#endif
