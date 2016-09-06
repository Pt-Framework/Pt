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
#include <Pt/Types.h>
#include <Pt/Gfx/ImageFormat.h>

namespace Pt {

namespace Gfx {

class ImageInfo
{
    public:
        explicit ImageInfo(const ImageFormat& format)
        : _format(&format)
        , _data(0)
        , _size()
        , _stride(0)
        {
          _lineSize = (_size.width() * _format->pixelSize())  + _stride;
        }

        ImageInfo(const ImageFormat& format, Pt::uint8_t* data, 
                  const Size& size, Pt::ssize_t stride)
        : _format(&format)
        , _data(data)
        , _size(size)
        , _stride(stride)
        {
          _lineSize = (_size.width() * _format->pixelSize())  + _stride;
        }

        void set(const ImageFormat& format, Pt::uint8_t* data, 
                 const Size& size, Pt::ssize_t stride)
        {
            _format = &format;
            _data = data;
            _size = size;
            _stride = stride;
            _lineSize = (_size.width() * _format->pixelSize())  + _stride;
        }

        void set(Pt::uint8_t* data, const Size& size, Pt::ssize_t stride)
        {
            _data = data;
            _size = size;
            _stride = stride;
            _lineSize = (_size.width() * _format->pixelSize())  + _stride;
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

        Pt::ssize_t stride() const
        { return _stride; }

        Pt::ssize_t lineSize() const
        {
            return _lineSize;
        }

    private:
        const ImageFormat* _format;

        Pt::uint8_t* _data;
        Size         _size;
        Pt::ssize_t  _stride;
        Pt::ssize_t  _lineSize;
};


class Pixel
{
    public:
        Pixel(const ImageFormat& info, Pt::uint8_t* data)
        : _info(&info)
        , _data(data)
        , _meta(0)
        { }

        Pixel(const Pixel& p)
        : _info(p._info)
        , _data(p._data)
        , _meta(p._meta)
        { }

        Pixel& operator=(const Pixel& p)
        {
            _info->assign(*this, p);
            return *this;
        }

        void reset(const ImageFormat& info, Pt::uint8_t* data)
        {
             _info = &info;
             _data = data;
             _meta = 0;
        }

        void reset(const Pixel& p)
        {
             _info = p._info;
             _data = p._data;
             _meta = p._meta;
        }

        void reset(Pt::uint8_t* data)
        {
             _data = data;
        }

        const ImageFormat& info() const
        { return *_info; }
        
        Pt::uint8_t* data()
        { return _data; }

        const Pt::uint8_t* data() const
        { return _data; }
        
        Pt::uint32_t meta() const
        { return _meta; }

    private:
        const ImageFormat* _info;
        Pt::uint8_t* _data;
        Pt::uint32_t _meta;
};

} // namespace

} // namespace

#endif
