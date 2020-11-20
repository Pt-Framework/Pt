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

using std::max;
using std::min;
#include <Windows.h>
#include <Gdiplus.h>

namespace Pt {

namespace Hmi {

class PaintData : public Gfx::PaintData
{
    public:
        PaintData()
        : _pen( new Gdiplus::Pen(Gdiplus::Color(0, 0, 0), 1) )
        , _brush( new Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0)) )
        , _font( new Gdiplus::Font(L"", 1) )
        { }

        ~PaintData()
        {
            delete _pen;
            delete _brush;
            delete _font;
        }

        void setPen(const Gfx::Pen& pen)
        {
            delete _pen;

            _pen = new Gdiplus::Pen(toGdi(pen.color()), static_cast<Gdiplus::REAL>(pen.size()));

            switch( pen.style() )
            {
                case Gfx::Pen::Solid:
                    _pen->SetDashStyle(Gdiplus::DashStyleSolid);
                break;

                case Gfx::Pen::Dash:
                    _pen->SetDashStyle(Gdiplus::DashStyleDash);
                break;

                case Gfx::Pen::Dot:
                    _pen->SetDashStyle(Gdiplus::DashStyleDot);
                break;

                case Gfx::Pen::DashPattern:
                {
                    const std::vector<Pt::uint8_t>& dashPattern = pen.dashPattern();
                    const std::vector<Gdiplus::REAL> gdiPattern(dashPattern.begin(), dashPattern.end());

                    _pen->SetDashPattern(&gdiPattern[0], gdiPattern.size());
                }
                break;
            }

            switch( pen.capStyle() )
            {
                case Gfx::Pen::FlatCap:
                    _pen->SetLineCap(Gdiplus::LineCapFlat, Gdiplus::LineCapFlat, Gdiplus::DashCapFlat);
                break;

                case Gfx::Pen::RoundCap:
                    _pen->SetLineCap(Gdiplus::LineCapRound, Gdiplus::LineCapRound, Gdiplus::DashCapRound);
                break;

                case Gfx::Pen::SquareCap:
                    _pen->SetLineCap(Gdiplus::LineCapSquare, Gdiplus::LineCapSquare, Gdiplus::DashCapFlat);
                break;
            }

            switch( pen.joinStyle() )
            {
                case Gfx::Pen::RoundJoin:
                    _pen->SetLineJoin(Gdiplus::LineJoinRound);
                break;

                case Gfx::Pen::BevelJoin:
                    _pen->SetLineJoin(Gdiplus::LineJoinBevel);
                break;

                case Gfx::Pen::MiterJoin:
                    _pen->SetLineJoin(Gdiplus::LineJoinMiter);
                break;
            }
        }

        const Gdiplus::Pen& pen() const
        {
            return *_pen;
        }

        void setBrush(const Gfx::Brush& brush)
        {
            delete _brush;

            switch( brush.fillStyle() ) 
            {
                case Gfx::Brush::Solid: 
                {
                    _brush = new Gdiplus::SolidBrush(toGdi(brush.color()));
                }
                break;

                case Gfx::Brush::Texture: 
                {
                    /*
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
                    */
                }
                break;

                case Gfx::Brush::Gradient:
                {
                    switch (brush.gradient())
                    {
                        case Gfx::Brush::Linear:
                        case Gfx::Brush::Horizontal:
                        case Gfx::Brush::Vertical:
                            _brush = new Gdiplus::LinearGradientBrush(toGdi(brush.gradientBegin()),
                                                                      toGdi(brush.gradientEnd()),
                                                                      toGdi(brush.color()), 
                                                                      toGdi(brush.gradientColor()));
                        break;

                        case Gfx::Brush::Radial:

                        break;
                    }
                }
                break;

                default:
                break;
            }
        }

        const Gdiplus::Brush& brush() const
        {
            return *_brush;
        }

        void setCompositionMode(const Gfx::CompositionMode& mode)
        {
        }

        void setFont(const Gfx::Font& font)
        {
            delete _font;
            _font = toGdi(font);

            _font->GetFamily(&_fontFamily);
        }
        
        const Gdiplus::Font& font() const
        {
           return *_font;
        }

        const Gdiplus::FontFamily& fontFamily() const
        {
            return _fontFamily;
        }

