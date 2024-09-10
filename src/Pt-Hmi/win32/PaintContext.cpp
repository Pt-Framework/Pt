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
#include "PixmapSurfaceImpl.h"
#include "win32.h"

#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Gfx/Algorithm.h>

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
        const std::string& fontName = Pt::Hmi::PixmapSurfaceImpl::defaultFont();
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

namespace Hmi {

#ifndef PT_HMI_WIN32_RASTER

PaintContext::PaintContext()
: Gfx::PaintContext()
, _pen(0)
, _penColor()
, _brush(0)
, _gradientBrush(false)
, _clipRect(0)
, _font(0)
{
}


PaintContext::~PaintContext()
{
    if(_pen)
        DeleteObject(_pen);

    if(_brush)
        DeleteObject(_brush);

    if(_font)
        DeleteObject(_font);

    if(_clipRect)
        DeleteObject(_clipRect);
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
    double scaledSize = scaling().toPhysical( pen.size() );

    // keep pen size when downscaling
    size_t penSize = scaledSize < 1.0 ? 1 
                                : static_cast<size_t>(scaledSize);

    if(_pen)
    {
        DeleteObject(_pen);
        _pen = 0;
    }

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
                                                       
    Gfx::Rect rect = round( scaling().toPhysical(*rectF) );
                            
    // CreateRectRgn only includes the interior of the rect
    _clipRect = CreateRectRgn( rect.x(), 
                               rect.y(), 
                               rect.bottomRight().x(), 
                               rect.bottomRight().y() );
}

#endif

} // namespace

} // namespace
