/*
 * Copyright (C) 2006 Marc Boris Dürner
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

#include "PixmapImpl.h"
#include "PixmapPainter.h"

#include <Pt/Gui/Pixmap.h>

#include <tchar.h>
#include <iostream>
using namespace std;


namespace Pt {

namespace Gui {


PixmapImpl::PixmapImpl(size_t width, size_t height)
: _size( max(width, size_t(1)), max(height, size_t(1)) )
, _painter(0)
, _deviceContext(CreateCompatibleDC(CreateDC(_T("DISPLAY"), NULL, NULL, NULL)))
{
    // Device context of the display. ("DISPLAY" == Predefined Windows device.)
    HDC screenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

    // Create a Bitmap compatible to the current display.
    _bitmapHandle = CreateCompatibleBitmap(screenDC, _size.width(), _size.height());

    // Free the screen context.
    DeleteDC(screenDC);


    setupDeviceContext();
}


PixmapImpl::PixmapImpl(const PixmapImpl& oldPixmap)
: _size( oldPixmap.size() )
, _painter(0)
, _deviceContext(CreateCompatibleDC(CreateDC(_T("DISPLAY"), NULL, NULL, NULL)))
{
    // Device context of the display. ("DISPLAY" == Predefined Windows device.)
    HDC screenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

    // Create a Bitmap compatible to the current display.
    _bitmapHandle = CreateCompatibleBitmap(screenDC, _size.width(), _size.height());

    // Create and select a device context for the destination bitmap (new).
    HDC destinationDC = CreateCompatibleDC(screenDC);
    SelectObject(destinationDC, _bitmapHandle);

    // Create and select a device context for the source bitmap.
    HDC sourceDC = CreateCompatibleDC(screenDC);
    SelectObject(sourceDC, oldPixmap._bitmapHandle);

    // Copy contents from the source bitmap to the destination (=new) bitmap.
    BitBlt(destinationDC, 0, 0, _size.width(), _size.height(), sourceDC, 0, 0, SRCCOPY);

    // Free the device contexts.
    DeleteDC(screenDC);
    DeleteDC(destinationDC);
    DeleteDC(sourceDC);


    setupDeviceContext();
}


void PixmapImpl::setupDeviceContext()
{
    _oldPen   = (HPEN)  GetCurrentObject(_deviceContext, OBJ_PEN);
    _oldBrush = (HBRUSH)GetCurrentObject(_deviceContext, OBJ_BRUSH);
    _oldFont  = (HFONT) GetCurrentObject(_deviceContext, OBJ_FONT);

    // Activate the pixmap for the device context.
    SelectObject(_deviceContext, _bitmapHandle);
}


PixmapImpl::~PixmapImpl()
{
    // Destroy the painter (in case we created one).
    delete _painter;


    HPEN oldPen = (HPEN)SelectObject(_deviceContext, _oldPen);
    DeleteObject(oldPen);

    HPEN oldBrush = (HPEN)SelectObject(_deviceContext, _oldBrush);
    DeleteObject(oldBrush);

    HPEN oldFont = (HPEN)SelectObject(_deviceContext, _oldFont);
    DeleteObject(oldFont);


    // Delete the DC of this painter.
    DeleteDC(_deviceContext);

    // Delete the bitmap/pixmap.
    DeleteObject(_bitmapHandle);
}


Painter PixmapImpl::painter()
{
    if (0 == _painter) {
        _painter = new PixmapPainter(*this);
    }

    return Painter(_painter);
}


HBITMAP PixmapImpl::bitmapHandle() const
{
    return _bitmapHandle;
}


HDC PixmapImpl::beginPaint()
{
    return _deviceContext;
}


void PixmapImpl::endPaint()
{
}


HDC PixmapImpl::deviceContext() const
{
    return _deviceContext;
}


bool PixmapImpl::isPainting() const
{
    return true;
}


} // namespace Gui

} // namespace Pt
