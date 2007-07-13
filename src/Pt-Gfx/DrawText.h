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

#include <Pt/Gfx/Api.h>

#include "freetype/include/ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H

#include "FreeType.h"
#include <Pt/Gfx/ARgbImage.h>
#include "Pt/Math/Point.h"
#include "Pt/String.h"
#include "Pt/Gfx/ARgbColor.h"
#include "Pt/Gfx/Font.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Gfx/FontMetrics.h"

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
        void draw( ARgbImage& image, const ARgbColor& color, const Math::Point& pos, const String& text, const ARgbColor* background = 0 );

    private:
        void drawGlyph( ARgbImage& image, const ARgbColor& color, int xpos, int ypos, int bmPitch, int height, int width, const unsigned char* buffer )
        {
            Pt::uint32_t                yOffset = 0;
            int                            dsy        = ypos;
            int                            dsx        = 0;
            const Pt::ssize_t            x2        = image.width() - 1;
            const Pt::ssize_t            y2        = image.height() - 1;
            ARgbImage::PixelIterator    pixel;

            if( bmPitch < width )
                bmPitch += width;

            for( Pt::int32_t y = 0; y < height; ++y, ++dsy )
            {
                yOffset = y * bmPitch;

                if( dsy < 0 )
                    continue;

                if( dsy > y2 )
                    break;

                dsx        = xpos;
                pixel   = image.iterator( dsx, dsy );

                for( Pt::int32_t x = 0; x < width; ++x, ++dsx, ++pixel )
                {
                    if( dsx < 0 )
                        continue;

                    if( dsx > x2 )
                        break;

                    const int px = yOffset + x ;

                                        void *p = &(*pixel);
                                        void *b = &(*image.begin());
                                        void *e = &(*image.end());
                                        assert(p>=b && p<=e);

                    if( buffer[ px ] )
                        mixColor( *pixel, color, buffer[ px ] );

                }
            }
        }

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

        inline FTC_FaceID faceId() const
        { return (FTC_FaceID) &_faceId ; }

        static FT_Error fontRequest( FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face* aface );

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

} //namespace Gfx
} //namespace Pt

#endif
