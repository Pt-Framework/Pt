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

#include <Pt/Gfx/Image.h>

namespace Pt {

namespace Gfx {
 
Image::Image(const ImageFormat& format)		
: _info(format)
{
}
      
        
Image::Image(const Gfx::Size& size, const ImageFormat& format, size_t stride)
: _info(format)
{
    resize(size, format, stride);
}
    

Image::Image(Pt::uint8_t* buffer, const Gfx::Size& size, 
             const ImageFormat& format, size_t stride)
: _info(format)
{
    resize(buffer, size, format, stride);
}


Image::Image(const Image& image)
: _info( image.format() )
{
    *this = image;
}


Image::~Image()		
{
}


const Image& Image::operator=(const Image& image)
{
    // TODO: it may be better to always copy the pixels

    _buffer = image._buffer;

    Pt::uint8_t* data = _buffer.empty() ? image._info.data()
                                        : &_buffer[0];

    _info.set(image.format(), data, image.size(), image.stride());

	  return *this;
}


Image Image::convert(const ImageFormat& toFormat) const
{
    if( format() == toFormat )
        return *this;

    Image image( size(), toFormat, stride() );

    for(size_t y = 0; y < height(); ++y)
    {
        for(size_t x = 0; x < width(); ++x)
        {
            const Color pixelColor = color(x,y);
            image.setColor(x , y, pixelColor);
        }
    }

    return image;
}


Image Image::blockScale(const Size& newSize) const
{
  Image resultImage( newSize, format(), stride() );

  const double dx = newSize.width() /(double)width();
  const double dy = newSize.height() /(double) width();

  double xTarget = 0;
  double yTarget = 0;

  for( size_t ySource = 0; ySource < height(); ++ySource)
  {        
    xTarget = 0;

    for( size_t xSource = 0; xSource < width(); ++xSource)
    {
            
      const Pt::uint8_t* pixelSource = pixel( xSource, ySource);
      
      Pt::uint8_t* pixelTarget = resultImage.pixel( (size_t) xTarget, (size_t)yTarget);       
      memcpy( pixelTarget, pixelSource, format().pixelSize());

      for( size_t i = 1; i < dx; ++i)
      {
        const size_t xPos = (size_t)xTarget + i;
        const size_t yPos = (size_t)yTarget;

        if( xPos >= resultImage.width() )
          break;

        memcpy( resultImage.pixel( xPos, yPos), pixelSource, format().pixelSize());
      }

       xTarget += dx;
    }
        
    const Pt::uint8_t* sourceLine = resultImage.pixel( 0, (size_t)yTarget);     
    
    for( size_t i = 1 ; i < dy ; ++i)
    {      
      const size_t yPos = (size_t) yTarget + i;

      if( yPos >= resultImage.height() )
        break;
                 
      memcpy( resultImage.pixel(0, yPos), 
              sourceLine, 
              resultImage.width() * format().pixelSize() + stride());
    }

    yTarget += dy;
  }  

  return resultImage;
}


Image Image::subImage(const Rect& rect) const
{
  int width = this->width();
  int height = this->height();
  int x = rect.left();
  int y = rect.top();

  if( rect.left() + rect.width() >= width )
    width = width - rect.left();

  if( rect.top() + rect.height() >= height )
    height = height - rect.top() ;

  if( rect.left() < 0)
    x = 0;

  if( rect.top() < 0)
    y = 0;

  Rect clipRect( Point(x,y), Size(width, height) );

  Image image( clipRect.size(), format(), stride() );

  for( int y = clipRect.top(); y <= clipRect.bottom(); ++y)
  {
    const int lineBeginOffset = y * (this->width() * format().pixelSize() + stride());

    const int targetY = y - clipRect.top();
    
    const int xStartOffset = lineBeginOffset + (clipRect.left() * format().pixelSize());
    const int xEnd   = lineBeginOffset + (clipRect.right() * format().pixelSize());

    const int lineSize = (clipRect.right() - clipRect.left() + 1) * format().pixelSize();

    Pt::uint8_t* from = _info.data();
    memcpy( image.pixel(0, targetY), &from[xStartOffset], lineSize );
  }

  return image;
}


void Image::setColor(const Color& color)
{
    Pt::uint8_t* it = data();	
  
    std::vector<Pt::uint8_t> pixel( format().pixelSize() );
    format().setColor(&pixel[0], color);
  
    const size_t count = (width() + stride()) * height();

    for(size_t i = 0; i < count; ++i)
    {				
        memcpy(it, &(pixel[0]), pixel.size());
        it += pixel.size();
    }
}


void Image::resize(const Gfx::Size& size, size_t strideInBytes)
{
    Pt::ssize_t n = (size.width() * format().pixelSize() + stride()) * size.height();
    _buffer.resize(n); 
 
    _info.set(&_buffer[0], size, strideInBytes); 	       
}


void Image::resize(const Gfx::Size& size, const ImageFormat& f, size_t strideInBytes)
{
    Pt::ssize_t n = (size.width() * format().pixelSize() + stride()) * size.height();
    _buffer.resize(n); 
 
    _info.set(f, &_buffer[0], size, strideInBytes); 
}


void Image::resize(Pt::uint8_t* buffer, const Gfx::Size& size, size_t strideInBytes)
{
    _info.set(buffer, size, strideInBytes);
    _buffer.clear();  
}


void Image::resize(Pt::uint8_t* buffer, const Gfx::Size& size, 
                   const ImageFormat& format, size_t strideInBytes)
{
    _info.set(format, buffer, size, strideInBytes);
    _buffer.clear();  
}

} // namespace

} // namespace
