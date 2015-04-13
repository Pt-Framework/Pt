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


void Cursor::loadCursor( const char* pngFile, const Gfx::ARgbColor& alphaColor, Cursor& cursor )
{
  std::fstream fs( pngFile, std::ios::binary |  std::ios::in );

  if( !fs )
    throw std::invalid_argument( "File not found." );

  loadCursor( fs, alphaColor, cursor);
}


void Cursor::loadCursor( std::istream& pngStream, const Gfx::ARgbColor& alphaColor, Cursor& cursor )
{
	Pt::Gfx::ARgbImage* im = Pt::Gfx::ImageReader::read(pngStream);

	//Generate alpha channel
	for( size_t y = 0;  y < im->height(); ++y )
	{
		for( size_t x = 0;  x < im->width(); ++x )
		{
			Pt::Gfx::ARgbColor& pix =  im->pixel(x,y);
				
			if( pix.red() == alphaColor.red() && 
          pix.green() == alphaColor.green() &&
          pix.blue() == alphaColor.blue() )
      {
				pix.setAlpha(0);
      }
			else
      {
				pix.setAlpha(0xffff);
      }
		}
	}			

  fromImage(*im, cursor);
  delete im;
}


void Cursor::loadCursor( const Pt::uint8_t* pngStream, const size_t size, const Gfx::ARgbColor& alphaColor, Cursor& cursor )
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

	loadCursor( g_moveCursor, g_moveCursorSize, Gfx::ARgbColor( 255, 0, 0 ), cursor );
  cursor.setXHotspot( 11 );
  cursor.setYHotspot( 11 );
  cursor.setName( "move" );
	return cursor;
}

const Cursor& Cursor::arrowCursor()
{	
	static Cursor cursor;

	if( cursor.width() != 0 )
		return cursor;

	loadCursor( g_arrowCursor, g_arrowCursorSize, Gfx::ARgbColor( 255, 0, 0 ), cursor );
  cursor.setXHotspot( 0 );
  cursor.setYHotspot( 0 );
  cursor.setName( "arrow" );
	return cursor;
}


const Cursor& Cursor::waitCursor()
{
	static Cursor cursor;

	if( cursor.width() != 0 )
		return cursor;

	loadCursor( g_waitCursor, g_waitCursorSize, Gfx::ARgbColor( 255, 0, 0 ), cursor );
  cursor.setXHotspot( 12 );
  cursor.setYHotspot( 16 );
  cursor.setName( "wait" );
	return cursor;
}


const Cursor& Cursor::sizeNWSECursor()
{
	static Cursor cursor;

	if( cursor.width() != 0 )
		return cursor;

	loadCursor( g_sizeNWSECursor, g_sizeNWSECursorSize, Gfx::ARgbColor( 255, 0, 0 ), cursor );
  cursor.setXHotspot( 8 );
  cursor.setYHotspot( 8 );
  cursor.setName( "sizeNWSE" );
	return cursor;
}


const Cursor& Cursor::sizeNESWCursor()
{
	static Cursor cursor;

	if( cursor.width() != 0 )
		return cursor;

	loadCursor( g_sizeNESWCursor, g_sizeNESWCursorSize, Gfx::ARgbColor( 255, 0, 0 ), cursor );
  cursor.setXHotspot( 8 );
  cursor.setYHotspot( 8 );
  cursor.setName( "sizeNESW" );
	return cursor;
}


const Cursor& Cursor::sizeWECursor()
{
	static Cursor cursor;

	if( cursor.width() != 0 )
		return cursor;

	loadCursor( g_sizeWECursor, g_sizeWECursorSize, Gfx::ARgbColor( 255, 0, 0 ), cursor );
  cursor.setXHotspot( 11 );
  cursor.setYHotspot( 4 );
  cursor.setName( "sizeWE" );

	return cursor;
}


const Cursor& Cursor::sizeNSCursor()
{
	static Cursor cursor;

	if( cursor.width() != 0 )
		return cursor;

	loadCursor( g_sizeNSCursor, g_sizeNSCursorSize, Gfx::ARgbColor( 255, 0, 0 ), cursor );
  cursor.setXHotspot( 4 );
  cursor.setYHotspot( 12 );
  cursor.setName( "sizeNS" );

	return cursor;
}


void Cursor::fromImage( const Pt::Gfx::ARgbImage& image, Cursor& cursor)
{
	cursor._height   = image.height();
	cursor._width    = image.width();

	for( size_t y = 0; y < cursor._height; ++y )
	{
		for( size_t x = 0; x < cursor._width; ++x )
		{
			const Gfx::ARgbColor& color = image.pixel( x, y );

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
					
				cursor._xorMask.push_back( (Pt::uint8_t) color.red() );
				cursor._xorMask.push_back( (Pt::uint8_t) color.green() );
				cursor._xorMask.push_back( (Pt::uint8_t) color.blue() );
				cursor._xorMask.push_back( 0xFF);
			}
		}
	}
}

}}
