/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2019 Laurentiu-Gheorghe Crisan

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

#include "FreeTypeFontProvider.h"
#include "DejaVuSans.h"
#include "DejaVuSansBold.h"
#include "DejaVuSansItalic.h"
#include "DejaVuSansBoldItalic.h"

#include FT_TRUETYPE_TABLES_H

#include <Pt/Gfx/FontRegistry.h>

#include <Pt/System/Directory.h>
#include <Pt/System/FileInfo.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Logger.h>

#include <algorithm>
#include <stdexcept>
#include <iostream>

PT_LOG_DEFINE("Pt.Gfx.FreeType");

static const unsigned DefaultFontSize = 12;

// some versions of freetype do not work with NULL
static const FTC_FaceID DefaultFaceId = reinterpret_cast<FTC_FaceID>(1);

namespace Pt {

namespace Gfx {

FontFace::Weight FreeTypeFontProvider::fontWeightFromStyleFlags(FT_Long styleFlags)
{
    return (styleFlags & FT_STYLE_FLAG_BOLD) != 0
         ? FontFace::Weight::Bold
         : FontFace::Weight::Normal;
}


FontFace::Slant FreeTypeFontProvider::fontSlantFromStyleFlags(FT_Long styleFlags)
{
    return (styleFlags & FT_STYLE_FLAG_ITALIC) != 0
         ? FontFace::Slant::Italic
         : FontFace::Slant::Normal;
}


FontFace::Stretch FreeTypeFontProvider::fontStretchFromOS2Width(FT_UShort widthClass)
{
    if(widthClass >= 9)
        return FontFace::Stretch::UltraExpanded;

    if(widthClass >= 1 && widthClass <= 9)
        return static_cast<FontFace::Stretch>(widthClass);

    return FontFace::Stretch::Normal;
}


int FreeTypeFontProvider::fontMatchScore(Font::Weight weight,
                                         Font::Slant slant,
                                         Font::Stretch stretch,
                                         const FontFace& face)
{
    int score = static_cast<int>(face.weight()) - static_cast<int>(weight);
    if(score < 0)
        score = -score;

    int stretchDiff = static_cast<int>(face.stretch()) - static_cast<int>(stretch);
    if(stretchDiff < 0)
        stretchDiff = -stretchDiff;
    score += stretchDiff * 100;

    if(face.slant() != slant)
        score += 1000;

    return score;
}

FreeTypeFontProvider& FreeTypeFontProvider::instance()
{
    static FreeTypeFontProvider provider;
    return provider;
}


FreeTypeFontProvider::FreeTypeFontProvider()
: _defaultFace(DefaultFaceId)
{
    if( FT_Init_FreeType( &_ft ) )
        throw std::runtime_error("FT_Init_FreeType");

    if( FTC_Manager_New( _ft, 0, 0, 0, &FreeTypeFontProvider::fontRequest, this, &_manager ) )
        throw std::runtime_error( "FTC_Manager_New" );

    if( FTC_ImageCache_New( _manager, &_imageCache ) )
        throw std::runtime_error( "FTC_ImageCache_New" );

    if( FTC_CMapCache_New( _manager, &_charMapCache ) )
        throw std::runtime_error( "FTC_CMapCache_New" );

    if( FTC_SBitCache_New( _manager, &_bitmapCache ) )
        throw std::runtime_error( "FTC_SBitCache_New" );

    const std::vector<System::Path>& fontFiles = FontRegistry::instance().fontFiles();
    for(std::vector<System::Path>::const_iterator it = fontFiles.begin(); it != fontFiles.end(); ++it)
        addFont(*it);

    System::Path  path = System::Path( System::Path::curdir()) / "fonts";
    if(fontFiles.empty())
        FontRegistry::instance().addFonts(path);
}


FreeTypeFontProvider::~FreeTypeFontProvider()
{
    FTC_Manager_Done( _manager );
    FT_Done_FreeType( _ft );
}


const std::string& FreeTypeFontProvider::defaultFont() const
{
    // LOCK

    return _defaultFont;

    // UNLOCK
}


FTC_FaceID FreeTypeFontProvider::defaultFace() const
{
    // LOCK

    return _defaultFace;

    // UNLOCK
}


void FreeTypeFontProvider::setDefaultFont(const std::string& font)
{
    // LOCK

    _defaultFont = font;
    _defaultFace = findFaceId(Font(_defaultFont, DefaultFontSize));

    // UNLOCK
}


std::vector<std::string> FreeTypeFontProvider::fontFamilies() const
{
    std::vector<std::string> families;
    families.reserve(_faces.size() + 1);

    for(std::list<FaceEntry>::const_iterator it = _faces.begin(); it != _faces.end(); ++it)
        families.push_back(it->face.family());

    families.push_back("DejaVu Sans");

    std::sort(families.begin(), families.end());
    families.erase(std::unique(families.begin(), families.end()), families.end());

    return families;
}


std::vector<FontFace> FreeTypeFontProvider::fontFaces(const std::string& family) const
{
    std::vector<FontFace> faces;
    if(family.empty())
        return faces;

    for(std::list<FaceEntry>::const_iterator it = _faces.begin(); it != _faces.end(); ++it)
    {
        if(it->face.family() == family)
            faces.push_back(it->face);
    }

    if(family == "DejaVu Sans")
        faces.push_back(FontFace("DejaVu Sans"));

    std::sort(faces.begin(), faces.end());
    faces.erase(std::unique(faces.begin(), faces.end()), faces.end());
    return faces;
}


FTC_FaceID FreeTypeFontProvider::findFaceId(const Font& font) const
{
    std::string family = font.family();

    if(family.empty() && font.category() != Font::Category::None)
        family = categoryDefaultFamily(font.category());

    if(family.empty())
        family = _defaultFont;

    const FaceEntry* entry = findFaceEntry(family, font.styleName(), font.weight(), font.slant(), font.stretch());
    if(entry)
        return reinterpret_cast<FTC_FaceID>(const_cast<FaceEntry*>(entry));

    return DefaultFaceId;
}


FT_Error FreeTypeFontProvider::fontRequest( FTC_FaceID faceId, FT_Library library,
                                            FT_Pointer data, FT_Face* face )
{
    FreeTypeFontProvider* provider = static_cast<FreeTypeFontProvider*>(data);
    return provider->onFontRequest(faceId, face);
}


FT_Error FreeTypeFontProvider::onFontRequest(FTC_FaceID faceId, FT_Face* face)
{
    if(faceId == DefaultFaceId)
    {
        return FT_New_Memory_Face(_ft, DejaVuSans, DejaVuSansSize, 0, face);
    }

    const FaceEntry* entry = reinterpret_cast<const FaceEntry*>(faceId);

    if( _faceEntries.find(entry) == _faceEntries.end() )
        return FT_New_Memory_Face(_ft, DejaVuSans, DejaVuSansSize, 0, face);

    // named instance encoded in upper bits of face index
    long faceIndex = entry->faceIndex;
    if(entry->namedInstanceIndex > 0)
        faceIndex |= (entry->namedInstanceIndex << 16);

    FT_Error err = FT_New_Face(_ft, entry->source.toLocal().c_str(), faceIndex, face);

    // synthetic instance with explicit axis coordinates
    if(err == 0 && ! entry->syntheticCoords.empty())
    {
        FT_Set_Var_Design_Coordinates(*face,
            static_cast<FT_UInt>(entry->syntheticCoords.size()),
            const_cast<FT_Fixed*>(entry->syntheticCoords.data()));
    }

    return err;
}


const FreeTypeFontProvider::FaceEntry* FreeTypeFontProvider::findFaceEntry(const std::string& family,
                                                                           const std::string& styleName,
                                                                           Font::Weight weight,
                                                                           Font::Slant slant,
                                                                           Font::Stretch stretch) const
{
    if(family.empty())
        return 0;

    const FaceEntry* best = 0;
    int bestScore = 0;

    for(std::list<FaceEntry>::const_iterator it = _faces.begin(); it != _faces.end(); ++it)
    {
        if(it->face.family() != family)
            continue;

        if( ! styleName.empty() && it->face.styleName() != styleName)
            continue;

        const int score = fontMatchScore(weight, slant, stretch, it->face);
        if(!best || score < bestScore)
        {
            best = &*it;
            bestScore = score;
        }
    }

    return best;
}


std::string FreeTypeFontProvider::categoryDefaultFamily(Font::Category category) const
{
    switch(category)
    {
        case Font::Category::Serif:     return "DejaVu Serif";
        case Font::Category::SansSerif: return "DejaVu Sans";
        case Font::Category::Monospace: return "DejaVu Sans Mono";
        case Font::Category::Cursive:   return "DejaVu Sans";
        case Font::Category::Fantasy:   return "DejaVu Sans";
        default:                            return std::string();
    }
}


void FreeTypeFontProvider::onAddFont(const System::Path& path)
{
    openFontFile(path);

    if(_defaultFont.empty())
        _defaultFace = DefaultFaceId;
    else
        _defaultFace = findFaceId(Font(_defaultFont, DefaultFontSize));
}


void FreeTypeFontProvider::onRemoveFont(const System::Path& path)
{
    bool removed = false;

    for(std::list<FaceEntry>::iterator it = _faces.begin(); it != _faces.end(); )
    {
        if(it->source != path)
        {
            ++it;
            continue;
        }

        _faceEntries.erase(&(*it));
        it = _faces.erase(it);
        removed = true;
    }

    if(removed)
        FTC_Manager_Reset(_manager);

    if(_defaultFont.empty())
        _defaultFace = DefaultFaceId;
    else
        _defaultFace = findFaceId(Font(_defaultFont, DefaultFontSize));
}


FontFace::Weight FreeTypeFontProvider::fontWeightFromAxis(FT_Fixed value)
{
    int wght = static_cast<int>(value >> 16);

    if(wght <= 150)  return FontFace::Weight::Thin;
    if(wght <= 250)  return FontFace::Weight::ExtraLight;
    if(wght <= 350)  return FontFace::Weight::Light;
    if(wght <= 450)  return FontFace::Weight::Normal;
    if(wght <= 550)  return FontFace::Weight::Medium;
    if(wght <= 650)  return FontFace::Weight::SemiBold;
    if(wght <= 750)  return FontFace::Weight::Bold;
    if(wght <= 850)  return FontFace::Weight::ExtraBold;

    return FontFace::Weight::Black;
}


FontFace::Slant FreeTypeFontProvider::fontSlantFromAxes(FT_MM_Var* mmVar,
                                                        const FT_Fixed* coords)
{
    for(FT_UInt a = 0; a < mmVar->num_axis; ++a)
    {
        if(mmVar->axis[a].tag == FT_MAKE_TAG('i','t','a','l'))
        {
            if((coords[a] >> 16) > 0)
                return FontFace::Slant::Italic;
        }
        else if(mmVar->axis[a].tag == FT_MAKE_TAG('s','l','n','t'))
        {
            if((coords[a] >> 16) != 0)
                return FontFace::Slant::Oblique;
        }
    }

    return FontFace::Slant::Normal;
}


FontFace::Stretch FreeTypeFontProvider::fontStretchFromAxis(FT_Fixed value)
{
    int wdth = static_cast<int>(value >> 16);

    if(wdth <= 50)  return FontFace::Stretch::UltraCondensed;
    if(wdth <= 62)  return FontFace::Stretch::ExtraCondensed;
    if(wdth <= 75)  return FontFace::Stretch::Condensed;
    if(wdth <= 87)  return FontFace::Stretch::SemiCondensed;
    if(wdth <= 100) return FontFace::Stretch::Normal;
    if(wdth <= 112) return FontFace::Stretch::SemiExpanded;
    if(wdth <= 125) return FontFace::Stretch::Expanded;
    if(wdth <= 150) return FontFace::Stretch::ExtraExpanded;

    return FontFace::Stretch::UltraExpanded;
}


bool FreeTypeFontProvider::getNamedInstances(FT_Face face, const System::Path& path,
                                             long faceIndex, const std::string& family)
{
    FT_MM_Var* mmVar = nullptr;
    if(FT_Get_MM_Var(face, &mmVar) != 0)
        return false;

    bool added = false;

    for(FT_UInt ni = 1; ni <= mmVar->num_namedstyles; ++ni)
    {
        long instanceFaceIndex = faceIndex | (static_cast<long>(ni) << 16);

        FT_Face instFace = nullptr;
        FT_Error err = FT_New_Face(_ft, path.toLocal().c_str(), instanceFaceIndex, &instFace);
        if(err != 0)
            continue;

        const FT_Fixed* coords = mmVar->namedstyle[ni - 1].coords;

        FontFace::Weight weight = FontFace::Weight::Normal;
        FontFace::Stretch stretch = FontFace::Stretch::Normal;

        for(FT_UInt a = 0; a < mmVar->num_axis; ++a)
        {
            if(mmVar->axis[a].tag == FT_MAKE_TAG('w','g','h','t'))
                weight = fontWeightFromAxis(coords[a]);
            else if(mmVar->axis[a].tag == FT_MAKE_TAG('w','d','t','h'))
                stretch = fontStretchFromAxis(coords[a]);
        }

        FontFace::Slant slant = fontSlantFromAxes(mmVar, coords);

        std::string instFamily = instFace->family_name ? instFace->family_name : family;
        std::string instStyle = instFace->style_name ? instFace->style_name : std::string();

        FT_Done_Face(instFace);

        if(instFamily.empty())
            continue;

        _faces.push_back(FaceEntry(FontFace(instFamily, weight, slant, stretch, instStyle),
                                   path, faceIndex, ni));
        _faceEntries.insert(&(_faces.back()));
        added = true;

        PT_LOG_INFO("loaded variable instance: " << instFamily << ' ' << instStyle);
    }

    // generate synthetic instances if no named instances cover standard weights
    if( ! added)
    {
        FontFace::Slant defaultSlant = fontSlantFromAxes(mmVar, mmVar->namedstyle[0].coords);
        added = getSyntheticInstances(mmVar, path, faceIndex, family, defaultSlant);
    }

    FT_Done_MM_Var(_ft, mmVar);
    return added;
}


bool FreeTypeFontProvider::getSyntheticInstances(FT_MM_Var* mmVar, const System::Path& path,
                                                 long faceIndex, const std::string& family,
                                                 FontFace::Slant defaultSlant)
{
    FT_UInt wghtAxis = mmVar->num_axis; // invalid sentinel
    FT_Fixed wghtMin = 0;
    FT_Fixed wghtMax = 0;

    for(FT_UInt a = 0; a < mmVar->num_axis; ++a)
    {
        if(mmVar->axis[a].tag == FT_MAKE_TAG('w','g','h','t'))
        {
            wghtAxis = a;
            wghtMin = mmVar->axis[a].minimum;
            wghtMax = mmVar->axis[a].maximum;
            break;
        }
    }

    if(wghtAxis >= mmVar->num_axis)
        return false;

    static const int standardWeights[] = {
        static_cast<int>(Font::Weight::Thin),
        static_cast<int>(Font::Weight::ExtraLight),
        static_cast<int>(Font::Weight::Light),
        static_cast<int>(Font::Weight::Normal),
        static_cast<int>(Font::Weight::Medium),
        static_cast<int>(Font::Weight::SemiBold),
        static_cast<int>(Font::Weight::Bold),
        static_cast<int>(Font::Weight::ExtraBold),
        static_cast<int>(Font::Weight::Black)
    };

    // build default coordinates from axis defaults
    std::vector<FT_Fixed> defaultCoords(mmVar->num_axis);
    for(FT_UInt a = 0; a < mmVar->num_axis; ++a)
        defaultCoords[a] = mmVar->axis[a].def;

    bool added = false;

    for(int w : standardWeights)
    {
        FT_Fixed wghtValue = static_cast<FT_Fixed>(w) << 16;
        if(wghtValue < wghtMin || wghtValue > wghtMax)
            continue;

        std::vector<FT_Fixed> coords = defaultCoords;
        coords[wghtAxis] = wghtValue;

        FontFace::Weight weight = fontWeightFromAxis(wghtValue);

        _faces.push_back(FaceEntry(FontFace(family, weight, defaultSlant, FontFace::Stretch::Normal),
                                   path, faceIndex, coords));
        _faceEntries.insert(&(_faces.back()));
        added = true;

        PT_LOG_INFO("loaded synthetic instance: " << family << " weight=" << w);
    }

    return added;
}


bool FreeTypeFontProvider::openFontFile(const System::Path& path)
{
    const std::string localPath = path.toLocal();

    FT_Face probe = 0;
    FT_Error err = FT_New_Face(_ft, localPath.c_str(), 0, &probe);
    if(err != 0)
    {
        PT_LOG_WARN("font error: " << localPath << ' ' << err);
        return false;
    }

    long faceCount = probe->num_faces;
    FT_Done_Face(probe);

    bool added = false;
    for(long faceIndex = 0; faceIndex < faceCount; ++faceIndex)
    {
        FT_Face face = 0;
        err = FT_New_Face(_ft, localPath.c_str(), faceIndex, &face);
        if(err != 0)
        {
            PT_LOG_WARN("font error: " << localPath << ' ' << err);
            continue;
        }

        if((face->face_flags & FT_FACE_FLAG_SCALABLE) == 0)
        {
            FT_Done_Face(face);
            continue;
        }

        std::string family = face->family_name ? face->family_name : std::string();

        // variable font with multiple masters
        if((face->face_flags & FT_FACE_FLAG_MULTIPLE_MASTERS) != 0 && ! family.empty())
        {
            if(getNamedInstances(face, path, faceIndex, family))
                added = true;

            FT_Done_Face(face);
            continue;
        }

        // static font
        std::string style = face->style_name ? face->style_name : std::string();
        FontFace::Weight weight = fontWeightFromStyleFlags(face->style_flags);
        FontFace::Slant slant = fontSlantFromStyleFlags(face->style_flags);

        FontFace::Stretch stretch = FontFace::Stretch::Normal;
        TT_OS2* os2 = static_cast<TT_OS2*>(FT_Get_Sfnt_Table(face, FT_SFNT_OS2));
        if(os2)
            stretch = fontStretchFromOS2Width(os2->usWidthClass);

        if( ! family.empty() )
        {
            _faces.push_back(FaceEntry(FontFace(family, weight, slant, stretch, style), path, faceIndex));
            _faceEntries.insert(&(_faces.back()));
            added = true;

            PT_LOG_INFO("loaded font: " << family << ' ' << style);
        }

        FT_Done_Face(face);
    }

    return added;
}

} // namespace Gfx

} // namespace Pt
