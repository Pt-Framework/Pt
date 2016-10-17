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
#include "PaintSurfaceImpl.h"
#include "PictureImpl.h"
#include <Pt/Hmi/Application.h>
#include <tchar.h>

namespace {

std::string getDefaultFont()
{
    HDC dc = GetDC(NULL);

    std::vector<TCHAR> buffer(32);
    GetTextFace(dc, buffer.size(), &buffer[0]);

    ReleaseDC(NULL, dc);

    return Pt::win32::toMultiByte(&buffer[0]);
}

}

namespace Pt {

namespace Hmi {

#ifdef _WIN32_WCE

static int CALLBACK EnumFontsProc(LOGFONT *logFont, TEXTMETRIC *physFont, DWORD type, LPARAM param)
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

static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *logFont, NEWTEXTMETRICEX *physFont, DWORD type, LPARAM param)
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

std::string PaintSurfaceImpl::_defaultFont; //getDefaultFont();


PaintSurfaceImpl::PaintSurfaceImpl()
{
}


PaintSurfaceImpl::~PaintSurfaceImpl()
{
}


std::string PaintSurfaceImpl::defaultFont()
{
    return _defaultFont;
}


void PaintSurfaceImpl::setDefaultFont(const std::string& f)
{
    _defaultFont = f;
}


std::vector<std::string> PaintSurfaceImpl::fontNames()
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

    fonts.erase( std::unique(fonts.begin(), fonts.end()), fonts.end() );
    return fonts;
}


Gfx::FontMetrics PaintSurfaceImpl::fontMetrics(const Gfx::Font& font, const Pt::String& text)
{   
    HDC dc = GetDC(NULL);

    HFONT newFont = getFont(font);
    HFONT oldFont = (HFONT)SelectObject(dc, newFont);
    
    SIZE textSize;
    TEXTMETRIC tm;
    GetTextMetrics(dc, &tm);

    std::wstring wtext;
    text.toUtf16( std::back_inserter(wtext) );
    
    GetTextExtentPoint32W(dc, wtext.c_str(), wtext.size(), &textSize);
    
    Gfx::Size size(textSize.cx, textSize.cy);
    Gfx::SizeF sizeF = Application::instance().screen().toUnit(size);

    SelectObject(dc, oldFont);
    DeleteObject(newFont);

    ReleaseDC(NULL, dc);

    return Gfx::FontMetrics(tm.tmAscent, 
                            tm.tmDescent, 
                            (int)sizeF.width(), 
                            (int)sizeF.height());
}


HFONT PaintSurfaceImpl::getFont(const Pt::Gfx::Font& font)
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

    LOGFONT lf;
    lf.lfHeight         = -((int)font.size());         // converted to device units
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
        memcpy(lf.lfFaceName, Pt::Hmi::PaintSurfaceImpl::defaultFont().c_str(), std::min<size_t>( LF_FACESIZE, Pt::Hmi::PaintSurfaceImpl::defaultFont().size() + 1) );
    }
    else
    {
        memcpy(lf.lfFaceName, font.name().c_str(), std::min<size_t>( LF_FACESIZE, font.name().size() + 1) );
    }

    HFONT hf = CreateFontIndirect(&lf);
    return hf;
}

} // namespace

} // namespace
