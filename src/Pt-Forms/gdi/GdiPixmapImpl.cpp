/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include "win32.h"
#include "GdiPixmapImpl.h"
#include "GdiPixmapCanvas.h"
#include "GdiFontProvider.h"

#include <Pt/Forms/View.h>
#include <Pt/Forms/Pixmap.h>

#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintContext.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Rgb32.h>
#include <Pt/Gfx/Argb32.h>

#include <cassert>

namespace Pt {

namespace Forms {

GdiPixmapImpl::GdiPixmapImpl()
: _physicalSize(0, 0)
, _width(0)
, _height(0)
, _dc(0)
, _bitmap(0)
, _canvas(0)
{
    HDC screenDC = GetDC(NULL);
    _dc = CreateCompatibleDC(screenDC);
    ReleaseDC(NULL, screenDC);

    _oldBitmap = (HBITMAP) GetCurrentObject(_dc, OBJ_BITMAP);

    SetBkMode(_dc, TRANSPARENT);
    SetGraphicsMode(_dc, GM_ADVANCED);
}


GdiPixmapImpl::~GdiPixmapImpl()
{
    if(_bitmap)
    {
        SelectObject(_dc, _oldBitmap);
        DeleteObject(_bitmap);
    }

    DeleteDC(_dc);
}


void GdiPixmapImpl::reset(const Gfx::SizeF& size)
{
    LONG width = lround( size.width() );
    LONG height = lround( size.height() );

    if( _width == width && _height == height )
        return;
    
    HDC screenDC = GetDC(NULL);
    HBITMAP bitmap = CreateCompatibleBitmap(screenDC, width, height);
    ReleaseDC(NULL, screenDC);

    SelectObject(_dc, bitmap);

    if(_bitmap)
        DeleteObject(_bitmap);

    _bitmap = bitmap;
    _width = width;
    _height = height;

    _physicalSize.set(width, height);
}


void GdiPixmapImpl::reset()
{
    if(_bitmap)
    {
        SelectObject(_dc, _oldBitmap);
        DeleteObject(_bitmap);
        _bitmap = 0;
    }

    _width = 0;
    _height = 0;

    _physicalSize.set(0, 0);
}


void GdiPixmapImpl::reset(const Gfx::Image& image)
{
    size_t width = image.width();
    size_t height = image.height();

    Gfx::SizeF size(width, height);
    reset(size);

    const Pt::uint8_t* data = image.data();
    Gfx::Rgb32Image rgb32Image;

    if(image.format() != Gfx::ImageFormat::rgb32() || image.padding() != 0)
    {
        rgb32Image.reset(width, height);
        Gfx::copyView(image, rgb32Image);
        data = rgb32Image.data();
    }

    const size_t depth = 32;

    HBITMAP bitmap = CreateBitmap(image.width(), image.height(), 
                                  1, depth, (VOID*)data);

    if (bitmap == NULL)
    {
        BITMAPINFO bitmapInfo;
        ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

        bitmapInfo.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
        bitmapInfo.bmiHeader.biWidth        = width;
        bitmapInfo.bmiHeader.biHeight       = -(ssize_t)height; // top-down image
        bitmapInfo.bmiHeader.biPlanes       = 1;                        // always 1            
        bitmapInfo.bmiHeader.biBitCount     = static_cast<WORD>(depth); // bits per pixel
        bitmapInfo.bmiHeader.biCompression  = BI_RGB;                   // uncompressed RGB
        bitmapInfo.bmiHeader.biSizeImage    = 0;                        // automatic
        bitmapInfo.bmiHeader.biClrUsed      = 0;                        // no color table
        bitmapInfo.bmiHeader.biClrImportant = 0;                        // no color table

        VOID* imageBits = 0;
        bitmap = CreateDIBSection(_dc, &bitmapInfo, DIB_RGB_COLORS, 
                                  &imageBits, NULL, 0);
        memcpy(imageBits, data, image.width() * image.height() * 4);
    }

    HDC bitmapDC = CreateCompatibleDC(NULL);
    SelectObject(bitmapDC, bitmap);

    BitBlt(_dc, 0, 0, image.width(), image.height(), bitmapDC, 0, 0, SRCCOPY);

    DeleteDC(bitmapDC);
    DeleteObject(bitmap);
}


Gfx::Image GdiPixmapImpl::toImage() const
{
    BITMAPINFO bitmapInfo;
    ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = _width;
    bitmapInfo.bmiHeader.biHeight = -(ssize_t)_height;  // top-down image
    bitmapInfo.bmiHeader.biPlanes = 1;                         // always 1
    bitmapInfo.bmiHeader.biBitCount = 32;                      // bits per pixel
    bitmapInfo.bmiHeader.biCompression = BI_RGB;               // uncompressed RGB
    bitmapInfo.bmiHeader.biSizeImage = 0;                      // automatic
    bitmapInfo.bmiHeader.biClrUsed = 0;                        // no color table
    bitmapInfo.bmiHeader.biClrImportant = 0;                   // no color table

    Pt::Gfx::Image image(_width, _height, Pt::Gfx::Rgb32());
    Pt::uint8_t* data = image.data();

    int ret = GetDIBits(_dc, _bitmap, 0, _height, data, 
                        &bitmapInfo, DIB_RGB_COLORS);

    Gfx::Argb32PixelView pixels(data, _width, _height);
    for( Gfx::Argb32PixelView::Pixel& p : pixels )
        p.setAlpha(255);

    return image;
}


void GdiPixmapImpl::getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const
{
    bitmap.reset( rect.size() );

    Gfx::Image image = this->toImage();

    Gfx::PaintContext ctx(bitmap);
    Gfx::Painter painter(ctx);
    painter.drawImage(Gfx::PointF(0, 0), image, rect);
}


const Gfx::SizeF& GdiPixmapImpl::size() const
{
    return _physicalSize;
}


void GdiPixmapImpl::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);
}


HDC GdiPixmapImpl::deviceContext() const
{
    return _dc;
}


const Gfx::ImageFormat& GdiPixmapImpl::format() const
{
    return Gfx::ImageFormat::rgb32();
}


const Gfx::Scaling& GdiPixmapImpl::scaling() const
{
    return _scaling;
}


Gfx::Canvas* GdiPixmapImpl::createCanvas(Gfx::Canvas* reuse)
{
    GdiPixmapCanvas* canvas = dynamic_cast<GdiPixmapCanvas*>(reuse);
    if( ! canvas ) 
        canvas  = new GdiPixmapCanvas();
    
    canvas->setPixmap(*this);

    _canvas = canvas;
    return _canvas;
}


void GdiPixmapImpl::releaseCanvas()
{
    // NOTE: this might be called from the attached canvas base class destructor

    ModifyWorldTransform(_dc, NULL, MWT_IDENTITY);
    SelectObject(_dc, GetStockObject(BLACK_PEN));
    SelectObject(_dc, GetStockObject(WHITE_BRUSH));
    SelectObject(_dc, GetStockObject(SYSTEM_FONT));
    SelectClipRgn(_dc, NULL);
    AbortPath(_dc);

    _canvas = 0;
}


void GdiPixmapImpl::sync()
{
}


void GdiPixmapImpl::finish()
{
}


void GdiPixmapImpl::drawPixmap(Gfx::Canvas& canvas,
                            const Gfx::PointF& to,
                            const Pixmap& pm,
                            const Gfx::RectF* rect)
{
    assert(_canvas == &canvas);

    if(_canvas == &canvas)
        _canvas->drawPixmap(to, pm, rect);
}

} // namespace

} // namespace

