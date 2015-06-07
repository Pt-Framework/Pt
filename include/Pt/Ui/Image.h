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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef PT_UI_IMAGE_H
#define PT_UI_IMAGE_H

#include <Pt/Types.h>
#include <Pt/Ui/Api.h>
#include <Pt/Ui/ImageFormat.h>
#include <vector>
#include <cstring>

namespace Pt{
namespace Ui{

class PT_UI_API Image
{
	public:		
		Image(const ImageFormat& format = ImageFormat::argb8888());
				
		Image(size_t width, size_t height, size_t stride = 0, const ImageFormat& format = ImageFormat::argb8888());
		

		virtual ~Image();
	
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


		void setColor( const Color& color );

		void resize( size_t width, size_t height, size_t strideInBytes = 0, const ImageFormat& format = ImageFormat::argb8888() )
		{
			_format = &format;
			_stride = strideInBytes;
			_width = width;
			_height = height;
			_buffer.resize( ( width * _format->pixelSize() + _stride) * height ); 
		}
		
		Color color(size_t x, size_t y) const
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

}}

#endif
