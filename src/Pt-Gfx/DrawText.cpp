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
/*
    if( FT_New_Face( _ft, "c:\\WINDOWS\\fonts\\tahoma.ttf", 0, &_face ) )
        throw IllegalArgument( "FT_New_Face  error", PT_SOURCEINFO );

        FT_Attach_File( _face, "c:\\WINDOWS\\fonts\\tahoma" );
*/      

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

FontMetrics DrawText::fontMetrics( const Text::String& text )
{
    FT_UInt         previous    = 0;
    FT_GlyphSlot    slot        = _face->glyph;
    FT_Vector       delta;
    FT_Glyph        glyph;
    FT_BBox         gbbox = {0,0,0,0};
    FT_BBox         tbbox = {100000,100000,-100000,-100000};    

    int pen_x = 0;
    int pen_y = 0;  
    
    for( Text::String::const_iterator it = text.begin(); it != text.end(); ++it )
    {        
        FT_UInt glyph_index = FT_Get_Char_Index( _face, it->value() );
        
        if( !glyph_index )
            continue;
          
        if( FT_HAS_KERNING( _face ) && previous )
        {
            FT_Get_Kerning( _face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
            pen_x += delta.x >> 6;
            pen_y -= delta.y >> 6;
        }            
        
        if( FT_Load_Glyph( _face, glyph_index, FT_LOAD_DEFAULT ) )
            continue;

        const int incX = slot->advance.x >> 6;
        const int incY = slot->advance.y >> 6;    
        
        if( FT_Get_Glyph( _face->glyph, &glyph ))
            continue;
        
        FT_Glyph_Get_CBox( glyph, FT_GLYPH_BBOX_PIXELS, &gbbox );
        
        gbbox.xMin += pen_x;
        gbbox.xMax += pen_x;
        
        FT_Done_Glyph( glyph );
        
        tbbox.xMin = std::min( gbbox.xMin, tbbox.xMin );
        tbbox.xMax = std::max( gbbox.xMax, tbbox.xMax );

        pen_x += incX;
        pen_y -= incY;
        
        previous = glyph_index;
    }    
    
    return FontMetrics(_face->size->metrics.ascender>>6, (-_face->size->metrics.descender)>>6, tbbox.xMax - tbbox.xMin, _face->size->metrics.height>>6 );
}

void DrawText::_drawText( ARgbImage& image, const Pen& pen, const Math::Point& pos, const Text::String& text, const ARgbColor* backGround )
{
    FT_Vector       glyphPos;
    FT_Vector       delta;
    FT_UInt         previous = 0;
    FT_GlyphSlot    slot     = _face->glyph;

    glyphPos.x = pos.x();
    glyphPos.y = pos.y();

    if( _font.angle() != 0 )
        FT_Set_Transform( _face, &_matrix, &glyphPos );

    FT_UInt glyph_index = 0;

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
        const int incX = slot->advance.x >> 6;
        const int incY = slot->advance.y >> 6;
        
        if( backGround )
        {
            const int left      = glyphPos.x + slot->bitmap_left;
            const int top       = glyphPos.y - slot->bitmap_top;
            const int leftUp    = left + 1;
            const int leftDown  = left - 1;
            const int topUp     = top + 1;
            const int topDown   = top - 1;
        
            drawGlyph( image, *backGround, leftUp, top, slot->bitmap );
            drawGlyph( image, *backGround, leftDown, top, slot->bitmap );
            drawGlyph( image, *backGround, leftDown, topUp, slot->bitmap );
            drawGlyph( image, *backGround, leftDown, topDown, slot->bitmap );
            drawGlyph( image, *backGround, leftUp, topDown, slot->bitmap );
            drawGlyph( image, *backGround, leftUp, topUp, slot->bitmap );
            drawGlyph( image, *backGround, leftDown, topDown, slot->bitmap );
            drawGlyph( image, *backGround, leftDown, topUp, slot->bitmap );
        }

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

void DrawText::drawGlyph( ARgbImage& image, const ARgbColor& color,  int xpos, int ypos, FT_Bitmap& bm )
{
    const Pt::uint8_t* bitmap       = bm.buffer;
    int                bmWidth      = bm.width;
    int                bmPitch      = bm.pitch;
    int                bmHeight     = bm.rows;
    int                bmStartX     = 0;
    int                bmStartY     = 0;     
    int                glyphPosX    = xpos;
    int                glyphPosY    = ypos;

    if( bmPitch < bmWidth)
        bmPitch += bmWidth;
        
    //Clipping left X        
    if( xpos < 0 )
    {
        bmStartX  = -xpos;
        glyphPosX = 0;    
    }

    //Clipping right X           
    if( xpos + bmWidth > image.width() )
        bmWidth -=  ( ( xpos + bmWidth ) - image.width() );      

    //Clipping top Y    
    if( ypos < 0 )
    {
        bmStartY  = -ypos;
        glyphPosY = 0;
    }
    
    //Clipping bottom Y
    if( ypos + bmHeight > image.height() )
         bmHeight -=  ( ( ypos + bmHeight ) - image.height() );

    int dsy = glyphPosY;
    int dsx = 0;
    
    for( Pt::int32_t y = bmStartY; y < bmHeight; ++y, ++dsy )
    {
        const Pt::uint32_t yOffset = y * bmPitch;

        dsx = glyphPosX;
        
        for( Pt::int32_t x = bmStartX; x < bmWidth; ++x, ++dsx )
        {
            const Pt::uint8_t col = bitmap[ yOffset + x ];

            if( col )
                mixColor( image.pixel(dsx, dsy), color, col );
        }
    }
}

} //namespace Pt

} //namespace Gfx
