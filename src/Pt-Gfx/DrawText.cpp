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
#include "Pt/Gfx/Exception.h"
#include "Pt/Gfx/Font.h"
#include "Vera.h"
#include <Pt/System/Clock.h>
#include "Pt/Unicode.h"

namespace Pt {
namespace Gfx {

DrawText::DrawText()
: _fontAngle( 0 )
, _faceId( 0 )
, _charMapId( 0 )
{
    _matrix.xx = 0;
    _matrix.xy = 0;
    _matrix.yx = 0;
    _matrix.yy = 0;

    if( FTC_Manager_New(  FreeType::instance().library(), 0, 0, 0, &DrawText::fontRequest, (FT_Pointer) this, &_manager ) )
        throw std::runtime_error( "FTC_Manager_New failed" + PT_SOURCEINFO );

    if( FTC_ImageCache_New( _manager, &_imageChace ) )
        throw std::runtime_error( "FTC_ImageCache_New failed" + PT_SOURCEINFO );

    if( FTC_CMapCache_New( _manager, &_charMapCache ) )
        throw std::runtime_error( "FTC_CMapCache_New failed" + PT_SOURCEINFO );

    if( FTC_SBitCache_New( _manager, &_bitmapCache ) )
        throw std::runtime_error( "FTC_SBitCache_New failed" + PT_SOURCEINFO );
}

DrawText::~DrawText()
{
    FTC_Manager_Done( _manager );
}

FT_Error DrawText::fontRequest( FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face* aface )
{
    return  FT_New_Memory_Face( library, vera, veraSize, 0, aface );

    /* FT_Error error  = FT_New_Face( library, "c:\\WINDOWS\\fonts\\tahoma.ttf", 0, aface );
    FT_Attach_File( *aface, "c:\\WINDOWS\\fonts\\tahoma" );
     return error;*/
}

void DrawText::setFont( const Font& font )
{
    //Setup the image type.
    _imageType.face_id    = faceId();
    _imageType.width    = font.size();
    _imageType.height    = font.size();
    _imageType.flags    = FT_RENDER_MODE_NORMAL;

    //Setup the rotation matrix.
    _fontAngle = font.angle() % 3600;

    if ( _fontAngle < 0 )
        _fontAngle += 3600;

    const double angle     = ( _fontAngle / 10.0  *  3.14159) / 180.0 ;
    const double cosinus = cos( angle ) * 0x10000L;
    const double sinus   = sin( angle ) * 0x10000L;

    _matrix.xx = (FT_Fixed) ceil( cosinus );
    _matrix.xy = (FT_Fixed) ceil( -sinus );
    _matrix.yx = (FT_Fixed) ceil( sinus );
    _matrix.yy = (FT_Fixed) ceil( cosinus );

    //Search the unicode charmap.
    FT_Face    face;

    FTC_Manager_LookupFace( _manager, faceId(), &face );

    bool charMapFound = false;

    for( int i = 0; i < face->num_charmaps; ++i )
    {
        if( face->charmap[i].encoding == FT_ENCODING_UNICODE )
        {
        _charMapId = face->charmap[i].encoding_id;
        charMapFound = true;
        break;
        }
    }

    if( !charMapFound )
        throw InvalidFont( "No unicode charmap found" + PT_SOURCEINFO );
}

FontMetrics DrawText::fontMetrics( const String& text )
{
    FT_UInt                previous    = 0;
    FT_Vector            delta;
    FT_Glyph            glyph;
    FT_BBox                gbbox = { 0 ,0 , 0, 0 };
    FT_BBox                tbbox = { std::numeric_limits<FT_Pos>::max(), std::numeric_limits<FT_Pos>::max(),
                            std::numeric_limits<FT_Pos>::min(), std::numeric_limits<FT_Pos>::min() };
    FTC_Node            node;
    FT_UInt                glyph_index;

    //
    // Lookup global data of the face be getting a Size struct from the 
    // manager.
    //
    FT_Size size;
    FTC_ScalerRec_ scaler;
    scaler.face_id = _imageType.face_id;
    scaler.width = _imageType.width;
    scaler.height = _imageType.height;
    scaler.pixel = 1; // 1 means TRUE and scaler.x_res and scaler.y_res are ignored
    FTC_Manager_LookupSize( _manager, &scaler, &size );

    FT_Face face;
    FTC_Manager_LookupFace( _manager, faceId(), &face );
    int pen_x = 0;
    int pen_y = 0;

    for( String::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        glyph_index = FTC_CMapCache_Lookup(  _charMapCache,  faceId(), _charMapId,  it->value() );

        if( !glyph_index )
            continue;

        if( FT_HAS_KERNING( face ) && previous )
        {
            FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
            pen_x += delta.x; // << 16;
            pen_y -= delta.y; // << 16;
        }

        if(  FTC_ImageCache_Lookup( _imageChace, &_imageType, glyph_index, &glyph, &node ) )
            continue;

        FT_Glyph_Get_CBox( glyph, FT_GLYPH_BBOX_PIXELS, &gbbox );

        gbbox.xMin += ( pen_x >> 16 );
        gbbox.xMax += ( pen_x >> 16 );

        tbbox.xMin = std::min( gbbox.xMin, tbbox.xMin );
        tbbox.xMax = std::max( gbbox.xMax, tbbox.xMax );

        pen_x += glyph->advance.x;
        pen_y -= glyph->advance.y;

        previous = glyph_index;
    }

    return FontMetrics(size->metrics.ascender >> 6, (-size->metrics.descender) >> 6, 
                       tbbox.xMax - tbbox.xMin, size->metrics.height >> 6 );
}


void DrawText::draw( ARgbImage& image, const ARgbColor& color, const Math::Point& pos, const String& text, const ARgbColor* backGround )
{
    FT_Vector            glyphPos;
    FT_Vector            delta;
    FT_UInt                previous = 0;
    FT_Glyph            glyph;
    FT_Glyph            glyphCopy;
    FTC_Node            node;
    FT_Face                face;
    FTC_SBit            smalGlyphBitmap;
    FT_BitmapGlyph        glyphBitmap;
    FT_UInt                glyph_index;

    //Glyph bitmap description
    int                    incX;
    int                    incY;
    int                    left;
    int                    top;
    int                    pitch;
    int                    height;
    int                    width;
    unsigned char*        buffer;

    FTC_Manager_LookupFace( _manager, faceId(), &face );

    glyphPos.x = pos.x() << 16;
    glyphPos.y = pos.y() << 16;

    for( String::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        glyph_index = FTC_CMapCache_Lookup(  _charMapCache,  faceId(), _charMapId , it->value());

        if( !glyph_index )
            continue;

        if( FT_HAS_KERNING( face ) && previous )
        {
            FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );

            glyphPos.x += delta.x;
            glyphPos.y -= delta.y;
        }

        if( _fontAngle == 0 )
        {
            if( FTC_SBitCache_Lookup( _bitmapCache, &_imageType, glyph_index, &smalGlyphBitmap, &node ) )
                continue;

            incX        = smalGlyphBitmap->xadvance << 16;
            incY        = smalGlyphBitmap->yadvance << 16;

            left        = (glyphPos.x >> 16) + smalGlyphBitmap->left;
            top            = (glyphPos.y >> 16) - smalGlyphBitmap->top;
            pitch        = smalGlyphBitmap->pitch;
            height        = smalGlyphBitmap->height;
            width        = smalGlyphBitmap->width;
            buffer        = smalGlyphBitmap->buffer;
        }
        else
        {
            FTC_ImageCache_Lookup( _imageChace, &_imageType, glyph_index, &glyph, &node ) ;

            FT_Glyph_Copy( glyph, &glyphCopy );
            FT_Glyph_Transform( glyphCopy, &_matrix, 0);
            FT_Glyph_To_Bitmap( &glyphCopy, FT_RENDER_MODE_NORMAL,  0, 1 );

            glyphBitmap = (FT_BitmapGlyph) glyphCopy;

            incX        = glyphCopy->advance.x;
            incY        = glyphCopy->advance.y;

            left        = (glyphPos.x >> 16) + glyphBitmap->left;
            top            = (glyphPos.y >> 16) - glyphBitmap->top;
            pitch        = glyphBitmap->bitmap.pitch;
            height        = glyphBitmap->bitmap.rows;
            width        = glyphBitmap->bitmap.width;
            buffer        = glyphBitmap->bitmap.buffer;
        }

        if( false == Pt::Unicode::isSpace(*it) )
        {

//             FT_Glyph_Get_CBox( image, ft_glyph_bbox_pixels, &bbox );
//
//            if ( bbox.xMax <= 0 || bbox.xMin >= my_target_width  || bbox.yMax <= 0 || bbox.yMin >= my_target_height )
//                continue;

            if( backGround )
            {
                const int leftUp    = left + 1;
                const int leftDown  = left - 1;
                const int topUp     = top  + 1;
                const int topDown   = top  - 1;

                drawGlyph( image, *backGround, leftDown, topDown, pitch, height, width, buffer );
                drawGlyph( image, *backGround, left, topDown, pitch, height, width, buffer );
                drawGlyph( image, *backGround, leftUp, topDown, pitch, height, width, buffer );
                drawGlyph( image, *backGround, leftDown, top, pitch, height, width, buffer );
                drawGlyph( image, *backGround, leftUp, top, pitch, height, width, buffer );
                drawGlyph( image, *backGround, leftDown, topUp, pitch, height, width, buffer );
                drawGlyph( image, *backGround, left, topUp, pitch, height, width, buffer );
                drawGlyph( image, *backGround, leftUp, topUp, pitch, height, width, buffer );
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

