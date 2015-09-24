/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
 * Copyright (C) 2010 Aloysius Indrayanto
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef PT_GFX_DRAWTEXT_H
#define PT_GFX_DRAWTEXT_H

#include <Pt/Gfx/Api.h>

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H

#include "FreeType.h"
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Point.h>
#include <Pt/String.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/FontMetrics.h>
#include <cassert>

namespace Pt{
namespace Gfx{

/** @brief Draw text on an image

    This class is a function object to draw text on an image. Additionally,
    text metrics can be determined for given strings.
    TODO:
    + Optimize glyph bitmap clipping ( use ClippRect )
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


        /** @brief Sets the text font

            @param font The text font
        */
        void setFont( const Font& font );


        /** @brief Calculate font metrics for text

            @param font The text font
            @param text The text to calculate
        */
        FontMetrics fontMetrics( const String& text );

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
            @param background The background color of the font
        */
        void draw( Image& image, const Color& color, const PointF& pos, const String& text, const Color* background = 0 );

    private:
        void drawGlyph( Image& image, const Color& color, int xpos, int ypos, int bmPitch, int height, int width, const unsigned char* buffer )
        {
            Pt::uint32_t             yOffset = 0;
            int                      dsy     = 0;
            int                      dsx     = 0;
            const Pt::ssize_t        x2      = image.width() - 1;
            const Pt::ssize_t        y2      = image.height() - 1;

            if( bmPitch < width )
                bmPitch += width;

            // NOTE: The PixelIterator should not take negative X or Y coordinate, hence we need to offset the starting position
            int ofsx = 0;
            
            if(xpos < 0) 
            {
                ofsx = -xpos;
                xpos = 0;
            }
            
            int ofsy = 0;
            
            if(ypos < 0) 
            {
                ofsy = -ypos;
                ypos = 0;
            }

            dsy = ypos;

            for( Pt::int32_t y = ofsy; y < height; ++y, ++dsy )
            {
                yOffset = y * bmPitch;

                if( dsy < 0 )
                    continue;

                if( dsy > y2 )
                    break;

                dsx   = xpos;
                

                for( Pt::int32_t x = ofsx; x < width; ++x, ++dsx )
                {
                    if( dsx < 0 )
                        continue;

                    if( dsx > x2 )
                        break;

                    Color pixel = image.color( dsx, dsy );

                    const int px = yOffset + x ;

                    if( buffer[ px ] )
                        mixColor( pixel, color, buffer[ px ] );

										image.setColor(dsx, dsy, pixel);
									
                }
            }
        }

        inline void mixColor(Color& dst, const Color& src, const uint8_t& factor)
        {
             const float oF = factor/255.0f;
             const float rF = 1.0f - oF;

             const float dR = dst.red() * rF;
             const float dG = dst.green() * rF;
             const float dB = dst.blue() * rF;

             const float sR = src.red() * oF;
             const float sG = src.green() * oF;
             const float sB = src.blue()* oF;

             dst.setRed  ( (dR + sR) );
             dst.setGreen( (dG + sG) );
             dst.setBlue ( (dB + sB) );
        }

        inline FTC_FaceID faceId() const
        { 
          return (FTC_FaceID) &_faceId ; 
        }

        static FT_Error fontRequest( FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face* aface );

  private:
        FT_Matrix            _matrix;
        FTC_Manager            _manager;
        FTC_ImageCache        _imageChace;
        FTC_CMapCache        _charMapCache;
        FTC_SBitCache        _bitmapCache;
        Pt::ssize_t            _fontAngle;
        FTC_ImageTypeRec    _imageType;
        size_t                _faceId;
        size_t                _charMapId;
};

}} //namespace

#endif
