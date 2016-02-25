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
#include <Pt/Hmi/Application.h>
#include <tchar.h>

namespace Pt {

namespace Hmi {

PaintAreaImpl::PaintAreaImpl()
: _surface(0)
{
}


PaintAreaImpl::~PaintAreaImpl()
{
}


void PaintAreaImpl::set(PaintSurface& surface, const Gfx::RectF& area)
{
    _surface = &surface;
    _area = area;
}


HDC PaintAreaImpl::deviceContext() const
{
    if( ! _surface )
        return NULL;

    return _surface->impl()->deviceContext();
}


// TODO: move drawing code to PaintSurfaceImpl
//void PaintAreaImpl::drawLine(const Gfx::PointF& fromF, const Gfx::PointF& toF)
//{
//    _surface->impl()->drawLine(fromF + _area.topLeft(),
//                               toF + _area.topLeft() );
//}


Gfx::Point PaintAreaImpl::toDevice(const Gfx::PointF& p) const
{
    return _surface->impl()->toDevice( p + _area.topLeft() );
}


Gfx::Rect PaintAreaImpl::toDevice(const Gfx::RectF& r) const
{
    Gfx::RectF rect(r);
    rect.setOrigin(r.topLeft() + _area.topLeft());
    return _surface->impl()->toDevice(rect);
}


Gfx::Size PaintAreaImpl::toDevice(const Gfx::SizeF& s) const
{
    return _surface->impl()->toDevice(s);
}




PixmapSurfaceImpl::PixmapSurfaceImpl()
: _deviceContext(0)
{
    _size = Gfx::SizeF(10,10);

    Gfx::Size nsize = Application::instance().mainScreen().fromUnit(_size);

    HDC screenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
    _deviceContext = CreateCompatibleDC(screenDC);
    _bitmapHandle = CreateCompatibleBitmap(screenDC, nsize.width(), nsize.height());
    
    DeleteDC(screenDC);

    _oldPen   = (HPEN) GetCurrentObject(_deviceContext, OBJ_PEN);
    _oldBrush = (HBRUSH) GetCurrentObject(_deviceContext, OBJ_BRUSH);
    _oldFont  = (HFONT) GetCurrentObject(_deviceContext, OBJ_FONT);

    SelectObject(_deviceContext, _bitmapHandle);
    SetBkMode(_deviceContext, TRANSPARENT);
}


PixmapSurfaceImpl::~PixmapSurfaceImpl()
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


void PixmapSurfaceImpl::resize(const Gfx::SizeF& size)
{
    if( _size == size )
        return;

    _size = size;
    Gfx::Size nsize = Application::instance().mainScreen().fromUnit( _size );

    //Save the old settings
    COLORREF textColor = GetTextColor(_deviceContext);
    _oldPen   = (HPEN) GetCurrentObject(_deviceContext, OBJ_PEN);
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


HDC PixmapSurfaceImpl::deviceContext() const
{
    return _deviceContext;
}


Gfx::Point PixmapSurfaceImpl::toDevice(const Gfx::PointF& p) const
{
    return Application::instance().mainScreen().fromUnit(p);
}


Gfx::Rect PixmapSurfaceImpl::toDevice(const Gfx::RectF& r) const
{
    return Application::instance().mainScreen().fromUnit(r);
}


Gfx::Size PixmapSurfaceImpl::toDevice(const Gfx::SizeF& s) const
{
    return Application::instance().mainScreen().fromUnit(s);
}

} // namespace

} // namespace
