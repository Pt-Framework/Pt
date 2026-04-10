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

#include <Pt/Gfx/FontRegistry.h>

#include <Pt/System/Directory.h>
#include <Pt/System/FileInfo.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Logger.h>

#include <cctype>
#include <stdexcept>
#include <iostream>

namespace {

std::string normalizeFontToken(const std::string& text)
{
    std::string normalized;
    normalized.reserve(text.size());

    for(std::string::const_iterator it = text.begin(); it != text.end(); ++it)
    {
        unsigned char ch = static_cast<unsigned char>(*it);
        if(std::isalnum(ch))
            normalized += static_cast<char>(std::tolower(ch));
    }

    return normalized;
}


bool isRegularStyle(const std::string& style)
{
    return style.empty() ||
           style == "regular" ||
           style == "book" ||
           style == "roman";
}

} // namespace

PT_LOG_DEFINE("Pt.Gfx.FreeType");

static const unsigned DefaultFontSize = 12;

// some versions of freetype do not work with NULL
static const FTC_FaceID DefaultFaceId = reinterpret_cast<FTC_FaceID>(1);

namespace Pt {

namespace Gfx {

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


std::vector<FontFace> FreeTypeFontProvider::fonts() const
{
    std::vector<FontFace> faces;
    faces.reserve(_faces.size() + 1);

    for(std::list<FaceEntry>::const_iterator it = _faces.begin(); it != _faces.end(); ++it)
        faces.push_back(it->face);

    faces.push_back(FontFace("DejaVu Sans"));
    return faces;
}


FTC_FaceID FreeTypeFontProvider::findFaceId(const Font& font) const
{
    const std::string fontName = font.name().empty() ? _defaultFont : font.name();
    Font requested(fontName, font.size(), font.style());
    const FaceEntry* entry = findFaceEntry(requested);
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

    return FT_New_Face(_ft, entry->source.toLocal().c_str(), entry->faceIndex, face);
}


const FreeTypeFontProvider::FaceEntry* FreeTypeFontProvider::findFaceEntry(const Font& font) const
{
    if(font.name().empty())
        return 0;

    const std::string fontName = normalizeFontToken(font.name());
    const std::string fontStyle = normalizeFontToken(font.style());

    const FaceEntry* fallback = 0;

    for(std::list<FaceEntry>::const_iterator it = _faces.begin(); it != _faces.end(); ++it)
    {
        if(normalizeFontToken(it->face.name()) != fontName)
            continue;

        const std::string faceStyle = normalizeFontToken(it->face.style());
        if(fontStyle.empty())
        {
            if(isRegularStyle(faceStyle))
                return &*it;

            if(!fallback)
                fallback = &*it;
        }
        else if(faceStyle == fontStyle)
        {
            return &*it;
        }
    }

    return fontStyle.empty() ? fallback : 0;
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
        std::string style = face->style_name ? face->style_name : std::string();

        if( ! family.empty() )
        {
            _faces.push_back(FaceEntry(FontFace(family, style), path, faceIndex));
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
