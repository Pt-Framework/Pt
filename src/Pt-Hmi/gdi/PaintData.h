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

#ifndef Pt_Hmi_PaintData_h
#define Pt_Hmi_PaintData_h

#include "win32.h"

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/String.h>
#include <algorithm>
#include <cctype>

using std::max;
using std::min;
#include <Windows.h>
#include <Gdiplus.h>

//#define PT_HMI_GDIPLUS 1

namespace Pt {

namespace Hmi {

class PaintData : public Gfx::PaintData
{
    public:
        PaintData()
        : _pen(0)
        , _penColor(0)
        , _brush(0)
        , _gradientBrush(false)
        , _clipRect(0)
        , _font(0)
        { }

        ~PaintData()
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

        void setPen(const Gfx::Pen& pen)
        {
            if(_pen)
            {
                DeleteObject(_pen);
                _pen = 0;
            }

            DWORD penStyle = getPenStyle(pen);

            _penColor = RGB( pen.color().red()  / 257, 
                             pen.color().green() / 257, 
                             pen.color().blue()  / 257 );

#ifdef _WIN32_WCE
            _pen = CreatePen(penStyle, pen.size(), _penColor);
#else
            LOGBRUSH brush;
            brush.lbStyle = BS_SOLID;
            brush.lbColor = _penColor;

            _pen = ExtCreatePen(penStyle, pen.size(), &brush, 0, NULL);
#endif
        }

        HPEN pen() const
        {
            return _pen;
        }

        DWORD penColor() const
        {
            return _penColor;
        }

        void setBrush(const Gfx::Brush& brush)
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
                    break;
                }
                
                default:
                    break;
            }
        }

        HBRUSH brush() const
        {
            return _brush;
        }

        bool gradientBrush() const
        {
            return _gradientBrush;
        }

        void setClip(const Gfx::RectF& rectF)
        {
            Gfx::Rect rect = round(rectF);

            if(_clipRect)
            {
                DeleteObject(_clipRect);
                _clipRect = NULL;
            }
            
            // CreateRectRgn only includes the interior of the rect
            _clipRect = CreateRectRgn( rect.x(), 
                                       rect.y(), 
                                       rect.bottomRight().x(), 
                                       rect.bottomRight().y() );
        }

        void resetClip()
        {
            if(_clipRect)
            {
                DeleteObject(_clipRect);
                _clipRect = NULL;
            }
        }

        HRGN clipRect() const
        {
            return _clipRect;
        }

        void setCompositionMode(const Gfx::CompositionMode& mode)
        {
        }

        void setFont(const Gfx::Font& font)
        {
            if(_font)
            {
                DeleteObject(_font);
                _font = 0;
            }

            _font = getFont(font);
        }
        
        HFONT font() const
        {
            return _font;
        }
        
#ifndef PT_HMI_GDIPLUS
        static Gfx::FontMetrics fontMetrics(const Gfx::Font& font, 
                                            const Pt::String& text)
        {   
            HDC dc = GetDC(NULL);
            HFONT newFont = getFont(font);
            HGDIOBJ oldFont = SelectObject(dc, newFont);

            TEXTMETRIC tm;
            GetTextMetrics(dc, &tm);

            std::wstring wtext;
            text.toUtf16( std::back_inserter(wtext) );
    
            SIZE textSize;
            GetTextExtentPoint32W(dc, wtext.c_str(), wtext.size(), &textSize);
    
            SelectObject(dc, oldFont);
            DeleteObject(newFont);
            ReleaseDC(NULL, dc);

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
            return fm;
        }

#else
        static Gfx::FontMetrics fontMetrics(const Gfx::Font& font, 
                                            const Pt::String& text)
        {   
            std::wstring wtext;
            text.toUtf16( std::back_inserter(wtext) );

            HDC dc = GetDC(NULL);
            HFONT newFont = getFont(font);
            HGDIOBJ oldFont = SelectObject(dc, newFont);

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
            double scaling = 96.0 / dpix;

            SelectObject(dc, oldFont);
            DeleteObject(newFont);
            ReleaseDC(NULL, dc);

            Gfx::FontMetrics fm;
            fm.setAscent(asc * scaling);
            fm.setDescent(des * scaling);
            fm.setCapHeight(cap * scaling);
            fm.setLeading(exl* scaling);
            fm.setWidth(textRect.Width * scaling);
            return fm;
        }
#endif
        static std::string defaultFont()
        {
            return getDefaultFont();
        }

        static void setDefaultFont(const std::string& f)
        {
            getDefaultFont() = f;
        }

        static std::string& getDefaultFont()
        { 
            static std::string _defaultFont;
            return _defaultFont; 
        }
    
    private:
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

        static HFONT getFont(const Pt::Gfx::Font& font)
        {
            int fontWeight = FW_NORMAL;
    
            std::string style = font.style();
            std::transform(style.begin(), style.end(),  style.begin(), ::tolower);

            if(style == "bold" || style == "bold italic" || style == "bolditalic")
                fontWeight = FW_BOLD;

            BYTE italic = (style == "italic" || style == "bold italic" || style == "bolditalic");

            HDC dc = GetDC(NULL);
            int logicalPPI = GetDeviceCaps(dc, LOGPIXELSY);
            int height = MulDiv(font.size(), logicalPPI, 72);
            ReleaseDC(NULL, dc);
            
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
                memcpy(lf.lfFaceName, defaultFont().c_str(), std::min<size_t>( LF_FACESIZE, defaultFont().size() + 1) );
            }
            else
            {
                memcpy(lf.lfFaceName, font.name().c_str(), std::min<size_t>( LF_FACESIZE, font.name().size() + 1) );
            }

            HFONT hf = CreateFontIndirect(&lf);
            return hf;
        }

    private:
        HPEN   _pen;
        DWORD  _penColor;
        HBRUSH _brush;
        bool   _gradientBrush;
        HRGN   _clipRect;
        HFONT  _font;
};

} // namespace

} // namespace

#endif
