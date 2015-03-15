/*
 * Copyright (C) 2014 Laurentiu-Gheorghe Crisan
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
#include "PaintSurfaceImpl.h"
#include "PainterImpl.h"
#include <tchar.h>
#include <Pt/Hmi/Application.h>

namespace Pt{
namespace Hmi{

PaintSurfaceImpl::PaintSurfaceImpl()
: _deviceContext(0)
{
	_size = Pt::Gfx::SizeF(10,10);
	Pt::Gfx::Size nsize = Application::instance().fromUnit(_size);

	HDC screenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	_deviceContext = CreateCompatibleDC(screenDC);
	_bitmapHandle = CreateCompatibleBitmap(screenDC, nsize.width(), nsize.height());
    
	DeleteDC(screenDC);

   _oldPen   = (HPEN)  GetCurrentObject(_deviceContext, OBJ_PEN);
   _oldBrush = (HBRUSH) GetCurrentObject(_deviceContext, OBJ_BRUSH);
   _oldFont  = (HFONT) GetCurrentObject(_deviceContext, OBJ_FONT);

  SelectObject(_deviceContext, _bitmapHandle);
	SetBkMode(_deviceContext, TRANSPARENT);
}

PaintSurfaceImpl::~PaintSurfaceImpl()
{
    HPEN oldPen = (HPEN)SelectObject(_deviceContext, _oldPen);
    DeleteObject(oldPen);

    HPEN oldBrush = (HPEN)SelectObject(_deviceContext, _oldBrush);
    DeleteObject(oldBrush);

    HPEN oldFont = (HPEN)SelectObject(_deviceContext, _oldFont);
    DeleteObject(oldFont);

    DeleteDC(_deviceContext);
    DeleteObject(_bitmapHandle);
}

Pt::Gfx::ARgbImage PaintSurfaceImpl::toImage()
{
	Pt::Gfx::Size size = Application::instance().fromUnit(_size);
	//TODO: conver _bitmapHandle to PT image
	Pt::Gfx::ARgbImage	image(size.width(), size.height());
	return image;
}
	
void PaintSurfaceImpl::resize(const Pt::Gfx::SizeF& size)
{
	_size = size;
	Pt::Gfx::Size nsize = Application::instance().fromUnit(_size);

	//Save the old settings
	COLORREF textColor = GetTextColor(_deviceContext);
  _oldPen   = (HPEN)  GetCurrentObject(_deviceContext, OBJ_PEN);
  _oldBrush = (HBRUSH) GetCurrentObject(_deviceContext, OBJ_BRUSH);
  _oldFont  = (HFONT) GetCurrentObject(_deviceContext, OBJ_FONT);

  //Delete the context and bitmap
  DeleteDC(_deviceContext);
	DeleteObject(_bitmapHandle);

    //Create a new context and bitmap
    HDC screenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
    _deviceContext = CreateCompatibleDC(screenDC);
    _bitmapHandle = CreateCompatibleBitmap(screenDC, nsize.width(), nsize.height());
    DeleteDC(screenDC);

	//Restore the old settings
	SelectObject(_deviceContext, _bitmapHandle);
	SelectObject(_deviceContext, _oldFont);
	SelectObject(_deviceContext, _oldBrush);
	SelectObject(_deviceContext, _oldPen);
	SetTextColor(_deviceContext, textColor);
	SetBkMode(_deviceContext, TRANSPARENT);
}

}}

