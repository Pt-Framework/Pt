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
#include "PaintData.h"
#include "PixmapSurfaceImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Gfx/Argb32Format.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Algorithm.h>

using std::max;
using std::min;
#include <Gdiplus.h>

#ifdef PT_HMI_WIN32_RASTER

namespace Pt {

namespace Hmi {

PixmapSurfaceImpl::PixmapSurfaceImpl(PixmapSurface&)
{ 
}

} // namespace

} // namespace

#else // PT_HMI_WIN32_RASTER

namespace {

#ifdef _WIN32_WCE

static int CALLBACK EnumFontsProc(LOGFONT* logFont, TEXTMETRIC* physFont, DWORD type, LPARAM param)
{
    WCHAR* faceName = logFont->lfFaceName;

    // Ignore fonts with @ as first character.
    if (faceName[0] != '@')
    {
        std::string name = win32::toMultiByte(faceName);
        reinterpret_cast<std::vector<std::string>*>(param)->push_back(name);
    }

    return 1;
}

#else

static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX* logFont, NEWTEXTMETRICEX* physFont, DWORD type, LPARAM param)
{
    char* faceName = logFont->elfLogFont.lfFaceName;

    // Ignore fonts with @ as first character.
    if (faceName[0] != '@')
    {
        reinterpret_cast<std::vector<std::string>*>(param)->push_back(faceName);
    }

    return 1;
}

#endif

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

PixmapSurfaceImpl::PixmapSurfaceImpl(Gfx::PaintSurface& surface)
: Gfx::Canvas(surface)
, _size(0, 0)
, _dc(0)
, _paint(0)
, _gradientBrush(false)
, _compositionMode(Gfx::CompositionMode::SourceCopy)
{
    Gfx::SizeF size = Gfx::SizeF(10, 10);

    HDC screenDC = GetDC(NULL);
    _dc = CreateCompatibleDC(screenDC);
    _bitmap = CreateCompatibleBitmap(screenDC, lround(size.width()), 
                                               lround(size.height()));
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


void PixmapSurfaceImpl::resize(const Gfx::SizeF& sizeF)
{
    Gfx::SizeF size = _scaling.toPhysical(sizeF);

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


void PixmapSurfaceImpl::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);
}


const Gfx::ImageFormat& PixmapSurfaceImpl::format() const
{
    return Gfx::ImageFormat::argb32();
}


const Gfx::SizeF& PixmapSurfaceImpl::size() const
{
    return _size;
}


const Gfx::Scaling& PixmapSurfaceImpl::surfaceScaling() const
{
    return _scaling;
}


const Gfx::Scaling& PixmapSurfaceImpl::onGetScaling() const
{
    return _scaling;
}


bool PixmapSurfaceImpl::onBeginPaint(const Gfx::Paint& paint, Gfx::PaintContext* context)
{
    PaintData* paintContext = dynamic_cast<PaintData*>(context);
    if( ! paintContext )
        return false;

    _paint = paintContext;

    setCompositionMode(*_paint);
    setPen(*_paint);
    setBrush(*_paint, paint.brush());
    setFont(*_paint);
    return true;
}


Gfx::PaintContext* PixmapSurfaceImpl::onBeginPaint(const Gfx::Paint& paint)
{
    PaintData* paintContext  = new PaintData();

    _paint = paintContext;
    return paintContext;
}


void PixmapSurfaceImpl::onReleasePaint()
{
    SelectObject(_dc, _oldPen);
    SelectObject(_dc, _oldBrush);
    SelectObject(_dc, _oldFont);
    SelectClipRgn(_dc, NULL);

    _paint = 0;
}


void PixmapSurfaceImpl::setCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;
}


void PixmapSurfaceImpl::setCompositionMode(const PaintData& paint)
{
    _compositionMode = paint.compositionMode();
}


void PixmapSurfaceImpl::setPen(const Gfx::Pen& pen)
{
    if( ! _paint )
        return;

    setPen(*_paint);
}


