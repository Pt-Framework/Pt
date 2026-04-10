/* Copyright (C) 2015 Marc Boris Duerner

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

#ifndef PT_GFX_FREETYPEFONTPROVIDER_H
#define PT_GFX_FREETYPEFONTPROVIDER_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H

#include <Pt/Types.h>
#include <Pt/Gfx/FontFace.h>
#include <Pt/Gfx/FontProvider.h>
#include <Pt/Gfx/Font.h>
#include <Pt/System/Path.h>

#include <list>
#include <set>
#include <string>
#include <vector>

namespace Pt {

namespace Gfx {

class FreeTypeRenderer;

class FreeTypeFontProvider : public FontProvider
{
    friend class FreeTypeRenderer;

    private:
        struct FaceEntry
        {
          FaceEntry(const FontFace& face,
                    const System::Path& source,
                    long faceIndex)
          : face(face)
          , source(source)
          , faceIndex(faceIndex)
          { }

          FontFace face;
          System::Path source;
          long faceIndex;
        };

    public:
        struct Init
        {
            Init()
            { FreeTypeFontProvider::instance(); }
        };

        static FreeTypeFontProvider& instance();

        ~FreeTypeFontProvider();

        const std::string& defaultFont() const;

        void setDefaultFont(const std::string& font);

        std::vector<std::string> fontFamilies() const;

        std::vector<FontFace> fontFaces(const std::string& family) const;

    public:
        FTC_FaceID findFaceId(const Font& font) const;

    protected:
        virtual void onAddFont(const System::Path& path) override;

        virtual void onRemoveFont(const System::Path& path) override;

    private:
        FreeTypeFontProvider();

        FreeTypeFontProvider(const FreeTypeFontProvider&) = delete;

        FreeTypeFontProvider& operator=(const FreeTypeFontProvider&) = delete;

        FTC_FaceID defaultFace() const;

        static FT_Error fontRequest(FTC_FaceID face_id, FT_Library library,
                                    FT_Pointer request_data, FT_Face* face);

        FT_Error onFontRequest(FTC_FaceID face_id, FT_Face* face);

        const FaceEntry* findFaceEntry(const std::string& family,
                                       const std::string& styleName,
                                       FontBase::Weight weight,
                                       FontBase::Slant slant) const;

        static FontFace::Weight fontWeightFromStyleFlags(FT_Long styleFlags);

        static FontFace::Slant fontSlantFromStyleFlags(FT_Long styleFlags);

        static int fontMatchScore(FontBase::Weight weight,
                                  FontBase::Slant slant,
                                  const FontFace& face);

        bool openFontFile(const System::Path& path);

        FT_Library     _ft;
        FTC_Manager    _manager;
        FTC_ImageCache _imageCache;
        FTC_CMapCache  _charMapCache;
        FTC_SBitCache  _bitmapCache;
        std::string    _defaultFont;
        mutable FTC_FaceID _defaultFace;
        std::list<FaceEntry> _faces;
        std::set<const FaceEntry*> _faceEntries;
};

static FreeTypeFontProvider::Init initFreeTypeFontProvider;

} // namespace

} // namespace

#endif
