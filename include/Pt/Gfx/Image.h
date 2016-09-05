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

#ifndef PT_GFX_IMAGE_H
#define PT_GFX_IMAGE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ImageInfo.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Types.h>
#include <vector>
#include <cstring>
#include <cstddef>

namespace Pt {

namespace Gfx {

class PixelIterator
{
    public:
        PixelIterator(const ImageInfo& image, Pt::ssize_t x, Pt::ssize_t y)
        : _image(&image)
        , _offset((y * image.width()) + x)
        , _x(x)
        , _y(y)
        , _pixel(image, 0)
        {
            _image->format().getPixel(_pixel, image, x, y);
        }

        PixelIterator(const PixelIterator& it)
        : _image(it._image)
        , _offset(it._offset)
        , _x(it._x)
        , _y(it._y)
        , _pixel(it._pixel)
        {}

        PixelIterator& operator=(const PixelIterator& it)
        {
            _image  = it._image;
            _offset = it._offset;
            _x      = it._x;
            _y      = it._y;
            
            _pixel.reset(it._pixel);
            
            return *this;
        }

        bool operator!=(const PixelIterator& it) const
        { 
            return _offset != it._offset;  
        }

        bool operator==(const PixelIterator& it) const
        { 
            return _offset == it._offset; 
        }
        
        Pixel& operator*()
        { 
            return _pixel; 
        }

        PixelIterator& operator++()
        {
            ++_offset;
            
            if( ++_x >= _image->width() )
            {
                _x = 0;
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
            _offset += n;

            _y = _offset / _image->width();
            _x = _offset % _image->width();
            
            _image->format().getPixel(_pixel, *_image, _x, _y);  
            return *this; 
        }

    private:
        const ImageInfo* _image;
        Pt::ssize_t      _offset;
        Pt::ssize_t      _x;
        Pt::ssize_t      _y;
        Pixel            _pixel;
};


class PT_GFX_API Image
{
  public:
    Image( const ImageFormat& format = ImageFormat::argb8888() );
    
    Image(const Size& size, 
          const ImageFormat& format = ImageFormat::argb8888(), 
          size_t stride = 0);

    Image(Pt::uint8_t* buffer, const Size& size, 
          const ImageFormat& format = ImageFormat::argb8888(), 
          size_t stride = 0);
    
    Image( const Image& image);
              
    virtual ~Image();

    const Image& operator=(const Image& image);

    const ImageFormat& format() const
    {
        return _info.format();
    }

    const ImageInfo& info() const
    {
        return _info;
    }

    size_t width() const
    {
      return _info.width();
    }

    size_t height() const
    {
      return _info.height();
    }
    
    const Size& size() const
    {
        return _info.size();
    }

    Pt::uint8_t* data()
    { 
        return _info.data(); 
    }

    const Pt::uint8_t* data() const
    { 
        return _info.data(); 
    }

    size_t stride() const
    {
      return _info.stride();
    }

    bool empty() const
    {
      return _info.empty();
    }

    PixelIterator begin()
    { return PixelIterator(_info, 0, 0); }

    PixelIterator end()
    { return PixelIterator(_info, 0, height()); }

    void setColor( const Color& color );

    void resize( const Size& size,  size_t strideInBytes = 0 );

    void resize( const Size& size, const ImageFormat& format, size_t strideInBytes = 0);

    void resize( Pt::uint8_t* buffer, const Size& size,size_t strideInBytes = 0 );    

    void resize( Pt::uint8_t* buffer, const Size& size, const ImageFormat& format, size_t strideInBytes = 0 );

    Color color(size_t x, size_t y) const
    {
        return format().color(pixel(x,y));
    }

    void setColor(size_t x, size_t y, const Color& c, 
                  CompositionMode mode = CompositionMode::SourceCopy)
    {
        format().setColor(pixel(x,y), c, mode);
    }

    Pt::uint8_t* pixel(size_t x, size_t y)
    {
        Pt::uint8_t* data = _info.data();
        return &data[ pixelOffsetInBytes(x,y) ];
    }

    const Pt::uint8_t* pixel(size_t x, size_t y) const 
    {
        std::size_t off = y * _info.lineSize() + x * format().pixelSize();
        return &_info.data()[off];
    }

    Image convert(const ImageFormat& toFormat) const;
    
    Image blockScale( const Size& newSize) const;

  protected:
    size_t pixelOffsetInBytes(size_t x, size_t y) const
    {
      const size_t rowOffsetInBytes = y * _info.lineSize();
      return rowOffsetInBytes + x * format().pixelSize();
    }

  private:
    ImageInfo _info;
    std::vector<Pt::uint8_t> _buffer;
};


template<typename IteratorT>
void blockScale(IteratorT from, Pt::ssize_t fromWidth, Pt::ssize_t fromHeight,
                IteratorT to,   Pt::ssize_t toWidth,   Pt::ssize_t toHeight)
{
    Pt::ssize_t dh = 0;
    Pt::ssize_t y  = 0;

    while(y < toHeight) 
    {
        IteratorT pos = from;
        do 
        {
            Pt::ssize_t dw = 0;
            for(Pt::ssize_t x = 0; x < toWidth; ++x) 
            {
                *to = *from;
                
                ++to;
                for(dw += fromWidth; dw >= toWidth; ++from, dw -= toWidth)
                    ;
            }
            from = pos;
            y++;
        }
        while( (dh += fromHeight) < toHeight );

        while(dh >= toHeight) 
        {
            from += fromWidth;
            dh -= toHeight;
        }
    }
}

} // namespace

} // namespace

#endif