void PixmapSurfaceImpl::setPen(const PaintData& paint)
{
    HPEN hpen = paint.pen();
    if(hpen)
        SelectObject(_dc, hpen);

    _penColor = paint.penColor();

    DWORD penColor = RGB( _penColor.red()  / 257, 
                          _penColor.green() / 257, 
                          _penColor.blue()  / 257 );

    SetTextColor(_dc, penColor);
}


void PixmapSurfaceImpl::setBrush(const Gfx::Brush& brush)
{
    if( ! _paint )
        return;

    setBrush(*_paint, brush);
}


void PixmapSurfaceImpl::setBrush(const PaintData& paint, const Gfx::Brush& brush)
{
    _gradientBrush = false;

    if( paint.gradientBrush() )
    {
        _gradientBrush = true;
        _gradient = brush.gradient();
        _gradientStart = brush.color();
        _gradientStop = brush.gradientColor();

        // do not set a brush now, because the gradient brush pattern can
        // only be calculated later, when the fill area is known
        return;
    }

    HBRUSH hbrush = paint.brush();
    if(hbrush)
        SelectObject(_dc, hbrush);
}


void PixmapSurfaceImpl::setFont(const Gfx::Font& font)
{
    if( ! _paint )
        return;

    setFont(*_paint);
}


void PixmapSurfaceImpl::setFont(const PaintData& paint)
{
    HFONT hfont = paint.font();
    if(hfont)
        SelectObject(_dc, hfont);

    SetTextAlign(_dc, TA_BASELINE | TA_LEFT | TA_NOUPDATECP);
}


void PixmapSurfaceImpl::setClip(const Gfx::RectF& clipRect)
{
    if( ! _paint )
        return;

    setClip(_paint);
}


void PixmapSurfaceImpl::resetClip()
{  
    setClip(0);
}


void PixmapSurfaceImpl::setClip(const PaintData* paint)
{
    if( ! paint )
    {
        SelectClipRgn(_dc, NULL);
        return;
    }

    HRGN hrgn = paint->clipRect();
    if(hrgn)
        SelectClipRgn(_dc, hrgn);
    else
        SelectClipRgn(_dc, NULL);
}


void PixmapSurfaceImpl::drawLine(const Gfx::Line& line)
{
    //Pt::Gfx::PointF p0 = _scaling.toPhysical( line.from() ); 
    //Pt::Gfx::PointF p1 = _scaling.toPhysical( line.to() );

    Pt::Gfx::PointF p0 = line.from(); 
    Pt::Gfx::PointF p1 = line.to();

    POINT points[2];
    
    points[0].x = lround( p0.x() - 0.4999 );
    points[0].y = lround( p0.y() - 0.4999 );
    
    points[1].x = lround( p1.x() - 0.4999 );
    points[1].y = lround( p1.y() - 0.4999 );

    Polyline(_dc, points, 2);
}


void PixmapSurfaceImpl::drawPolyline(const Gfx::Polyline& line)
{
    std::size_t n = line.size();

    std::vector<POINT> points(n);

    for(unsigned i = 0; i < n; i++)
    {
        Gfx::PointF p = _scaling.toPhysical( line.at(i) );

        points[i].x = Pt::lround(p.x() - 0.4999);
        points[i].y = Pt::lround(p.y() - 0.4999);
    }

    Polyline( _dc, &points[0], points.size() );
}


