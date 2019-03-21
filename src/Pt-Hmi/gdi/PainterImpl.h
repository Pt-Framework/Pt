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

#ifndef Pt_Hmi_PainterImpl_h
#define Pt_Hmi_PainterImpl_h

#include <Pt/Hmi/Api.h>
#include "win32.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/String.h>
#include <Windows.h>

namespace Pt {

namespace Hmi {

class PainterImpl
{
    public:
        PainterImpl()
        : _pen(0)
        , _penColor(0)
        , _brush(0)
        , _brushPen(0)
        , _gradientBrush(false)
        , _clipRect(0)
        , _font(0)
        { }

        ~PainterImpl()
        {
            if(_pen)
                DeleteObject(_pen);

            if(_brush)
                DeleteObject(_brush);

            if(_brushPen)
                DeleteObject(_brushPen);

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
            
            if(_brushPen)
            {
                DeleteObject(_brushPen);
                _brushPen = 0;
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
                    _brushPen = CreatePen(PS_SOLID, 1, brushColor);
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

                    bi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);    
                    bi.bmiHeader.biWidth        = texture.width();                // width
                    bi.bmiHeader.biHeight       = -(ssize_t)texture.height();     // top-down image
                    bi.bmiHeader.biPlanes       = 1;                              // always 1
                    bi.bmiHeader.biBitCount     = texture.view().pixelStride()*8; // 32-bit
                    bi.bmiHeader.biCompression  = BI_RGB;                         // uncompressed RGB
                    bi.bmiHeader.biSizeImage    = 0;                              // automatic
                    bi.bmiHeader.biClrUsed      = 0;                              // no color table
                    bi.bmiHeader.biClrImportant = 0;                              // no color table

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

        HPEN brushPen() const
        {
            return _brushPen;
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
                _clipRect = 0;
            }

            if( rect.isNull() )
                return;
            
            // CreateRectRgn only includes the interior of the rect
            _clipRect = CreateRectRgn( rect.x(), 
                                       rect.y(), 
                                       rect.bottomRight().x(), 
                                       rect.bottomRight().y() );
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

            Screen& screen = Application::instance().screen();
            std::size_t scaledSize = screen.scaleFactor() * font.size();
            Gfx::Font f = Gfx::Font(font.name(), scaledSize, 
                                    font.style(), font.angle());

            _font = getFont(f);
        }
        
        HFONT font() const
        {
            return _font;
        }
        
        static Gfx::FontMetrics fontMetrics(const Gfx::Font& font, 
                                            const Pt::String& text)
        {   
            Screen& screen = Application::instance().screen();
            std::size_t scaledSize = screen.scaleFactor() * font.size();
            Gfx::Font f = Gfx::Font(font.name(), scaledSize, 
                                    font.style(), font.angle());

            HDC dc = GetDC(NULL);
            HFONT newFont = getFont(f);
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

            return Gfx::FontMetrics( lround(tm.tmAscent / screen.scaleFactor()), 
                                     lround(tm.tmDescent / screen.scaleFactor()), 
                                     lround(textSize.cx / screen.scaleFactor()), 
                                     lround(tm.tmHeight / screen.scaleFactor()) );
        }
        
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
            }

#ifndef _WIN32_WCE
            switch( pen.capStyle() )
            {
                case Gfx::Pen::RoundCap:
                    penStyle |= PS_ENDCAP_ROUND;
                break;
                case Gfx::Pen::FlatCap:
                    penStyle |= PS_ENDCAP_FLAT;
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
            }
#endif

            return penStyle;
        }

        static HFONT getFont(const Pt::Gfx::Font& font)
        {
            int fontWeight;
    
            switch( font.style() ) 
            {
                default:
                case Pt::Gfx::Font::Normal:
                case Pt::Gfx::Font::Italic:
                    fontWeight = FW_NORMAL;
                    break;

                case Pt::Gfx::Font::Bold:
                case Pt::Gfx::Font::BoldItalic:
                    fontWeight = FW_BOLD;
                    break;
            }

            BYTE italic = font.style() == Pt::Gfx::Font::Italic || 
                          font.style() == Pt::Gfx::Font::BoldItalic;

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
            lf.lfEscapement     = font.angle();                // escapement angle
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
        HPEN   _brushPen;
        bool   _gradientBrush;
        HRGN   _clipRect;
        HFONT  _font;
};

} // namespace

} // namespace

#endif
