/* Copyright (C) 2015 Laurentiu-Gheorghe Crisan
 
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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#include <Pt/Hmi/Cursor.h>
#include <Pt/Byteorder.h>
#include <Pt/Gfx/ImageReader.h>
#include <fstream>
#include <sstream>
#include "ArrowCursor.h"
#include "SizeWECursor.h"
#include "SizeNSCursor.h"
#include "SizeNWSECursor.h"
#include "SizeNESWCursor.h"
#include "MoveCursor.h"
#include "WaitCursor.h"

namespace Pt{
namespace Hmi{

Cursor::Cursor()
: _width(0)
, _height(0)
{
	
}


Cursor::~Cursor()
{

}


const Cursor& Cursor::defaultCursor()
{
	return arrowCursor();
}


void Cursor::loadCursor( const char* pngFile, const Gfx::Color& alphaColor, Cursor& cursor )
{
  std::fstream fs( pngFile, std::ios::binary |  std::ios::in );

  if( !fs )
    throw std::invalid_argument( "File not found." );

  loadCursor( fs, alphaColor, cursor);
}


void Cursor::loadCursor( std::istream& pngStream, const Gfx::Color& alphaColor, Cursor& cursor )
{
	Gfx::Image* im =Gfx::ImageReader::read(pngStream);

	//Generate alpha channel
	for( size_t y = 0;  y < im->height(); ++y )
	{
		for( size_t x = 0;  x < im->width(); ++x )
		{
			Gfx::Color color =  im->color(x,y);
				
			if( color.red() == alphaColor.red() &&  color.green() == alphaColor.green() && color.blue() == alphaColor.blue() )				
				color.setAlpha(0);
			else
				color.setAlpha(1);

			im->setColor(x,y, color);
		}
	}			

	im->setColor(cursor.xHotspot(),cursor.yHotspot(), Gfx::Color(0,1,0) );
    fromImage(*im, cursor);
    delete im;
}


void Cursor::loadCursor( const Pt::uint8_t* pngStream, const size_t size, const Gfx::Color& alphaColor, Cursor& cursor )
{						
	std::stringstream ms;
		
	ms.write((char*)pngStream, size);	

  loadCursor(ms, alphaColor, cursor);
}


const Cursor& Cursor::moveCursor()
{
	static Cursor cursor;

	if( cursor.width() != 0 )
		return cursor;

    cursor.setXHotspot( 11 );
    cursor.setYHotspot( 11 );
    cursor.setName( "move" );

	loadCursor( g_moveCursor, g_moveCursorSize, Gfx::Color( 1, 0, 0 ), cursor );
	return cursor;
}

const Cursor& Cursor::arrowCursor()
{	
	static Cursor cursor;

	if( cursor.width() != 0 )
		return cursor;

    cursor.setXHotspot( 0 );
    cursor.setYHotspot( 0 );
    cursor.setName( "arrow" );

	loadCursor( g_arrowCursor, g_arrowCursorSize, Gfx::Color( 1, 0, 0 ), cursor );
	return cursor;
}


const Cursor& Cursor::waitCursor()
{
	static Cursor cursor;

	if( cursor.width() != 0 )
		return cursor;

    cursor.setXHotspot( 12 );
    cursor.setYHotspot( 16 );
    cursor.setName( "wait" );

	loadCursor( g_waitCursor, g_waitCursorSize, Gfx::Color( 1, 0, 0 ), cursor );
	return cursor;
}


const Cursor& Cursor::sizeNWSECursor()
{
	static Cursor cursor;

	if( cursor.width() != 0 )
		return cursor;

    cursor.setXHotspot( 8 );
    cursor.setYHotspot( 8 );
    cursor.setName( "sizeNWSE" );

	loadCursor( g_sizeNWSECursor, g_sizeNWSECursorSize, Gfx::Color( 1, 0, 0 ), cursor );
	return cursor;
}


const Cursor& Cursor::sizeNESWCursor()
{
	static Cursor cursor;

	if( cursor.width() != 0 )
		return cursor;

    cursor.setXHotspot( 8 );
    cursor.setYHotspot( 8 );
    cursor.setName( "sizeNESW" );

	loadCursor( g_sizeNESWCursor, g_sizeNESWCursorSize, Gfx::Color( 1, 0, 0 ), cursor );
	return cursor;
}


const Cursor& Cursor::sizeWECursor()
{
	static Cursor cursor;

	if( cursor.width() != 0 )
		return cursor;

    cursor.setXHotspot( 11 );
    cursor.setYHotspot( 4 );
    cursor.setName( "sizeWE" );

	loadCursor( g_sizeWECursor, g_sizeWECursorSize, Gfx::Color( 1, 0, 0 ), cursor );

	return cursor;
}


const Cursor& Cursor::sizeNSCursor()
{
	static Cursor cursor;

	if( cursor.width() != 0 )
		return cursor;

    cursor.setXHotspot( 4 );
    cursor.setYHotspot( 12 );
    cursor.setName( "sizeNS" );

	loadCursor( g_sizeNSCursor, g_sizeNSCursorSize, Gfx::Color( 1, 0, 0 ), cursor );
	return cursor;
}


void Cursor::fromImage( const Gfx::Image& image, Cursor& cursor)
{
	cursor._height   = image.height();
	cursor._width    = image.width();

	for( size_t y = 0; y < cursor._height; ++y )
	{
		for( size_t x = 0; x < cursor._width; ++x )
		{
			const Gfx::Color& color = image.color( x, y );

			if( color.alpha() == 0 )
			{//Transparent
				cursor._andMask.push_back( 0xff );
				cursor._andMask.push_back( 0xff );
				cursor._andMask.push_back( 0xff );				
				cursor._andMask.push_back( 0xff );	

				cursor._xorMask.push_back( 0 );
				cursor._xorMask.push_back( 0 );
				cursor._xorMask.push_back( 0 );
				cursor._xorMask.push_back( 0 );
			}
			else
			{
				cursor._andMask.push_back( 0 );
				cursor._andMask.push_back( 0 );
				cursor._andMask.push_back( 0 );
				cursor._andMask.push_back( 0 );
					
				cursor._xorMask.push_back( (Pt::uint8_t) (color.red() * 255.0) );
				cursor._xorMask.push_back( (Pt::uint8_t) (color.green()* 255.0) );
				cursor._xorMask.push_back( (Pt::uint8_t) (color.blue() * 255.0));
				cursor._xorMask.push_back( 0xFF);
			}
		}
	}
}

}}