void PixmapSurfaceImpl::fillPolygon(const Gfx::Polyline& line)
{
    std::size_t n = line.size();

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
        Gfx::PointF p = _scaling.toPhysical( line.at(i) );

        points[i].x = Pt::lround(p.x() - 0.4999);
        points[i].y = Pt::lround(p.y() - 0.4999);

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


#ifndef PT_HMI_GDIPLUS
Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Pt::String& text) const
{
    TEXTMETRIC tm;
    GetTextMetrics(_dc, &tm);

    std::wstring wtext;
    text.toUtf16( std::back_inserter(wtext) );
    
    XFORM oldTrans = { 1, 0, 0, 1, 0 , 0 };
    GetWorldTransform(_dc, &oldTrans);

    double scaleFactor = _scaling.scaleFactor();
    Gfx::Transform tt;
    tt.scale(scaleFactor, scaleFactor);

    XFORM newTrans = { static_cast<FLOAT>( tt.m11() ), 
                       static_cast<FLOAT>( tt.m12() ),
                       static_cast<FLOAT>( tt.m21() ), 
                       static_cast<FLOAT>( tt.m22() ),
                       static_cast<FLOAT>( tt.dx() ),  
                       static_cast<FLOAT>( tt.dy() ) };

    SetWorldTransform(_dc, &newTrans);

    SIZE textSize;
    GetTextExtentPoint32W(_dc, wtext.c_str(), wtext.size(), &textSize);
    
    SetWorldTransform(_dc, &oldTrans);

    long asc = tm.tmAscent;
    long des = tm.tmDescent;
    long inl = tm.tmInternalLeading;
    long cap = tm.tmAscent - tm.tmInternalLeading;
    long exl = tm.tmExternalLeading;
    long lh = asc + des + exl;

    Gfx::FontMetrics fm;
    fm.setAscent(asc);
    fm.setDescent(des);
    fm.setCapHeight(cap);
    fm.setLeading(exl);
    fm.setWidth(textSize.cx);

    //std::clog << "### " << text.narrow() << " " << fm.width() << std::endl;
    return fm;
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

    int dpix = GetDeviceCaps(_dc, LOGPIXELSX);
    double scaling = 96.0 / dpix;

    Gfx::FontMetrics fm;
    fm.setAscent(asc * scaling);
    fm.setDescent(des * scaling);
    fm.setCapHeight(cap * scaling);
    fm.setLeading(exl * scaling);
    fm.setWidth(textRect.Width * scaling);
    return fm;
}
#endif

void PixmapSurfaceImpl::drawText(const Gfx::PointF& toF, 
                                 const Pt::String& text, 
                                 const Gfx::Transform& t)
{
    Pt::Gfx::PointF to = _scaling.toPhysical(toF); 

    _text.clear();
    text.toUtf16( std::back_inserter(_text) );

    Gfx::Transform tt = t;
    
    tt.scale( _scaling.scaleFactor(), 
              _scaling.scaleFactor() );

    //const int dpix = GetDeviceCaps(_dc, LOGPIXELSX);
    //const double scaling = 96.0 / dpix;

#ifndef PT_HMI_GDIPLUS
    tt.translate( to.x(), to.y() );

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
    tt.scale( scaling, 
              scaling );
    
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
    Gdiplus::REAL offsetY = ascent + 0.5;
    
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

    const Gfx::Color& color = _penColor;
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

void PixmapSurfaceImpl::drawRect(const Gfx::RectF& rect)
{
    Gfx::RectF r = _scaling.toPhysical(rect); 

    HBRUSH originalBrush = (HBRUSH) SelectObject(_dc, GetStockObject(NULL_BRUSH));

    Rectangle(_dc, lround(r.left()   - 0.4999), 
                   lround(r.top()    - 0.4999), 
                   lround(r.right()  - 0.4999), 
                   lround(r.bottom() - 0.4999));

    SelectObject(_dc, originalBrush);
}


void PixmapSurfaceImpl::fillRect(const Gfx::RectF& rect)
{
    Gfx::RectF r = _scaling.toPhysical(rect); 
    
    RECT rectangle;
    rectangle.left   =  lround( r.left() );
    rectangle.top    =  lround( r.top() );
    rectangle.right  =  lround( r.right() + 0.001);    
    rectangle.bottom =  lround( r.bottom() + 0.001);

    if(_gradientBrush)
    {
        HBRUSH brush = gradientBrush(_dc, lround( r.width() ), lround( r.height() ),
                                     _gradientStart, _gradientStop, _gradient);

        POINT brushOrigin = {0};
        SetBrushOrgEx(_dc, lround(r.x()),  lround(r.y()), &brushOrigin);

        FillRect(_dc, &rectangle, brush);

        SetBrushOrgEx(_dc, brushOrigin.x, brushOrigin.y, NULL);
        DeleteObject(brush);
        return;
    }

    HBRUSH currentBrush = (HBRUSH) GetCurrentObject(_dc, OBJ_BRUSH);
    FillRect(_dc, &rectangle, currentBrush);
}


void PixmapSurfaceImpl::drawEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    Gfx::PointF topLeft = _scaling.toPhysical(topLeftF);
    Gfx::SizeF size = _scaling.toPhysical(sizeF);

    HBRUSH originalBrush = (HBRUSH)SelectObject(_dc, GetStockObject(NULL_BRUSH));

    Ellipse( _dc, lround( topLeft.x()),  
                  lround( topLeft.y()), 
                  lround( topLeft.x() + size.width() -1),     // - 0.999 ?
                  lround( topLeft.y() + size.height() -1 ));  // - 0.999 ?

    SelectObject(_dc, originalBrush);
}


