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
#include <Pt/Gfx/Rect.h>
#include <Pt/System/Path.h>
#include <Pt/Singleton.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

namespace Pt {

namespace Gfx {

class ColorF;
class Image;
class TextMetrics;
class CompositionMode;
class Transform;

class FreeType : public Pt::Singleton<FreeType>
{
    friend class Pt::Singleton<FreeType>;

  public:
    typedef BasicPoint<Pt::ssize_t> Point;
    typedef BasicSize<Pt::ssize_t> Size;
    typedef BasicRect<Pt::ssize_t> Rect;

    public:
        struct Init
        {
            Init()
            { FreeType::instance(); }
        };

        ~FreeType();

        const std::string& defaultFont() const;

        FTC_FaceID defaultFace() const;

        void setDefaultFont(const std::string& font);

        std::vector<std::string> fontNames() const;

        void setFontDir(const System::Path& path);

        TextMetrics textMetrics(const String& text,
                                FTC_FaceID faceId, 
                                std::size_t fontSize);

        static FT_Error fontRequest(FTC_FaceID face_id, FT_Library library,
                                    FT_Pointer request_data, FT_Face* face);

        FTC_FaceID findFaceId(const Font& font);

        // TODO: use ImageView instead of clip

        void draw(Image& image, Pt::ssize_t x, Pt::ssize_t y, 
                  const String& text, const ColorF& color, const Rect& clip,
                  const CompositionMode& mode, const Transform& tf, 
                  FTC_FaceID faceId, std::size_t fontSize);

    protected:
        FreeType();

        FT_Error onFontRequest(FTC_FaceID face_id, FT_Face* face);

        // TODO: use ImageView instead of clip

        void drawGlyph(Image& image, int xpos, int ypos, const ColorF& color, 
                       int bmPitch, int height, int width, const unsigned char* buffer, 
                       const Rect& clip, const CompositionMode& mode);

    private:
        typedef std::set<System::Path*> Files;

        struct CompareFont
        {
            inline bool operator()(const Font& a, const Font& b) const
            {
                return a.name() == b.name() ? a.style() < b.style()
                                            : a.name() < b.name();
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
