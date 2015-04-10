#include <Pt/Hmi/Cursor.h>
#include <Pt/Byteorder.h>
#include <fstream>

namespace Pt{
namespace Hmi{

Cursor::Cursor()
: _width(0)
, _height(0)
{
	*this = defaultCursor();
}


Cursor::~Cursor()
{

}

const Cursor& Cursor::defaultCursor()
{
	static Cursor cursor;
	return cursor;
}

const Cursor& Cursor::arrowCursor()
{
	static Cursor cursor;
	return cursor;
}

const Cursor& Cursor::waitCursor()
{
	static Cursor cursor;
	return cursor;
}

const Cursor& Cursor::sizeNWSECursor()
{
	static Cursor cursor;
	return cursor;
}

const Cursor& Cursor::sizeNESWCursor()
{
	static Cursor cursor;
	return cursor;
}

const Cursor& Cursor::sizeWECursor()
{
	static Cursor cursor;
	return cursor;
}

const Cursor& Cursor::sizeNSCursor()
{
	static Cursor cursor;
	return cursor;
}

void Cursor::load( const Pt::Gfx::ARgbImage& image, size_t xHotspot, size_t yHotspot )
{
	_xHotspot = xHotspot;
	_yHotspot = yHotspot;
	_height = image.height();
	_width = image.width();

	for( size_t y = 0; y < _height; ++y )
	{
		for( size_t x = 0; x < _width; ++x)
		{
			const Gfx::ARgbColor& color = image.pixel(x,y);

			if( color.alpha() == 0 )
			{
				_andMask.push_back(0xff);
				_andMask.push_back(0xff);
				_andMask.push_back(0xff);

				_xorMask.push_back(0);
				_xorMask.push_back(0);
				_xorMask.push_back(0);
			}
			else
			{
				_andMask.push_back(0);
				_andMask.push_back(0);
				_andMask.push_back(0);
					
				_xorMask.push_back((Pt::uint8_t) color.red());
				_xorMask.push_back((Pt::uint8_t)color.green());
				_xorMask.push_back((Pt::uint8_t)color.blue());
			}
		}
	}
}

}}