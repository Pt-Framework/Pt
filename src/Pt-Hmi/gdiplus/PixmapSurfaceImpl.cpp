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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#include "win32.h"
#include "PainterImpl.h"
#include "PaintSurfaceImpl.h"
#include "PixmapSurfaceImpl.h"
#include "PictureImpl.h"
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Gfx/Argb32Format.h>

namespace Pt {

namespace Hmi {

PixmapSurfaceImpl::PixmapSurfaceImpl()
: _painter(0)
, _dc(0)
, _graphics(0)
{
    _size = Gfx::SizeF(10 ,10);

    HDC screenDC = GetDC(NULL);
    _dc = CreateCompatibleDC(screenDC);

    _bitmap = CreateCompatibleBitmap(screenDC, lround(_size.width()), 
                                               lround(_size.height()));
    ReleaseDC(NULL, screenDC);

    SelectObject(_dc, _bitmap);
    SetBkMode(_dc, TRANSPARENT);

    SetGraphicsMode(_dc, GM_ADVANCED);

    _graphics = new Gdiplus::Graphics(_dc);

    _graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);
    _graphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
}


PixmapSurfaceImpl::~PixmapSurfaceImpl()
{
    delete _graphics;
    DeleteDC(_dc);
    DeleteObject(_bitmap);
}


void PixmapSurfaceImpl::clear(const Gfx::Color& c)
{
}


void PixmapSurfaceImpl::resize(const Gfx::SizeF& size)
{
    if( _size == size )
        return;

    _size = size;

    DeleteObject(_bitmap);
    DeleteDC(_dc);

    HDC screenDC = GetDC(NULL);
    HBITMAP bitmap = CreateCompatibleBitmap(screenDC, lround( _size.width() ), 
                                                      lround( _size.height() ) );

    _dc = CreateCompatibleDC(screenDC);
    ReleaseDC(NULL, screenDC);

    SelectObject(_dc, bitmap);
    _bitmap = bitmap;

    delete _graphics;
    _graphics = new Gdiplus::Graphics(_dc);
    _graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);
    _graphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
}


const Gfx::SizeF& PixmapSurfaceImpl::size() const
{
    return _size;
}


void PixmapSurfaceImpl::begin(Painter& painter)
{
    _painter = &painter;
}


void PixmapSurfaceImpl::finish()
{
    _painter = 0;
}


const Gfx::ImageFormat& PixmapSurfaceImpl::format() const
{
    return Gfx::ImageFormat::argb32();
}


void PixmapSurfaceImpl::setClip(const Gfx::RectF& clipRect)
{
    if( clipRect.isNull() )
        _graphics->ResetClip();
    else
        _graphics->SetClip(PainterImpl::toGdi(clipRect));
}


void PixmapSurfaceImpl::resetClip()
{
    _graphics->ResetClip();
}


void PixmapSurfaceImpl::setCompositionMode(const Gfx::CompositionMode& mode)
{
}


void PixmapSurfaceImpl::setPen(const Gfx::Pen& pen)
{
}


void PixmapSurfaceImpl::setBrush(const Gfx::Brush& brush)
{
}


void PixmapSurfaceImpl::setFont(const Gfx::Font& font)
{
}


Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Pt::String& text) const
{
    std::wstring wtext;
    text.toUtf16( std::back_inserter(wtext) );

    const Gdiplus::StringFormat* format = Gdiplus::StringFormat::GenericTypographic();
    const Gdiplus::Font& font = _painter->impl()->font();
    const Gdiplus::FontFamily& family = _painter->impl()->fontFamily();

    Gdiplus::REAL height = font.GetHeight( _graphics->GetDpiY() );

    UINT16 ascentUnits = family.GetCellAscent( font.GetStyle() );
    UINT16 descentUnits = family.GetCellDescent( font.GetStyle() );
    UINT16 heightUnits = family.GetLineSpacing( font.GetStyle() );

    Gdiplus::REAL pixelsPerUnit = height / heightUnits;
    Gdiplus::REAL ascentF = ascentUnits * pixelsPerUnit;
    Gdiplus::REAL descentF = descentUnits * pixelsPerUnit;

    Gdiplus::RectF textRect;
    _graphics->MeasureString(wtext.c_str(), wtext.size(), &font,
                             Gdiplus::PointF(0, 0), format, &textRect);

    const int dpix = GetDeviceCaps(_dc, LOGPIXELSX);
    const double scaling = 96.0 / dpix;

    return Gfx::FontMetrics(ascentF*scaling, descentF* scaling, 
                            textRect.Width* scaling, textRect.Height* scaling);
}


