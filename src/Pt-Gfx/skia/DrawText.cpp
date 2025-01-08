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
#include "FreeType.h"

namespace Pt {

namespace Gfx {

DrawText::DrawText()
: _faceId(0)
, _fontSize(10)
, _clip( Point(0, 0), Size(999999, 999999) )
{
  _faceId = FreeType::instance().defaultFace();
}


DrawText::~DrawText()
{ 
}


void DrawText::setPen(const Pen& pen)
{
    _textColor = pen.color();
}


void DrawText::setFont(const Font& font)
{
    // findFaceId returns default font for emtpy font names
    _faceId = FreeType::instance().findFaceId(font);

    _fontSize = font.size();

    // setup the image type
    _imageType.face_id = _faceId;
    _imageType.width   = font.size();
    _imageType.height  = font.size();
    _imageType.flags   = FT_LOAD_DEFAULT;

    _transform.reset();
}


void DrawText::setClip(const Rect& clip)
{
    _clip = clip;
}


FontMetrics DrawText::fontMetrics(const String& text)
{
    return FreeType::instance().fontMetrics(text, _faceId, _fontSize);
}


void DrawText::draw(Image& image, Pt::ssize_t x, Pt::ssize_t y, 
                    const String& text, const CompositionMode& mode)
{
    return FreeType::instance().draw(image, x, y, text, _textColor, _clip, 
                                     mode, _transform, _faceId, _fontSize);
}


void DrawText::draw(Image& image, Pt::ssize_t x, Pt::ssize_t y, 
                    const String& text, const CompositionMode& mode, 
                    const Transform& trans)
{
    return FreeType::instance().draw(image, x, y, text, _textColor, _clip, 
                                     mode, trans, _faceId, _fontSize);
}

} //namespace

} //namespace
