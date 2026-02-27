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
#include "win32.h"

using std::max;
using std::min;
#include <Gdiplus.h>

namespace {

DWORD getPenStyle(const Pt::Gfx::Pen& pen)
{
  using namespace Pt;

#ifdef _WIN32_WCE
    DWORD penStyle = 0;
#else
    DWORD penStyle = PS_GEOMETRIC;
#endif

    switch( pen.style() )
    {
        case Gfx::Pen::Solid:
            penStyle |= PS_SOLID;
            break;
                
        case Gfx::Pen::Dash:
            penStyle |= PS_DASH;
            break;

        case Gfx::Pen::Dot:
#ifdef _WIN32_WCE
            penStyle |= PS_DASH;
#else
            penStyle |= PS_DOT;
#endif
            break;
    }

#ifndef _WIN32_WCE
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
#endif

    return penStyle;
}


HFONT getFont(const Pt::Gfx::Font& font)
{
    int fontWeight = FW_NORMAL;
    
    std::string style = font.style();
    std::transform(style.begin(), style.end(),  style.begin(), ::tolower);

    if(style == "bold" || style == "bold italic" || style == "bolditalic")
        fontWeight = FW_BOLD;

    BYTE italic = (style == "italic" || style == "bold italic" || style == "bolditalic");

    //HDC dc = GetDC(NULL);
    //int logicalPPI = GetDeviceCaps(dc, LOGPIXELSY);
    int logicalPPI = 96;

    int height = MulDiv(font.size(), logicalPPI, 72);
    //ReleaseDC(NULL, dc);

    // If a negative value is used for lfHeight, the font is
    // looked up by character size, which is only the ascent.
    // Looking up fonts by ascent seems to be more portable.

    LOGFONT lf;
    lf.lfHeight         = -height;                     // will be converted to device units    
    lf.lfWidth          = 0;                           // default width of the font
    lf.lfEscapement     = 0;                           // escapement angle
    lf.lfOrientation    = 0;                           // orientation
    lf.lfWeight         = fontWeight;                  // font weight
    lf.lfItalic         = italic;                      // italic
    lf.lfUnderline      = FALSE;                       // underline
    lf.lfStrikeOut      = FALSE;                       // strikeout
    lf.lfCharSet        = DEFAULT_CHARSET;             // use the default charset
    lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;          // default output precision
    lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;         // default clipping behaviour
    lf.lfQuality        = DEFAULT_QUALITY;             // default quality
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE; // default pitch and family

    if( font.name().empty() )
    {
        const std::string& fontName = Pt::Forms::PixmapImpl::defaultFont();
        memcpy(lf.lfFaceName, fontName.c_str(), std::min<size_t>( LF_FACESIZE, fontName.size() + 1) );
    }
    else
    {
        memcpy(lf.lfFaceName, font.name().c_str(), std::min<size_t>( LF_FACESIZE, font.name().size() + 1) );
    }

    HFONT hf = CreateFontIndirect(&lf);
    return hf;
}

HBRUSH getGradientBrush(HDC dc, int width, int height,
                        Pt::Gfx::ColorF gradientStart, 
                        Pt::Gfx::ColorF gradientStop, 
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


void toPreMulAlpha(const Pt::Gfx::Image& image, 
                   std::vector<Pt::uint8_t>& bitmapData)
{
    size_t _width = image.width();
    size_t _height = image.height();

    Pt::Gfx::ConstPixelView fromView(image);
    Pt::Gfx::ConstPixelView::Iterator it = fromView.begin();
    Pt::Gfx::ConstPixelView::Iterator end = fromView.end();

    for( ; it != end; ++it)
    {
        Pt::Gfx::Argb32Color color = it->toColor();

        const Pt::uint8_t r = color.red();
        const Pt::uint8_t g = color.green();
        const Pt::uint8_t b = color.blue();
        const Pt::uint8_t a = color.alpha();

        bitmapData.push_back((Pt::uint8_t) (a * b / 255));
        bitmapData.push_back((Pt::uint8_t) (a * g / 255));
        bitmapData.push_back((Pt::uint8_t) (a * r / 255));
        bitmapData.push_back((Pt::uint8_t) (a));
    }
}

} // namespace

namespace Pt {

namespace Forms {

#ifndef PT_FORMS_WIN32_RASTER

PixmapCanvas::PixmapCanvas()
: Gfx::Canvas()
, _pixmap(0)
, _pen(0)
, _penSize(1)
, _penColor()
, _brush(0)
, _gradientBrush(false)
, _clipRect(0)
, _font(0)
{
}


PixmapCanvas::~PixmapCanvas()
{
    // remove objects from surface DC before deleting them
    if(_pixmap)
        _pixmap->releaseCanvas();

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


void PixmapCanvas::onBeginPaint(const Gfx::Paint& paint)
{
    double scaleFactor = scaling().scaleFactor();

    size_t penSize = paint.pen().size();
    
    // keep pen size when downscaling
    penSize = scaleFactor < 1.0 ? penSize
                                : static_cast<size_t>( penSize * scaleFactor );

    if(_penSize != penSize)
        onSetPen( paint.pen() );
}


void PixmapCanvas::onFinishPaint()
{
    // NOTE: this might be called from the attached surface base class destructor

    if(_pixmap)
        _pixmap = 0;
}


void PixmapCanvas::onSetCompositionMode(const Gfx::CompositionMode& mode) 
{
    _compositionMode = mode;
}


void PixmapCanvas::onApplyCompositionMode(const Gfx::CompositionMode& mode) 
{
    _compositionMode = mode;
}


void PixmapCanvas::onSetPen(const Gfx::Pen& pen)
{
    double scaleFactor = scaling().scaleFactor();

    // keep pen size when downscaling
    size_t penSize = scaleFactor < 1.0 ? pen.size() 
                                       : static_cast<size_t>( pen.size() * scaleFactor );

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
                           
#ifdef _WIN32_WCE
    _pen = CreatePen(penStyle, penSize, color);
#else
    LOGBRUSH brush;
    brush.lbStyle = BS_SOLID;
    brush.lbColor = color;

    _pen = ExtCreatePen(penStyle, penSize, &brush, 0, NULL);
#endif
}


void PixmapCanvas::onApplyPen(const Gfx::Pen& pen)
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

            std::size_t pixelSize = texture.pixelStride();
            std::size_t depth = pixelSize * 8;

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
            memcpy(imageBits, 
                    texture.data(), 
                    texture.width() * texture.height() * pixelSize);

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


void PixmapCanvas::onApplyBrush(const Gfx::Brush& brush)
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

    _font = getFont(font);
}


void PixmapCanvas::onApplyFont(const Gfx::Font& font)
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
    // TODO: reuse HRGN instead of always creating a new one

