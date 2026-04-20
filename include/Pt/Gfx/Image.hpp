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
// BasicImage
///////////////////////////////////////////////////////////////////////

template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>::BasicImage(const Format& format)
: ViewBase()
, _format( TraitsT::clone(format) )
, _data(nullptr)
{
}


template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>::BasicImage(Pt::ssize_t width, Pt::ssize_t height, 
                                                Pt::ssize_t padding, const Format& format)
: ViewBase(width, height, width * Traits::pixelStride(format) + padding)
, _format( TraitsT::clone(format) )
, _data(nullptr)
{
    _buffer.resize( Traits::imageSize(this->format(), width, height, padding) );
    _data = _buffer.empty() ? nullptr : _buffer.data();
}


template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>::BasicImage(Pt::ssize_t width, Pt::ssize_t height, 
                                                const Format& format)
: ViewBase(width, height, width * Traits::pixelStride(format))
, _format( TraitsT::clone(format) )
, _data(nullptr)
{
    _buffer.resize( Traits::imageSize(this->format(), width, height, 0) );
    _data = _buffer.empty() ? nullptr : _buffer.data();
}


template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>::BasicImage(Pt::uint8_t* data, Pt::ssize_t width,
                                                Pt::ssize_t height,Pt::ssize_t padding,
                                                const Format& format)
: ViewBase(width, height, width * Traits::pixelStride(format) + padding)
, _format( TraitsT::clone(format) )
, _data(data)
{
}


template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>::BasicImage(Pt::uint8_t* data, Pt::ssize_t width,
                                                Pt::ssize_t height, const Format& format)
: ViewBase(width, height, width * Traits::pixelStride(format))
, _format( TraitsT::clone(format) )
, _data(data)
{
}


template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>::BasicImage(const BasicImage& image)
: ViewBase(image.width(), image.height(), image.stride())
, _format( TraitsT::clone(image.format()) )
, _data(nullptr)
{
    if( image.data() )
    {
        _buffer.assign(image.data(), image.data() + image.size());
        _data = _buffer.data();
    }
}


template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>::~BasicImage()
{
}


template <typename FormatT, typename TraitsT>
inline BasicImage<FormatT, TraitsT>&
BasicImage<FormatT, TraitsT>::operator=(const BasicImage& image)
{
    _format = TraitsT::clone(image.format());

    if( image.data() )
    {
        _buffer.assign(image.data(), image.data() + image.size());
        _data = _buffer.data();
    }
    else
    {
        _buffer.clear();
        _data = nullptr;
    }

    this->setDimensions(image.width(), image.height(), image.stride());
    return *this;
}


template <typename FormatT, typename TraitsT>
inline void BasicImage<FormatT, TraitsT>::reset(Pt::ssize_t width, Pt::ssize_t height,
                                                Pt::ssize_t padding, const Format& format)
{
    _buffer.resize( Traits::imageSize(format, width, height, padding) );

    _format = TraitsT::clone(format);

    _data = _buffer.empty() ? nullptr : _buffer.data();
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding);
}


template <typename FormatT, typename TraitsT>
inline void BasicImage<FormatT, TraitsT>::reset(Pt::ssize_t width, Pt::ssize_t height,
                                                const Format& format)
{
    this->reset(width, height, 0, format);
}


template <typename FormatT, typename TraitsT>
inline void BasicImage<FormatT, TraitsT>::reset(Pt::ssize_t width, Pt::ssize_t height,
                                                Pt::ssize_t padding)
{
    _buffer.resize( Traits::imageSize(this->format(), width, height, padding) );

    _data = _buffer.empty() ? nullptr : _buffer.data();
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding);
}


template <typename FormatT, typename TraitsT>
inline void BasicImage<FormatT, TraitsT>::reset(Pt::ssize_t width, Pt::ssize_t height)
{
    this->reset(width, height, 0);
}


template <typename FormatT, typename TraitsT>
inline void BasicImage<FormatT, TraitsT>::reset(Pt::uint8_t* data, Pt::ssize_t width,
                                                Pt::ssize_t height, Pt::ssize_t padding,
                                                const Format& format)
{
    _buffer.clear();

    _format = TraitsT::clone(format);

    _data = data;
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding);
}


template <typename FormatT, typename TraitsT>
inline void BasicImage<FormatT, TraitsT>::reset(Pt::uint8_t* data, Pt::ssize_t width,
                                                Pt::ssize_t height, const Format& format)
{
    this->reset(data, width, height, 0, format);
}


template <typename FormatT, typename TraitsT>
inline void BasicImage<FormatT, TraitsT>::reset(Pt::uint8_t* data, Pt::ssize_t width,
                                                Pt::ssize_t height, Pt::ssize_t padding)
{
    _buffer.clear();

    _data = data;
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding);
}


template <typename FormatT, typename TraitsT>
inline void BasicImage<FormatT, TraitsT>::reset(Pt::uint8_t* data, Pt::ssize_t width,
                                                Pt::ssize_t height)
{
    this->reset(data, width, height, 0);
}


