/* Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include <algorithm>

#include <Pt/String.h>

#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Math.h>

#include "DrawText2.h"


namespace Pt {
namespace Gfx {


DrawText2::DrawText2()
: _faceId(0), _fontAngle(0)
{
    _matrix.xx = 0;
    _matrix.xy = 0;
    _matrix.yx = 0;
    _matrix.yy = 0;
}

DrawText2::~DrawText2()
{}

void DrawText2::setFont(const Font& font)
{
    if( font.name().empty() ) {
        Font defaultFont(FreeType2::instance().defaultFont(), font);
        _faceId = FreeType2::instance().findFaceId(defaultFont);
    }
    else {
        _faceId = FreeType2::instance().findFaceId(font);
    }

    // Setup the image type
    _imageType.face_id = _faceId;
    _imageType.width   = font.size();
    _imageType.height  = font.size();
    _imageType.flags   = FT_LOAD_DEFAULT;

    // Setup the rotation matrix
    _fontAngle = font.angle() % 3600;

    if(_fontAngle < 0) _fontAngle += 3600;

    const float angle   = (_fontAngle / 10.0f * Gfx::Math::Pi) / 180.0f;
    const float cosinus = Gfx::Math::fastCos( angle ) * 0x10000L;
    const float sinus   = Gfx::Math::fastSin( angle ) * 0x10000L;

    _matrix.xx = (FT_Fixed) ( cosinus);
    _matrix.xy = (FT_Fixed) (-sinus  );
    _matrix.yx = (FT_Fixed) ( sinus  );
    _matrix.yy = (FT_Fixed) ( cosinus);
}

FontMetrics DrawText2::fontMetrics(const String& text)
{
    return FreeType2::instance().fontMetrics(text, _faceId, &_imageType);
}

void DrawText2::draw(Image& image, const Color& color, const Point& pos, const String& text, const CompositionMode& mode)
{
    return FreeType2::instance().draw(image, _clip, pos, color, _fontAngle, mode, text, _matrix, _faceId, &_imageType, false);
}

void DrawText2::drawMono(Image& image, const Color& color, const Point& pos, const String& text, const CompositionMode& mode)
{
    return FreeType2::instance().draw(image, _clip, pos, color, _fontAngle, mode, text, _matrix, _faceId, &_imageType, true);
}

void DrawText2::genPointsFromChar(std::vector<PointF>& dst, const Char& chr)
{
    FreeType2::instance().genPointsFromChar(dst, chr, _faceId);
}


} //namespace
} //namespace
