/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PT_GFX_DRAWTEXT_H
#define PT_GFX_DRAWTEXT_H

#include "freetype/include/ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "Pt/Gfx/ARgbImage.h"
#include "Pt/Math/Point.h"
#include "Pt/Text/String.h"
#include "Pt/Gfx/ARgbColor.h"
#include "Pt/Gfx/Font.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Gfx/FontMetrics.h"

namespace Pt{
namespace Gfx{

/** @brief Draw text on an image

    This class is a function object to draw text on an image. Additionally,
    text metrics can be determined for given strings.
 */
class DrawText
{
    public:
        /** @brief Default constructor
        */
        DrawText();

        /** @brief Destructor
        */
        ~DrawText();

        /** @brief Sets the current font

            @param font The current font
        */
        void setFont( const Font& font );

        /** @brief Calculate font metrics for text

            @param text The text to calculate
        */
        FontMetrics fontMetrics( const Text::String& text );

        /** @brief Draw text on the image

            The given Text is drawn at the given position (pos) using the given
            font and the given pen color. The specified point to which the
            text is	drawn is the base-line of the Text/font. If the outline
            color is set to 0 no text outline is drawn. Clipping is performed
            before the text is drawn.

			@param image The target image
            @param pen The pen to be used
            @param pos The position to draw
            @param text The text to draw
            @param outline The outline color
        */
        void draw( ARgbImage& image, const Pen& pen, const Math::Point& pos, const Text::String& text, const ARgbColor* outline = 0 );

    private:
        void drawGlyph( ARgbImage& image, const ARgbColor& color,  int xpos, int ypos, FT_Bitmap& bm );

        void _draw( ARgbImage& image, const Pen& pen, const Math::Point& pos, const Text::String& text, const ARgbColor* backGround = 0);

        inline void mixColor(ARgbColor& dst, const ARgbColor& src, const uint8_t& factor)
        {
             const uint32_t oF = factor;
             const uint32_t rF = 255 - oF;

             const uint32_t dR = uint32_t( dst.red()   ) * rF;
             const uint32_t dG = uint32_t( dst.green() ) * rF;
             const uint32_t dB = uint32_t( dst.blue()  ) * rF;

             const uint32_t sR = uint32_t( src.red()   ) * oF;
             const uint32_t sG = uint32_t( src.green() ) * oF;
             const uint32_t sB = uint32_t( src.blue()  ) * oF;

             dst.setRed  ( (dR + sR) >> 8);
             dst.setGreen( (dG + sG) >> 8);
             dst.setBlue ( (dB + sB) >> 8);
        }

        FT_Library  _ft;
        FT_Face     _face;
        Font        _font;
        FT_Matrix   _matrix;
};

} //namespace Gfx
} //namespace Pt

#endif