void PixmapSurfaceImpl::fillEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    Gfx::PointF topLeft = _scaling.toPhysical( topLeftF);
    Gfx::SizeF size = _scaling.toPhysical(sizeF);

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
                  lround( topLeft.x() + size.width() - 1),    // - 0.999 ?
                  lround( topLeft.y() + size.height() - 1) ); // - 0.999 ?

    SelectObject(_dc, originalPen);

    if(_gradientBrush)
    {
        HBRUSH brush = (HBRUSH) SelectObject(_dc, oldBrush);
        DeleteObject(brush);

        SetBrushOrgEx(_dc, brushOrigin.x, brushOrigin.y, NULL);
    }
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& to, 
                                    const Gfx::PaintSurface& surface)
{
    const Canvas* canvas = surface.canvas();
    const PixmapSurfaceImpl* pixmap = dynamic_cast<const PixmapSurfaceImpl*>(canvas);
    if(pixmap)
    {
        drawPixmap(to, *pixmap);
        return;
    }

    //const PixmapSurface* pixmap = dynamic_cast<const PixmapSurface*>(&surface);
    //if(pixmap)
    //{
    //    drawPixmap(to, *pixmap);
    //    return;
    //}

    Pt::Gfx::Image image = surface.toImage();
    if( image.format() == format() )
    {
        drawImage(to, image);
        return;
    }

    Pt::Gfx::Image dest( format(), image.size() );
    Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
    drawImage(to, dest);
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& to,
                                    const Gfx::PaintSurface& surface,
                                    const Gfx::RectF& rect)
{
    const Canvas* canvas = surface.canvas();
    const PixmapSurfaceImpl* pixmap = dynamic_cast<const PixmapSurfaceImpl*>(canvas);
    if(pixmap)
    {
        drawPixmap(to, *pixmap, rect);
        return;
    }

    //const PixmapSurface* pixmap = dynamic_cast<const PixmapSurface*>(&surface);
    //if(pixmap)
    //{
    //    drawPixmap(to, *pixmap, rect);
    //    return;
    //}

    Pt::Gfx::Image image = surface.toImage();
    if( image.format() == format() )
    {
        drawImage(to, image, rect);
        return;
    }

    Pt::Gfx::Image dest( format(), image.size() );
    Pt::Gfx::copy( image.begin(), image.end(), dest.begin() );
    drawImage(to, dest, rect);
}


void PixmapSurfaceImpl::drawPixmap(const Gfx::PointF& toF, 
                                   const PixmapSurfaceImpl& surface)
{
    Gfx::PointF to = _scaling.toPhysical(toF);
    Gfx::Size size = Gfx::round( surface.size() );

    switch (_compositionMode)
    {
        case Gfx::CompositionMode::SourceCopy:
        {
            BitBlt(_dc, lround(to.x()), lround(to.y()), size.width(), size.height(),
                   surface.deviceContext(), 0, 0, SRCCOPY);
        }
        break;

        case  Gfx::CompositionMode::SourceOver:
        {
            BLENDFUNCTION bf;
            bf.BlendOp = AC_SRC_OVER;
            bf.BlendFlags = 0;
            bf.SourceConstantAlpha = 0xFF; // only per pixel alpha
            bf.AlphaFormat = AC_SRC_ALPHA;

            AlphaBlend(_dc, to.x(), to.y(), size.width(), size.height(),
                       surface.deviceContext(), 
                       0, 0, size.width(), size.height(), bf);
        }
        break;
    }
}


