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

using std::max;
using std::min;
#include <Gdiplus.h>

namespace {

HBRUSH gradientBrush(HDC dc, int width, int height,
                     Pt::Gfx::Color gradientStart, 
                     Pt::Gfx::Color gradientStop, 
                     Pt::Gfx::Brush::GradientStyle gradient)
{

    BITMAPINFO bi;
    ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));

    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    bi.bmiHeader.biPlanes       = 1;         // always 1
    bi.bmiHeader.biBitCount     = 32;        // ARGB 32
    bi.bmiHeader.biCompression  = BI_RGB;    // uncompressed RGB
    bi.bmiHeader.biSizeImage    = 0;         // automatic
    bi.bmiHeader.biClrUsed      = 0;         // no color table
    bi.bmiHeader.biClrImportant = 0;         // no color table 
    
    if( gradient == Pt::Gfx::Brush::Horizontal )
    {
        bi.bmiHeader.biWidth    = width;
        bi.bmiHeader.biHeight   = 1;
    }
    else // Pt::Gfx::Brush::Vertical
    {
        bi.bmiHeader.biWidth    = 1;
        bi.bmiHeader.biHeight   = height;

        std::swap(gradientStart, gradientStop);
    }

    int length = bi.bmiHeader.biWidth + bi.bmiHeader.biHeight - 1;

    VOID* imageBits = NULL;
    HBITMAP bitmap = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &imageBits, NULL, 0);

    Pt::uint8_t* pixel = reinterpret_cast<Pt::uint8_t*>(imageBits);
            
    for(int n = 0; n < length; ++n)
    {
        float f1 = (length - n) / float(length);
        float f2 = n / float(length);

        float r1 = gradientStart.red() * f1;
        float r2 = gradientStop.red() * f2;

        float g1 = gradientStart.green() * f1;
        float g2 = gradientStop.green() * f2;

        float b1 = gradientStart.blue() * f1;
        float b2 = gradientStop.blue() * f2;
                
        pixel[0] = static_cast<Pt::uint8_t>( (b1 + b2) / 257 );
        pixel[1] = static_cast<Pt::uint8_t>( (g1 + g2) / 257 );
        pixel[2] = static_cast<Pt::uint8_t>( (r1 + r2) / 257 );
        pixel[3] = 0;

        pixel += 4;
    }

    HBRUSH brush = CreatePatternBrush(bitmap);
    DeleteObject(bitmap);
    
    return brush;
}

} // namespace