        static Gfx::FontMetrics fontMetrics(const Gfx::Font& font, 
                                            const Pt::String& text)
        {   
            std::wstring wtext;
            text.toUtf16( std::back_inserter(wtext) );

            HDC dc = GetDC(NULL);
            Gdiplus::Font* gdiFont = toGdi(font);
            
            Gdiplus::Graphics graphics(dc);
            graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);
            graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);

            const Gdiplus::StringFormat* format = Gdiplus::StringFormat::GenericTypographic();

            Gdiplus::FontFamily family;
            gdiFont->GetFamily(&family);

            Gdiplus::REAL height = gdiFont->GetHeight( graphics.GetDpiY() );

            UINT16 ascentUnits = family.GetCellAscent( gdiFont->GetStyle() );
            UINT16 descentUnits = family.GetCellDescent( gdiFont->GetStyle() );
            UINT16 heightUnits = family.GetLineSpacing( gdiFont->GetStyle() );
            
            Gdiplus::REAL pixelsPerUnit = height / heightUnits;
            Gdiplus::REAL ascentF = ascentUnits * pixelsPerUnit;
            Gdiplus::REAL descentF = descentUnits * pixelsPerUnit;
            Gdiplus::REAL heightF = ascentF + descentF;

            Gdiplus::RectF textRect;
            graphics.MeasureString(wtext.c_str(), wtext.size(), gdiFont, 
                                   Gdiplus::PointF(0, 0), format, &textRect);

            const int dpix = GetDeviceCaps(dc, LOGPIXELSX);
            const double scaling = 96.0 /dpix;

            ReleaseDC(NULL, dc);
            delete gdiFont;

            return Gfx::FontMetrics(ascentF* scaling, descentF* scaling, 
                                    textRect.Width * scaling, heightF * scaling);
        }
    
        static Gdiplus::Color toGdi(const Gfx::Color& c)
        {
            return Gdiplus::Color(c.alpha() / 257, c.red() / 257, c.green() / 257, c.blue() / 257);
        }

        static Gdiplus::PointF toGdi(const Gfx::PointF& p)
        {
            return Gdiplus::PointF(static_cast<Gdiplus::REAL>(p.x()),
                                   static_cast<Gdiplus::REAL>(p.y()));
        }

        static Gdiplus::SizeF toGdi(const Gfx::SizeF& s)
        {
            return Gdiplus::SizeF(static_cast<Gdiplus::REAL>(s.width()),
                                  static_cast<Gdiplus::REAL>(s.height()));
        }

        static Gdiplus::RectF toGdi(const Gfx::RectF& r)
        {
            return Gdiplus::RectF(toGdi(r.topLeft()), toGdi(r.size()));
        }

        static Gdiplus::RectF toGdi(const Gfx::PointF& p, const Gfx::SizeF& s)
        {
            return Gdiplus::RectF(toGdi(p), toGdi(s));
        }

        static Gdiplus::Font* toGdi(const Pt::Gfx::Font& font)
        {
            Gdiplus::Font* f = 0;

            const std::wstring fname(font.name().begin(), font.name().end());

            switch (font.style())
            {
                default:
                case Pt::Gfx::Font::Normal:
                    f = new Gdiplus::Font(fname.c_str(), static_cast<Gdiplus::REAL>(font.size()),
                                          Gdiplus::FontStyleRegular);
                break;

                case Pt::Gfx::Font::Italic:
                    f = new Gdiplus::Font(fname.c_str(), static_cast<Gdiplus::REAL>(font.size()),
                                          Gdiplus::FontStyleItalic);
                break;

                case Pt::Gfx::Font::Bold:
                    f = new Gdiplus::Font(fname.c_str(), static_cast<Gdiplus::REAL>(font.size()),
                                          Gdiplus::FontStyleBold);
                break;

                case Pt::Gfx::Font::BoldItalic:
                    f = new Gdiplus::Font(fname.c_str(), static_cast<Gdiplus::REAL>(font.size()), 
                                          Gdiplus::FontStyleBoldItalic);
                break;
            }

            return f;
        }

    private:
        Gdiplus::Pen*       _pen;
        Gdiplus::Brush*     _brush;
        Gdiplus::Font*      _font;
        Gdiplus::FontFamily _fontFamily;
};

} // namespace

} // namespace

#endif