void PixmapSurfaceImpl::drawPixmap(const Gfx::PointF& toF, 
                                   const PixmapSurfaceImpl& pm, 
                                   const Gfx::RectF& rectF)
{
    Gfx::PointF to = _scaling.toPhysical(toF);
    Gfx::RectF& pmRect = _scaling.toPhysical(rectF);

    const Gfx::Size size = Gfx::round(pmRect.size());
    const Gfx::Point from = Gfx::round(pmRect.topLeft());

    switch (_compositionMode)
    {
        case Gfx::CompositionMode::SourceCopy:
        {
            BitBlt(_dc, lround(to.x()), lround(to.y()), size.width(), size.height(),
                   pm.deviceContext(), from.x(), from.y(), SRCCOPY);
        }
        break;

        case  Gfx::CompositionMode::SourceOver:
        {
            BLENDFUNCTION bf;
            bf.BlendOp = AC_SRC_OVER;
            bf.BlendFlags = 0;
            bf.SourceConstantAlpha = 0xFF; // only per pixel alpha
            bf.AlphaFormat = AC_SRC_ALPHA;

            AlphaBlend(_dc, to.x(), to.y(), size.width(), size.height(),
                       pm.deviceContext(), 
                       from.x(), from.y(), size.width(), size.height(), bf);
        }
        break;
    }
}


void PixmapSurfaceImpl::toPreMulAlpha(const Pt::Gfx::Image& image, 
                                      std::vector<Pt::uint8_t>& bitmapData)
{
    size_t _width = image.width();
    size_t _height = image.height();

    for (std::size_t y = 0; y < image.height(); ++y)
    {
        for (std::size_t x = 0; x < image.width(); ++x)
        {
            Gfx::ConstPixel pixel(image.view(), x, y);
            Gfx::Color color = image.format().getColor(pixel);

            const Pt::uint8_t r = color.red() / 257;
            const Pt::uint8_t g = color.green() / 257;
            const Pt::uint8_t b = color.blue() / 257;
            const Pt::uint8_t a = color.alpha() / 257;

            bitmapData.push_back((Pt::uint8_t) (a * b / 255));
            bitmapData.push_back((Pt::uint8_t) (a * g / 255));
            bitmapData.push_back((Pt::uint8_t) (a * r / 255));
            bitmapData.push_back((Pt::uint8_t) (a));
        }
    }
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
    Gfx::PointF toR = _scaling.toPhysical( toF );
    Gfx::Point to = Gfx::round(toR);

    switch (_compositionMode)
    {
        case Gfx::CompositionMode::SourceCopy:
        {
            const Pt::uint8_t* data = image.data();
            const size_t depth = image.view().pixelStride() * 8;

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
                bitmap = CreateDIBSection(_dc, &bitmapInfo,
                                          DIB_RGB_COLORS, &imageBits, NULL, 0);
                memcpy(imageBits, data, image.width() * image.height() * 4);
            }

            HDC bitmapDC = CreateCompatibleDC(NULL);
            SelectObject(bitmapDC, bitmap);

            BitBlt(_dc, to.x(), to.y(), image.width(), image.height(),
                   bitmapDC, 0, 0, SRCCOPY);

            DeleteDC(bitmapDC);
            DeleteObject(bitmap);
            break;
        }

        case Gfx::CompositionMode::SourceOver:
        {
            std::vector<Pt::uint8_t> bitmapData;
            toPreMulAlpha(image, bitmapData);

            const Pt::uint8_t* data = bitmapData.empty() ? 0 : &bitmapData[0];
            size_t depth = 32;

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
                bitmap = CreateDIBSection(_dc, &bitmapInfo,
                                          DIB_RGB_COLORS, &imageBits, NULL, 0);
                memcpy(imageBits, data, image.width() * image.height() * 4);
            }

            HDC bitmapDC = CreateCompatibleDC(NULL);

            SelectObject(bitmapDC, bitmap);

            BLENDFUNCTION bf;
            bf.BlendOp = AC_SRC_OVER;
            bf.BlendFlags = 0;
            bf.SourceConstantAlpha = 0xFF; // only per pixel alpha
            bf.AlphaFormat = AC_SRC_ALPHA;

            AlphaBlend(_dc, to.x(), to.y(), _size.width(), _size.height(), 
                       bitmapDC, 0, 0, _size.width(), _size.height(), bf);

            DeleteObject(bitmap);
            DeleteDC(bitmapDC);
            break;
        }
    }
}


