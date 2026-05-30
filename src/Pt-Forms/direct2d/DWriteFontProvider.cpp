/* Copyright (C) 2015-2026 Marc Boris Duerner

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

#include "DWriteFontProvider.h"
#include "D2DDevice.h"
#include "ApplicationImpl.h"

#include <Pt/Forms/Application.h>

#include <cassert>

namespace Pt {

namespace Forms {

DWriteFontProvider::DWriteFontProvider()
: _defaultFont("Segoe UI")
{
}


DWriteFontProvider::~DWriteFontProvider()
{
}


DWriteFontProvider& DWriteFontProvider::instance()
{
    static DWriteFontProvider provider;
    return provider;
}


const std::string& DWriteFontProvider::defaultFont() const
{
    return _defaultFont;
}


void DWriteFontProvider::setDefaultFont(const std::string& font)
{
    _defaultFont = font;
}


std::vector<std::string> DWriteFontProvider::fontFamilies() const
{
    std::vector<std::string> families;

    IDWriteFactory* factory = Application::instance().impl()->d2d().dwriteFactory();
    if( ! factory)
        return families;

    IDWriteFontCollection* collection = nullptr;
    HRESULT hr = factory->GetSystemFontCollection(&collection);
    if(FAILED(hr) || ! collection)
        return families;

    UINT32 count = collection->GetFontFamilyCount();
    families.reserve(count);

    for(UINT32 i = 0; i < count; ++i)
    {
        IDWriteFontFamily* family = nullptr;
        hr = collection->GetFontFamily(i, &family);
        if(FAILED(hr) || ! family)
            continue;

        IDWriteLocalizedStrings* names = nullptr;
        hr = family->GetFamilyNames(&names);
        if(FAILED(hr) || ! names)
        {
            family->Release();
            continue;
        }

        UINT32 nameIndex = 0;
        BOOL exists = FALSE;
        names->FindLocaleName(L"en-us", &nameIndex, &exists);
        if( ! exists)
            nameIndex = 0;

        UINT32 length = 0;
        names->GetStringLength(nameIndex, &length);

        std::wstring wname(length + 1, L'\0');
        names->GetString(nameIndex, &wname[0], length + 1);
        wname.resize(length);

        // Convert wide string to narrow (ASCII family names)
        std::string name;
        name.reserve(wname.size());
        for(size_t c = 0; c < wname.size(); ++c)
            name.push_back( static_cast<char>(wname[c]) );
        families.push_back(name);

        names->Release();
        family->Release();
    }

    collection->Release();
    return families;
}


std::vector<Gfx::FontFace> DWriteFontProvider::fontFaces(
    const std::string& family) const
{
    std::vector<Gfx::FontFace> faces;

    IDWriteFactory* factory = Application::instance().impl()->d2d().dwriteFactory();
    if( ! factory)
        return faces;

    IDWriteFontCollection* collection = nullptr;
    HRESULT hr = factory->GetSystemFontCollection(&collection);
    if(FAILED(hr) || ! collection)
        return faces;

    std::wstring wfamily(family.begin(), family.end());

    UINT32 index = 0;
    BOOL exists = FALSE;
    collection->FindFamilyName(wfamily.c_str(), &index, &exists);

    if( ! exists)
    {
        collection->Release();
        return faces;
    }

    IDWriteFontFamily* fontFamily = nullptr;
    hr = collection->GetFontFamily(index, &fontFamily);
    if(FAILED(hr) || ! fontFamily)
    {
        collection->Release();
        return faces;
    }

    UINT32 count = fontFamily->GetFontCount();

    for(UINT32 i = 0; i < count; ++i)
    {
        IDWriteFont* dwFont = nullptr;
        hr = fontFamily->GetFont(i, &dwFont);
        if(FAILED(hr) || ! dwFont)
            continue;

        DWRITE_FONT_WEIGHT weight = dwFont->GetWeight();
        DWRITE_FONT_STYLE style = dwFont->GetStyle();

        Gfx::Font::Weight faceWeight = Gfx::Font::Weight::Normal;
        if(weight >= DWRITE_FONT_WEIGHT_BOLD)
            faceWeight = Gfx::Font::Weight::Bold;
        else if(weight <= DWRITE_FONT_WEIGHT_LIGHT)
            faceWeight = Gfx::Font::Weight::Light;

        Gfx::Font::Slant faceSlant = Gfx::Font::Slant::Normal;
        if(style == DWRITE_FONT_STYLE_ITALIC)
            faceSlant = Gfx::Font::Slant::Italic;
        else if(style == DWRITE_FONT_STYLE_OBLIQUE)
            faceSlant = Gfx::Font::Slant::Oblique;

        faces.push_back( Gfx::FontFace(family, faceWeight, faceSlant) );

        dwFont->Release();
    }

    fontFamily->Release();
    collection->Release();
    return faces;
}


void DWriteFontProvider::onAddFont(const System::Path& /*path*/)
{
}


void DWriteFontProvider::onRemoveFont(const System::Path& /*path*/)
{
}

} // namespace

} // namespace
