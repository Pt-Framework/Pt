/* Copyright (C) 2016 Marc Boris Duerner 
  
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

#include "PixmapCanvas.h"
#include "PixmapImpl.h"
#include "GdiFontProvider.h"
#include "win32.h"

#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Rgb32.h>

#include <cmath>
#include <limits>

namespace {

DWORD getPenStyle(const Pt::Gfx::Pen& pen)
{
  using namespace Pt;

    DWORD penStyle = PS_GEOMETRIC;

    switch( pen.style() )
    {
        case Gfx::Pen::Solid:
            penStyle |= PS_SOLID;
            break;
                
        case Gfx::Pen::Dash:
            penStyle |= PS_DASH;
            break;

        case Gfx::Pen::Dot:
            penStyle |= PS_DOT;
            break;
    }

    switch( pen.capStyle() )
    {
        case Gfx::Pen::FlatCap:
            penStyle |= PS_ENDCAP_FLAT;
            break;

        case Gfx::Pen::RoundCap:
            penStyle |= PS_ENDCAP_ROUND;
            break;

        case Gfx::Pen::SquareCap:
            penStyle |= PS_ENDCAP_SQUARE;
            break;
    }

    switch( pen.joinStyle() )
    {
        case Gfx::Pen::RoundJoin:
              penStyle |= PS_JOIN_ROUND;
            break;
                
        case Gfx::Pen::BevelJoin:
              penStyle |= PS_JOIN_BEVEL;
              break;

        case Gfx::Pen::MiterJoin:
              penStyle |= PS_JOIN_MITER;
              break;
    }

    return penStyle;
}


double lineScaleFactor(const Pt::Gfx::Transform& tx)
{
    return std::sqrt( std::abs(tx.determinant()) );
}


DWORD scalePenSize(DWORD penSize, double scaleFactor)
{
    return scaleFactor < 1.0 ? penSize
                             : static_cast<DWORD>( penSize * scaleFactor );
}

HBRUSH getGradientBrush(HDC dc, int width, int height,
                        Pt::Gfx::ColorF gradientStart, 
                        Pt::Gfx::ColorF gradientStop, 
                        Pt::Gfx::Brush::GradientStyle gradient)
{
    BITMAPINFO bi;
    ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));

    bi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biPlanes       = 1;
    bi.bmiHeader.biBitCount     = 32;
    bi.bmiHeader.biCompression  = BI_RGB;
    bi.bmiHeader.biSizeImage    = 0;
    bi.bmiHeader.biClrUsed      = 0;
    bi.bmiHeader.biClrImportant = 0;

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

    Pt::Gfx::Rgb32Image brushBuffer(static_cast<Pt::uint8_t*>(imageBits),
                                     bi.bmiHeader.biWidth,
                                     std::abs(bi.bmiHeader.biHeight), 0);

    Pt::Gfx::Rgb32PixelView pixelView(brushBuffer);
    Pt::Gfx::Rgb32PixelView::Iterator pixel = pixelView.begin();

    for(int n = 0; n < length; ++n, ++pixel)
    {
        float f1 = (length - n) / float(length);
        float f2 = n / float(length);

        Pt::uint8_t a = static_cast<Pt::uint8_t>((gradientStart.alpha() * f1 + gradientStop.alpha() * f2) / 257.0f);
        Pt::uint8_t r = static_cast<Pt::uint8_t>((gradientStart.red()   * f1 + gradientStop.red()   * f2) / 257.0f);
        Pt::uint8_t g = static_cast<Pt::uint8_t>((gradientStart.green() * f1 + gradientStop.green() * f2) / 257.0f);
        Pt::uint8_t b = static_cast<Pt::uint8_t>((gradientStart.blue()  * f1 + gradientStop.blue()  * f2) / 257.0f);

        *pixel = Pt::Gfx::Color(a, r, g, b);
    }

    HBRUSH brush = CreatePatternBrush(bitmap);
    DeleteObject(bitmap);

    return brush;
}


} // namespace

namespace Pt {

namespace Forms {

#ifndef PT_FORMS_WIN32_RASTER

PixmapCanvas::PixmapCanvas()
: Gfx::Canvas()
, _pixmap(0)
, _lastScaleFactor(1.0)
, _pen(0)
, _penSize(1)
, _penColor()
, _brush(0)
, _gradientBrush(false)
, _hasClip(false)
, _clipRect(0)
, _font(0)
{
}


PixmapCanvas::~PixmapCanvas()
{
    if(_pixmap)
    {
        HDC dc = _pixmap->deviceContext();
        SelectObject(dc, GetStockObject(BLACK_PEN));
        SelectObject(dc, GetStockObject(WHITE_BRUSH));
        SelectObject(dc, GetStockObject(SYSTEM_FONT));
        SelectClipRgn(dc, NULL);
    }

    if(_pen)
        DeleteObject(_pen);

    if(_brush)
        DeleteObject(_brush);

    if(_font)
        DeleteObject(_font);

    if(_clipRect)
        DeleteObject(_clipRect);
}


void PixmapCanvas::setPixmap(PixmapImpl& pixmap)
{
    _pixmap = &pixmap;
}


void PixmapCanvas::suspend()
{
    invalidate(DirtyAll);

    if(_pixmap)
        RestoreDC(_pixmap->deviceContext(), -1);
}


void PixmapCanvas::resume()
{
    if(_pixmap)
        SaveDC(_pixmap->deviceContext());
}


void PixmapCanvas::onBeginPaint(const Gfx::Paint& paint)
{
    if(_pixmap)
        SaveDC(_pixmap->deviceContext());
}


void PixmapCanvas::onFinishPaint()
{
    if(_pixmap)
        RestoreDC(_pixmap->deviceContext(), -1);

    _pixmap = 0;
}


void PixmapCanvas::onSetCompositionMode(const Gfx::CompositionMode& mode) 
{
    _compositionMode = mode;
}


void PixmapCanvas::onApplyCompositionMode() 
{
}


void PixmapCanvas::onApplyTransform()
{
}


void PixmapCanvas::onSetTransform(const Gfx::Transform& tx)
{
    if(_pixmap && isActive())
    {
        HDC dc = _pixmap->deviceContext();
        RestoreDC(dc, -1);
        SaveDC(dc);
        invalidate(DirtyAll & ~DirtyTransform);
    }

    double scaleFactor = lineScaleFactor(tx);

    if( std::abs(_lastScaleFactor - scaleFactor) >= 0.0001 )
    {
        _lastScaleFactor = scaleFactor;
        onSetPen(_logicalPen);
        invalidate(DirtyPen);
    }

    _fontMetrics = getFontMetrics();
}


void PixmapCanvas::onSetPen(const Gfx::Pen& pen)
{
    _logicalPen = pen;

    DWORD penSize = scalePenSize( static_cast<DWORD>( pen.size() ), _lastScaleFactor );

    if(_pen)
    {
        DeleteObject(_pen);
        _pen = 0;
    }

    _penSize = penSize;
    _penColor = pen.color();

    DWORD penStyle = getPenStyle(pen);

    DWORD color = RGB( _penColor.red()  / 257, 
                       _penColor.green() / 257, 
                       _penColor.blue()  / 257 );
                           
    LOGBRUSH brush;
    brush.lbStyle = BS_SOLID;
    brush.lbColor = color;

    _pen = ExtCreatePen(penStyle, penSize, &brush, 0, NULL);
}


void PixmapCanvas::onApplyPen()
{
    if(_pixmap)
    {
        DWORD penColor = RGB( _penColor.red()  / 257, 
                              _penColor.green() / 257, 
                              _penColor.blue()  / 257 );
        
        HDC dc = _pixmap->deviceContext();
        if(_pen)
            SelectObject(dc, _pen);

        SetTextColor(dc, penColor);
    }
}


void PixmapCanvas::onSetBrush(const Gfx::Brush& brush)
{
    if(_brush)
    {
        DeleteObject(_brush);
        _brush = 0;
    }
           
    _gradientBrush = false;

    DWORD brushColor = RGB(brush.color().red() / 257, 
                           brush.color().green() / 257, 
                           brush.color().blue() / 257);

    switch( brush.fillStyle() ) 
    {
        case Gfx::Brush::Solid: 
        {
            _brush = CreateSolidBrush(brushColor);
            break;
        }

        case Gfx::Brush::Texture: 
        {
            const Gfx::Image& texture = brush.texture();

            // use an empty brush for empty textures
            if(texture.width() == 0)
            {
                _brush = (HBRUSH) GetStockObject(NULL_BRUSH);
                break;
            }

            BITMAPINFO bi;
            ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));

            const Pt::uint8_t* texData = texture.data();
            Gfx::Rgb32Image rgb32Texture;

            if(texture.format() != Gfx::ImageFormat::rgb32() || texture.padding() != 0)
            {
                rgb32Texture.reset(texture.width(), texture.height());
                Gfx::copyView(texture, rgb32Texture);
                texData = rgb32Texture.data();
            }

            const size_t depth = 32;

            bi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);    
            bi.bmiHeader.biWidth        = texture.width();
            bi.bmiHeader.biHeight       = -(ssize_t)texture.height(); // top-down image
            bi.bmiHeader.biPlanes       = 1;                          // always 1
            bi.bmiHeader.biBitCount     = static_cast<WORD>(depth);   // bits per pixel
            bi.bmiHeader.biCompression  = BI_RGB;                     // uncompressed RGB
            bi.bmiHeader.biSizeImage    = 0;                          // automatic
            bi.bmiHeader.biClrUsed      = 0;                          // no color table
            bi.bmiHeader.biClrImportant = 0;                          // no color table

            HDC dc = GetDC(NULL);

            VOID* imageBits;
            HBITMAP bitmap = CreateDIBSection(dc, &bi, 
                                              DIB_RGB_COLORS, &imageBits, NULL, 0);
            memcpy(imageBits, texData, 
                    texture.width() * texture.height() * 4);

            _brush = CreatePatternBrush(bitmap);
            DeleteObject(bitmap);
            ReleaseDC(NULL, dc);
            break;     
        }
                
        case Gfx::Brush::Gradient:
        {
            _gradientBrush = true;
            _gradient = brush.gradient();
            _gradientStart = brush.color();
            _gradientStop = brush.gradientColor();
            break;
        }
                
        default:
            break;
    }
}


void PixmapCanvas::onApplyBrush()
{
    if( ! _pixmap )
        return;

    if( _gradientBrush )
    {
        // do not set a brush now, because the gradient brush pattern can
        // only be calculated later, when the fill area is known
        return;
    }

    HDC dc = _pixmap->deviceContext();

    if(_brush)
    {
        SelectObject(dc, _brush);
    }
}


void PixmapCanvas::onSetFont(const Gfx::Font& font)
{
    if(_font)
    {
        DeleteObject(_font);
        _font = 0;
    }

    _font = GdiFontProvider::instance().lookupFont(font);
    _fontMetrics = getFontMetrics();
}


void PixmapCanvas::onApplyFont()
{
    if( ! _pixmap )
        return;

    HDC dc = _pixmap->deviceContext();

    if(_font)
        SelectObject(dc, _font);

    SetTextAlign(dc, TA_BASELINE | TA_LEFT | TA_NOUPDATECP);
}


void PixmapCanvas::onSetClip(const Gfx::RectF* rectF)
{
    if(_pixmap && isActive())
    {
        HDC dc = _pixmap->deviceContext();
        RestoreDC(dc, -1);
        SaveDC(dc);
        invalidate(DirtyAll & ~DirtyClip);
    }

    _hasClip = rectF != 0;

    if(rectF)
        _clip = *rectF;
    else
        _clip.clear();
}


void PixmapCanvas::onApplyClip()
{  
    if(_pixmap)
    {
        if(_clipRect)
        {
            DeleteObject(_clipRect);
            _clipRect = NULL;
        }

        HDC dc = _pixmap->deviceContext();

        if(_hasClip)
        {
            Gfx::PointF origin = transform() * _clip.origin();
            Gfx::SizeF size = transform() * _clip.size();
            Gfx::RectF rectP(origin, size);

            long x = Pt::lround( rectP.x() );
            long y = Pt::lround( rectP.y() );
            long width = Pt::lround( rectP.width() );
            long height = Pt::lround( rectP.height() );

            _clipRect = CreateRectRgn(x, y, x + width, y + height);
        }

        if(_clipRect)
            SelectClipRgn(dc, _clipRect);
        else
            SelectClipRgn(dc, NULL);
    }
}


POINT PixmapCanvas::toContext(const Gfx::PointF& p)
{
    return toContext(p.x(), p.y());
}


POINT PixmapCanvas::toContext(double x, double y)
{
    Gfx::PointF p = transform() * Gfx::PointF(x, y);

    POINT pp;
    pp.x = Pt::lround(p.x() - 0.4999);
    pp.y = Pt::lround(p.y() - 0.4999);

    return pp;
}


void PixmapCanvas::onDrawLine(const Gfx::PointF& p0, const Gfx::PointF& p1)
{
    if( ! _pixmap )
        return;

    POINT points[2];
    points[0] = toContext( p0.x(), p0.y() );
    points[1] = toContext( p1.x(), p1.y() );

    HDC dc = _pixmap->deviceContext();
    Polyline(dc, points, 2);
}


void PixmapCanvas::onDrawPolyline(const Gfx::PointF* pts, const size_t n)
{
    if( ! _pixmap )
        return;

    std::vector<POINT> points(n);

    for(unsigned i = 0; i < n; i++)
    {
        const Gfx::PointF& p = pts[i];
        points[i] = toContext(p.x(), p.y());
    }

    HDC dc = _pixmap->deviceContext();
    Polyline( dc, &points[0], points.size() );
}


void PixmapCanvas::onFillPolygon(const Gfx::PointF* ps, const size_t n)
{
    if( ! _pixmap )
        return;

    POINT brushOrigin = {0};

    int left = std::numeric_limits<int>::max();
    int top = std::numeric_limits<int>::max();
    int right = 0;
    int bottom = 0;

    std::vector<POINT> points(n);

    for(size_t i = 0; i < n; i++)
    {
        const Gfx::PointF& p = ps[i];

        points[i] = toContext( p.x(), p.y() );

        if( p.y() < top)
            top = p.y();

        if( p.y() > bottom)
            bottom = p.y();

        if( p.x() < left)
            left = p.x();

        if( p.x() > right)
            right = p.x();
    }

    HDC dc = _pixmap->deviceContext();

    HGDIOBJ oldBrush = 0;

    if(_gradientBrush)
    {
        HBRUSH brush = getGradientBrush(dc, right - left, bottom - top,
                                        _gradientStart, _gradientStop, _gradient);

        oldBrush = SelectObject(dc, brush);

        SetBrushOrgEx(dc, left, top, NULL);
    }

    HPEN originalPen = (HPEN) SelectObject(dc, GetStockObject(NULL_PEN) );
    
    Polygon( dc, &points[0], points.size() );
    
    SelectObject(dc, originalPen);

    if(_gradientBrush)
    {
        HBRUSH brush = (HBRUSH) SelectObject(dc, oldBrush);
        DeleteObject(brush);

        SetBrushOrgEx(dc, brushOrigin.x, brushOrigin.y, NULL);
    }
}


void PixmapCanvas::onDrawRect(const Gfx::RectF& r)
{
    if( ! _pixmap )
        return;

    Gfx::PointF origin =  transform() * r.topLeft();
    Gfx::SizeF size =  transform() * r.size();
    Gfx::RectF rect(origin, size);

    HDC dc = _pixmap->deviceContext();

    HBRUSH originalBrush = (HBRUSH) SelectObject(dc, GetStockObject(NULL_BRUSH));

    Rectangle(dc, lround(rect.left()   - 0.4999), 
                  lround(rect.top()    - 0.4999), 
                  lround(rect.right()  - 0.4999), 
                  lround(rect.bottom() - 0.4999));

    SelectObject(dc, originalBrush);
}


void PixmapCanvas::onFillRect(const Gfx::RectF& r)
{
    if( ! _pixmap )
        return;

    Gfx::PointF origin =  transform() * r.topLeft();
    Gfx::SizeF size =  transform() * r.size();
    Gfx::RectF rect(origin, size);

    HDC dc = _pixmap->deviceContext();

    RECT rectangle;
    rectangle.left   =  lround( rect.left() );
    rectangle.top    =  lround( rect.top() );
    rectangle.right  =  lround( rect.right() + 0.001);    
    rectangle.bottom =  lround( rect.bottom() + 0.001);

    if(_gradientBrush)
    {
        HBRUSH brush = getGradientBrush(dc, lround( rect.width() ), lround( rect.height() ),
                                        _gradientStart, _gradientStop, _gradient);

        POINT brushOrigin = {0};
        SetBrushOrgEx(dc, lround(rect.x()),  lround(rect.y()), &brushOrigin);

        FillRect(dc, &rectangle, brush);

        SetBrushOrgEx(dc, brushOrigin.x, brushOrigin.y, NULL);
        DeleteObject(brush);
        return;
    }

    HBRUSH currentBrush = (HBRUSH) GetCurrentObject(dc, OBJ_BRUSH);
    FillRect(dc, &rectangle, currentBrush);
}


void PixmapCanvas::onDrawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if( ! _pixmap )
        return;

    HDC dc = _pixmap->deviceContext();

    Gfx::PointF p = transform() * topLeft;
    Gfx::SizeF s = transform() * size;

    HBRUSH originalBrush = (HBRUSH)SelectObject(dc, GetStockObject(NULL_BRUSH));

    Ellipse( dc, lround( p.x() ),  
                 lround( p.y() ), 
                 lround( p.x() + s.width() - 1),     // - 0.999 ?
                 lround( p.y() + s.height() - 1) );  // - 0.999 ?

    SelectObject(dc, originalBrush);
}


void PixmapCanvas::onFillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if( ! _pixmap )
        return;

    HDC dc = _pixmap->deviceContext();

    Gfx::PointF p = transform() * topLeft;
    Gfx::SizeF s = transform() * size;
    
    POINT brushOrigin = {0};
    HGDIOBJ oldBrush = 0;

    if(_gradientBrush)
    {
        HBRUSH brush = getGradientBrush(dc, lround( s.width() ), 
                                            lround( s.height() ),
                                        _gradientStart, _gradientStop, _gradient);

        oldBrush = SelectObject(dc, brush);

        SetBrushOrgEx(dc, lround(p.x()), lround(p.y()), &brushOrigin);
    }

    HPEN originalPen = (HPEN) SelectObject(dc, GetStockObject(NULL_PEN));

    Ellipse( dc, lround( p.x() ),
                  lround( p.y() ),
                  lround( p.x() + s.width() - 1),    // - 0.999 ?
                  lround( p.y() + s.height() - 1) ); // - 0.999 ?

    SelectObject(dc, originalPen);

    if(_gradientBrush)
    {
        HBRUSH brush = (HBRUSH) SelectObject(dc, oldBrush);
        DeleteObject(brush);

        SetBrushOrgEx(dc, brushOrigin.x, brushOrigin.y, NULL);
    }
}


Gfx::TextMetrics PixmapCanvas::onGetTextMetrics(const Pt::String& text) const
{
    if( ! _pixmap )
        return Gfx::TextMetrics();

    HDC dc = _pixmap->deviceContext();

    // select stored font into DC for measurement
    HGDIOBJ oldFont = _font ? SelectObject(dc, _font) : 0;

    //
    // NOTE: transformation is neccessary because GDI scales the text
    //       by the given scale factor to measure it and then scales
    //       the actual size back because GetTextExtentPoint32 should
    //       return logical coordinates. Without a XFORM the logical
    //       size is inaccurate and often too short.
    //

    const Gfx::Transform& tform = transform();

    XFORM xform = { static_cast<FLOAT>( tform.m11() ), 
                    static_cast<FLOAT>( tform.m12() ),
                    static_cast<FLOAT>( tform.m21() ), 
                    static_cast<FLOAT>( tform.m22() ),
                    static_cast<FLOAT>( tform.dx() ),  
                    static_cast<FLOAT>( tform.dy() ) };

    XFORM oldXForm = { 1, 0, 0, 1, 0 , 0 };
    GetWorldTransform(dc, &oldXForm);
    SetWorldTransform(dc, &xform);

    std::wstring wtext;
    text.toUtf16( std::back_inserter(wtext) );

    SIZE textSize;
    GetTextExtentPoint32W(dc, wtext.c_str(), wtext.size(), &textSize);

    TEXTMETRIC tm;
    GetTextMetrics(dc, &tm);

    Gfx::Float bearingX = 0;
    Gfx::Float boundingWidth = textSize.cx;

    if( ! wtext.empty() )
    {
        ABC abcFirst;
        if( GetCharABCWidthsW(dc, wtext.front(), wtext.front(), &abcFirst) )
            bearingX = abcFirst.abcA;

        ABC abcLast;
        if( GetCharABCWidthsW(dc, wtext.back(), wtext.back(), &abcLast) )
            boundingWidth = textSize.cx - bearingX - abcLast.abcC;
    }

    SetWorldTransform(dc, &oldXForm);

    if(oldFont)
        SelectObject(dc, oldFont);

    Gfx::TextMetrics fm;
    fm.setAdvance(textSize.cx);
    fm.setBearingX(bearingX);
    fm.setBearingY(tm.tmAscent);
    fm.setBoundingWidth(boundingWidth);
    fm.setBoundingHeight(tm.tmAscent + tm.tmDescent);
    return fm;
}


const Gfx::FontMetrics& PixmapCanvas::onGetFontMetrics() const
{
    return _fontMetrics;
}


Gfx::FontMetrics PixmapCanvas::getFontMetrics() const
{
    if( ! _pixmap )
        return Gfx::FontMetrics();

    HDC dc = _pixmap->deviceContext();

    HGDIOBJ oldFont = _font ? SelectObject(dc, _font) : 0;

    const Gfx::Transform& tform = transform();

    XFORM xform = { static_cast<FLOAT>( tform.m11() ), 
                    static_cast<FLOAT>( tform.m12() ),
                    static_cast<FLOAT>( tform.m21() ), 
                    static_cast<FLOAT>( tform.m22() ),
                    static_cast<FLOAT>( tform.dx() ),  
                    static_cast<FLOAT>( tform.dy() ) };

    XFORM oldXForm = { 1, 0, 0, 1, 0 , 0 };
    GetWorldTransform(dc, &oldXForm);
    SetWorldTransform(dc, &xform);

    TEXTMETRIC tm;
    GetTextMetrics(dc, &tm);

    SetWorldTransform(dc, &oldXForm);

    if(oldFont)
        SelectObject(dc, oldFont);

    Gfx::FontMetrics fm;
    fm.setAscent(tm.tmAscent);
    fm.setDescent(tm.tmDescent);
    fm.setCapHeight(tm.tmAscent - tm.tmInternalLeading);
    fm.setLeading(tm.tmExternalLeading);
    return fm;
}


void PixmapCanvas::onDrawText(const Gfx::PointF& to, 
                              const Pt::String& text, 
                              const Gfx::Transform* tform)
{
    if( ! _pixmap )
        return;

    HDC dc = _pixmap->deviceContext();

    _text.clear();
    text.toUtf16( std::back_inserter(_text) );

    Gfx::Transform tf = transform();
    tf.translate( to.x(), to.y() );

    if(tform)
        tf *= *tform;

    XFORM xform = { static_cast<FLOAT>( tf.m11() ), 
                    static_cast<FLOAT>( tf.m12() ),
                    static_cast<FLOAT>( tf.m21() ), 
                    static_cast<FLOAT>( tf.m22() ),
                    static_cast<FLOAT>( tf.dx() ),  
                    static_cast<FLOAT>( tf.dy() ) };

    XFORM oldXForm = { 1, 0, 0, 1, 0 , 0 };
    GetWorldTransform(dc, &oldXForm);

    SetWorldTransform(dc, &xform);
    TextOutW(dc, 0, 0, _text.c_str(), _text.size());
    SetWorldTransform(dc, &oldXForm);
}


void PixmapCanvas::onDrawImage(const Gfx::PointF& toF, 
                               const Gfx::Image& image,
                               const Gfx::RectF* rect)
{
    if( ! _pixmap )
        return;

    HDC dc = _pixmap->deviceContext();

    Gfx::PointF to = transform() * toF;

    int fromX = 0;
    int fromY = 0;
    int width = image.width();
    int height = image.height();

    if(rect)
    {
        fromX = lround( rect->x() );
        fromY = lround( rect->y()) ;
        width = lround( rect->width() );
        height = lround( rect->height() );
    }

    const Pt::uint8_t* data = image.data();
    std::size_t pixelSize = image.pixelStride();
    size_t depth = pixelSize * 8;

    BITMAPINFO bitmapInfo;
    ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

    bitmapInfo.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth        = image.width();
    bitmapInfo.bmiHeader.biHeight       = -(ssize_t)image.height(); // top-down image
    bitmapInfo.bmiHeader.biPlanes       = 1;                        // always 1
    bitmapInfo.bmiHeader.biBitCount     = static_cast<WORD>(depth); // bits per pixel
    bitmapInfo.bmiHeader.biCompression  = BI_RGB;                   // uncompressed RGB
    bitmapInfo.bmiHeader.biSizeImage    = 0;                        // automatic
    bitmapInfo.bmiHeader.biClrUsed      = 0;                        // no color table
    bitmapInfo.bmiHeader.biClrImportant = 0;                        // no color table

    VOID* imageBits = 0;
    HBITMAP bitmap = CreateDIBSection(dc, &bitmapInfo,
                                      DIB_RGB_COLORS, &imageBits, NULL, 0);
    memcpy(imageBits, data, image.width() * image.height() * pixelSize);

    HDC bitmapDC = CreateCompatibleDC(NULL);
    SelectObject(bitmapDC, bitmap);

    switch (_compositionMode)
    {
        case Gfx::CompositionMode::SourceCopy:
        {
            BitBlt(dc, lround(to.x()), lround(to.y()), width, height,
                   bitmapDC, fromX, fromY, SRCCOPY);
            break;
        }

        case Gfx::CompositionMode::SourceOver:
        {
            BLENDFUNCTION bf;
            bf.BlendOp = AC_SRC_OVER;
            bf.BlendFlags = 0;
            bf.SourceConstantAlpha = 0xFF; // only per pixel alpha
            bf.AlphaFormat = AC_SRC_ALPHA;

            AlphaBlend(dc, lround(to.x()), lround(to.y()), width, height, 
                       bitmapDC, fromX, fromY, width, height, bf);
            break;
        }
    }

    DeleteDC(bitmapDC);
    DeleteObject(bitmap);
}


void PixmapCanvas::onSetPath(const Gfx::Path& path)
{
    _path = path;
}


void PixmapCanvas::onDrawPath()
{
    if(_pixmap)
    {
        HDC dc = _pixmap->deviceContext();
        addPath(dc, _path);
        StrokePath(dc);
    }
}


void PixmapCanvas::onFillPath()
{
    if(_pixmap)
    {
        HDC dc = _pixmap->deviceContext();
        addPath(dc, _path);
        FillPath(dc);
    }
}


void PixmapCanvas::onDrawPath(const Gfx::Path& path)
{
    if(_pixmap)
    {
        HDC dc = _pixmap->deviceContext();
        addPath(dc, path);
        StrokePath(dc);
    }
}


void PixmapCanvas::onFillPath(const Gfx::Path& path)
{
    if(_pixmap)
    {
        HDC dc = _pixmap->deviceContext();
        addPath(dc, path);
        FillPath(dc);
    }
}


void PixmapCanvas::addPath(HDC dc, const Gfx::Path& path)
{
    BeginPath(dc);

    for(Gfx::PathIterator it = path.begin(); it != path.end(); ++it)
    {
        const Gfx::PathElement& e = *it;

        switch( e.type() )
        {
            default:
                break;

            case Gfx::Path::Close:
                CloseFigure(dc);
                break;

            case Gfx::Path::MoveTo:
            {
                const Gfx::PointF& to = it->point(0);
                POINT p = toContext(to);
                MoveToEx(dc, p.x, p.y, NULL);
                break;
            }

            case Gfx::Path::LineTo:
            {
                const Gfx::PointF& to = it->point(0);
                POINT p = toContext(to);
                LineTo(dc, p.x, p.y);
                break;
            }

            case Gfx::Path::QuadTo:
            {
                const Gfx::PointF& c1 = it->point(0);
                const Gfx::PointF& to = it->point(1);
                
                POINT p0;
                GetCurrentPositionEx(dc, &p0);
                POINT p1 = toContext(c1);
                POINT p2 = toContext(to);

                POINT cubicPoints[3];

                // control 1: P0 + (2/3) * (P1 - P0)
                cubicPoints[0].x = p0.x + (2 * (p1.x - p0.x)) / 3;
                cubicPoints[0].y = p0.y + (2 * (p1.y - p0.y)) / 3;

                // control 2: P2 + (2/3) * (P1 - P2)
                cubicPoints[1].x = p2.x + (2 * (p1.x - p2.x)) / 3;
                cubicPoints[1].y = p2.y + (2 * (p1.y - p2.y)) / 3;

                // end point
                cubicPoints[2] = p2;
                
                PolyBezierTo(dc, cubicPoints, 3);
                break;
            }
            
            case Gfx::Path::CubicTo:
            {
                const Gfx::PointF& c1 = it->point(0);
                const Gfx::PointF& c2 = it->point(1);
                const Gfx::PointF& to = it->point(2);

                POINT points[3];
                points[0] = toContext(c1);
                points[1] = toContext(c2);
                points[2] = toContext(to);
                
                PolyBezierTo(dc, points, 3);
                break;
            }
        }
    }

    EndPath(dc);
}

#endif // PT_FORMS_WIN32_RASTER

} // namespace

} // namespace
