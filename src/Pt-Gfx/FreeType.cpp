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
#include "WqyZenhei.h"
#include <stdexcept>
#include <iostream>

namespace Pt {

namespace Gfx {

FTC_FaceID FreeType::_dejavuSans = (FTC_FaceID)&DejaVuSans[0];
FTC_FaceID FreeType::_dejavuSansBold = (FTC_FaceID)&DejaVuSansBold[0];
FTC_FaceID FreeType::_dejavuSansItalic = (FTC_FaceID)&DejaVuSansItalic[0];
FTC_FaceID FreeType::_dejavuSansBoldItalic = (FTC_FaceID)&DejaVuSansBoldItalic[0];
FTC_FaceID FreeType::_wqyZenhei =  (FTC_FaceID)&WqyZenhei[0];

FreeType::FreeType()
{   
    if( FT_Init_FreeType( &_ft ) )
        throw std::runtime_error("FT_Init_FreeType");

    if( FTC_Manager_New( _ft, 0, 0, 0, &FreeType::fontRequest, 0, &_manager ) )
        throw std::runtime_error( "FTC_Manager_New" );

    if( FTC_ImageCache_New( _manager, &_imageCache ) )
        throw std::runtime_error( "FTC_ImageCache_New" );

    if( FTC_CMapCache_New( _manager, &_charMapCache ) )
        throw std::runtime_error( "FTC_CMapCache_New" );

    if( FTC_SBitCache_New( _manager, &_bitmapCache ) )
        throw std::runtime_error( "FTC_SBitCache_New" );
}


FreeType::~FreeType()
{
    FTC_Manager_Done( _manager );
    FT_Done_FreeType( _ft );
}


FT_Error FreeType::fontRequest( FTC_FaceID face_id, FT_Library library, 
                                FT_Pointer request_data, FT_Face* face )
{
    if(face_id == _dejavuSans)
        return FT_New_Memory_Face(library, DejaVuSans, DejaVuSansSize, 0, face);

    if(face_id == _dejavuSansBold)
        return FT_New_Memory_Face(library, DejaVuSansBold, DejaVuSansBoldSize, 0, face);

    if(face_id == _dejavuSansItalic)
        return FT_New_Memory_Face(library, DejaVuSansItalic, DejaVuSansItalicSize, 0, face);

    if(face_id == _dejavuSansBoldItalic)
        return FT_New_Memory_Face(library, DejaVuSansBoldItalic, DejaVuSansBoldItalicSize, 0, face);

    if(face_id == _wqyZenhei)
        return FT_New_Memory_Face(library, WqyZenhei, WqyZenheiSize, 0, face);
    
     return 1;
}


FTC_FaceID FreeType::findFaceId(const Font& font)
{
    if(font.name() == "DejaVu-Sans")
    {
        switch( font.fontStyle() )
        {
            default:
            case Font::NormalStyle:
                return _dejavuSans;
            case Font::BoldStyle:
                return _dejavuSansBold;
            case Font::ItalicStyle:
                return _dejavuSansItalic;
            case Font::BoldItalicStyle:
                return _dejavuSansBoldItalic;
        }

    }

    if( font.name() == "WenQuanYi Zen Hei")
       return _wqyZenhei;

    return _dejavuSans;
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

