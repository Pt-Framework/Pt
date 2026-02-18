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
#include <Pt/Gfx/BasicPixelSpan.h>
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
inline BasicView<FormatT>::BasicView(BasicImage<FormatT>& image)
: ViewBase(image.width(), image.height(), 
           image.stride(), image.padding())
, _data( image.data() )
, _format( &image.format() )
{
}


template <typename FormatT>
template <typename OtherFormatT>
inline BasicView<FormatT>::BasicView(BasicImage<OtherFormatT>& image)
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
inline void BasicView<FormatT>::reset(BasicImage<FormatT>& image)
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
inline BasicConstView<FormatT>::BasicConstView(const BasicView<FormatT>& view)
: ViewBase(view.width(), view.height(), 
           view.stride(), view.padding())
, _data( view.data() )
, _format( &view.format() )
{
}


template <typename FormatT>
inline BasicConstView<FormatT>::BasicConstView(const BasicImage<FormatT>& image)
: ViewBase(image.width(), image.height(), 
           image.stride(), image.padding())
, _data( image.data() )
, _format( &image.format() )
{
}


template <typename FormatT>
inline BasicConstView<FormatT>::BasicConstView(const BasicConstImage<FormatT>& image)
: ViewBase(image.width(), image.height(), 
           image.stride(), image.padding())
, _data( image.data() )
, _format( &image.format() )
{
}


template <typename FormatT>
template <typename OtherFormatT>
inline BasicConstView<FormatT>::BasicConstView(const BasicImage<OtherFormatT>& image)
: ViewBase(image.width(), image.height(), 
           image.stride(), image.padding())
, _data( image.data() )
, _format( &image.format() )
{
}


template <typename FormatT>
template <typename OtherFormatT>
inline BasicConstView<FormatT>::BasicConstView(const BasicConstImage<OtherFormatT>& image)
: ViewBase(image.width(), image.height(), 
           image.stride(), image.padding())
, _data( image.data() )
, _format( &image.format() )
{
}


template <typename FormatT>
inline void BasicConstView<FormatT>::reset(const BasicImage<FormatT>& image)
{
    _data = image.data();
    _format = &image.format();
    ViewBase::reset(image.width(), image.height(),
                    image.stride(), image.padding() );
}

template <typename FormatT>
inline void BasicConstView<FormatT>::reset(const BasicConstImage<FormatT>& image)
{
    _data = image.data();
    _format = &image.format();
    ViewBase::reset(image.width(), image.height(),
                    image.stride(), image.padding() );
}

///////////////////////////////////////////////////////////////////////
// Copy
///////////////////////////////////////////////////////////////////////

template <typename FormatT1, typename FormatT2>
void copy(const BasicConstView<FormatT1>& fromView, BasicView<FormatT2>& toView)
{
    //
    // TODO: use LineView and LineIterator
    //


    BasicConstPixelSpan<FormatT1> from(fromView, 0, 0, fromView.width());
    BasicPixelSpan<FormatT2> to(toView, 0, 0, toView.width());

    for( Pt::ssize_t y = 0; y < fromView.height(); ++y )
    {
        copy(from, to.begin());

        from.advance( fromView.width() );
        to.advance( toView.width() );
    }
}

} // namespace

} // namespace

#endif
