/* Copyright (C) 2026 Marc Boris Duerner

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

#include "NanoVGFontProvider.h"

#include "DejaVuSans.h"
#include "DejaVuSansBold.h"
#include "DejaVuSansItalic.h"
#include "DejaVuSansBoldItalic.h"

#include <Pt/Gfx/FontRegistry.h>
#include <Pt/System/Logger.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H

#include "nanovg.h"

#include <cstdlib>

PT_LOG_DEFINE("Pt.Forms.NanoVG.Font");

namespace Pt {

namespace Forms {

namespace {

// Maps a Gfx::Font::Slant to the matcher slant bucket. Italic and oblique are
// treated as the same slanted bucket so an oblique request still matches an
// italic face.
int slantBucket(Gfx::Font::Slant slant)
{
    return slant == Gfx::Font::Slant::Normal ? 0 : 1;
}


int weightFromStyleFlags(FT_Long styleFlags)
{
    return (styleFlags & FT_STYLE_FLAG_BOLD) != 0 ? 700 : 400;
}


int slantFromStyleFlags(FT_Long styleFlags)
{
    return (styleFlags & FT_STYLE_FLAG_ITALIC) != 0 ? 1 : 0;
}


int stretchFromOS2Width(FT_UShort widthClass)
{
    if(widthClass >= 1 && widthClass <= 9)
        return static_cast<int>(widthClass);

    return 5;
}


float faceScale(FT_Face face)
{
    if(face->ascender != 0)
        return static_cast<float>(face->height) / static_cast<float>(face->ascender);

    return 1.0f;
}


float memFaceScale(const unsigned char* data, int size)
{
    FT_Library ft = 0;
    if(FT_Init_FreeType(&ft) != 0)
        return 1.0f;

    FT_Face face = 0;
    float scale = 1.0f;
    if(FT_New_Memory_Face(ft, data, size, 0, &face) == 0)
    {
        scale = faceScale(face);
        FT_Done_Face(face);
    }

    FT_Done_FreeType(ft);
    return scale;
}

} // anonymous namespace


NanoVGFontProvider::NanoVGFontProvider(NVGcontext* nvg)
: _nvg(nvg)
, _defaultRegular(-1)
, _defaultBold(-1)
, _defaultItalic(-1)
, _defaultBoldItalic(-1)
{
    registerEmbedded();

    const std::vector<System::Path>& files = Gfx::FontRegistry::instance().fontFiles();
    for(std::vector<System::Path>::const_iterator it = files.begin(); it != files.end(); ++it)
        loadFile(it->toLocal());
}


void NanoVGFontProvider::registerEmbedded()
{
    _defaultRegular = nvgCreateFontMem(_nvg, "Pt-DejaVuSans",
        const_cast<unsigned char*>(Gfx::DejaVuSans),
        static_cast<int>(Gfx::DejaVuSansSize), 0);

    _defaultBold = nvgCreateFontMem(_nvg, "Pt-DejaVuSans-Bold",
        const_cast<unsigned char*>(Gfx::DejaVuSansBold),
        static_cast<int>(Gfx::DejaVuSansBoldSize), 0);

    _defaultItalic = nvgCreateFontMem(_nvg, "Pt-DejaVuSans-Italic",
        const_cast<unsigned char*>(Gfx::DejaVuSansItalic),
        static_cast<int>(Gfx::DejaVuSansItalicSize), 0);

    _defaultBoldItalic = nvgCreateFontMem(_nvg, "Pt-DejaVuSans-BoldItalic",
        const_cast<unsigned char*>(Gfx::DejaVuSansBoldItalic),
        static_cast<int>(Gfx::DejaVuSansBoldItalicSize), 0);

    // Missing glyphs fall back to the regular embedded face.
    if(_defaultBold >= 0 && _defaultRegular >= 0)
        nvgAddFallbackFontId(_nvg, _defaultBold, _defaultRegular);
    if(_defaultItalic >= 0 && _defaultRegular >= 0)
        nvgAddFallbackFontId(_nvg, _defaultItalic, _defaultRegular);
    if(_defaultBoldItalic >= 0 && _defaultRegular >= 0)
        nvgAddFallbackFontId(_nvg, _defaultBoldItalic, _defaultRegular);

    const struct
    {
        int                  weight;
        int                  slant;
        int                  handle;
        const unsigned char* data;
        int                  size;
    }
    embedded[] = {
        { 400, 0, _defaultRegular,    Gfx::DejaVuSans,           static_cast<int>(Gfx::DejaVuSansSize) },
        { 700, 0, _defaultBold,       Gfx::DejaVuSansBold,       static_cast<int>(Gfx::DejaVuSansBoldSize) },
        { 400, 1, _defaultItalic,     Gfx::DejaVuSansItalic,     static_cast<int>(Gfx::DejaVuSansItalicSize) },
        { 700, 1, _defaultBoldItalic, Gfx::DejaVuSansBoldItalic, static_cast<int>(Gfx::DejaVuSansBoldItalicSize) }
    };

    for(std::size_t i = 0; i < sizeof(embedded) / sizeof(embedded[0]); ++i)
    {
        if(embedded[i].handle < 0)
            continue;

        FaceEntry entry;
        entry.family = "DejaVu Sans";
        entry.weight = embedded[i].weight;
        entry.slant = embedded[i].slant;
        entry.stretch = 5;
        entry.handle = embedded[i].handle;
        entry.scale = memFaceScale(embedded[i].data, embedded[i].size);
        _faces.push_back(entry);
    }
}


void NanoVGFontProvider::loadFile(const std::string& path)
{
    FT_Library ft = 0;
    if(FT_Init_FreeType(&ft) != 0)
        return;

    FT_Face face = 0;
    if(FT_New_Face(ft, path.c_str(), 0, &face) != 0)
    {
        FT_Done_FreeType(ft);
        return;
    }

    const long faceCount = face->num_faces;

    for(long faceIndex = 0; faceIndex < faceCount; ++faceIndex)
    {
        if(faceIndex > 0)
        {
            if(FT_New_Face(ft, path.c_str(), faceIndex, &face) != 0)
                continue;
        }

        if((face->face_flags & FT_FACE_FLAG_SCALABLE) == 0)
        {
            FT_Done_Face(face);
            continue;
        }

        const std::string family = face->family_name ? face->family_name : std::string();
        if(family.empty())
        {
            FT_Done_Face(face);
            continue;
        }

        FaceEntry entry;
        entry.family = family;
        entry.styleName = face->style_name ? face->style_name : std::string();
        entry.weight = weightFromStyleFlags(face->style_flags);
        entry.slant = slantFromStyleFlags(face->style_flags);
        entry.stretch = 5;
        entry.scale = faceScale(face);

        TT_OS2* os2 = static_cast<TT_OS2*>(FT_Get_Sfnt_Table(face, FT_SFNT_OS2));
        if(os2)
            entry.stretch = stretchFromOS2Width(os2->usWidthClass);

        FT_Done_Face(face);

        // Register the face with nanovg under a unique name.
        std::string name = "Pt-";
        name += family;
        name += ':';
        name += entry.styleName;

        if(nvgFindFont(_nvg, name.c_str()) < 0)
            entry.handle = nvgCreateFontAtIndex(_nvg, name.c_str(), path.c_str(),
                                                static_cast<int>(faceIndex));
        else
            entry.handle = nvgFindFont(_nvg, name.c_str());

        if(entry.handle < 0)
            continue;

        if(_defaultRegular >= 0)
            nvgAddFallbackFontId(_nvg, entry.handle, _defaultRegular);

        _faces.push_back(entry);

        PT_LOG_INFO("registered font: " << family << ' ' << entry.styleName);
    }

    FT_Done_FreeType(ft);
}


int NanoVGFontProvider::matchScore(int weight, int slant, int stretch,
                                   const FaceEntry& face)
{
    int score = face.weight - weight;
    if(score < 0)
        score = -score;

    int stretchDiff = face.stretch - stretch;
    if(stretchDiff < 0)
        stretchDiff = -stretchDiff;
    score += stretchDiff * 100;

    if(face.slant != slant)
        score += 1000;

    return score;
}


const NanoVGFontProvider::FaceEntry*
NanoVGFontProvider::findBestMatch(const std::string& family,
                                  const std::string& styleName,
                                  int weight, int slant, int stretch) const
{
    const FaceEntry* best = 0;
    int bestScore = 0;

    for(std::vector<FaceEntry>::const_iterator it = _faces.begin(); it != _faces.end(); ++it)
    {
        if(it->family != family)
            continue;

        if( ! styleName.empty() && it->styleName != styleName)
            continue;

        const int score = matchScore(weight, slant, stretch, *it);
        if( ! best || score < bestScore)
        {
            best = &*it;
            bestScore = score;
        }
    }

    return best;
}


std::string NanoVGFontProvider::categoryDefaultFamily(Gfx::Font::Category category)
{
    switch(category)
    {
        case Gfx::Font::Category::Serif:     return "DejaVu Serif";
        case Gfx::Font::Category::SansSerif: return "DejaVu Sans";
        case Gfx::Font::Category::Monospace: return "DejaVu Sans Mono";
        case Gfx::Font::Category::Cursive:   return "DejaVu Sans";
        case Gfx::Font::Category::Fantasy:   return "DejaVu Sans";
        default:                             return std::string();
    }
}


float NanoVGFontProvider::sizeScale(int handle) const
{
    for(std::vector<FaceEntry>::const_iterator it = _faces.begin(); it != _faces.end(); ++it)
    {
        if(it->handle == handle)
            return it->scale;
    }

    return 1.0f;
}


int NanoVGFontProvider::fontFace(const Gfx::Font& font)
{
    std::string family = font.family();

    if(family.empty() && font.category() != Gfx::Font::Category::None)
        family = categoryDefaultFamily(font.category());

    if(family.empty())
        family = "DejaVu Sans";

    const int weight = static_cast<int>(font.weight());
    const int slant = slantBucket(font.slant());
    const int stretch = static_cast<int>(font.stretch());

    const FaceEntry* entry = findBestMatch(family, font.styleName(),
                                           weight, slant, stretch);
    if(entry)
        return entry->handle;

    // Fall back to the matching embedded default face.
    const bool bold = weight >= 600;
    const bool italic = slant != 0;

    if(bold && italic && _defaultBoldItalic >= 0)
        return _defaultBoldItalic;
    if(bold && _defaultBold >= 0)
        return _defaultBold;
    if(italic && _defaultItalic >= 0)
        return _defaultItalic;

    return _defaultRegular;
}

} // namespace

} // namespace