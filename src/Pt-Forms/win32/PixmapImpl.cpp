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
#include "PixmapImpl.h"
#include "PixmapCanvas.h"
#include "GdiFontProvider.h"

#include <Pt/Forms/View.h>
#include <Pt/Forms/Pixmap.h>

#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Rgb32.h>

#include <cassert>

namespace Pt {

namespace Forms {

#ifdef PT_FORMS_WIN32_RASTER

void PixmapImpl::drawPixmap(const Pt::Gfx::PointF& to,
                            const Pixmap& pixmap,
                            const Gfx::Paint& paint,
                            const Gfx::RectF* rect)
{
    const Gfx::Bitmap& bitmap = pixmap.impl()->_bitmap;
    _bitmap.drawBitmap(to, bitmap, paint, rect);
}


void PixmapImpl::drawPixmap(Gfx::Canvas& canvas,
                            const Pt::Gfx::PointF& to,
                            const Pixmap& pixmap,
                            const Gfx::RectF* rect)
{
    assert(_canvas == &canvas);

    if(_canvas == &canvas)
    {
        const Gfx::Image& image = pixmap.impl()->_bitmap.image();
        _canvas->drawImage(to, image, rect);
    }
}

#else // PT_FORMS_WIN32_RASTER

PixmapImpl::PixmapImpl()
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


PixmapImpl::~PixmapImpl()
{
    if(_bitmap)
    {
        SelectObject(_dc, _oldBitmap);
        DeleteObject(_bitmap);
    }

    DeleteDC(_dc);
}


void PixmapImpl::reset(const Gfx::SizeF& size)
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


void PixmapImpl::reset()
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


void PixmapImpl::reset(const Gfx::Image& image)
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


Gfx::Image PixmapImpl::toImage() const
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

    return image;
}


void PixmapImpl::getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const
{
    bitmap.reset( rect.size() );

    Gfx::Image image = this->toImage();

    Gfx::Painter painter(bitmap);
    painter.drawImage(Gfx::PointF(0, 0), image, rect);
}


const Gfx::SizeF& PixmapImpl::size() const
{
    return _physicalSize;
}


void PixmapImpl::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);
}


HDC PixmapImpl::deviceContext() const
{
    return _dc;
}


const Gfx::ImageFormat& PixmapImpl::format() const
{
    return Gfx::ImageFormat::rgb32();
}


const Gfx::Scaling& PixmapImpl::scaling() const
{
    return _scaling;
}


Gfx::Canvas* PixmapImpl::createCanvas(Gfx::Canvas* reuse)
{
    PixmapCanvas* canvas = dynamic_cast<PixmapCanvas*>(reuse);
    if( ! canvas ) 
        canvas  = new PixmapCanvas();
    
    canvas->setPixmap(*this);

    _canvas = canvas;
    return _canvas;
}


void PixmapImpl::releaseCanvas()
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


void PixmapImpl::sync()
{
}


void PixmapImpl::finish()
{
}


void PixmapImpl::drawPixmap(const Gfx::PointF& toF,
                              const Pixmap& pm,
                              const Gfx::Paint& paint,
                              const Gfx::RectF* rect)
{
    if( ! _bitmap)
        return;

    const PixmapImpl* pixmap = pm.impl();
    Gfx::PointF to = _scaling.toPhysical(toF);

    int fromX = 0;
    int fromY = 0;
    int width = lround( pixmap->size().width() );
    int height = lround( pixmap->size().height() );

    if(rect)
    {
        const Gfx::Scaling& scaling = pixmap->scaling();
        Gfx::RectF rectP = scaling.toPhysical(*rect);
        
        fromX = lround( rectP.x() );
        fromY = lround( rectP.y()) ;
        width = lround( rectP.width() );
        height = lround( rectP.height() );
    }

    Gfx::CompositionMode compositionMode = paint.compositionMode();
    HDC pixmapDC = pixmap->deviceContext();

    if(_canvas)
        _canvas->suspend();

    switch(compositionMode)
    {
        case Gfx::CompositionMode::SourceCopy:
        {
            BitBlt(_dc, lround(to.x()), lround(to.y()), width, height,
                   pixmapDC, fromX, fromY, SRCCOPY);
        }
        break;

        case Gfx::CompositionMode::SourceOver:
        {
            BLENDFUNCTION bf;
            bf.BlendOp = AC_SRC_OVER;
            bf.BlendFlags = 0;
            bf.SourceConstantAlpha = 0xFF; // only per pixel alpha
            bf.AlphaFormat = AC_SRC_ALPHA;

            AlphaBlend(_dc, lround(to.x()), lround(to.y()), width, height,
                       pixmapDC, fromX, fromY, width, height, bf);
        }
        break;
    }

    if(_canvas)
        _canvas->resume();
}


void PixmapImpl::drawPixmap(Gfx::Canvas& canvas,
                            const Gfx::PointF& to,
                            const Pixmap& pm,
                            const Gfx::RectF* rect)
{
    assert(_canvas == &canvas);

    if(_canvas == &canvas)
        _canvas->drawPixmap(to, pm, rect);
}


const std::string& PixmapImpl::defaultFont()
{
    return GdiFontProvider::instance().defaultFont();
}


void PixmapImpl::setDefaultFont(const std::string& family)
{
    GdiFontProvider::instance().setDefaultFont(family);
}


std::vector<std::string> PixmapImpl::fontFamilies()
{
    return GdiFontProvider::instance().fontFamilies();
}


std::vector<Gfx::FontFace> PixmapImpl::fontFaces(const std::string& family)
{
    return GdiFontProvider::instance().fontFaces(family);
}

#endif // PT_FORMS_WIN32_RASTER

} // namespace

} // namespace