void PixmapSurfaceImpl::drawText(const Gfx::PointF& to, 
                                 const Pt::String& text, 
                                 const Gfx::Transform& trans)
{
    _text.clear();
    text.toUtf16(std::back_inserter(_text));

    Gfx::Transform tt = trans;    

    const int dpix = GetDeviceCaps(_dc, LOGPIXELSX);
    const double scaling = 96.0 / dpix;

    tt.scale(scaling, scaling);
    tt.translate( to.x(), to.y());

    const Gdiplus::Font& font = _painter->impl()->font();
    const Gdiplus::FontFamily& family = _painter->impl()->fontFamily();
    
    Gdiplus::REAL height = font.GetHeight(_graphics->GetDpiY() );

    UINT16 ascentUnits = family.GetCellAscent( font.GetStyle() );
    UINT16 descentUnits = family.GetCellDescent( font.GetStyle() );
    UINT16 heightUnits = family.GetLineSpacing( font.GetStyle() );
    Gdiplus::REAL pixelsPerUnit = height / heightUnits;

    Gdiplus::REAL ascent = ascentUnits * pixelsPerUnit;
    Gdiplus::REAL descent = descentUnits * pixelsPerUnit;
    Gdiplus::REAL spacing = height - ascent - descent;
    Gdiplus::REAL offsetY = ascent + 1;
    
    Gdiplus::PointF origin( 0, -offsetY );

    const Gdiplus::StringFormat* format = Gdiplus::StringFormat::GenericTypographic();

    Gdiplus::Matrix oldMatrix;
    _graphics->GetTransform(&oldMatrix);

    Gdiplus::Matrix matrix( static_cast<Gdiplus::REAL>( tt.m11() ), 
                            static_cast<Gdiplus::REAL>( tt.m12() ),
                            static_cast<Gdiplus::REAL>( tt.m21() ), 
                            static_cast<Gdiplus::REAL>( tt.m22() ),
                            static_cast<Gdiplus::REAL>( tt.dx() ), 
                            static_cast<Gdiplus::REAL>( tt.dy() ) );

    _graphics->SetTransform(&matrix);

    const Gfx::Color& color = _painter->pen().color();
    BYTE alpha = color.alpha() / 257;
    BYTE red   = color.red()   / 257;
    BYTE green = color.green() / 257; 
    BYTE blue  = color.blue()  / 257;

    Gdiplus::SolidBrush brush( Gdiplus::Color(alpha, red, green, blue) );

    _graphics->DrawString( _text.c_str(), _text.size(), &font,
                         origin, format, &brush);

    _graphics->SetTransform(&oldMatrix);
}


void PixmapSurfaceImpl::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    const Gdiplus::Pen& pen = _painter->impl()->pen();
    _graphics->DrawLine(&pen, PainterImpl::toGdi(from), PainterImpl::toGdi(to));
}


void PixmapSurfaceImpl::drawRect(const Gfx::RectF& rect)
{
    const Gdiplus::Pen& pen = _painter->impl()->pen();
    _graphics->DrawRectangle(&pen, PainterImpl::toGdi(rect));
}


void PixmapSurfaceImpl::fillRect(const Gfx::RectF& rect)
{
    const Gdiplus::Brush& brush = _painter->impl()->brush();
    _graphics->FillRectangle(&brush, PainterImpl::toGdi(rect));
}


void PixmapSurfaceImpl::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    const Gdiplus::Pen& pen = _painter->impl()->pen();
    _graphics->DrawEllipse(&pen, PainterImpl::toGdi(Gfx::PointF(topLeft.x(), topLeft.y()), 
                                                    Gfx::SizeF(size.width(), size.height())));
}


void PixmapSurfaceImpl::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    const Gdiplus::Brush& brush = _painter->impl()->brush();
    _graphics->FillEllipse(&brush, PainterImpl::toGdi(Gfx::PointF(topLeft.x(), topLeft.y()), 
                                                      Gfx::SizeF(size.width(), size.height())));
}


void PixmapSurfaceImpl::drawPolyline(const Gfx::PointF* ps, const size_t n)
{
    if (!n)
        return;

    std::vector<Gdiplus::PointF> points(n);

    for(unsigned i = 0; i < n; i++)
        points[i] = PainterImpl::toGdi(ps[i]);

    const Gdiplus::Pen& pen = _painter->impl()->pen();
    _graphics->DrawLines(&pen, &points[0], n);
}