namespace Pt {

namespace Hmi {

PixmapSurfaceImpl::PixmapSurfaceImpl()
: _painter(0)
, _dc(0)
, _gradientBrush(false)
{
    _size = Gfx::SizeF(10 ,10);

    HDC screenDC = GetDC(NULL);
    _dc = CreateCompatibleDC(screenDC);
    _bitmap = CreateCompatibleBitmap(screenDC, lround(_size.width()), 
                                               lround(_size.height()));
    ReleaseDC(NULL, screenDC);

    _oldPen    = (HPEN) GetCurrentObject(_dc, OBJ_PEN);
    _oldBrush  = (HBRUSH) GetCurrentObject(_dc, OBJ_BRUSH);
    _oldFont   = (HFONT) GetCurrentObject(_dc, OBJ_FONT);
    _oldBitmap = (HBITMAP) GetCurrentObject(_dc, OBJ_BITMAP);
    
    SelectObject(_dc, _bitmap);
    SetBkMode(_dc, TRANSPARENT);

    SetGraphicsMode(_dc, GM_ADVANCED);
}


PixmapSurfaceImpl::~PixmapSurfaceImpl()
{
    SelectObject(_dc, _oldBitmap);

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
    
    HDC screenDC = GetDC(NULL);
    HBITMAP bitmap = CreateCompatibleBitmap(screenDC, lround( _size.width() ), 
                                                      lround( _size.height() ) );
    ReleaseDC(NULL, screenDC);

    SelectObject(_dc, bitmap);
    
    DeleteObject(_bitmap);
    _bitmap = bitmap;
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

    SelectObject(_dc, _oldPen);
    SelectObject(_dc, _oldBrush);
    SelectObject(_dc, _oldFont);
}


const Gfx::ImageFormat& PixmapSurfaceImpl::format() const
{
    return Gfx::ImageFormat::argb32();
}


void PixmapSurfaceImpl::setClip(const Gfx::RectF& clipRect)
{
    _painter->impl()->setClip(clipRect);
    
    HRGN hrgn = _painter->impl()->clipRect();
    if(hrgn)
        SelectClipRgn(_dc, hrgn);
    else
        SelectClipRgn(_dc, NULL);
}


void PixmapSurfaceImpl::resetClip()
{
    _painter->impl()->resetClip();
    SelectClipRgn(_dc, NULL);
}


void PixmapSurfaceImpl::setCompositionMode(const Gfx::CompositionMode& mode)
{

}


void PixmapSurfaceImpl::setPen(const Gfx::Pen& pen)
{
    HPEN hpen = _painter->impl()->pen();
    if(hpen)
        SelectObject(_dc, hpen);
    
    DWORD penColor = _painter->impl()->penColor();
    SetTextColor(_dc, penColor);
}


void PixmapSurfaceImpl::setBrush(const Gfx::Brush& brush)
{
    _gradientBrush = false;

    if( _painter->impl()->gradientBrush() )
    {
        _gradientBrush = true;
        _gradient = brush.gradient();
        _gradientStart = brush.color();
        _gradientStop = brush.gradientColor();

        // do not set a brush now, because the gradient brush pattern can
        // only be calculated later, when the fill area is known
        return;
    }

    HBRUSH hbrush = _painter->impl()->brush();
    if(hbrush)
        SelectObject(_dc, hbrush);
}


void PixmapSurfaceImpl::setFont(const Gfx::Font& font)
{
    HFONT hfont = _painter->impl()->font();
    if(hfont)
        SelectObject(_dc, hfont);

    SetTextAlign(_dc, TA_BASELINE | TA_LEFT | TA_NOUPDATECP);
}

#ifndef PT_HMI_GDIPLUS
Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Pt::String& text) const
{
    TEXTMETRIC tm;
    GetTextMetrics(_dc, &tm);

    std::wstring wtext;
    text.toUtf16( std::back_inserter(wtext) );
    
    SIZE textSize;
    GetTextExtentPoint32W(_dc, wtext.c_str(), wtext.size(), &textSize);
    
    return Gfx::FontMetrics(tm.tmAscent, 
                            tm.tmDescent, 
                            textSize.cx, 
                            tm.tmHeight);
}

#else
Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Pt::String& text) const
{
    std::wstring wtext;
    text.toUtf16( std::back_inserter(wtext) );

    Gdiplus::Font gdiFont(_dc);
    Gdiplus::Graphics graphics(_dc);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);
    graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);

    const Gdiplus::StringFormat* format = Gdiplus::StringFormat::GenericTypographic();

    Gdiplus::FontFamily family;
    gdiFont.GetFamily(&family);

    Gdiplus::REAL height = gdiFont.GetHeight( graphics.GetDpiY() );

    UINT16 ascentUnits = family.GetCellAscent( gdiFont.GetStyle() );
    UINT16 descentUnits = family.GetCellDescent( gdiFont.GetStyle() );
    UINT16 heightUnits = family.GetLineSpacing( gdiFont.GetStyle() );

    Gdiplus::REAL pixelsPerUnit = height / heightUnits;
    Gdiplus::REAL ascentF = ascentUnits * pixelsPerUnit;
    Gdiplus::REAL descentF = descentUnits * pixelsPerUnit;

    Gdiplus::RectF textRect;
    graphics.MeasureString(wtext.c_str(), wtext.size(), &gdiFont, 
                            Gdiplus::PointF(0, 0), format, &textRect);

    const int dpix = GetDeviceCaps(_dc, LOGPIXELSX);
    const double scaling = 96.0 / dpix;

    return Gfx::FontMetrics(ascentF* scaling, descentF* scaling, 
                            textRect.Width* scaling, textRect.Height* scaling);
}
#endif

