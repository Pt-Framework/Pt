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

#include "FreeType.h"
#include "DejaVuSans.h"
#include "DejaVuSansBold.h"
#include "DejaVuSansItalic.h"
#include "DejaVuSansBoldItalic.h"
#include <Pt/System/Directory.h>
#include <Pt/System/FileInfo.h>
#include <Pt/System/IOError.h>
#include <stdexcept>
#include <iostream>

namespace Pt {

namespace Gfx {

Pt::uint64_t FreeType::_id = 1;


FreeType::FreeType()
{   
    if( FT_Init_FreeType( &_ft ) )
        throw std::runtime_error("FT_Init_FreeType");

    if( FTC_Manager_New( _ft, 0, 0, 0, &FreeType::fontRequest, this, &_manager ) )
        throw std::runtime_error( "FTC_Manager_New" );

    if( FTC_ImageCache_New( _manager, &_imageCache ) )
        throw std::runtime_error( "FTC_ImageCache_New" );

    if( FTC_CMapCache_New( _manager, &_charMapCache ) )
        throw std::runtime_error( "FTC_CMapCache_New" );

    if( FTC_SBitCache_New( _manager, &_bitmapCache ) )
        throw std::runtime_error( "FTC_SBitCache_New" );

    setFontDir(System::Path( System::Path::curdir()) / "fonts");
}


FreeType::~FreeType()
{
    FTC_Manager_Done( _manager );
    FT_Done_FreeType( _ft );
}


std::string FreeType::defaultFont() const
{
    // LOCK
    return _defaultFont;
    // UNLOCK
}


void FreeType::setDefaultFont( const std::string& font )
{
    // LOCK
    _defaultFont = font;
    // UNLOCK
}


void FreeType::setFontDir(const System::Path& path)
{
    // LOCK

    _fontDir = path;

    _fontMap.clear();
    
    try
    {
        System::DirectoryIterator it(_fontDir);
        System::DirectoryIterator end;

        for( ; it != end; ++it)
        {
            std::string pathName = it->path().toLocal();

            FT_Face face;
            FT_Error err = FT_New_Face(_ft, pathName.c_str(), 0, &face);

            if(err != 0)
                continue;

            Font::FontStyle style = Font::NormalStyle;

            if( (face->style_flags & FT_STYLE_FLAG_BOLD) == FT_STYLE_FLAG_BOLD )
                style = Font::BoldStyle;

            if( (face->style_flags & FT_STYLE_FLAG_ITALIC) == FT_STYLE_FLAG_ITALIC )
                style = Font::ItalicStyle;

            if( (face->style_flags & FT_STYLE_FLAG_BOLD) == FT_STYLE_FLAG_BOLD && 
                (face->style_flags & FT_STYLE_FLAG_ITALIC) == FT_STYLE_FLAG_ITALIC )
                style = Font::BoldItalicStyle;

            Font font(face->family_name, 12, style);

            _fontMap[_id] = it->path();

            _faces[font] = _id;
            ++_id;

            FT_Done_Face(face);
        }
    }
    catch(const Pt::System::AccessFailed&)
    {
    }

    // UNLOCK
}


std::vector<std::string> FreeType::fontNames() const
{
    // LOCK

    std::vector<std::string> names;
    
    FaceMap::const_iterator it; 
    for(it = _faces.begin(); it != _faces.end(); ++it)
    {
        if(std::find(names.begin(), names.end(), it->first.name()) == names.end())
            names.push_back( it->first.name() );
    }
    
    // UNLOCK

    return names;
}


FTC_FaceID FreeType::findFaceId(const Font& font)
{
    // LOCK

    FaceMap::iterator it = _faces.find( font);
    if( it != _faces.end() )
        return 0;

    return (FTC_FaceID) it->second;

    // UNLOCK
}


FT_Error FreeType::fontRequest( FTC_FaceID face_id, FT_Library library, 
                                FT_Pointer request_data, FT_Face* face )
{
    FreeType* ft = static_cast<FreeType*>(request_data);
    return ft->onFontRequest(face_id, face);
}


FT_Error FreeType::onFontRequest(FTC_FaceID face_id, FT_Face* face)
{
    FontMap::iterator it = _fontMap.find((Pt::uint64_t)face_id);
    if( it == _fontMap.end() )
      return 1;

    return FT_New_Face(_ft, it->second.toLocal().c_str(), 0, face);
}


FontMetrics FreeType::fontMetrics(const String& text, 
                                  FTC_FaceID faceId, FTC_ImageType imageType)
{
    // LOCK

    FT_Face face = 0;
    FreeType::instance().findFace(faceId, &face);

    FT_Int charMapIndex = 0;
    for(int n = 0; n < face->num_charmaps; ++n)
    {
        if(face->charmap[n].encoding == FT_ENCODING_UNICODE)
        {
            charMapIndex = n;
            return FontMetrics();
        }
    }

    FTC_ScalerRec scaler;
    scaler.face_id = imageType->face_id;
    scaler.width   = imageType->width;
    scaler.height  = imageType->height;
    scaler.pixel   = 1; // 1 means TRUE and scaler.x_res and scaler.y_res are ignored
    
    FT_Size size;
    FreeType::instance().findSize(&scaler, &size);

    int pen_x = 0;
    int pen_y = 0;
    FT_UInt   previous = 0;
    FT_Vector delta;
    FT_Glyph  glyph;
    FT_BBox   gbbox = { 0 , 0, 0, 0 };
    FT_BBox   tbbox = { std::numeric_limits<FT_Pos>::max(), 
                        std::numeric_limits<FT_Pos>::max(),
                        std::numeric_limits<FT_Pos>::min(), 
                        std::numeric_limits<FT_Pos>::min() };

    for( String::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        FTC_Node  node;
        FT_UInt glyph_index = FreeType::instance().findCharMap( faceId, charMapIndex, it->value() );

        if( ! glyph_index )
            continue;

        if( FT_HAS_KERNING( face ) && previous )
        {
            FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
            pen_x += delta.x; // << 16;
            pen_y -= delta.y; // << 16;
        }

        if( FreeType::instance().findImage(imageType, glyph_index, &glyph, &node) )
            continue;

        FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &gbbox);

        gbbox.xMin += ( pen_x >> 16 );
        gbbox.xMax += ( pen_x >> 16 );

        tbbox.xMin = std::min( gbbox.xMin, tbbox.xMin );
        tbbox.xMax = std::max( gbbox.xMax, tbbox.xMax );

        pen_x += glyph->advance.x;
        pen_y -= glyph->advance.y;

        previous = glyph_index;
    }