void PixmapSurfaceImpl::fillPolygon(const Gfx::PointF* ps, const size_t n)
{
    if (!n)
        return;

    std::vector<Gdiplus::PointF> points(n);

    for (unsigned i = 0; i < n; i++)
        points[i] = PainterImpl::toGdi(ps[i]);

     const Gdiplus::Brush& brush = _painter->impl()->brush();
    _graphics->FillPolygon(&brush, &points[0], n);
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& to, 
                                    const PixmapSurface& surface)
{
    const Gfx::Size size = Gfx::round( surface.size() );

    BitBlt( _dc, lround(to.x()), lround(to.y()), size.width(), size.height(), 
            surface.pixmapImpl()->deviceContext(), 0, 0, SRCCOPY);
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& to, 
                                    const PixmapSurface& pm, 
                                    const Gfx::RectF& pmRect)
{
    const Gfx::Size size = Gfx::round(pmRect.size());
    const Gfx::Point from = Gfx::round(pmRect.topLeft());

    BitBlt( _dc, lround(to.x()), lround(to.y()), size.width(), size.height(), 
            pm.pixmapImpl()->deviceContext(), from.x(), from.y(), SRCCOPY);
}


void PixmapSurfaceImpl::drawPicture(const Gfx::PointF& toF, const Picture& pic)
{
    const PictureImpl* picImpl = pic.impl();

    Gfx::Point to = Gfx::round(toF);

    if( picImpl->empty() )
      return;

    if( picImpl->mask() )
    {
        bitBlit(to, picImpl->width(), picImpl->height(), pic.impl()->mask(), SRCAND);
        bitBlit(to, picImpl->width(), picImpl->height(), pic.impl()->bitmap(), SRCPAINT);
        return;
    }

    // NOTE: it is impossible to use SourceCopy since picture is
    //       already premultiplied...

    HDC bitmapDC = CreateCompatibleDC(NULL);

    SelectObject( bitmapDC, picImpl->bitmap() );

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 0xFF; // only per pixel alpha
    bf.AlphaFormat = AC_SRC_ALPHA;    

    AlphaBlend(_dc, to.x(), to.y(), picImpl->width(), picImpl->height(),
                bitmapDC, 0, 0, picImpl->width(), picImpl->height(), bf);

    DeleteDC(bitmapDC);
}


void PixmapSurfaceImpl::bitBlit( const Gfx::Point& to, size_t width, size_t height, HBITMAP bitmap, DWORD op )
{
    HDC bitmapDC = CreateCompatibleDC(NULL);
    SelectObject(bitmapDC, bitmap);

    BitBlt(_dc,  to.x(), to.y(), width, height, bitmapDC, 0, 0, op);

    DeleteDC(bitmapDC);
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& to, 
                                  const Gfx::Image& image, 
                                  const Gfx::RectF& imgRect)
{
    // TODO
    throw std::runtime_error("not implemented");
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& toF, const Gfx::Image& image)
{
    Gfx::Point to = Gfx::round(toF);

    size_t depth = image.view().pixelStride() * 8; 
    const Pt::uint8_t* data = image.data();

    HBITMAP bitmap = CreateBitmap(image.width(), image.height(), 1, depth, (VOID*)data);
    if (bitmap == NULL) 
    {
        BITMAPINFO bitmapInfo;
        ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

        bitmapInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER); 
        bitmapInfo.bmiHeader.biWidth       = image.width();   
        bitmapInfo.bmiHeader.biHeight      = -(ssize_t)image.height(); // top-down image
        bitmapInfo.bmiHeader.biPlanes      = 1;                        // always 1
        bitmapInfo.bmiHeader.biBitCount    = static_cast<WORD>(depth); // bits per pixel
        bitmapInfo.bmiHeader.biCompression = BI_RGB;                   // uncompressed RGB
        bitmapInfo.bmiHeader.biSizeImage   = 0;                        // automatic
        bitmapInfo.bmiHeader.biClrUsed     = 0;                        // no color table
        bitmapInfo.bmiHeader.biClrImportant= 0;                        // no color table

        VOID* imageBits = 0;
        bitmap = CreateDIBSection(_dc, &bitmapInfo, 
                                  DIB_RGB_COLORS, &imageBits, NULL, 0);
        memcpy(imageBits, data, image.width() * image.height() * 4);
    }

    HDC bitmapDC = CreateCompatibleDC(NULL);
    SelectObject(bitmapDC, bitmap);

    BitBlt(_dc, 
           to.x(), to.y(), image.width(), image.height(), 
           bitmapDC, 0, 0, SRCCOPY);

    DeleteDC(bitmapDC);
    DeleteObject(bitmap);
}


void PixmapSurfaceImpl::drawPath(const Gfx::Path& path, float smoothness)
{
    Gdiplus::GraphicsPath gdiPath;
    
    //Todo::

    /*
    const Gdiplus::Pen& pen = _painter->impl()->pen();
    _graphics->DrawPath(&pen, &gdiPath);
    */
}

HDC PixmapSurfaceImpl::deviceContext() const
{
    return _dc;
}

} // namespace

} // namespace
