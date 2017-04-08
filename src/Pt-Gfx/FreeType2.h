/* Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2017-2017 Aloysius Indrayanto

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

#ifndef PT_GFX_FREETYPE2_H
#define PT_GFX_FREETYPE2_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include <Pt/Types.h>
#include <Pt/Singleton.h>

#include <Pt/System/Mutex.h>
#include <Pt/System/Path.h>

#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Rect.h>


namespace Pt {
namespace Gfx {


class Color;
class Image;
class CompositionMode;


class FreeType2 {
    public:
        ~FreeType2();

        const FontMetrics fontMetrics(const String& text, FTC_FaceID faceId, FTC_ImageType imageType);

        void getCharSpacing(
            Pt::int32_t& x, Pt::int32_t& y,
            const Char& from, const Char& to, FTC_FaceID faceId, FTC_ImageType imageType
        );

        void pathFromChar(
            std::vector<PointF>& points, std::vector<Pt::uint8_t>& tags, std::vector<Pt::int32_t>& contours,
            const Char& chr, FTC_FaceID faceId
        );

        void draw(
            Image& image, const Rect& clip, const Point& pos, const Color& color, Pt::ssize_t fontAngle, const CompositionMode& mode,
            const String& text, FT_Matrix& matrix, FTC_FaceID faceId, FTC_ImageType imageType, bool mono
        );

        static FreeType2* getInstance(FTC_FaceID faceID);

        static void releaseInstance(FTC_FaceID faceID);

    private:
        FreeType2();

        void drawGlyph(
            Image& image, const Rect& clip, int xpos, int ypos, const Color& color, const CompositionMode& mode,
            int pitch, int width, int height, const unsigned char* buffer, bool mono
        );

        FT_Error onFontRequest(FTC_FaceID face_id, FT_Face* face);

        Pt::int32_t    _refCount;

        FTC_Manager    _manager;
        FTC_CMapCache  _charMapCache;
        FTC_SBitCache  _bitmapCache;
        FTC_ImageCache _imageCache;

    private:
        typedef std::set<System::Path*           > Files;
        typedef std::map<Font,       System::Path> Fonts;
        typedef std::map<FTC_FaceID, FreeType2*  > Instances;

        static System::Mutex  _mutex;

        static FT_Library     _ft;
        static Instances      _instances;

        static System::Path   _fontDir;
        static Fonts          _fonts;
        static Files          _files;

        static std::string    _defaultFont;

    public:
        static FT_Error fontRequest(FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face* face);

        static void setFontDir(const System::Path& path);

        static const std::vector<std::string> fontNames();

        static void setDefaultFont(const std::string& font);

        static const std::string defaultFont();

        static FTC_FaceID findFaceId(const Font& font);
};


} // namespace
} // namespace

#endif
