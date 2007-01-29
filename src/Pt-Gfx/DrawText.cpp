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

namespace Pt {

namespace Gfx {

DrawText::DrawText()
: _fontAngle( 0 )
, _fontSize( 0 )
{
	_matrix.xx = 0;
    _matrix.xy = 0;
    _matrix.yx = 0;
    _matrix.yy = 0;
    
    FT_Init_FreeType( &_ft );

	if( FTC_Manager_New( _ft, 0, 0, 0, &DrawText::fontRequest, 0, &_manager ) )
		throw std::runtime_error( "FTC_Manager_New failed" + PT_SOURCEINFO );

	FTC_ImageCache_New( _manager, &_imageChace );

    FTC_CMapCache_New( _manager, &_charMapCache );

    FTC_SBitCache_New( _manager, &_bitmapCache );
}


DrawText::~DrawText()
{
    FT_Done_FreeType( _ft );
}

FT_Error DrawText::fontRequest( FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face* aface )
{
//    FT_Error error = FT_New_Memory_Face( library, vera, veraSize, 0, aface );

     FT_Error error  = FT_New_Face( library, "c:\\WINDOWS\\fonts\\tahoma.ttf", 0, aface );
     FT_Attach_File( *aface, "c:\\WINDOWS\\fonts\\tahoma" );
     
/*    if( FT_Select_Charmap( _face, FT_ENCODING_UNICODE ) )
        FT_Select_Charmap( _face, FT_ENCODING_NONE );  */   

	return error;
}

void DrawText::setFont( const Font& font )
{
    // Devide size by 64 because free type works with 1/64 pixel.
	_fontSize = font.size();
	_fontAngle = font.angle();

    //Setup the rotation matrix
    double angle = ( _fontAngle / 36000.0 ) * 3.14159 * 2 ;

    _matrix.xx = (FT_Fixed) ( cos( angle ) * 0x10000L );
    _matrix.xy = (FT_Fixed) ( -sin( angle )* 0x10000L );
    _matrix.yx = (FT_Fixed) ( sin( angle ) * 0x10000L );
    _matrix.yy = (FT_Fixed) ( cos( angle ) * 0x10000L );
}

FontMetrics DrawText::fontMetrics( const Text::String& text )
{
	FT_Face face;
	FTC_Manager_LookupFace( _manager, 0, &face );

    FT_UInt         previous    = 0;
    FT_Vector       delta;
    FT_Glyph        glyph;
    FT_BBox         gbbox = {0,0,0,0};
    FT_BBox         tbbox = {100000,100000,-100000,-100000};
    FTC_Node        node;
    FT_UInt			glyph_index;

    FTC_ImageTypeRec	imageType;
    imageType.face_id	= 0;
    imageType.width		= _fontSize;
    imageType.height	= _fontSize;
    imageType.flags		= FT_RENDER_MODE_NORMAL;

    int pen_x = 0;
    int pen_y = 0;

    for( Text::String::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        glyph_index = FTC_CMapCache_Lookup(  _charMapCache,  0, 0,  it->value() );
        
        if( !glyph_index )
            continue;

        if( FT_HAS_KERNING( face ) && previous )
        {
            FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
            pen_x += (delta.x) >> 6;
            pen_y -= (delta.y) >> 6;
        }

        if(  FTC_ImageCache_Lookup( _imageChace, &imageType, glyph_index, &glyph, &node ) )
            continue;

        const int incX = ( glyph->advance.x >> 6 ) / 1000;
        const int incY = glyph->advance.y >> 6;

        FT_Glyph_Get_CBox( glyph, FT_GLYPH_BBOX_PIXELS, &gbbox );

        gbbox.xMin += pen_x;
        gbbox.xMax += pen_x;

        tbbox.xMin = std::min( gbbox.xMin, tbbox.xMin );
        tbbox.xMax = std::max( gbbox.xMax, tbbox.xMax );

        pen_x += incX;
        pen_y -= incY;

        previous = glyph_index;
    }

    return FontMetrics(face->size->metrics.ascender>>6, (-face->size->metrics.descender)>>6, tbbox.xMax - tbbox.xMin, face->size->metrics.height>>6 );
}

void DrawText::draw( ARgbImage& image, const ARgbColor& color, const Math::Point& pos, const Text::String& text, const ARgbColor* backGround )
{
    FT_Vector			glyphPos;
    FT_Vector			delta;
    FT_UInt				previous = 0;
    FT_Glyph			glyph;
    FT_Glyph			glyphCopy;
    FTC_Node			node;
    FT_Face				face;
	FTC_SBit			smalGlyphBitmap;
	FT_BitmapGlyph		glyphBitmap;
    FTC_ImageTypeRec	imageType = { 0, _fontSize, _fontSize, FT_RENDER_MODE_NORMAL };	
	FT_UInt				glyph_index;
	int					incX; 
	int					incY;
	int					left;
	int					top;
    int					pitch;
	int					height;
	int					width;
	unsigned char*		buffer;		
         
    FTC_Manager_LookupFace( _manager, 0, &face );
	
    glyphPos.x = pos.x();
    glyphPos.y = pos.y();

    for( Text::String::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        glyph_index = FTC_CMapCache_Lookup(  _charMapCache,  0, 0,  it->value() );
        
        if( !glyph_index )
            continue;

        if( FT_HAS_KERNING( face ) && previous )
        {
            FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
            glyphPos.x += ( delta.x >> 6);
            glyphPos.y -= ( delta.y >> 6);
        }
		
        if( false  == it->isSpace() )
        {
			if( _fontAngle == 0 )
			{
				if( FTC_SBitCache_Lookup( _bitmapCache, &imageType, glyph_index, &smalGlyphBitmap, &node ) )
					continue;
					
				incX		= smalGlyphBitmap->xadvance;
				incY		= smalGlyphBitmap->yadvance;					
	            left		= glyphPos.x + smalGlyphBitmap->left;
		        top			= glyphPos.y - smalGlyphBitmap->top;
		        pitch		= smalGlyphBitmap->pitch;
		        height		= smalGlyphBitmap->height;
		        width		= smalGlyphBitmap->width;
		        buffer		= smalGlyphBitmap->buffer;
			}
			else
			{
		        FTC_ImageCache_Lookup( _imageChace, &imageType, glyph_index, &glyph, &node ) ;
		        
				FT_Glyph_Copy( glyph, &glyphCopy );
				FT_Glyph_Transform( glyphCopy, &_matrix, 0 );
				FT_Glyph_To_Bitmap( &glyphCopy, FT_RENDER_MODE_NORMAL,  0, 1 );
				
				glyphBitmap = (FT_BitmapGlyph) glyphCopy;     				
				incX		= ( glyphCopy->advance.x >> 6 ) / 1024;
				incY		= ( glyphCopy->advance.y >> 6 ) / 1024;
	            left		= glyphPos.x + glyphBitmap->left;
		        top			= glyphPos.y - glyphBitmap->top;
		        pitch		= glyphBitmap->bitmap.pitch;
		        height		= glyphBitmap->bitmap.rows;
		        width		= glyphBitmap->bitmap.width;
		        buffer		= glyphBitmap->bitmap.buffer;				
			}		
					
            if( backGround )
            {
                const int leftUp    = left + 1;
                const int leftDown  = left - 1;
                const int topUp     = top + 1;
                const int topDown   = top - 1;

				drawGlyph( image, *backGround, leftUp, top, pitch, height, width, buffer );
                drawGlyph( image, *backGround, leftUp, top, pitch, height, width, buffer );
                drawGlyph( image, *backGround, leftDown, top, pitch, height, width, buffer );
                drawGlyph( image, *backGround, leftDown, topUp, pitch, height, width, buffer );
                drawGlyph( image, *backGround, leftDown, topDown, pitch, height, width, buffer );
                drawGlyph( image, *backGround, leftUp, topDown, pitch, height, width, buffer );
                drawGlyph( image, *backGround, leftUp, topUp, pitch, height, width, buffer );
                drawGlyph( image, *backGround, leftDown, topDown, pitch, height, width, buffer );
                drawGlyph( image, *backGround, leftDown, topUp, pitch, height, width, buffer );
            }

            drawGlyph( image, color, left, top, pitch, height, width, buffer );
        }

        glyphPos.x   += incX;
        glyphPos.y   -= incY;
        previous     = glyph_index;
    }
}

} //namespace Pt

} //namespace Gfx