    if(_clipRect)
    {
        DeleteObject(_clipRect);
        _clipRect = NULL;
    }

    if( ! rectF )
        return;

    Gfx::PointF origin =  transform() * rectF->origin();
    Gfx::SizeF size =  transform() * rectF->size();
    Gfx::RectF rectP(origin, size);
               
    long x = Pt::lround( rectP.x() );
    long y = Pt::lround( rectP.y() );
    long width = Pt::lround( rectP.width() );
    long height = Pt::lround( rectP.height() );
                            
    // CreateRectRgn only includes the interior of the rect
    _clipRect = CreateRectRgn(x , y, x + width, y + height);
}


void PixmapCanvas::onApplyClip(const Gfx::RectF* rectF)
{  
    if(_pixmap)
    {       
        HDC dc = _pixmap->deviceContext();

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


#ifndef PT_FORMS_GDIPLUS

Gfx::TextMetrics PixmapCanvas::onGetTextMetrics(const Pt::String& text) const
{
    if( ! _pixmap )
        return Gfx::TextMetrics();

    HDC dc = _pixmap->deviceContext();

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

    TEXTMETRIC tm;
    GetTextMetrics(dc, &tm);

    std::wstring wtext;
    text.toUtf16( std::back_inserter(wtext) );

    SIZE textSize;
    GetTextExtentPoint32W(dc, wtext.c_str(), wtext.size(), &textSize);

    SetWorldTransform(dc, &oldXForm);

    long asc = tm.tmAscent;
    long des = tm.tmDescent;
    long inl = tm.tmInternalLeading;
    long cap = tm.tmAscent - tm.tmInternalLeading;
    long exl = tm.tmExternalLeading;
    long lh = asc + des + exl;

    Gfx::TextMetrics fm;
    fm.setAscent(asc);
    fm.setDescent(des);
    fm.setCapHeight(cap);
    fm.setLeading(exl);
    fm.setWidth(textSize.cx);

    //std::clog << "### " << text.narrow() << " " << fm.width() << std::endl;
    return fm;
}

#else

Gfx::TextMetrics PixmapCanvas::onGetTextMetrics(const Pt::String& text) const
{
    if( ! _pixmap )
        return Gfx::TextMetrics();

    HDC dc = _pixmap->deviceContext();

    std::wstring wtext;
    text.toUtf16( std::back_inserter(wtext) );

    Gdiplus::Font gdiFont(dc);
    Gdiplus::Graphics graphics(dc);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);
    graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);

    const Gdiplus::StringFormat* format = Gdiplus::StringFormat::GenericTypographic();

    Gdiplus::FontFamily family;
    gdiFont.GetFamily(&family);

    Gdiplus::REAL lineSpacingF = gdiFont.GetHeight(graphics.GetDpiY());
    Gdiplus::REAL sizeUnits = gdiFont.GetSize();

    UINT16 ascentUnits = family.GetCellAscent(gdiFont.GetStyle());
    UINT16 descentUnits = family.GetCellDescent(gdiFont.GetStyle());
    UINT16 lineSpacingUnits = family.GetLineSpacing(gdiFont.GetStyle());
    UINT16 emHeightUnits = family.GetEmHeight(gdiFont.GetStyle());

    Gdiplus::REAL pixelsPerUnit = lineSpacingF / lineSpacingUnits;
    Gdiplus::REAL ascentF = ascentUnits * pixelsPerUnit;
    Gdiplus::REAL descentF = descentUnits * pixelsPerUnit;
    Gdiplus::REAL heightF = ascentF + descentF;
    Gdiplus::REAL emHeightF = emHeightUnits * pixelsPerUnit;

    Gdiplus::REAL asc = ascentF;
    Gdiplus::REAL des = descentF;
    Gdiplus::REAL cap = emHeightF - descentF;
    Gdiplus::REAL inl = ascentF - cap;
    Gdiplus::REAL exl = lineSpacingF - heightF;
    Gdiplus::REAL lh = asc + des + exl;

    Gdiplus::RectF textRect;
    graphics.MeasureString(wtext.c_str(), wtext.size(), &gdiFont, 
                            Gdiplus::PointF(0, 0), format, &textRect);

    int dpix = GetDeviceCaps(dc, LOGPIXELSX);
    double pixelRatio = 96.0 / dpix;

    Gfx::TextMetrics tm;
    tm.setAscent(asc * pixelRatio);
    tm.setDescent(des * pixelRatio);
    tm.setCapHeight(cap * pixelRatio);
    tm.setLeading(exl * pixelRatio);
    tm.setWidth(textRect.Width * pixelRatio);
    return tm;
}
#endif


void PixmapCanvas::onDrawText(const Gfx::PointF& to, 
                              const Pt::String& text, 
                              const Gfx::Transform* tform)
{
    if( ! _pixmap )
        return;

    HDC dc = _pixmap->deviceContext();

    _text.clear();
    text.toUtf16( std::back_inserter(_text) );

    Gfx::Transform tf;
    if(tform)
        tf *= *tform;

    tf.translate( to.x(), to.y() );
    tf *= transform();

#ifndef PT_FORMS_GDIPLUS
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
#else
    Gdiplus::Matrix matrix;
    matrix.SetElements( static_cast<Gdiplus::REAL>( tf.m11() ), 
                        static_cast<Gdiplus::REAL>( tf.m12() ),
                        static_cast<Gdiplus::REAL>( tf.m21() ), 
                        static_cast<Gdiplus::REAL>( tf.m22() ),
                        static_cast<Gdiplus::REAL>( tf.dx() ), 
                        static_cast<Gdiplus::REAL>( tf.dy() ) );

    Gdiplus::Graphics graphics(dc);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);
    graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);