void PixmapSurfaceImpl::drawText(const Gfx::PointF& to, 
                                 const Pt::String& text, 
                                 const Gfx::Transform& trans)
{
    _text.clear();
    text.toUtf16(std::back_inserter(_text));

    const int dpix = GetDeviceCaps(_dc, LOGPIXELSX);
    const double scaling = 96.0 / dpix;

    Gfx::Transform tt = trans;

#ifndef PT_HMI_GDIPLUS
    tt.translate(to.x(), to.y());

    XFORM oldTrans = { 1, 0, 0, 1, 0 , 0 };
    GetWorldTransform(_dc, &oldTrans);

    XFORM newTrans = { static_cast<FLOAT>( tt.m11() ), 
                       static_cast<FLOAT>( tt.m12() ),
                       static_cast<FLOAT>( tt.m21() ), 
                       static_cast<FLOAT>( tt.m22() ),
                       static_cast<FLOAT>( tt.dx() ),  
                       static_cast<FLOAT>( tt.dy() ) };

    SetWorldTransform(_dc, &newTrans);

    TextOutW(_dc, 0, 0, _text.c_str(), _text.size());

    SetWorldTransform(_dc, &oldTrans);
#else
    tt.scale(scaling, scaling);
    tt.translate(to.x(), to.y());

    Gdiplus::Graphics graphics(_dc);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);
    graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);

    Gdiplus::Font font(_dc);

    Gdiplus::FontFamily family;
    font.GetFamily(&family);

    Gdiplus::REAL height = font.GetHeight( graphics.GetDpiY() );

    UINT16 ascentUnits = family.GetCellAscent( font.GetStyle() );
    UINT16 descentUnits = family.GetCellDescent( font.GetStyle() );
    UINT16 heightUnits = family.GetLineSpacing( font.GetStyle() );
    Gdiplus::REAL pixelsPerUnit = height / heightUnits;

    Gdiplus::REAL ascent = ascentUnits * pixelsPerUnit;
    Gdiplus::REAL descent = descentUnits * pixelsPerUnit;
    Gdiplus::REAL spacing = height - ascent - descent;
    Gdiplus::REAL offsetY = ascent + 1;
    
    Gdiplus::REAL toX = static_cast<Gdiplus::REAL>( to.x() );
    Gdiplus::REAL toY = static_cast<Gdiplus::REAL>( to.y() );
    Gdiplus::PointF origin( 0, -offsetY );
    
    const Gdiplus::StringFormat* format = Gdiplus::StringFormat::GenericTypographic();

    Gdiplus::Matrix oldMatrix;
    graphics.GetTransform(&oldMatrix);

    Gdiplus::Matrix matrix( static_cast<Gdiplus::REAL>( tt.m11() ), 
                            static_cast<Gdiplus::REAL>( tt.m12() ),
                            static_cast<Gdiplus::REAL>( tt.m21() ), 
                            static_cast<Gdiplus::REAL>( tt.m22() ),
                            static_cast<Gdiplus::REAL>( tt.dx() ), 
                            static_cast<Gdiplus::REAL>( tt.dy() ) );

    graphics.SetTransform(&matrix);

    //Gdiplus::RectF textRect;
    //graphics.MeasureString(_text.c_str(), _text.size(), &font, 
    //                       Gdiplus::PointF(to.x(), to.y()), format, &textRect);

    //Gdiplus::Pen blackPen( Gdiplus::Color(255, 0, 0, 0), 1 );
    //graphics.DrawRectangle(&blackPen, textRect);

    const Gfx::Color& color = _painter->pen().color();
    BYTE alpha = color.alpha() / 257;
    BYTE red   = color.red()   / 257;
    BYTE green = color.green() / 257; 
    BYTE blue  = color.blue()  / 257;

    Gdiplus::SolidBrush blackBrush( Gdiplus::Color(alpha, red, green, blue) );

    graphics.DrawString( _text.c_str(), _text.size(), &font, 
                         origin, format, &blackBrush);

    graphics.SetTransform(&oldMatrix);
#endif
}


void PixmapSurfaceImpl::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    POINT points[2];
    
    points[0].x = lround( from.x() - 0.4999 );
    points[0].y = lround( from.y() - 0.4999 );
    
    points[1].x = lround( to.x() - 0.4999 );
    points[1].y = lround( to.y() - 0.4999 );

    Polyline(_dc, points, 2);
}


void PixmapSurfaceImpl::drawRect(const Gfx::RectF& rect)
{
    HBRUSH originalBrush = (HBRUSH) SelectObject(_dc, GetStockObject(NULL_BRUSH));

    Rectangle(_dc, lround(rect.left()   - 0.4999), 
                   lround(rect.top()    - 0.4999), 
                   lround(rect.right()  - 0.4999), 
                   lround(rect.bottom() - 0.4999));

    SelectObject(_dc, originalBrush);
}


