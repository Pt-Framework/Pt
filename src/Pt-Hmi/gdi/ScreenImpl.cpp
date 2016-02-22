/* Copyright (C) 2015 Marc Boris Duerner 
 * Copyright (C) 2015 Laurentiu-Gheorghe Crisan
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
#include "ScreenImpl.h"

namespace Pt{
namespace Hmi{

ScreenImpl::ScreenImpl(ApplicationImpl&)
: _dpi(96.0)
, _cursorHandle(0)
, _currentCursor(0)
{
 	 _size = screeResolution();

	_width  = _size.width() * unitSizeInch()*_dpi;
	_height = _size.height() * unitSizeInch()*_dpi;
	
	_factorX = _width / _size.width();
	_factorY = _height / _size.height();

	_offsetX = 0;
	_offsetY = 0;	
}


ScreenImpl::~ScreenImpl()
{	
   if( _cursorHandle != 0 )			
	   DestroyCursor( _cursorHandle );
}

double ScreenImpl::width() const
{
  const HWND hDesktop = GetDesktopWindow();
  RECT desktop;   
  GetWindowRect(hDesktop, &desktop);
  return  desktop.right;
}

		
double ScreenImpl::height() const
{
	const HWND hDesktop = GetDesktopWindow();
  RECT desktop;   
  GetWindowRect(hDesktop, &desktop);
  return  desktop.bottom;
}


void ScreenImpl::setResolution(double dpi)
{
	_dpi = dpi;
}


double ScreenImpl::resolutionDPI() const
{
	return _dpi;
}


int ScreenImpl::fromUnit(double unit)
{
	return (int) (unit *unitSizeInch()* _dpi);
}


double ScreenImpl::toUnit(int unit)
{
	return unitSizeInch()/_dpi * unit;
}


Gfx::PointF ScreenImpl::toUnit(const Gfx::Point& value)
{
	const double x = value.x() * _factorX  + _offsetX;
	const double y = value.y() * _factorY  + _offsetY;

	return Gfx::PointF(std::ceil(x),std::ceil(y));
}


Gfx::SizeF ScreenImpl::toUnit(const Gfx::Size& value)
{
	const double width = value.width() * _factorX  + _offsetX;
	const double height = value.height() * _factorY  + _offsetY;

	return Gfx::SizeF(std::ceil(width),std::ceil(height));
}


Gfx::Point ScreenImpl::fromUnit(const Gfx::PointF& value)
{
	double factorX = _size.width() / _width;
	double factorY = _size.height() / _height;
	int x = (int) ( value.x() * factorX); 
	int y = (int) ( value.y() * factorY);

	return Gfx::Point(x,y);
}


Gfx::Size ScreenImpl::fromUnit(const Gfx::SizeF& value)
{
	double factorX = _size.width() / _width;
	double factorY = _size.height() / _height;
	int width = (int) ( value.width() * factorX); 
	int height = (int) ( value.height() * factorY);
	return Gfx::Size(width,height);
}


Gfx::Rect ScreenImpl::fromUnit(const Gfx::RectF& value)
{
	Gfx::Rect rect(Gfx::Point(value.x(), value.y()),Gfx::Size(value.width(), value.height()));
	return rect;
}


double ScreenImpl::unitSizeInch() const
{
	return 1.0/96.0;
}


double ScreenImpl::unitSizeMm() const
{
	return 25.4 * unitSizeInch();
}


HBITMAP ScreenImpl::createImage888(const Pt::uint8_t* data, size_t width, size_t height)
{
	HDC hDC        = ::GetDC(NULL);
	HDC hMainDC    = ::CreateCompatibleDC(hDC); 

	BITMAPINFO bitmapInfo;
	ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

	bitmapInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER); // Size of this struct.
	bitmapInfo.bmiHeader.biWidth       = width;             // Bitmap width.
	bitmapInfo.bmiHeader.biHeight      = -(ssize_t)height;  // Bitmap height. Negative value = top-down image.
	bitmapInfo.bmiHeader.biPlanes      = 1;                 // Always 1.
	bitmapInfo.bmiHeader.biBitCount    = 32;                // We internally use a 32-bit bitmap.
	bitmapInfo.bmiHeader.biCompression = BI_RGB;            // Uncompressed (top-down) RGB bitmap.
	bitmapInfo.bmiHeader.biSizeImage   = 0;                 // 0 = automatic for BI_RGB-images.
	bitmapInfo.bmiHeader.biClrUsed     = 0;                 // 0 = No color table.
	bitmapInfo.bmiHeader.biClrImportant= 0;                 // 0 = No color table.

	VOID* imageBits;
	HBITMAP bitmap = CreateDIBSection(hMainDC, &bitmapInfo, DIB_RGB_COLORS, &imageBits, NULL, 0);

	memcpy(imageBits, data, width * height * 3);

	::DeleteDC(hMainDC);
	::ReleaseDC(NULL,hDC);

	return bitmap;
}


void ScreenImpl::setCursor(const Cursor* cursor)
{	  
    if( _currentCursor == cursor )
        return;

    _currentCursor = cursor;

	if( cursor == 0 )
		return;

   if( _cursorHandle != 0 )			
	   DestroyCursor( _cursorHandle );

	if( cursor->empty() )
    {
        SetCursor(0);
		return;
    }

	HBITMAP andMask = createImage888( &cursor->andRgb888()[0], cursor->width(),  cursor->height() );
	HBITMAP xorMask = createImage888( &cursor->xorRgb888()[0], cursor->width(),  cursor->height() );

	ICONINFO iconInfo;

	iconInfo.fIcon = false; 
	iconInfo.xHotspot = cursor->xHotspot();
	iconInfo.yHotspot = cursor->yHotspot();
	iconInfo.hbmColor = xorMask;
	iconInfo.hbmMask  = andMask;

	_cursorHandle = CreateIconIndirect(&iconInfo);

	if( _cursorHandle != 0 )
	  SetCursor( _cursorHandle );	

	DeleteObject( andMask );
	DeleteObject( xorMask );
}


Gfx::Size ScreenImpl::screeResolution()
{
  RECT desktop;    
  GetWindowRect(GetDesktopWindow(), &desktop);

  return Gfx::Size( desktop.right, desktop.bottom );
}

}}
