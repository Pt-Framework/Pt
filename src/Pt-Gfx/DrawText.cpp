/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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

#include "DrawText.h"
#include <Pt/Gfx/Font.h>
#include <Pt/String.h>
#include <algorithm>
#include <cmath>

namespace Pt {

namespace Gfx {

DrawText::DrawText()
: _faceId(0)
, _fontAngle(0)
// TODO: handle _clip.isNull() like no clipping
, _clip( Point(0, 0), Size(999999, 999999) ) 
{
    _matrix.xx = 0;
    _matrix.xy = 0;
    _matrix.yx = 0;
    _matrix.yy = 0;
}


DrawText::~DrawText()
{
}


void DrawText::setFont(const Font& font)
{
    if( font.name().empty() )
    {
        Font defaultFont(FreeType::instance().defaultFont(), font);

        _faceId = FreeType::instance().findFaceId(defaultFont);
    }
    else
    {
        _faceId = FreeType::instance().findFaceId(font);
    }
    
    // setup the image type
    _imageType.face_id = _faceId;
    _imageType.width   = font.size();
    _imageType.height  = font.size();
    _imageType.flags   =  FT_LOAD_DEFAULT | FT_LOAD_RENDER;

    // setup the rotation matrix
    _fontAngle = font.angle() % 3600;

    if ( _fontAngle < 0 )
        _fontAngle += 3600;

    const double angle   = (_fontAngle / 10.0  *  3.14159) / 180.0 ;
    const double cosinus = std::cos( angle ) * 0x10000L;
    const double sinus   = std::sin( angle ) * 0x10000L;

    _matrix.xx = (FT_Fixed) std::ceil( cosinus );
    _matrix.xy = (FT_Fixed) std::ceil( -sinus );
    _matrix.yx = (FT_Fixed) std::ceil( sinus );
    _matrix.yy = (FT_Fixed) std::ceil( cosinus );
}


FontMetrics DrawText::fontMetrics(const String& text)
{
    return FreeType::instance().fontMetrics(text, _faceId, &_imageType);
}


void DrawText::draw(Image& image, const Color& color, 
                    const Point& pos, const String& text)
{
    return FreeType::instance().draw(image, color, _fontAngle, pos, text,_clip, 
                                     _matrix, _faceId, &_imageType);
}

} //namespace

} //namespace
