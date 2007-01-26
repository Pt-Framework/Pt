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
#include <Pt/System/Clock.h>

namespace Pt{
namespace Gfx{

DrawText::DrawText()
{
    FT_Init_FreeType( &_ft );

	if( FTC_Manager_New( _ft, 0, 0, 0, &DrawText::fontRequest, 0, &_manager ) )
		throw std::runtime_error( "FTC_Manager_New failed" + PT_SOURCEINFO );

	FTC_ImageCache_New( _manager, &_imageChace );
  
    FTC_CMapCache_New( _manager, &_charMapCache );
     
    FTC_SBitCache_New( _manager, &_bitmapCache );

	
/*
    if( FT_New_Face( _ft, "c:\\WINDOWS\\fonts\\tahoma.ttf", 0, &_face ) )
        throw IllegalArgument( "FT_New_Face  error", PT_SOURCEINFO );

        FT_Attach_File( _face, "c:\\WINDOWS\\fonts\\tahoma" );
*/

/*    if( FT_Select_Charmap( _face, FT_ENCODING_UNICODE ) )
        FT_Select_Charmap( _face, FT_ENCODING_NONE );  */
}

DrawText::~DrawText()
{
//    FT_Done_Face( _face );
    FT_Done_FreeType( _ft );
}

FT_Error DrawText::fontRequest( FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face* aface )
{
    //FT_Error error = FT_New_Memory_Face( library, vera, veraSize, 0, aface );
   
     FT_Error error  = FT_New_Face( library, "c:\\WINDOWS\\fonts\\tahoma.ttf", 0, aface );

     FT_Attach_File( *aface, "c:\\WINDOWS\\fonts\\tahoma" );


	return error;
}

void DrawText::setFont( const Font& font )
{

    //Devide size by 64 because free type works with 1/64 pixel.
//    FT_Set_Char_Size( face, font.size()<<6, font.size()<<6, 72, 72 );

	FTC_ScalerRec  scaler;
	scaler.width  = font.size()<<6;
	scaler.height = font.size()<<6;
	scaler.x_res  = 72;
	scaler.pixel  = 1;
	scaler.y_res  = 72;
	
	_fontSize = font.size();
	
		
/*	FTC_Manager_LookupSize( _manager, &scaler, &_size );
	FT_Set_Char_Size( _size->face, font.size()<<6, font.size()<<6, 72, 72 )*/;	

    //Setup the rotation matrix
    double angle = (font.angle()/3600.0) * 3.14159 * 2 ;

    _matrix.xx = (FT_Fixed) ( cos( angle )* 0x10000L );
    _matrix.xy = (FT_Fixed) ( -sin( angle )* 0x10000L );
    _matrix.yx = (FT_Fixed) ( sin( angle )* 0x10000L );
    _matrix.yy = (FT_Fixed) ( cos( angle )* 0x10000L );
}

FontMetrics DrawText::fontMetrics( const Text::String& text )
{
	FT_Face face;
	FTC_Manager_LookupFace( _manager, 0, &face );

    FT_UInt         previous    = 0;
    FT_GlyphSlot    slot        = face->glyph;
    FT_Vector       delta;
    FT_Glyph        glyph;
    FT_BBox         gbbox = {0,0,0,0};
    FT_BBox         tbbox = {100000,100000,-100000,-100000};

    int pen_x = 0;
    int pen_y = 0;

    for( Text::String::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        const FT_UInt glyph_index = FT_Get_Char_Index( face, it->value() );

        if( !glyph_index )
            continue;

        if( FT_HAS_KERNING( face ) && previous )
        {
            FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
            pen_x += (delta.x) >> 6;
            pen_y -= (delta.y) >> 6;
        }

        if( FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT ) )
            continue;

        const int incX = slot->advance.x >> 6;
        const int incY = slot->advance.y >> 6;

        if( FT_Get_Glyph( face->glyph, &glyph ))
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

    return FontMetrics(face->size->metrics.ascender>>6, (-face->size->metrics.descender)>>6, tbbox.xMax - tbbox.xMin, face->size->metrics.height>>6 );
}

void DrawText::_draw( ARgbImage& image, const ARgbColor& color, const Math::Point& pos, const Text::String& text, const ARgbColor* backGround )
{
    FT_Vector			glyphPos;
    FT_Vector			delta;
    FT_UInt				previous = 0;
    FT_Glyph			glyph;
    FTC_Node			node;
    FTC_ImageTypeRec	imageType;
    FT_Face				face;
    Pt::System::Clock   clock; 
    FTC_SBit			glyphBitmap;
    
    imageType.face_id	= 0;
    imageType.width		= _fontSize;
    imageType.height	= _fontSize;
    imageType.flags		= FT_RENDER_MODE_NORMAL;     	
    FT_UInt glyph_index;
	
	FTC_Manager_LookupFace( _manager, 0, &face );

    glyphPos.x = pos.x();
    glyphPos.y = pos.y();

    FT_Set_Transform( face, &_matrix, &glyphPos );

    for( Text::String::const_iterator it = text.begin(); it != text.end(); ++it )
    {         
        glyph_index = FTC_CMapCache_Lookup(  _charMapCache,  0, 0,  it->value() );
        //Time: 0.0
        
        if( !glyph_index )
            continue;

		
        if( FT_HAS_KERNING( face ) && previous )
        {
            FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
            glyphPos.x += delta.x >> 6;
            glyphPos.y -= delta.y >> 6;
        }
		//Time 1e-006

		if(  FTC_ImageCache_Lookup( _imageChace, &imageType, glyph_index, &glyph, &node ) )
			continue;		     
		
		//Time 1e-006 after caching		                                 
        const int incX = ( glyph->advance.x >> 6 ) /1000;
        const int incY = glyph->advance.y >> 6;	
			
		FTC_SBitCache_Lookup( _bitmapCache, &imageType, glyph_index, &glyphBitmap, &node );
		// Time 1e-006 
			
		
		const int left      = glyphPos.x + glyphBitmap->left;
        const int top       = glyphPos.y - glyphBitmap->top;
		
        if( backGround )
        {            
            const int leftUp    = left + 1;
            const int leftDown  = left - 1;
            const int topUp     = top + 1;
            const int topDown   = top - 1;

            drawGlyph( image, *backGround, leftUp, top, glyphBitmap );
            drawGlyph( image, *backGround, leftDown, top, glyphBitmap );
            drawGlyph( image, *backGround, leftDown, topUp, glyphBitmap );
            drawGlyph( image, *backGround, leftDown, topDown, glyphBitmap );
            drawGlyph( image, *backGround, leftUp, topDown, glyphBitmap );
            drawGlyph( image, *backGround, leftUp, topUp, glyphBitmap );
            drawGlyph( image, *backGround, leftDown, topDown, glyphBitmap );
            drawGlyph( image, *backGround, leftDown, topUp, glyphBitmap );
        }

        drawGlyph( image, color, left, top, glyphBitmap );

        glyphPos.x   += incX;
        glyphPos.y   -= incY;
        previous     = glyph_index;
    }
}

void DrawText::draw( ARgbImage& image, const ARgbColor& color, const Math::Point& pos, const Text::String& text, const ARgbColor* outline)
{
    _draw( image, color,  pos, text, outline );
}

void DrawText::drawGlyph( ARgbImage& image, const ARgbColor& color,  int xpos, int ypos, FTC_SBit& bm )
{
    int							bmPitch = bm->pitch;
	Pt::uint32_t				yOffset = 0;
    int							dsy		= ypos;
    int							dsx		= 0;        
    const Pt::ssize_t			x2		= image.width() - 1;
    const Pt::ssize_t			y2		= image.height() - 1;
    ARgbImage::PixelIterator	pixel;         

    if( bmPitch < bm->width )
        bmPitch += bm->width;        
    
    for( Pt::int32_t y = 0; y < bm->height; ++y, ++dsy )
    {
        yOffset = y * bmPitch;

        if( dsy < 0 )
            continue;

        if( dsy > y2 )
            break;

        dsx		= xpos;
        pixel   = image.iterator( dsx, dsy );

        for( Pt::int32_t x = 0; x < bm->width; ++x, ++dsx )
        {
            if( dsx < 0 )
                 continue;

            if( dsx > x2 )
                break;
			
			const int px = yOffset + x ;			
			
            if( bm->buffer[ px ] )
                mixColor( *pixel, color, bm->buffer[ px ] );

			pixel++;                                                
        }
    }
}

} //namespace Pt

} //namespace Gfx