    Gdiplus::Font font(dc);

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
    Gdiplus::REAL offsetY = ascent + 0.5;
    
    Gdiplus::PointF origin( 0, -offsetY );
    
    const Gdiplus::StringFormat* format = Gdiplus::StringFormat::GenericTypographic();

    Gdiplus::Matrix oldMatrix;
    graphics.GetTransform(&oldMatrix);
    graphics.SetTransform(&matrix);

    const Gfx::ColorF& color = _penColor;
    BYTE alpha = color.alpha() / 257;
    BYTE red   = color.red()   / 257;
    BYTE green = color.green() / 257; 
    BYTE blue  = color.blue()  / 257;

    Gdiplus::SolidBrush brush( Gdiplus::Color(alpha, red, green, blue) );

    graphics.DrawString( _text.c_str(), _text.size(), &font, 
                         origin, format, &brush);

    graphics.SetTransform(&oldMatrix);
#endif
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

    switch (_compositionMode)
    {
        case Gfx::CompositionMode::SourceCopy:
        {
            const Pt::uint8_t* data = image.data();
            std::size_t pixelSize = image.pixelStride();
            size_t depth = pixelSize * 8;

            HBITMAP bitmap = CreateBitmap(image.width(), image.height(), 1, 
                                          depth, (VOID*)data);
            if (bitmap == NULL)
            {
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
                bitmap = CreateDIBSection(dc, &bitmapInfo,
                                          DIB_RGB_COLORS, &imageBits, NULL, 0);
                memcpy(imageBits, data, image.width() * image.height() * pixelSize);
            }

            HDC bitmapDC = CreateCompatibleDC(NULL);
            SelectObject(bitmapDC, bitmap);

            BitBlt(dc, lround(to.x()), lround(to.y()), width, height,
                   bitmapDC, fromX, fromY, SRCCOPY);

            DeleteDC(bitmapDC);
            DeleteObject(bitmap);
            break;
        }

        case Gfx::CompositionMode::SourceOver:
        {
            std::vector<Pt::uint8_t> bitmapData;
            toPreMulAlpha(image, bitmapData);

            const Pt::uint8_t* data =  bitmapData.empty() ? 0 : &bitmapData[0];
            std::size_t pixelSize = image.pixelStride();
            size_t depth = pixelSize * 8;

            HBITMAP bitmap = CreateBitmap(image.width(), image.height(), 1, 
                                          depth, (VOID*)data);
            if (bitmap == NULL)
            {
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
                bitmap = CreateDIBSection(dc, &bitmapInfo,
                                          DIB_RGB_COLORS, &imageBits, NULL, 0);
                memcpy(imageBits, data, image.width() * image.height() * pixelSize);
            }

            HDC bitmapDC = CreateCompatibleDC(NULL);
            SelectObject(bitmapDC, bitmap);

            BLENDFUNCTION bf;
            bf.BlendOp = AC_SRC_OVER;
            bf.BlendFlags = 0;
            bf.SourceConstantAlpha = 0xFF; // only per pixel alpha
            bf.AlphaFormat = AC_SRC_ALPHA;

            AlphaBlend(dc, lround(to.x()), lround(to.y()), width, height, 
                       bitmapDC, fromX, fromY, width, height, bf);

            DeleteObject(bitmap);
            DeleteDC(bitmapDC);
            break;
        }
    }
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
