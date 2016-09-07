/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#ifndef PT_GFX_IMAGEINFO_H
#define PT_GFX_IMAGEINFO_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

class ImageInfo
{
    public:
        explicit ImageInfo(const ImageFormat& format)
        : _format(&format)
        , _data(0)
        , _size()
        , _padding(0)
        {
            _pitch = (_size.width() * _format->pixelSize())  + _padding;
        }

        ImageInfo(const ImageFormat& format, Pt::uint8_t* data, 
                  const Size& size, Pt::ssize_t padding)
        : _format(&format)
        , _data(data)
        , _size(size)
        , _padding(padding)
        {
            _pitch = (_size.width() * _format->pixelSize())  + _padding;
        }

        void set(const ImageFormat& format, Pt::uint8_t* data, 
                 const Size& size, Pt::ssize_t padding)
        {
            _format = &format;
            _data = data;
            _size = size;
            _padding = padding;
            _pitch = (_size.width() * _format->pixelSize())  + _padding;
        }

        void set(Pt::uint8_t* data, const Size& size, Pt::ssize_t padding)
        {
            _data = data;
            _size = size;
            _padding = padding;
            _pitch = (_size.width() * _format->pixelSize())  + _padding;
        }

        const ImageFormat& format() const
        { return *_format; }

        Pt::uint8_t* data() const
        { return _data; }

        std::size_t pixelSize() const
        { return _format->pixelSize(); }

        Pt::ssize_t width() const
        { return _size.width(); }

        Pt::ssize_t height() const
        { return _size.height(); }

        const Size& size() const
        { return _size; }

        bool empty() const
        { return _size.width() == 0 || _size.height() == 0; }

        Pt::ssize_t padding() const
        { return _padding; }

        Pt::ssize_t pitch() const
        { return _pitch; }

    private:
        const ImageFormat* _format;

        Pt::uint8_t* _data;
        Size         _size;
        Pt::ssize_t  _padding;
        Pt::ssize_t  _pitch;
};


class PixelIterator
{
    public:
        PixelIterator(const ImageInfo& image, Pt::ssize_t x, Pt::ssize_t y)
        : _image(&image)
        , _x(x)
        , _y(y)
        , _pixel(image.format(), 0)
        {
            _image->format().getPixel(_pixel, image, x, y);
        }

        PixelIterator(const PixelIterator& it)
        : _image(it._image)
        , _x(it._x)
        , _y(it._y)
        , _pixel(it._pixel)
        {}

        PixelIterator& operator=(const PixelIterator& it)
        {
            _image  = it._image;
            _x      = it._x;
            _y      = it._y;
            
            _pixel.reset(it._pixel);
            
            return *this;
        }

        bool operator!=(const PixelIterator& it) const
        { 
            return _x != it._x || _y != it._y;  
        }

        bool operator==(const PixelIterator& it) const
        { 
            return _x == it._x && _y == it._y; 
        }
        
        Pixel& operator*()
        { 
            return _pixel; 
        }

        PixelIterator& operator++()
        {           
            if( ++_x >= _image->width() )
            {
                _x = 0;
                ++_y;
                _image->format().getPixel(_pixel, *_image, _x, _y);
            }
            else
            {
                _image->format().advance(_pixel);
            }
            
            return *this; 
        }

        PixelIterator& operator+=(Pt::ssize_t n)
        {
            Pt::ssize_t off = _x + n;
            _y += off / _image->width();
            _x += off % _image->width();
            
            _image->format().getPixel(_pixel, *_image, _x, _y);  
            return *this; 
        }

    private:
        const ImageInfo* _image;
        Pt::ssize_t      _x;
        Pt::ssize_t      _y;
        Pixel            _pixel;
};

} // namespace

} // namespace

#endif