void PixmapSurfaceImpl::fillRect(const Gfx::RectF& rect)
{
    RECT rectangle;
    rectangle.left   =  lround( rect.left() );
    rectangle.top    =  lround( rect.top() );
    rectangle.right  =  lround( rect.right() + 0.001 );    
    rectangle.bottom =  lround( rect.bottom() + 0.001 );

    if(_gradientBrush)
    {
        HBRUSH brush = gradientBrush(_dc, lround(rect.width()), lround(rect.height()),
                                     _gradientStart, _gradientStop, _gradient);

        POINT brushOrigin = {0};
        SetBrushOrgEx(_dc, lround(rect.x()),  lround(rect.y()), &brushOrigin);

        FillRect(_dc, &rectangle, brush);

        SetBrushOrgEx(_dc, brushOrigin.x, brushOrigin.y, NULL);
        DeleteObject(brush);
        return;
    }

    HBRUSH currentBrush = (HBRUSH) GetCurrentObject(_dc, OBJ_BRUSH);
    FillRect(_dc, &rectangle, currentBrush);
}


void PixmapSurfaceImpl::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    HBRUSH originalBrush = (HBRUSH)SelectObject(_dc, GetStockObject(NULL_BRUSH));

    Ellipse( _dc, lround( topLeft.x()),  
                  lround( topLeft.y()), 
                  lround( topLeft.x() + size.width() -1), 
                  lround( topLeft.y() + size.height() -1 ));

    SelectObject(_dc, originalBrush);
}


void PixmapSurfaceImpl::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    POINT brushOrigin = {0};

    HGDIOBJ oldBrush = 0;

    if(_gradientBrush)
    {
        HBRUSH brush = gradientBrush(_dc, lround( size.width() ), 
                                          lround( size.height() ),
                                          _gradientStart, 
                                          _gradientStop, 
                                          _gradient);

        oldBrush = SelectObject(_dc, brush);

        SetBrushOrgEx(_dc, lround(topLeft.x()), lround(topLeft.y()), &brushOrigin);
    }

    HPEN originalPen = (HPEN) SelectObject(_dc, GetStockObject(NULL_PEN));

    Ellipse( _dc, lround( topLeft.x() ),
                  lround( topLeft.y() ),
                  lround( topLeft.x() + size.width() - 1),
                  lround( topLeft.y() + size.height() - 1) );

    SelectObject(_dc, originalPen);

    if(_gradientBrush)
    {
        HBRUSH brush = (HBRUSH) SelectObject(_dc, oldBrush);
        DeleteObject(brush);

        SetBrushOrgEx(_dc, brushOrigin.x, brushOrigin.y, NULL);
    }
}


void PixmapSurfaceImpl::drawPolyline(const Gfx::PointF* ps, const size_t n)
{
    std::vector<POINT> points(n);

    for(unsigned i = 0; i < n; i++)
    {
        points[i].x = Pt::lround(ps[i].x() - 0.4999);
        points[i].y = Pt::lround(ps[i].y() - 0.4999);
    }

    Polyline( _dc, &points[0], points.size() );
}


void PixmapSurfaceImpl::fillPolygon(const Gfx::PointF* ps, const size_t n)
{
    if( ! n ) 
        return;

    POINT brushOrigin = {0};

    int left = std::numeric_limits<int>::max();
    int top = std::numeric_limits<int>::max();
    int right = 0;
    int bottom = 0;

    std::vector<POINT> points(n);

    for(size_t i = 0; i < n; i++)
    {
        Gfx::Point p( Pt::lround(ps[i].x() - 0.4999),
                      Pt::lround(ps[i].y() - 0.4999) );

        points[i].x = p.x();
        points[i].y = p.y();

        if( p.y() < top)
            top = p.y();

        if( p.y() > bottom)
            bottom = p.y();

        if( p.x() < left)
            left = p.x();

        if( p.x() > right)
            right = p.x();
    }

    HGDIOBJ oldBrush = 0;

    if(_gradientBrush)
    {
        HBRUSH brush = gradientBrush(_dc, right - left, bottom - top,
                                     _gradientStart, _gradientStop, _gradient);

        oldBrush = SelectObject(_dc, brush);

        SetBrushOrgEx(_dc, left, top, NULL);
    }

    HPEN originalPen = (HPEN) SelectObject(_dc, GetStockObject(NULL_PEN) );
    
    Polygon(_dc, &points[0], points.size());
    
    SelectObject(_dc, originalPen);

    if(_gradientBrush)
    {
        HBRUSH brush = (HBRUSH) SelectObject(_dc, oldBrush);
        DeleteObject(brush);

        SetBrushOrgEx(_dc, brushOrigin.x, brushOrigin.y, NULL);
    }
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


HDC PixmapSurfaceImpl::deviceContext() const
{
    return _dc;
}

} // namespace

} // namespace