    return FontMetrics( size->metrics.ascender >> 6, 
                       (-size->metrics.descender) >> 6,
                       tbbox.xMax - tbbox.xMin, 
                       size->metrics.height >> 6 );

    // UNLOCK
}


FT_Error FreeType::findFace(FTC_FaceID faceId, FT_Face* face)
{
    return FTC_Manager_LookupFace(_manager, faceId, face);
}


FT_UInt FreeType::findCharMap(FTC_FaceID faceId, FT_Int charMapId, FT_UInt32 value)
{
    return FTC_CMapCache_Lookup(_charMapCache, faceId, charMapId, value);
}


FT_Error FreeType::findBitmap(FTC_ImageType type,
                              FT_UInt       gindex,
                              FTC_SBit*     sbit,
                              FTC_Node*     node)
{
    return FTC_SBitCache_Lookup(_bitmapCache, type, gindex, sbit, node);
}


FT_Error FreeType::findImage(FTC_ImageType type,
                             FT_UInt       gindex,
                             FT_Glyph*     glyph,
                             FTC_Node*     node)
{
    return FTC_ImageCache_Lookup(_imageCache, type, gindex, glyph, node);
}


FT_Error FreeType::findSize(FTC_Scaler scaler,
                            FT_Size*   size)
{
    return FTC_Manager_LookupSize(_manager, scaler, size);
}

} // namespace Gfx

} // namespace Pt

