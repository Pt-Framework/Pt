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

#include "GdiFontProvider.h"

#ifndef PT_FORMS_WIN32_RASTER

#include <Pt/Gfx/FontRegistry.h>
#include <Pt/System/FileInfo.h>

#include <algorithm>
#include <limits>

namespace Pt {

namespace Forms {

Gfx::FontFace::Weight GdiFontProvider::fontWeightFromLogFontWeight(LONG weight)
{
    if(weight >= 900)
        return Gfx::FontFace::Weight::Black;

    if(weight >= 800)
        return Gfx::FontFace::Weight::ExtraBold;

    if(weight >= 700)
        return Gfx::FontFace::Weight::Bold;

    if(weight >= 600)
        return Gfx::FontFace::Weight::SemiBold;

    if(weight >= 500)
        return Gfx::FontFace::Weight::Medium;

    if(weight >= 400)
        return Gfx::FontFace::Weight::Normal;

    if(weight >= 300)
        return Gfx::FontFace::Weight::Light;

    if(weight >= 200)
        return Gfx::FontFace::Weight::ExtraLight;

    return Gfx::FontFace::Weight::Thin;
}


Gfx::FontFace::Slant GdiFontProvider::fontSlantFromLogFontItalic(BYTE italic)
{
    return italic != FALSE
         ? Gfx::FontFace::Slant::Italic
         : Gfx::FontFace::Slant::Normal;
}


int CALLBACK GdiFontProvider::enumFontFamExProc(ENUMLOGFONTEX* logFont, NEWTEXTMETRICEX* physFont, DWORD type, LPARAM param)
{
    if(logFont->elfLogFont.lfFaceName[0] != '@')
    {
        std::string family = Pt::win32::toMultiByte(logFont->elfLogFont.lfFaceName);
        std::string style = Pt::win32::toMultiByte(reinterpret_cast<LPCTSTR>(logFont->elfStyle));
        Gfx::FontFace::Weight weight = fontWeightFromLogFontWeight(logFont->elfLogFont.lfWeight);
        Gfx::FontFace::Slant slant = fontSlantFromLogFontItalic(logFont->elfLogFont.lfItalic);
        reinterpret_cast<std::vector<Gfx::FontFace>*>(param)->push_back(Gfx::FontFace(family, weight, slant, style));
    }

    return 1;
}


int CALLBACK GdiFontProvider::enumFontFamilyNamesExProc(ENUMLOGFONTEX* logFont, NEWTEXTMETRICEX* physFont, DWORD type, LPARAM param)
{
    if(logFont->elfLogFont.lfFaceName[0] != '@')
        reinterpret_cast<FontFamilyList*>(param)->families.push_back(Pt::win32::toMultiByte(logFont->elfLogFont.lfFaceName));

    return 1;
}


GdiFontProvider& GdiFontProvider::instance()
{
    static GdiFontProvider provider;
    return provider;
}


GdiFontProvider::GdiFontProvider()
: _defaultFont()
{
    const std::vector<Pt::System::Path>& fontFiles = Pt::Gfx::FontRegistry::instance().fontFiles();
    for(std::vector<Pt::System::Path>::const_iterator it = fontFiles.begin(); it != fontFiles.end(); ++it)
        addFont(*it);
}


GdiFontProvider::~GdiFontProvider()
{
    while( ! _files.empty() )
    {
        unregisterFontFile(_files.back());
        _files.pop_back();
    }
}


const std::string& GdiFontProvider::defaultFont() const
{
    return _defaultFont;
}


void GdiFontProvider::setDefaultFont(const std::string& font)
{
    _defaultFont = font;
}


HFONT GdiFontProvider::lookupFont(const Gfx::Font& font) const
{
    int fontWeight = static_cast<int>(font.weight());
    BYTE italic = font.slant() == Pt::Gfx::Font::Slant::Normal ? FALSE : TRUE;

    int logicalPPI = 96;

    std::size_t logicalSize = font.size();
    int fontSize = logicalSize > static_cast<std::size_t>(std::numeric_limits<int>::max())
                 ? std::numeric_limits<int>::max()
                 : static_cast<int>(logicalSize);
    int height = MulDiv(fontSize, logicalPPI, 72);

    LOGFONT lf;
    ZeroMemory(&lf, sizeof(lf));
    lf.lfHeight         = -height;
    lf.lfWidth          = 0;
    lf.lfEscapement     = 0;
    lf.lfOrientation    = 0;
    lf.lfWeight         = fontWeight;
    lf.lfItalic         = italic;
    lf.lfUnderline      = FALSE;
    lf.lfStrikeOut      = FALSE;
    lf.lfCharSet        = DEFAULT_CHARSET;
    lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
    lf.lfQuality        = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

    const std::string& family = font.family().empty() ? _defaultFont
                                                       : font.family();
    if( ! family.empty() )
    {
        std::basic_string<TCHAR> nativeFamily = Pt::win32::fromMultiByte(family);
        std::size_t copySize = std::min<std::size_t>(static_cast<std::size_t>(LF_FACESIZE), nativeFamily.size() + 1);
        memcpy(lf.lfFaceName, nativeFamily.c_str(), copySize * sizeof(TCHAR));
    }

    return CreateFontIndirect(&lf);
}


std::vector<std::string> GdiFontProvider::fontFamilies() const
{
    FontFamilyList familyList;
    HDC dc = GetDC(NULL);

    LOGFONT lf;
    ZeroMemory(&lf, sizeof(lf));
    lf.lfCharSet = DEFAULT_CHARSET;

    EnumFontFamiliesEx(dc, &lf, (FONTENUMPROC)&enumFontFamilyNamesExProc, (LPARAM)(&familyList), 0);

    ReleaseDC(NULL, dc);

    std::sort(familyList.families.begin(), familyList.families.end());
    familyList.families.erase(std::unique(familyList.families.begin(), familyList.families.end()), familyList.families.end());
    return familyList.families;
}


std::vector<Gfx::FontFace> GdiFontProvider::fontFaces(const std::string& family) const
{
    std::vector<Gfx::FontFace> faces;
    if(family.empty())
        return faces;

    HDC dc = GetDC(NULL);

    LOGFONT lf;
    ZeroMemory(&lf, sizeof(lf));
    lf.lfCharSet = DEFAULT_CHARSET;

    std::basic_string<TCHAR> nativeFamily = Pt::win32::fromMultiByte(family);
    std::size_t copySize = std::min<std::size_t>(static_cast<std::size_t>(LF_FACESIZE), nativeFamily.size() + 1);
    memcpy(lf.lfFaceName, nativeFamily.c_str(), copySize * sizeof(TCHAR));

    EnumFontFamiliesEx(dc, &lf, (FONTENUMPROC)&enumFontFamExProc, (LPARAM)(&faces), 0);

    ReleaseDC(NULL, dc);

    std::sort(faces.begin(), faces.end());
    faces.erase(std::unique(faces.begin(), faces.end()), faces.end());
    return faces;
}


void GdiFontProvider::onAddFont(const Pt::System::Path& path)
{
    if( ! Pt::System::FileInfo::exists(path) )
        return;

    if(std::find(_files.begin(), _files.end(), path) != _files.end())
        return;

    if( ! registerFontFile(path) )
        return;

    _files.push_back(path);
}


void GdiFontProvider::onRemoveFont(const Pt::System::Path& path)
{
    std::vector<Pt::System::Path>::iterator pos = std::find(_files.begin(), _files.end(), path);
    if(pos == _files.end())
        return;

    unregisterFontFile(*pos);
    _files.erase(pos);
}


bool GdiFontProvider::registerFontFile(const Pt::System::Path& path)
{
    std::string localPath = path.toLocal();
    return AddFontResourceExA(localPath.c_str(), FR_PRIVATE, 0) != 0;
}


void GdiFontProvider::unregisterFontFile(const Pt::System::Path& path)
{
    std::string localPath = path.toLocal();
    RemoveFontResourceExA(localPath.c_str(), FR_PRIVATE, 0);
}

} // namespace Forms

} // namespace Pt

#endif // PT_FORMS_WIN32_RASTER