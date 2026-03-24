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

#ifndef PT_GFX_IMAGE_HPP
#define PT_GFX_IMAGE_HPP

#include <Pt/Gfx/Image.h>
#include <Pt/TypeTraits.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// FormatPtr
///////////////////////////////////////////////////////////////////////

template <typename FormatT, typename TraitsT>
inline FormatPtr<FormatT, TraitsT>::FormatPtr(const FormatT& format)
: _ownedFormat( TraitsT::clone(format) )
{ }

template <typename FormatT, typename TraitsT>
inline const FormatT& FormatPtr<FormatT, TraitsT>::getFormat() const
{ return *_ownedFormat; }

template <typename FormatT, typename TraitsT>
inline void FormatPtr<FormatT, TraitsT>::resetFormat(const FormatT& format)
{
    _ownedFormat = TraitsT::clone(format);
}

///////////////////////////////////////////////////////////////////////
// BasicImage
///////////////////////////////////////////////////////////////////////

template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>::BasicImage(const Format& format)
: FormatPtr<FormatT, TraitsT>(format)
, BasicImageView<FormatT, TraitsT>( this->getFormat() )
{ }

template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>::BasicImage(
    Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding, const Format& format)
: FormatPtr<FormatT, TraitsT>(format)
, BasicImageView<FormatT, TraitsT>( this->getFormat() )
{
    _buffer.resize( Traits::imageSize(this->format(), width, height, padding) );
    
    this->setData( _buffer.empty() ? nullptr : _buffer.data() );
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding,
                        padding);
}

template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>::BasicImage(
    Pt::ssize_t width, Pt::ssize_t height, const Format& format)
: FormatPtr<FormatT, TraitsT>(format)
, BasicImageView<FormatT, TraitsT>( this->getFormat() )
{
    _buffer.resize( Traits::imageSize(this->format(), width, height, 0) );
    
    this->setData( _buffer.empty() ? nullptr : _buffer.data() );
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()), 0);
}

template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>::BasicImage(
    Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
    Pt::ssize_t padding, const Format& format)
: FormatPtr<FormatT, TraitsT>(format)
, BasicImageView<FormatT, TraitsT>( this->getFormat() )
{
    this->setData(data);
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding,
                        padding);
}

template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>::BasicImage(
    Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height, const Format& format)
: FormatPtr<FormatT, TraitsT>(format)
, BasicImageView<FormatT, TraitsT>( this->getFormat() )
{
    this->setData(data);
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()), 0);
}

template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>::BasicImage(const BasicImage& image)
: FormatPtr<FormatT, TraitsT>( image.format() )
, BasicImageView<FormatT, TraitsT>( this->getFormat() )
, _buffer(image._buffer)
{
    this->setData( _buffer.empty() ? nullptr : _buffer.data() );
    this->setDimensions(image.width(), image.height(),
                        image.stride(), image.padding());
}

template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>::~BasicImage()
{ }

template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>&
BasicImage<FormatT, TraitsT>::operator=(const BasicImage& image)
{ 
    _buffer = image._buffer;

    this->resetFormat( image.format() );

    this->setFormat( this->getFormat() );
    this->setData( _buffer.empty() ? nullptr : _buffer.data() );
    this->setDimensions(image.width(), image.height(),
                        image.stride(), image.padding());
    return *this;
}

template <typename FormatT, typename TraitsT>
inline void BasicImage<FormatT, TraitsT>::reset(
    Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding)
{ 
    _buffer.resize( Traits::imageSize(this->format(), width, height, padding) );

    this->setData( _buffer.empty() ? nullptr : _buffer.data() );
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding,
                        padding);
}

template <typename FormatT, typename TraitsT>
inline void BasicImage<FormatT, TraitsT>::reset(
    Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding)
{
    _buffer.clear();

    this->setData(data);
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding,
                        padding);
}

template <typename FormatT, typename TraitsT>
inline void BasicImage<FormatT, TraitsT>::reset(
    const Format& format, Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding)
{ 
    _buffer.resize( Traits::imageSize(format, width, height, padding) );

    this->resetFormat(format);

    this->setFormat( this->getFormat() );
    this->setData( _buffer.empty() ? nullptr : _buffer.data() );
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding,
                        padding);
}

template <typename FormatT, typename TraitsT>
inline void BasicImage<FormatT, TraitsT>::reset(
    const Format& format, Pt::uint8_t* data,
    Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding)
{
    _buffer.clear();

    this->resetFormat(format);

    this->setFormat( this->getFormat() );
    this->setData(data);
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding,
                        padding);
}

template <typename FormatT, typename TraitsT>
inline void BasicImage<FormatT, TraitsT>::clear()
{
    _buffer.clear();

    this->setData(nullptr);
    this->setDimensions(0, 0, 0, 0);
}

