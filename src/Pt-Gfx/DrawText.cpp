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
#include "DrawText.h"
#include <cmath>
#include "Pt/Exception.h"
#include "Pt/Gfx/Font.h"
#include "Vera.h"

namespace Pt{
namespace Gfx{

DrawText::DrawText()
: _font( "Vera", 12 )
{
    FT_Init_FreeType( &_ft );

    if( FT_New_Memory_Face( _ft, vera, veraSize, 0, &_face) )
        throw RuntimeError( "FT_New_Memory_Face failed", PT_SOURCEINFO );

    if( FT_Select_Charmap( _face, FT_ENCODING_UNICODE ) )
        FT_Select_Charmap( _face, FT_ENCODING_NONE );

    this->setFont( _font );
}

DrawText::~DrawText()
{
    FT_Done_Face( _face );
    FT_Done_FreeType( _ft );
}

void DrawText::setFont( const Font& font )
{
    _font = font;

    //Devide size by 64 because free type works with 1/64 pixel.
    FT_Set_Char_Size( _face, _font.size()<<6, _font.size()<<6, 72, 72 );

    //Setup the rotation matrix
    if( _font.angle() != 0 )
    {
        double angle = (_font.angle()/3600.0) * 3.14159 * 2 ;

        _matrix.xx = (FT_Fixed) ( cos( angle )* 0x10000L );
        _matrix.xy = (FT_Fixed) ( -sin( angle )* 0x10000L );
        _matrix.yx = (FT_Fixed) ( sin( angle )* 0x10000L );
        _matrix.yy = (FT_Fixed) ( cos( angle )* 0x10000L );
    }
}

void DrawText::_drawText( ARgbImage& image, const Pen& pen, const Math::Point& pos, const Text::String& text, const ARgbColor* backGround )
{
    FT_Vector       glyphPos;
    FT_Vector       delta;
    int             incX        = 0;
    int             incY        = 0;
    FT_UInt         previous    = 0;
    FT_GlyphSlot    slot        = _face->glyph;

    glyphPos.x = pos.x();
    glyphPos.y = pos.y();

    if( _font.angle() != 0 )
        FT_Set_Transform( _face, &_matrix, &glyphPos );

    for( Text::String::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        // 1.7e-05
        FT_UInt glyph_index = FT_Get_Char_Index( _face, it->value() );

        if( !glyph_index )
            continue;

        // 1.9e-05

        if( FT_HAS_KERNING( _face ) && previous )
        {
            FT_Get_Kerning( _face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
            glyphPos.x += delta.x >> 6;
            glyphPos.y -= delta.y >> 6;
        }

        // 1.9e-05

        if( FT_Load_Glyph( _face, glyph_index, FT_LOAD_RENDER ) )
            continue;

        // 0.00025

        incX = slot->advance.x >> 6;
        incY = slot->advance.y >> 6;

       if( backGround )
        {
            drawGlyph( image, *backGround, glyphPos.x + slot->bitmap_left + 1, glyphPos.y - slot->bitmap_top, slot->bitmap );
            drawGlyph( image, *backGround, glyphPos.x + slot->bitmap_left - 1, glyphPos.y - slot->bitmap_top, slot->bitmap );
            drawGlyph( image, *backGround, glyphPos.x + slot->bitmap_left - 1, glyphPos.y - slot->bitmap_top + 1, slot->bitmap );
            drawGlyph( image, *backGround, glyphPos.x + slot->bitmap_left - 1, glyphPos.y - slot->bitmap_top - 1, slot->bitmap );
            drawGlyph( image, *backGround, glyphPos.x + slot->bitmap_left + 1, glyphPos.y - slot->bitmap_top - 1, slot->bitmap );
            drawGlyph( image, *backGround, glyphPos.x + slot->bitmap_left + 1, glyphPos.y - slot->bitmap_top + 1, slot->bitmap );
            drawGlyph( image, *backGround, glyphPos.x + slot->bitmap_left - 1, glyphPos.y - slot->bitmap_top - 1, slot->bitmap );
            drawGlyph( image, *backGround, glyphPos.x + slot->bitmap_left - 1, glyphPos.y - slot->bitmap_top + 1, slot->bitmap );
        }

        // 0.00075

        drawGlyph( image, pen.color(), glyphPos.x + slot->bitmap_left, glyphPos.y - slot->bitmap_top, slot->bitmap );

        glyphPos.x  += incX;
        glyphPos.y  -= incY;
        previous    = glyph_index;

        // 0.00084
    }
}


void DrawText::draw( ARgbImage& image, const Pen& pen,  const Math::Point& pos, const Text::String& text )
{
    _drawText( image, pen, pos, text, 0 );
}

void DrawText::draw( ARgbImage& image, const Pen& pen, const Math::Point& pos, const Text::String& text, const ARgbColor& backGround )
{
    _drawText( image, pen,  pos, text, &backGround );
}

void DrawText::drawGlyph( ARgbImage& image, const ARgbColor& color,  int glyphPosX, int glyphPosY, FT_Bitmap& bm )
{
    const Pt::uint8_t* bitmap =  bm.buffer;
    const int          bmWidth  = bm.width;
    int                bmPitch  = bm.pitch;
    const int          bmHeight = bm.rows;

    const Pt::ssize_t x1 = 0;
    const Pt::ssize_t x2 = image.width() - 1;
    const Pt::ssize_t y1 = 0;
    const Pt::ssize_t y2 = image.height() - 1;

    if( bmPitch < bmWidth)
        bmPitch += bmWidth;

    /*int numpix = bmWidth;
    int numrows = bmHeight;

    const int maxX = glyphPosX + bmWidth;
    if(maxX > x2)
    {
        numpix = bmWidth - (maxX - x2);
        if(numpix < 0) numpix = 0;
    }

    const int maxY = glyphPosY + bmWidth;
    if(maxY > y2)
    {
        numrows = bmHeight - (maxY - y2);
        if(numrows < 0) numrows = 0;
    }*/

    int dsy = glyphPosY;
    for( Pt::int32_t y = 0; y < bmHeight; ++y, ++dsy )
    {
        if( dsy < y1 )
            continue;

        if( dsy > y2 )
            break;

        const Pt::uint32_t yOffset = y * bmPitch;

        int dsx = glyphPosX;
        for( Pt::int32_t x = 0; x < bmWidth; ++x, ++dsx )
        {
            if( dsx < x1 )
                continue;

            if( dsx > x2 )
                break;

            const Pt::uint8_t col = bitmap[ yOffset + x ];
            if( col )
            {
                //image.pixel(dsx, dsy) = color;
                //mixColor( image.pixel(dsx, dsy), color, col / 255.0f );
                mixColor( image.pixel(dsx, dsy), color, col );
            }

        }
    }
}

} //namespace Pt

} //namespace Gfx
