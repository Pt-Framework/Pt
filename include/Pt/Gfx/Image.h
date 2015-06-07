/*
 * Copyright (C) 2015 Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PT_GFX_IMAGE_H
#define PT_GFX_IMAGE_H

#include <Pt/Types.h>
#include <Pt/Gfx/Api.h>
#include <vector>
#include <cstring>

namespace Pt{
namespace Gfx2{


class PT_GFX_API Color
{	
	public:	
		Color()
		: _a(1)
		, _r(0)
		, _g(0)
		, _b(0)
		{
		}

		Color(float a, float r, float g, float b )
		: _a(a)
		, _r(r)
		, _g(g)
		, _b(b) 
		{
		}

		float alpha() const
		{
			return _a;
		}

		float red() const
		{
			return _r;
		}
		
		float green() const
		{
			return _g;
		}
		
		float blue() const		
		{
			return _b;
		}
		
		void setAlpha( float c)
		{
			_a = c;
		}
		
		void setRed( float c)
		{
			_r = c;
		}
		
		void setGreen( float c)
		{
			_g = c;
		}
		
		void setBlue( float c)
		{
			_b = c;
		}

	private:
		float _a;
		float _r;
		float _g;
		float _b;
};


class Rgb565Format;
class Rgb888Format;
class Argb8888Format;

class PT_GFX_API ImageFormat
{
	public:
		ImageFormat( size_t pixelSize, size_t channels )
		: _pixelSize(pixelSize)
		, _channels(channels)
		{
		}

		size_t pixelSize() const
		{
			return _pixelSize;
		}

		size_t channels() const
		{
			return _channels;
		}

		virtual void setColor(Pt::uint8_t* pixel, const Color& c) const = 0; 		

		virtual Color color(const Pt::uint8_t* pixel ) const = 0;

		std::vector<Pt::uint8_t> toPixel( const Color& c) const
		{
			std::vector<Pt::uint8_t>	buffer( _pixelSize );
			setColor(&buffer[0], c);
			return buffer;
		}

		static const ImageFormat& rgb565();
		static const ImageFormat& rgb888();
		static const ImageFormat& argb8888();

	private:
		 size_t _pixelSize;
		 size_t _channels;
};



class PT_GFX_API Image
{
	public:		

		Image(const ImageFormat& format = ImageFormat::argb8888())		
		:_format(&format)
		{
			resize(1, 1);
		}
				
		Image(size_t width, size_t height, size_t stride = 0, const ImageFormat& format = ImageFormat::argb8888())
		:_format(&format)
		{
			resize(width, height, stride);
		}
		
		virtual ~Image()		
		{
		}
	
		size_t width() const
		{
			return _width;
		}

		size_t height() const
		{
			return _height;
		}

		size_t stride() const
		{
			return _stride;
		}

		void resize( size_t width, size_t height, size_t strideInBytes = 0 )
		{
			_stride = strideInBytes;
			_buffer.resize( ( width * _format->pixelSize() + _stride) * height ); 
		}
		
		Color color(size_t x, size_t y)
		{
			return _format->color(pixel(x,y));
		}

		void setColor(size_t x, size_t y, const Color& c)
		{
			_format->setColor( pixel(x,y), c);
		}

		Pt::uint8_t* pixel(size_t x, size_t y)
		{
			return &_buffer[pixelOffsetInBytes(x,y)];
		}

		const Pt::uint8_t* pixel(size_t x, size_t y) const 
		{
			return &_buffer[pixelOffsetInBytes(x,y)];
		}

		void setPixel( size_t x, size_t y, const Pt::uint8_t* p)
		{
				memcpy( pixel(x, y), p, _format->pixelSize() );
		}

		void setPixels( size_t x, size_t y, size_t count, const Pt::uint8_t* pixel)
		{
			for( size_t i = x; i < (x + count); ++i) 
				setPixel(i, y, pixel);			
		}

		const ImageFormat& format() const
		{
			return *_format;
		}		

		
	protected:
		size_t pixelOffsetInBytes( size_t x, size_t y) const
		{
			const size_t rowOffsetInBytes   = y * (_width * _format->pixelSize() + _stride);
			return rowOffsetInBytes + x * _format->pixelSize();
		}

	private:
	  const ImageFormat* _format;
		std::vector<Pt::uint8_t> _buffer;		
		size_t _width;
		size_t _height;
		size_t _stride;
};



class PT_GFX_API Argb8888Format : public ImageFormat
{
	public:	
		Argb8888Format()
		: ImageFormat(4, 4)
		{
		}

		void setColor(Pt::uint8_t* pixel, const Color& c) const
		{
			*pixel = (Pt::uint8_t) (c.alpha() * 255.0f);
			pixel++;
			*pixel = (Pt::uint8_t)(c.red() * 255.0f);
			pixel++;
			*pixel = (Pt::uint8_t)(c.green() * 255.0f);
			pixel++;
			*pixel = (Pt::uint8_t)(c.blue() * 255.0f);			 
		}

		Color color(const Pt::uint8_t* pixel) const
		{
			return Color( (*pixel) / 255.0f, *(pixel +1)/255.0f, *(pixel +2)/255.0f, *(pixel +3)/255.0f);
		}
};


class PT_GFX_API Rgb888Format : public ImageFormat
{
	public:	
		Rgb888Format()
		: ImageFormat(3, 3)
		{
		}

		void setColor(Pt::uint8_t* pixel, const Color& c) const
		{
			*pixel = (Pt::uint8_t)(c.red() * 255.0f);
			pixel++;
			*pixel = (Pt::uint8_t)(c.green() * 255.0f);
			pixel++;
			*pixel = (Pt::uint8_t)(c.blue() * 255.0f);			 
		}

		Color color(const Pt::uint8_t* pixel) const
		{
			return Color(1, *(pixel +1)/255.0f, *(pixel +2)/255.0f, *(pixel +3)/255.0f);
		}


};



class PT_GFX_API Rgb565Format : public ImageFormat
{
	public:	
		Rgb565Format()
		: ImageFormat(2, 3)
		{
		}

		void setColor(Pt::uint8_t* pixel, const Color& c) const
		{
			Pt::uint16_t* val = (Pt::uint16_t*) pixel;			
			*val  =  (Pt::uint16_t) (c.red() * 32.0f);
			*val  |=  ((Pt::uint16_t) (c.green() * 64.0f))  << 5;
			*val  |=  ((Pt::uint16_t) (c.blue() * 32.0f))  << 11;
		}

		Color color(const Pt::uint8_t* pixel) const
		{
			const Pt::uint16_t* val = (const Pt::uint16_t*) pixel;

			const float r = ((*val & 0xF800) >> 11) / 32.0f;
			const float g = ((*val & 0x07E0) >> 5) / 64.0f;
			const float b = (*val & 0x001F) / 32.0f;

			return Color(1, r, g, b );
		}

};


}}

#endif