template <typename FormatT, typename TraitsT>
inline void BasicImage<FormatT, TraitsT>::clear()
{
    _buffer.clear();

    _data = nullptr;
    this->setDimensions(0, 0, 0);
}


template <typename FormatT, typename TraitsT>
inline Pt::ssize_t BasicImage<FormatT, TraitsT>::pixelStride() const
{
    return Traits::pixelStride(this->format());
}


template <typename FormatT, typename TraitsT>
inline std::size_t BasicImage<FormatT, TraitsT>::size() const
{
    return Traits::imageSize(this->format(), this->width(), this->height(), this->padding());
}

///////////////////////////////////////////////////////////////////////
// BasicConstImage
///////////////////////////////////////////////////////////////////////

template <typename FormatT, typename TraitsT>
inline BasicConstImage<FormatT, TraitsT>::BasicConstImage(const Format& format)
: ViewBase()
, _format( TraitsT::clone(format) )
, _data(nullptr)
{
}


template <typename FormatT, typename TraitsT>
inline BasicConstImage<FormatT, TraitsT>::BasicConstImage(const Pt::uint8_t* data, Pt::ssize_t width,
                                                          Pt::ssize_t height, Pt::ssize_t padding, 
                                                          const Format& format)
: ViewBase(width, height, width * Traits::pixelStride(format) + padding)
, _format( TraitsT::clone(format) )
, _data(data)
{
}


template <typename FormatT, typename TraitsT>
inline BasicConstImage<FormatT, TraitsT>::BasicConstImage(const Pt::uint8_t* data, Pt::ssize_t width, 
                                                          Pt::ssize_t height,const Format& format)
: ViewBase(width, height, width * Traits::pixelStride(format))
, _format( TraitsT::clone(format) )
, _data(data)
{
}


template <typename FormatT, typename TraitsT>
inline BasicConstImage<FormatT, TraitsT>::BasicConstImage(const BasicImage<FormatT, TraitsT>& image)
: ViewBase(image.width(), image.height(), image.stride())
, _format( TraitsT::clone(image.format()) )
, _data(image.data())
{
}


template <typename FormatT, typename TraitsT>
inline BasicConstImage<FormatT, TraitsT>::BasicConstImage(const BasicConstImage& image)
: ViewBase(image.width(), image.height(), image.stride())
, _format( TraitsT::clone(image.format()) )
, _data(image.data())
{
}


template <typename FormatT, typename TraitsT>
inline BasicConstImage<FormatT, TraitsT>::~BasicConstImage()
{
}


template <typename FormatT, typename TraitsT>
inline void BasicConstImage<FormatT, TraitsT>::reset(const BasicConstImage& image)
{
    _format = TraitsT::clone(image.format());

    _data = image.data();
    this->setDimensions(image.width(), image.height(), image.stride());
}


template <typename FormatT, typename TraitsT>
inline void BasicConstImage<FormatT, TraitsT>::reset(const BasicImage<FormatT, TraitsT>& image)
{
    _format = TraitsT::clone(image.format());

    _data = image.data();
    this->setDimensions(image.width(), image.height(), image.stride());
}


template <typename FormatT, typename TraitsT>
inline void BasicConstImage<FormatT, TraitsT>::reset(const Pt::uint8_t* data, Pt::ssize_t width,
                                                     Pt::ssize_t height, Pt::ssize_t padding,
                                                     const Format& format)
{
    _format = TraitsT::clone(format);

    _data = data;
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding);
}


template <typename FormatT, typename TraitsT>
inline void BasicConstImage<FormatT, TraitsT>::reset(const Pt::uint8_t* data, Pt::ssize_t width,
                                                     Pt::ssize_t height, const Format& format)
{
    this->reset(data, width, height, 0, format);
}


template <typename FormatT, typename TraitsT>
inline void BasicConstImage<FormatT, TraitsT>::reset(const Pt::uint8_t* data, Pt::ssize_t width,
                                                     Pt::ssize_t height, Pt::ssize_t padding)
{
    _data = data;
    this->setDimensions(width, height,
                        width * Traits::pixelStride(this->format()) + padding);
}


template <typename FormatT, typename TraitsT>
inline void BasicConstImage<FormatT, TraitsT>::reset(const Pt::uint8_t* data, Pt::ssize_t width,
                                                     Pt::ssize_t height)
{
    this->reset(data, width, height, 0);
}


template <typename FormatT, typename TraitsT>
inline void BasicConstImage<FormatT, TraitsT>::clear()
{
    _data = nullptr;
    this->setDimensions(0, 0, 0);
}


template <typename FormatT, typename TraitsT>
inline Pt::ssize_t BasicConstImage<FormatT, TraitsT>::pixelStride() const
{
    return Traits::pixelStride(this->format());
}


template <typename FormatT, typename TraitsT>
inline std::size_t BasicConstImage<FormatT, TraitsT>::size() const
{
    return Traits::imageSize(this->format(), this->width(), this->height(), this->padding());
}

} // namespace

} // namespace

#endif
