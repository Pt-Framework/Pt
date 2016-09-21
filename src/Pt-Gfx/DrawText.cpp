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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#include "DrawText.h"
#include <Pt/Gfx/Font.h>
#include <Pt/String.h>
#include <algorithm>
#include <cmath>

namespace Pt {

namespace Gfx {

DrawText::DrawText()
: _charMapIndex(0)
, _faceId(0)
, _face(0)
, _fontAngle(0)
// TODO: handle _clip.isNull() like no clipping
, _clip( Point(0, 0), Size(999999, 999999) ) 
{
    _matrix.xx = 0;
    _matrix.xy = 0;
    _matrix.yx = 0;
    _matrix.yy = 0;

    //if( FTC_Manager_New( FreeType::instance().library(), 0, 0, 0, &FreeType::fontRequest, 0, &_manager ) )
    //    throw std::runtime_error( "FTC_Manager_New failed" + PT_SOURCEINFO );

    //if( FTC_ImageCache_New( _manager, &_imageCache ) )
    //    throw std::runtime_error( "FTC_ImageCache_New failed" + PT_SOURCEINFO );

    //if( FTC_CMapCache_New( _manager, &_charMapCache ) )
    //    throw std::runtime_error( "FTC_CMapCache_New failed" + PT_SOURCEINFO );

    //if( FTC_SBitCache_New( _manager, &_bitmapCache ) )
    //    throw std::runtime_error( "FTC_SBitCache_New failed" + PT_SOURCEINFO );
}


DrawText::~DrawText()
{
    //FTC_Manager_Done( _manager );
}


void DrawText::setFont(const Font& font)
{
    _faceId = FreeType::instance().findFaceId(font);

    // setup the image type
    _imageType.face_id = _faceId;
    _imageType.width   = font.size();
    _imageType.height  = font.size();
    _imageType.flags   = FT_RENDER_MODE_NORMAL;

    // setup the rotation matrix
    _fontAngle = font.angle() % 3600;

    if ( _fontAngle < 0 )
        _fontAngle += 3600;

    const double angle   = ( _fontAngle / 10.0  *  3.14159) / 180.0 ;
    const double cosinus = std::cos( angle ) * 0x10000L;
    const double sinus   = std::sin( angle ) * 0x10000L;

    _matrix.xx = (FT_Fixed) std::ceil( cosinus );
    _matrix.xy = (FT_Fixed) std::ceil( -sinus );
    _matrix.yx = (FT_Fixed) std::ceil( sinus );
    _matrix.yy = (FT_Fixed) std::ceil( cosinus );

    //FTC_Manager_LookupFace( _manager, _faceId, &_face );
    FreeType::instance().findFace(_faceId, &_face);

    for( int index = 0; index < _face->num_charmaps; ++index )
    {
        if( _face->charmap[index].encoding == FT_ENCODING_UNICODE )
        {
            _charMapIndex = index;
            return;
        }
    }

    throw std::invalid_argument("No fonts installed" );
}


FontMetrics DrawText::fontMetrics( const String& text )
{
    FT_UInt             previous    = 0;
    FT_Vector           delta;
    FT_Glyph            glyph;
    FT_BBox             gbbox = { 0 ,0 , 0, 0 };
    FT_BBox             tbbox = { std::numeric_limits<FT_Pos>::max(), std::numeric_limits<FT_Pos>::max(),
                        std::numeric_limits<FT_Pos>::min(), std::numeric_limits<FT_Pos>::min() };
    FTC_Node            node;
    FT_UInt             glyph_index;

    FT_Size size;
    FTC_ScalerRec scaler;
    scaler.face_id = _imageType.face_id;
    scaler.width = _imageType.width;
    scaler.height = _imageType.height;
    scaler.pixel = 1; // 1 means TRUE and scaler.x_res and scaler.y_res are ignored
    
    //FTC_Manager_LookupSize( _manager, &scaler, &size );
    FreeType::instance().findSize(&scaler, &size);

    FT_Face face = _face;
    //FTC_Manager_LookupFace(_manager, _faceId, &face);

    int pen_x = 0;
    int pen_y = 0;

    for( String::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        //glyph_index = FTC_CMapCache_Lookup( _charMapCache, _faceId, _charMapIndex, it->value() );
        glyph_index = FreeType::instance().findCharMap(_faceId, _charMapIndex, it->value() );

        if( ! glyph_index )
            continue;

        if( FT_HAS_KERNING( face ) && previous )
        {
            FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
            pen_x += delta.x; // << 16;
            pen_y -= delta.y; // << 16;
        }

        //if( FTC_ImageCache_Lookup(_imageCache, &_imageType, glyph_index, &glyph, &node) )
        //    continue;

        if( FreeType::instance().findImage(&_imageType, glyph_index, &glyph, &node) )
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


void DrawText::draw( Image& image, const Color& color, const Point& pos, const String& text )
{
    FT_Vector      glyphPos;
    FT_Vector      delta;
    FT_UInt        previous = 0;
    FT_Glyph       glyph;
    FT_Glyph       glyphCopy = 0;
    FTC_Node       node;
    FT_Face        face = _face;
    FTC_SBit       smalGlyphBitmap;
    FT_BitmapGlyph glyphBitmap;
    FT_UInt        glyph_index;

    //Glyph bitmap description
    int            incX;
    int            incY;
    int            left;
    int            top;
    int            pitch;
    int            height;
    int            width;
    unsigned char* buffer;
   
    //FTC_Manager_LookupFace( _manager, _faceId, &face );

    glyphPos.x = (int) pos.x() << 16;
    glyphPos.y = (int) pos.y() << 16;

    for( String::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        //glyph_index = FTC_CMapCache_Lookup( _charMapCache, _faceId, _charMapIndex, it->value() );
        glyph_index = FreeType::instance().findCharMap(_faceId, _charMapIndex, it->value() );

        if( ! glyph_index )
            continue;

        if( FT_HAS_KERNING(face) && previous )
        {
            FT_Get_Kerning(face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);

            glyphPos.x += delta.x;
            glyphPos.y -= delta.y;
        }

        if( _fontAngle == 0 )
        {
            //if( FTC_SBitCache_Lookup( _bitmapCache, &_imageType, glyph_index, &smalGlyphBitmap, &node ) )
            //    continue;

            if( FreeType::instance().findBitmap(&_imageType, glyph_index, &smalGlyphBitmap, &node ) )
                continue;

            incX        = smalGlyphBitmap->xadvance << 16;
            incY        = smalGlyphBitmap->yadvance << 16;

            left        = (glyphPos.x >> 16) + smalGlyphBitmap->left;
            top         = (glyphPos.y >> 16) - smalGlyphBitmap->top;
            pitch       = smalGlyphBitmap->pitch;
            height      = smalGlyphBitmap->height;
            width       = smalGlyphBitmap->width;
            buffer      = smalGlyphBitmap->buffer;
        }
        else
        {
            //FTC_ImageCache_Lookup(_imageCache, &_imageType, glyph_index, &glyph, &node);
            FreeType::instance().findImage(&_imageType, glyph_index, &glyph, &node);

            FT_Glyph_Copy( glyph, &glyphCopy );
            FT_Glyph_Transform( glyphCopy, &_matrix, 0);
            FT_Glyph_To_Bitmap( &glyphCopy, FT_RENDER_MODE_NORMAL,  0, 1 );

            glyphBitmap = (FT_BitmapGlyph) glyphCopy;

            incX        = glyphCopy->advance.x;
            incY        = glyphCopy->advance.y;

            left        = (glyphPos.x >> 16) + glyphBitmap->left;
            top         = (glyphPos.y >> 16) - glyphBitmap->top;
            pitch       = glyphBitmap->bitmap.pitch;
            height      = glyphBitmap->bitmap.rows;
            width       = glyphBitmap->bitmap.width;
            buffer      = glyphBitmap->bitmap.buffer;
        }

        if( ! isspace(*it) )
        {
            //FT_Glyph_Get_CBox(image, ft_glyph_bbox_pixels, &bbox );

            //if ( bbox.xMax <= 0 || bbox.xMin >= my_target_width  || 
            //     bbox.yMax <= 0 || bbox.yMin >= my_target_height )
            //    continue;

            drawGlyph(image, color, left, top, pitch, height, width, buffer);
        }

        glyphPos.x  += incX;
        glyphPos.y  -= incY;
        previous    = glyph_index;

        if(glyphCopy)
        {
            FT_Done_Glyph( glyphCopy );
            glyphCopy = 0;
        }
    }
}


void DrawText::drawGlyph(Image& image, const Color& color, int xpos, int ypos,
                         int bmPitch, int height, int width, const unsigned char* buffer)
{
    const int clipRight  = _clip.x() + _clip.width();
    const int clipBottom = _clip.y() + _clip.height();
    Pt::ssize_t yOffset  = 0;
    Pt::ssize_t dsy      = 0;
    Pt::ssize_t dsx      = 0;
    const Pt::ssize_t x2 = clipRight;
    const Pt::ssize_t y2 = clipBottom;

    if( bmPitch < width )
        bmPitch += width;

    int ofsx = 0;
            
    if(xpos < _clip.x() ) 
    {
        ofsx = _clip.x()  - xpos;
        xpos =  _clip.x();
    }
            
    int ofsy = 0;
            
    if(ypos < _clip.y()) 
    {
        ofsy = _clip.y() - ypos;
        ypos = _clip.y();
    }

    dsy = ypos;

    Color pixelColor = color;

    for( Pt::int32_t y = ofsy; y < height; ++y, ++dsy )
    {
        yOffset = y * bmPitch;

        if( dsy < _clip.y() )
            continue;

        if( dsy > y2 )
            break;

        dsx   = xpos;

        for( Pt::int32_t x = ofsx; x < width; ++x, ++dsx )
        {
            if( dsx < _clip.x() )
                continue;

            if( dsx > x2 )
                break;


            Pixel pixel(image.view(), dsx, dsy);

            const int px = yOffset + x;
            unsigned char value = buffer[px];
                    
            if(value != 255)
            {
                pixelColor.setAlpha(value * 257);
                image.format().setPixel(pixel, pixelColor,
                                        CompositionMode::SourceOver);
            }
            else
            {                    
                image.format().setPixel(pixel, color, CompositionMode::SourceCopy);
            }
        }
    }
}

} //namespace

} //namespace
