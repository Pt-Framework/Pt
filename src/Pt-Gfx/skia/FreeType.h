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

#ifndef PT_GFX_FREETYPE_H
#define PT_GFX_FREETYPE_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H

#include <Pt/Types.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontFace.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/System/Path.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

namespace Pt {

namespace Gfx {

class ColorF;
class FontMetrics;
class TextMetrics;
class CompositionMode;
class Transform;

class FreeType
{
  public:

    public:
        struct Init
        {
            Init()
            { FreeType::instance(); }
        };

                static FreeType& instance();

        ~FreeType();

        const std::string& defaultFont() const;

        FTC_FaceID defaultFace() const;

        void setDefaultFont(const std::string& font);

        std::vector<std::string> fontFamilies() const;

        std::vector<FontFace> fontFaces(const std::string& family) const;

        void addFonts(const System::Path& path);

        TextMetrics textMetrics(const String& text,
                                FTC_FaceID faceId, 
                                std::size_t fontSize);

        FontMetrics fontMetrics(FTC_FaceID faceId,
                                std::size_t fontSize);

        static FT_Error fontRequest(FTC_FaceID face_id, FT_Library library,
                                    FT_Pointer request_data, FT_Face* face);

        FTC_FaceID findFaceId(const Font& font);

        // TODO: use ImageView instead of clip

        void draw(Image& image, Pt::ssize_t x, Pt::ssize_t y, 
                  const String& text, const ColorF& color, const RectI& clip,
                  const CompositionMode& mode, const Transform& tf, 
                  FTC_FaceID faceId, std::size_t fontSize);

    private:
        FreeType();

        FreeType(const FreeType&) = delete;

        FreeType& operator=(const FreeType&) = delete;

        FT_Error onFontRequest(FTC_FaceID face_id, FT_Face* face);

        // TODO: use ImageView instead of clip

        void drawGlyph(Image& image, int xpos, int ypos, const ColorF& color, 
                       int bmPitch, int height, int width, const unsigned char* buffer, 
                       const RectI& clip, const CompositionMode& mode);

    private:
        typedef std::set<System::Path*> Files;

        struct CompareFont
        {
            inline bool operator()(const Font& a, const Font& b) const
            {
                if(a.family() != b.family())
                    return a.family() < b.family();

                if(a.weight() != b.weight())
                    return a.weight() < b.weight();

                if(a.slant() != b.slant())
                    return a.slant() < b.slant();

                if(a.stretch() != b.stretch())
                    return a.stretch() < b.stretch();

                if(a.category() != b.category())
                    return a.category() < b.category();

                if(a.styleName() != b.styleName())
                    return a.styleName() < b.styleName();

                return false;
            }
        };

        typedef std::map<Font, System::Path, CompareFont> Fonts;

        FT_Library     _ft;
        FTC_Manager    _manager;
        FTC_ImageCache _imageCache;
        FTC_CMapCache  _charMapCache;
        FTC_SBitCache  _bitmapCache;
        System::Path   _fontDir;
        std::string    _defaultFont;
        FTC_FaceID     _defaultFace;
        Fonts          _fonts;
        Files          _files;
};

static FreeType::Init initFreeType;

} // namespace

} // namespace

#endif
