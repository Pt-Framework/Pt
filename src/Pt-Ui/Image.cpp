
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
#include <Pt/Ui/Image.h>

namespace Pt{
namespace Ui{


Image::Image(const ImageFormat& format)		
: _format(&format)
{
	resize(0, 0);
}
			
      	
Image::Image(size_t width, size_t height, size_t stride, const ImageFormat& format )
: _format(&format)
{
	resize(width, height, stride);
}
		

Image::~Image()		
{

}


Image Image::convert(const ImageFormat& toFormat) const
{
	if( *_format == toFormat )
		return *this;

	Image image(_width, _height, _stride, toFormat);

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


Image Image::blockScale( const SizeF& newSize) const
{
  Image resultImage( newSize.width(), newSize.height(), _stride, *_format);

  const double dx = newSize.width() /_width;
  const double dy = newSize.height() / _height;

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


Image Image::subImage( const Region& regionIn) const
{
	//Cliping.
	Region clipedRegion = regionIn;
	
	if( regionIn.left() + regionIn.width() >= _width )
		clipedRegion.setWidth( _width - regionIn.left() );

	if( regionIn.top() + regionIn.height() >= _height )
		clipedRegion.setWidth( _height - regionIn.top() );

	if( regionIn.left() < 0)
		clipedRegion.setLeft(0);

	if( regionIn.top() < 0)
		clipedRegion.setTop(0);

	//Copy to new image
	Image image(clipedRegion.width(), clipedRegion.height(), _stride, *_format);

	for( size_t y = clipedRegion.top(); y  < clipedRegion.bottom(); ++y)
	{
		const size_t lineBeginOffset = y * (_width * _format->pixelSize() + _stride);

		const size_t targetY = y - clipedRegion.top();
		
		const size_t xStartOffset = lineBeginOffset + (clipedRegion.left() * _format->pixelSize());
		const size_t xEnd   = lineBeginOffset + (clipedRegion.right() * _format->pixelSize());

		const size_t lineSize = (clipedRegion.right() - clipedRegion.left()) * _format->pixelSize();

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
	
  		
}}