template <typename FormatT, typename TraitsT>
inline Pt::ssize_t BasicImage<FormatT, TraitsT>::pixelStride() const
{ 
    return Traits::pixelStride(this->format()); 
}

template <typename FormatT, typename TraitsT>
inline std::size_t BasicImage<FormatT, TraitsT>::size(
    Pt::ssize_t width, Pt::ssize_t height, std::size_t padding) const
{
    return Traits::imageSize(this->format(), width, height, padding);
}

///////////////////////////////////////////////////////////////////////
// BasicConstImage
///////////////////////////////////////////////////////////////////////

template <typename FormatT, typename TraitsT>
inline BasicConstImage<FormatT, TraitsT>::BasicConstImage(const Format& format)
: FormatPtr<FormatT, TraitsT>(format)
, BasicConstImageView<FormatT, TraitsT>( this->getFormat() )
{ }

template <typename FormatT, typename TraitsT>
inline BasicConstImage<FormatT, TraitsT>::BasicConstImage(
    const Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
    Pt::ssize_t padding, const Format& format)
: FormatPtr<FormatT, TraitsT>(format)
, BasicConstImageView<FormatT, TraitsT>( this->getFormat() )
{
    this->setData(data);
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding,
                        padding);
}

template <typename FormatT, typename TraitsT>
inline BasicConstImage<FormatT, TraitsT>::BasicConstImage(
    const Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
    const Format& format)
: FormatPtr<FormatT, TraitsT>(format)
, BasicConstImageView<FormatT, TraitsT>( this->getFormat() )
{
    this->setData(data);
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()), 0);
}

template <typename FormatT, typename TraitsT>
inline BasicConstImage<FormatT, TraitsT>::BasicConstImage(
    const BasicImage<FormatT, TraitsT>& image)
: FormatPtr<FormatT, TraitsT>(image.format())
, BasicConstImageView<FormatT, TraitsT>( this->getFormat() )
{
    this->setData(image.data());
    this->setDimensions(image.width(), image.height(),
                        image.stride(), image.padding());
}

template <typename FormatT, typename TraitsT>
inline BasicConstImage<FormatT, TraitsT>::BasicConstImage(const BasicConstImage& image)
: FormatPtr<FormatT, TraitsT>(image.format())
, BasicConstImageView<FormatT, TraitsT>( this->getFormat() )
{
    this->setData(image.data());
    this->setDimensions(image.width(), image.height(),
                        image.stride(), image.padding());
}

template <typename FormatT, typename TraitsT>
inline BasicConstImage<FormatT, TraitsT>::~BasicConstImage()
{ }

template <typename FormatT, typename TraitsT>
inline void BasicConstImage<FormatT, TraitsT>::reset(const BasicConstImage& image)
{ 
    this->resetFormat( image.format() );
    
    this->setFormat( this->getFormat() );
    this->setData(image.data());
    this->setDimensions(image.width(), image.height(),
                        image.stride(), image.padding());
}

template <typename FormatT, typename TraitsT>
inline void BasicConstImage<FormatT, TraitsT>::reset(
    const BasicImage<FormatT, TraitsT>& image)
{ 
    this->resetFormat( image.format() );
    this->setFormat( this->getFormat() );
    this->setData( image.data() );
    this->setDimensions(image.width(), image.height(),
                        image.stride(), image.padding());
}

template <typename FormatT, typename TraitsT>
inline void BasicConstImage<FormatT, TraitsT>::reset(
    const Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height,
    Pt::ssize_t padding)
{
    this->setData(data);
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding,
                        padding);
}

template <typename FormatT, typename TraitsT>
inline void BasicConstImage<FormatT, TraitsT>::reset(
    const Format& format, const Pt::uint8_t* data,
    Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding)
{
    this->resetFormat(format);
    
    this->setFormat( this->getFormat() );
    this->setData(data);
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding,
                        padding);
}

template <typename FormatT, typename TraitsT>
inline void BasicConstImage<FormatT, TraitsT>::clear()
{
    this->setData(nullptr);
    this->setDimensions(0, 0, 0, 0);
}

template <typename FormatT, typename TraitsT>
inline Pt::ssize_t BasicConstImage<FormatT, TraitsT>::pixelStride() const
{ 
    return Traits::pixelStride(this->format()); 
}

template <typename FormatT, typename TraitsT>
inline std::size_t BasicConstImage<FormatT, TraitsT>::size(
    Pt::ssize_t width, Pt::ssize_t height, std::size_t padding) const
{
    return Traits::imageSize(this->format(), width, height, padding);
}

} // namespace

} // namespace

#endif
