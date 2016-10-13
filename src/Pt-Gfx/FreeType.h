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
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/System/Path.h>
#include <Pt/Singleton.h>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

namespace Pt {

namespace Gfx {

class FreeType : public Pt::Singleton<FreeType>
{
    friend class Pt::Singleton<FreeType>;

    public:
        struct Init
        {
            Init()
            { FreeType::instance(); }
        };

        ~FreeType();

        std::string defaultFont() const;

        void setDefaultFont(const std::string& font);

        std::vector<std::string> fontNames() const;

        void setFontDir(const System::Path& path);

        FontMetrics fontMetrics(const String& text, 
                                FTC_FaceID faceId, FTC_ImageType imageType);

        FT_Library library() const
        { return _ft; }

        static FT_Error fontRequest(FTC_FaceID face_id, FT_Library library, 
                                    FT_Pointer request_data, FT_Face* face);

        FTC_FaceID findFaceId(const Font& font);

        FT_Error findFace(FTC_FaceID faceId, FT_Face* face);

        FT_UInt findCharMap(FTC_FaceID faceId, FT_Int charMapId, FT_UInt32 value);

        FT_Error findBitmap(FTC_ImageType type,
                            FT_UInt       gindex,
                            FTC_SBit*     sbit,
                            FTC_Node*     node);
        
        FT_Error findImage(FTC_ImageType type,
                           FT_UInt       gindex,
                           FT_Glyph*     glyph,
                           FTC_Node*     node);

        FT_Error findSize(FTC_Scaler scaler, FT_Size* size);

    protected:
        FreeType();

        FT_Error onFontRequest(FTC_FaceID face_id, FT_Face* face);

    private:
        typedef std::map<Pt::uint64_t, System::Path> FontMap;
        typedef std::map<Font, Pt::uint64_t> FaceMap;

        FT_Library     _ft;
        FTC_Manager    _manager;
        FTC_ImageCache _imageCache;
        FTC_CMapCache  _charMapCache;
        FTC_SBitCache  _bitmapCache;
        System::Path   _fontDir;
        std::string    _defaultFont;
        FontMap        _fontMap;
        FaceMap        _faces;

    private:
        static Pt::uint64_t _id;
};

static FreeType::Init initFreeType;

} // namespace

} // namespace

#endif
