
/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2015 Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include <Pt/Gfx/Image.h>

namespace Pt{
namespace Gfx{

 
Image::Image(const ImageFormat& format)		
: _format(&format)
{
	resize(Gfx::Size(0,0) );
}
			
      	
Image::Image(const Gfx::Size& size,  const ImageFormat& format, size_t stride )
: _buffer(0)
{
	resize(size, format, stride);
}
		


Image::Image( Pt::uint8_t* buffer, const Gfx::Size& size, const ImageFormat& format, size_t stride )
{
  resize( buffer, size, format, stride );
}


Image::~Image()		
{

}


Image Image::convert(const ImageFormat& toFormat) const
{
	if( *_format == toFormat )
		return *this;

	Image image( Size(_width, _height), toFormat, _stride );

	for( size_t y = 0; y < _height; ++y)
	{
		for( size_t x = 0; x < _width; ++x )
		{
			const Color pixelColor = color(x,y);
			image.setColor(x , y, pixelColor);
		}
	}

	return image;
}


Image Image::blockScale( const Size& newSize) const
{
  Image resultImage(newSize,  *_format, _stride );

  const double dx = newSize.width() /(double)_width;
  const double dy = newSize.height() /(double) _height;

  double xTarget = 0;
  double yTarget = 0;

  for( size_t ySource = 0; ySource < _height; ++ySource)
  {        
    xTarget = 0;

    for( size_t xSource = 0; xSource < _width;  ++xSource)
    {
      const Pt::uint8_t* pixelSource = pixel( xSource, ySource);
      
      Pt::uint8_t* pixelTarget = resultImage.pixel( (size_t) xTarget, (size_t)yTarget);       
      memcpy( pixelTarget, pixelSource, _format->pixelSize());
      
      for( size_t i = 1; i < dx; ++i)
      {
        const size_t xPos = (size_t)xTarget + i;
        const size_t yPos = (size_t)yTarget;

        if( xPos >= resultImage.width() )
          break;

        memcpy( resultImage.pixel( xPos, yPos), pixelSource, _format->pixelSize());
      }

       xTarget += dx;
    }
        
    const Pt::uint8_t* sourceLine = resultImage.pixel( 0, (size_t)yTarget);     
    
    for( size_t i = 1 ; i < dy ; ++i)
    {      
      const size_t yPos = (size_t) yTarget + i;

      if( yPos >= resultImage.height() )
        break;
                 
      memcpy( resultImage.pixel( 0, yPos), sourceLine, resultImage.width() * _format->pixelSize() + _stride);
    }

    yTarget += dy;
  }  

  return resultImage;
}


Image Image::subImage( const Rect& regionIn) const
{
	//Cliping.
	int width = _width;
	int height = _height;
	int x = regionIn.left();
	int y = regionIn.top();

	if( regionIn.left() + regionIn.width() >= _width )
		width =  _width - regionIn.left();

	if( regionIn.top() + regionIn.height() >= _height )
		height =  _height - regionIn.top() ;

	if( regionIn.left() < 0)
		x = 0;

	if( regionIn.top() < 0)
		y = 0;

	Rect clipedRegion( Point(x,y), Size(width, height ) );

	//Copy to new image
	Image image(Gfx::Size( clipedRegion.width(), clipedRegion.height()), *_format, _stride );

	for( int y = clipedRegion.top(); y  <= clipedRegion.bottom(); ++y)
	{
		const int lineBeginOffset = y * (_width * _format->pixelSize() + _stride);

		const int targetY = y - clipedRegion.top();
		
		const int xStartOffset = lineBeginOffset + (clipedRegion.left() * _format->pixelSize());
		const int xEnd   = lineBeginOffset + (clipedRegion.right() * _format->pixelSize());

		const int lineSize = (clipedRegion.right() - clipedRegion.left() + 1) * _format->pixelSize();

		memcpy( image.pixel(0,targetY), &_buffer[xStartOffset], lineSize );
	}

	return image;
}


void Image::setColor( const Color& color )
{
	Pt::uint8_t* it =  pixel(0,0);	
	
	std::vector<Pt::uint8_t> pixel( format().pixelSize() );
	format().setColor(&pixel[0], color);
	
	const size_t count = (_width + _stride) * _height;

	for( size_t  i = 0; i < count; ++i )
	{				
		memcpy(it, &(pixel[0]), pixel.size());
		it += pixel.size();
	}
}



void Image::resize( const Gfx::Size& size,  size_t strideInBytes )
{
  _stride = strideInBytes;
  _width  = size.width();
  _height = size.height();

  _defaultBuffer.clear();

  if( _width == 0 || _height == 0 )
      return;

  _defaultBuffer.resize( ( _width * _format->pixelSize() + _stride) * _height ); 
  _buffer = &_defaultBuffer[0];  	       
}


void Image::resize( const Gfx::Size& size, const ImageFormat& format, size_t strideInBytes )
{
  _format = &format;
  resize( size, strideInBytes );
}


void Image::resize( Pt::uint8_t* buffer, const Gfx::Size& size,size_t strideInBytes  )
{
  _stride = strideInBytes;
	_width  = size.width();
	_height = size.height();
  _buffer = buffer;
  _defaultBuffer.clear();  
}


void Image::resize( Pt::uint8_t* buffer, const Gfx::Size& size, const ImageFormat& format, size_t strideInBytes  )
{
  _format = &format;
  resize( buffer, size, strideInBytes );
}


}}