Gfx::Image PixmapSurfaceImpl::toImage() const
{
    BITMAPINFO bitmapInfo;
    ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = _size.width();
    bitmapInfo.bmiHeader.biHeight = -(ssize_t)_size.height();  // top-down image
    bitmapInfo.bmiHeader.biPlanes = 1;                         // always 1
    bitmapInfo.bmiHeader.biBitCount = 32;                      // bits per pixel
    bitmapInfo.bmiHeader.biCompression = BI_RGB;               // uncompressed RGB
    bitmapInfo.bmiHeader.biSizeImage = 0;                      // automatic
    bitmapInfo.bmiHeader.biClrUsed = 0;                        // no color table
    bitmapInfo.bmiHeader.biClrImportant = 0;                   // no color table

    Pt::Gfx::Image image( Pt::Gfx::ImageFormat::argb32(), round(_size) );
    Pt::uint8_t* data = image.data();

    int ret = GetDIBits(_dc, _bitmap, 0, _size.height(), data, 
                        &bitmapInfo, DIB_RGB_COLORS);

    return image;
}


void PixmapSurfaceImpl::set(const Gfx::Image& image)
{
    resize( Gfx::SizeF(image.size().width(), 
                       image.size().height() ) );

    size_t _width = image.width();
    size_t _height = image.height();

    std::vector<Pt::uint8_t> bitmapData;
    toPreMulAlpha(image, bitmapData);

    const Pt::uint8_t* data = bitmapData.empty() ? 0 : &bitmapData[0];

    const size_t depth = 32;

    HBITMAP bitmap = CreateBitmap(image.width(), image.height(), 
                                  1, depth, (VOID*)data);

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


const std::string& PixmapSurfaceImpl::defaultFont()
{
    return getDefaultFont();
}


void PixmapSurfaceImpl::setDefaultFont(const std::string& f)
{
    getDefaultFont() = f;
}


std::string& PixmapSurfaceImpl::getDefaultFont()
{
    static std::string _defaultFont; // = getSystemFont();
    return _defaultFont;
}


std::string PixmapSurfaceImpl::getSystemFont()
{
    HDC dc = GetDC(NULL);

    // TODO: returns a font named "System", which is useless... 

    std::vector<TCHAR> buffer(32);
    GetTextFace(dc, buffer.size(), &buffer[0]);

    ReleaseDC(NULL, dc);

    return Pt::win32::toMultiByte(&buffer[0]);
}


std::vector<std::string> PixmapSurfaceImpl::fontNames()
{
    std::vector<std::string> fonts;
    HDC dc = GetDC(NULL);

#ifdef _WIN32_WCE
    EnumFonts(dc, 0, (FONTENUMPROC)&EnumFontsProc, (LPARAM)this);
#else
    LOGFONT lf;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfFaceName[0] = '\0';
    lf.lfPitchAndFamily = 0;

    EnumFontFamiliesEx(dc, &lf, (FONTENUMPROC)&EnumFontFamExProc, (LPARAM)(&fonts), 0);
#endif

    ReleaseDC(NULL, dc);

    fonts.erase(std::unique(fonts.begin(), fonts.end()), fonts.end());
    return fonts;
}


void PixmapSurfaceImpl::setFontDir(const System::Path& path)
{
}


HDC PixmapSurfaceImpl::deviceContext() const
{
    return _dc;
}

} // namespace

} // namespace

#endif // PT_HMI_WIN32_RASTER
