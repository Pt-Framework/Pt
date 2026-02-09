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

#ifndef PT_GFX_BASIC_IMAGE_H
#define PT_GFX_BASIC_IMAGE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Types.h>
#include <vector>

namespace Pt {

namespace Gfx {

//inline namespace v2 {

template <typename FormatT>
std::size_t pixelStride(const FormatT& format)
{
    return format.pixelStride();
}


template <typename FormatT>
std::size_t imageSize(const FormatT& format, Pt::ssize_t width, Pt::ssize_t height,
                      std::size_t padding)
{
    return format.imageSize(width, height, padding);
}


template <typename FormatT>
FormatT* clone(const FormatT& format)
{
    return new FormatT(format);
}


template <typename FormatT>
void release(const FormatT* format)
{
    delete format;
}

/** @brief Basic image.
*/
template <typename FormatT>
class BasicImage
{
    public:
        typedef FormatT Format;

        typedef Pt::ssize_t pos_t;
        typedef Pt::ssize_t length_t;

    public:
        BasicImage(const Format& format);

        BasicImage(const Format& format, Pt::ssize_t width, Pt::ssize_t height, 
                   Pt::ssize_t padding = 0)
        : _format( clone(format) )
        , _data(0)
        , _width(width)
        , _height(height)
        ,  _padding(padding)
        {
            _buffer.resize( imageSize(*_format, width, height, padding) );
            _data = _buffer.empty() ? 0 : &_buffer[0];
        }

        BasicImage(const Format& format, Pt::uint8_t* data, 
                   Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding = 0)
        : _format( clone(format) )
        , _data(data)
        , _width(width)
        , _height(height)
        , _padding(padding)
        { }

        BasicImage(const BasicImage& image)
        : _format( image._format->clone() )
        , _buffer(image._buffer)
        , _data(image._data)
        , _width(image._width)
        , _height(image._height)
        , _padding(image._padding)
        { }

        virtual ~BasicImage()
        { 
            release(_format);
        }

        BasicImage& operator=(const BasicImage& image)
        { 
            Format* f = image.format().clone();
            release(_format);
            _format = f;
            
            _buffer = image._buffer;
            _data = image._data;
            
            _width = image._width;
            _height = image._height;
            _padding = image._padding;

            return *this;
        }

        void reset(Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding = 0)
        { 
            _buffer.resize( imageSize(*_format, width, height, padding) );
            _data = _buffer.empty() ? 0 : &_buffer[0];
            
            _width = width;
            _height = height;
            _padding = padding;
        }

        void reset(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height, 
                   Pt::ssize_t padding = 0)
        {
            _buffer.clear();
            _data = data;
            
            _width = width;
            _height = height;
            _padding = padding;
        }

        void reset(const Format& format, Pt::ssize_t width, Pt::ssize_t height, 
                   Pt::ssize_t padding = 0)
        { 
            Format* f = clone(format);
            release(_format);
            _format = f;

            _buffer.resize( imageSize(*_format, width, height, padding) );
            _data = _buffer.empty() ? 0 : &_buffer[0];
            
            _width = width;
            _height = height;
            _padding = _padding;
        }

        void reset(const Format& format, Pt::uint8_t* data, 
                   Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding = 0)
        {
            Format* f = clone(format);
            release(_format);
            _format = f;

            _buffer.clear();
            _data = data;

            _width = width;
            _height = height;
            _padding = padding;
        }

        void clear()
        {
            _buffer.clear();
            _data = 0;
            
            _width = 0;
            _height = 0;
            _padding = 0;
        }
        
        bool empty() const
        { return _width == 0 || _height == 0; }

        Pt::uint8_t* data()
        { return _data; }

        const Pt::uint8_t* data() const
        { return _data; }
        
        const Format& format() const
        { return *_format; }

        Pt::ssize_t width() const
        { return _width; }

        Pt::ssize_t height() const
        { return _height; }

        Pt::ssize_t padding() const
        { return _padding; }

        Pt::ssize_t stride() const
        { 
            return (_width * pixelStride(*_format)) + _padding; 
        }

        std::size_t size(Pt::ssize_t width, Pt::ssize_t height, 
                         std::size_t padding) const
        {
            return imageSize(*_format, width, height, padding);
        }

    private:
        Format*                  _format;
        std::vector<Pt::uint8_t> _buffer;
        Pt::uint8_t*  _data;
        Pt::ssize_t   _width;
        Pt::ssize_t   _height;
        Pt::ssize_t   _padding;
};

/** @brief Basic const image.
*/
template <typename FormatT>
class BasicConstImage
{
    public:
        typedef FormatT Format;

        typedef Pt::ssize_t pos_t;
        typedef Pt::ssize_t length_t;

    public:
        BasicConstImage(const Format& format)
        : _format(clone(format))
        , _data(0)
        , _width(0)
        , _height(0)
        , _padding(0)
        { }

        BasicConstImage(const Format& format, const Pt::uint8_t* data, 
                        Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding = 0)
        : _format(clone(format))
        , _data(data)
        , _width(width)
        , _height(height)
        , _padding(padding)
        { }

        BasicConstImage(const BasicImage<FormatT>& image)
        : _format( image.format().clone() )
        , _data( image.data() )
        , _width( image.width() )
        , _height( image.height() )
        , _padding( image.padding() )
        { }

        BasicConstImage(const BasicConstImage& image)
        : _format( image._format->clone() )
        , _data(image._data)
        , _width(image._width)
        , _height(image._height)
        , _padding(image._padding)
        { }

        virtual ~BasicConstImage()
        {
            release(_format);
        }

        BasicConstImage& operator=(const BasicConstImage& image)
        { 
            Format* f = image.format().clone();
            release(_format);
            _format = f;
            
            _data = image._data;
            
            _width = image._width;
            _height = image._height;
            _padding = image._padding;

            return *this;
        }

        BasicConstImage& operator=(const BasicImage<FormatT>& image)
        { 
            Format* f = image.format().clone();
            release(_format);
            _format = f;
            
            _data = image.data();
            
            _width = image.width();
            _height = image.height();
            _padding = image.padding();

            return *this;
        }

        void reset(const Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height, 
                   Pt::ssize_t padding = 0)
        {
            _data = data;
            
            _width = width;
            _height = height;
            _padding = padding;
        }

        void reset(const Format& format, const Pt::uint8_t* data, 
                   Pt::ssize_t width, Pt::ssize_t height, Pt::ssize_t padding = 0)
        {
            Format* f = clone(format);
            release(_format);
            _format = f;

            _data = data;
            
            _width = width;
            _height = height;
            _padding = padding;
        }

        void clear()
        {
            _data = 0;
            _width = 0;
            _height = 0;
            _padding = 0;
        }
        
        bool empty() const
        { return _width == 0 || _height == 0; }

        const Pt::uint8_t* data() const
        { return _data; }
        
        const Format& format() const
        { return *_format; }

        Pt::ssize_t width() const
        { return _width; }

        Pt::ssize_t height() const
        { return _height; }

        Pt::ssize_t padding() const
        { return _padding; }

        Pt::ssize_t stride() const
        { 
            return (_width * pixelStride(*_format)) + _padding; 
        }

        std::size_t size(Pt::ssize_t width, Pt::ssize_t height, 
                         std::size_t padding) const
        {
            return imageSize(*_format, width, height, padding);
        }

    private:
        Format*            _format;
        const Pt::uint8_t* _data;
        Pt::ssize_t        _width;
        Pt::ssize_t        _height;
        Pt::ssize_t        _padding;
};

// } // namespace

} // namespace

} // namespace

#include <Pt/Gfx/BasicImage.hpp>

#endif
