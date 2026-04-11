/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2010 Aloysius Indrayanto

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

#ifndef PT_GFX_DRAWTEXT_H
#define PT_GFX_DRAWTEXT_H

#include <Pt/Gfx/Api.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H

#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/TextMetrics.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/String.h>

namespace Pt {

namespace Gfx {

/** @brief Draw text on an image

    This class is a function object to draw text on an image. Additionally,
    text metrics can be determined for given strings.
    TODO:
    + Optimize glyph bitmap clipping ( use ClippRect )
 */
class DrawText
{
  public:
    typedef ImageView::Point Point;
    typedef ImageView::Size Size;
    typedef ImageView::Rect Rect;

    public:
        /** @brief Default constructor
        */
        DrawText();

        /** @brief Destructor
        */
        ~DrawText();

        /** @brief Sets the text color.
        */
        void setPen(const Pen& pen);

        /** @brief Sets the text font.
        */
        void setFont(const Font& font);

        /** @brief Sets the clip.
        */
        void setClip(const Rect& clip);

        /** @brief Calculate font metrics for text

            @param font The text font
            @param text The text to calculate
        */
        TextMetrics textMetrics(const String& text);

        FontMetrics fontMetrics() const;

        /** @brief Draw text on the image

            The given Text is drawn at the given position (pos) using the given
            font and the given pen color. The specified point to which the
            text is    drawn is the base-line of the Text/font. If the outline
            color is set to 0 no text outline is drawn. Clipping is performed
            before the text is drawn.

            @param image The target image
            @param color The text color
            @param pos The position to draw
            @param text The text to draw
            @param mode The composition mode of the text
        */
        void draw(Image& image, Pt::ssize_t x, Pt::ssize_t y, 
                  const String& text, const CompositionMode& mode);

        void draw(Image& image, Pt::ssize_t x, Pt::ssize_t y, 
                  const String& text, const CompositionMode& mode, 
                  const Transform& trans);

  private:
        FTC_FaceID       _faceId;
        std::size_t      _fontSize;
        FTC_ImageTypeRec _imageType;
        Transform        _transform;
        ColorF            _textColor;
        Rect             _clip;
};

} //namespace

} //namespace

#endif
