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

#include "PaintContext.h"
#include "PixmapImpl.h"
#include "win32.h"

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

} // namespace

namespace Pt {

namespace Forms {

#ifndef PT_FORMS_WIN32_RASTER

PaintContext::PaintContext()
: Gfx::PaintContext()
, _pixmapCanvas(0)
, _pen(0)
, _penSize(1)
, _penColor()
, _brush(0)
, _gradientBrush(false)
, _clipRect(0)
, _font(0)
{
    //std::clog << "PaintContext " << this << std::endl;
}


PaintContext::~PaintContext()
{
    //std::clog << "~PaintContext " << this << std::endl;

    if(_pen)
        DeleteObject(_pen);

    if(_brush)
        DeleteObject(_brush);

    if(_font)
        DeleteObject(_font);

    if(_clipRect)
        DeleteObject(_clipRect);
}


void PaintContext::setPixmap(PixmapCanvas& pixmap)
{
    _pixmapCanvas = &pixmap;
}


void PaintContext::onBeginPaint(const Gfx::Paint& paint)
{
    double scaleFactor = scaling().scaleFactor();

    size_t penSize = paint.pen().size();
    
    // keep pen size when downscaling
    penSize = scaleFactor < 1.0 ? penSize
                                : static_cast<size_t>( penSize * scaleFactor );

    if(_penSize != penSize)
        onSetPen( paint.pen() );

    //std::clog << "onBeginPaint " << this << std::endl;
}


void PaintContext::onResetPaint()
{
    //std::clog << "onResetPaint " << this << std::endl;

    if(_pixmapCanvas)
        _pixmapCanvas = 0;
}


const Gfx::CompositionMode& PaintContext::compositionMode() const
{
    return _compositionMode;
}


void PaintContext::onSetCompositionMode(const Gfx::CompositionMode& mode) 
{
    _compositionMode = mode;
}


HPEN PaintContext::pen() const
{
    return _pen;
}


Gfx::Color PaintContext::penColor() const
{
    return _penColor;
}


void PaintContext::onSetPen(const Gfx::Pen& pen)
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


void PaintContext::onApplyPen(const Gfx::Pen& pen)
{
    if(_pixmapCanvas)
    {
        DWORD penColor = RGB( _penColor.red()  / 257, 
                              _penColor.green() / 257, 
                              _penColor.blue()  / 257 );
        
        _pixmapCanvas->updatePen(_pen, penColor);
    }
}


HBRUSH PaintContext::brush() const
{
    return _brush;
}


bool PaintContext::gradientBrush() const
{
    return _gradientBrush;
}


const Gfx::Brush::GradientStyle& PaintContext::gradient() const
{
    return _gradient;
}

        
const Gfx::Color& PaintContext::gradientStart() const
{
    return _gradientStart;
}


const Gfx::Color& PaintContext::gradientStop() const
{
    return _gradientStop;
}


void PaintContext::onSetBrush(const Gfx::Brush& brush)
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

            std::size_t depth = texture.view().pixelStride() * 8;

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
                    texture.width() * texture.height() * texture.view().pixelStride());

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


HFONT PaintContext::font() const
{
    return _font;
}


void PaintContext::onSetFont(const Gfx::Font& font)
{
    if(_font)
    {
        DeleteObject(_font);
        _font = 0;
    }

    _font = getFont(font);
}


HRGN PaintContext::clipRect() const
{
    return _clipRect;
}


void PaintContext::onSetClip(const Gfx::RectF* rectF)
{
    // TODO: reuse HRGN instead of always creating a new one

    if(_clipRect)
    {
        DeleteObject(_clipRect);
        _clipRect = NULL;
    }

    if( ! rectF )
        return;
                                                       
    Gfx::RectF rectP = scaling().toPhysical(*rectF);
               
    long x = Pt::lround( rectP.x() );
    long y = Pt::lround( rectP.y() );
    long width = Pt::lround( rectP.width() );
    long height = Pt::lround( rectP.height() );
                            
    // CreateRectRgn only includes the interior of the rect
    _clipRect = CreateRectRgn(x , y, x + width, y + height);
}


void PaintContext::onSetPath(const Gfx::Path& path)
{
    _path = path;
}


POINT PaintContext::toContext(double x, double y)
{
    Gfx::PointF p = transform() * Gfx::PointF(x, y);

    POINT pp;
    pp.x = Pt::lround(p.x() - 0.4999);
    pp.y = Pt::lround(p.y() - 0.4999);

    return pp;
}


void PaintContext::buildPath(HDC dc, const Gfx::Path& path)
{
    BeginPath(dc);

    for(std::size_t n = 0; n < path.size(); ++n)
    {
        const Gfx::Element& e = path.at(n);

        switch( e.type )
        {
            default:
                break;

            case Gfx::Element::IT_Close:
                CloseFigure(dc);
                break;

            case Gfx::Element::IT_MoveTo:
            {
                POINT p = toContext( e.pxy.at(0), e.pxy.at(1) );
                MoveToEx(dc, p.x, p.y, NULL);
                break;
            }

            case Gfx::Element::IT_LineTo:
            {
                POINT p = toContext( e.pxy.at(0), e.pxy.at(1) );
                LineTo(dc, p.x, p.y);
                break;
            }

            case Gfx::Element::IT_QuadBezierTo:
            {
                POINT p0;
                GetCurrentPositionEx(dc, &p0);

                POINT p1 = toContext( e.pxy.at(0), e.pxy.at(1) );
                POINT p2 = toContext( e.pxy.at(2), e.pxy.at(3) );

                POINT cubicPoints[3];

                // control 1: P0 + (2/3) * (P1 - P0)
                cubicPoints[0].x = p0.x + (2 * (p1.x - p0.x)) / 3;
                cubicPoints[0].y = p0.y + (2 * (p1.y - p0.y)) / 3;

                // control2 2: P2 + (2/3) * (P1 - P2)
                cubicPoints[1].x = p2.x + (2 * (p1.x - p2.x)) / 3;
                cubicPoints[1].y = p2.y + (2 * (p1.y - p2.y)) / 3;

                // end point
                cubicPoints[2] = p2;
                
                PolyBezierTo(dc, cubicPoints, 3);
                break;
            }
            
            case Gfx::Element::IT_CubicBezierTo:
            {
                POINT points[3];

                points[0] = toContext( e.pxy.at(0), e.pxy.at(1) );
                points[1] = toContext( e.pxy.at(2), e.pxy.at(3) );
                points[2] = toContext( e.pxy.at(4), e.pxy.at(5) );
                
                PolyBezierTo(dc, points, 3);
                break;
            }

            //case Element::IT_GenNBezierTo:
            //    bezierToPoints(polygon.points(), curX, curY, ins.pxy, smoothness);
            //    curX = ins.pxy[ins.pxy.size() - 2];
            //    curY = ins.pxy[ins.pxy.size() - 1];
            //    break;
        }
    }

    EndPath(dc);
}


void PaintContext::onDrawPath()
{
    if(_pixmapCanvas)
    {
        HDC dc = _pixmapCanvas->deviceContext();
        buildPath(dc, _path);
        StrokePath(dc);
    }
}


void PaintContext::onFillPath()
{
    if(_pixmapCanvas)
    {
        HDC dc = _pixmapCanvas->deviceContext();
        buildPath(dc, _path);
        FillPath(dc);
    }
}

#endif // PT_FORMS_WIN32_RASTER

} // namespace

} // namespace
