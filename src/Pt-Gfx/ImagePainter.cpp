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
#include "freetype/include/ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "Vera.h"
#include "Pt/Gfx/ImagePainter.h"
#include "Pt/Gfx/ARgbImage.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Math/Point.h"
#include "Pt/Gfx/Brush.h"
#include "Pt/Gfx/Font.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/Text/String.h"

#include "DrawThinLine.h"
#include "DrawThickLine.h"
#include "DrawPolyline.h"
#include "DrawThinPolyline.h"
#include "DrawThickPolyline.h"
#include "FillConvexPolygon.h"
#include "DrawText.h"
#include <cmath>
#include <Pt/System/Clock.h>

namespace Pt{
namespace Gfx{


ImagePainter::ImagePainter( ARgbImage& image )
: _image( image )
, _drawLine( 0 )
, _drawThinLine( new DrawThinLine() )
, _drawThickLine( new DrawThickLine() )
, _drawPolyline( 0 )
, _drawThinPolyline( new DrawThinPolyline() )
, _drawThickPolyline( new DrawThickPolyline() )
, _fillConvexPolygon( new FillConvexPolygon() )
, _drawText( new DrawText() )
{
     _drawLine      = _drawThinLine.get();
     _drawPolyline  = _drawThinPolyline.get();
}

ImagePainter::~ImagePainter()
{ }

void ImagePainter::setPen( const Pen& pen )
{
    _pen = pen;

    if( _pen.size() == 1 )
    {
        _drawLine       = _drawThinLine.get();
        _drawPolyline   = _drawThinPolyline.get();
    }
    else
    {
        _drawLine       = _drawThickLine.get();
        _drawPolyline   = _drawThickPolyline.get();
    }
}


const Pen& ImagePainter::pen() const
{
    return _pen;
}

void ImagePainter::setBrush(const Brush& brush)
{
    _brush = brush;
}

const Brush& ImagePainter::brush() const
{
    return _brush;
}

void ImagePainter::setFont(const Font& font)
{
    _drawText->setFont(font);

}
const Font& ImagePainter::font() const
{
    static const Font font( "", 10 );
    return font;
}

FontMetrics ImagePainter::fontMetrics() const
{
    static const FontMetrics metrics(1,1,1,1);
    return metrics;
}

FontMetrics ImagePainter::fontMetrics( Text::String text) const
{
    return fontMetrics();
}

const std::list<std::string>& ImagePainter::fontFamilyNames()
{
    static const std::list<std::string> empty;
    return empty;
}

void ImagePainter::drawPixel(const  Math::Point& to)
{
}

void ImagePainter::drawLine(const Math::Point& from, const  Math::Point& to)
{
    if( _pen.size()  == 0 )
        return;

    _drawLine->draw( _image, _pen, from, to );
}

void ImagePainter::drawText(const  Math::Point& to, const Text::String& text)
{
    _drawText->draw( _image, _pen,  to, text, ARgbColor(0xffff,0xffff,0xffff) );
    
/*
    FT_Library  ft;
    FT_Face     face;
    int size = 28;


    FT_Init_FreeType( &ft );

    if( FT_New_Memory_Face(ft, vera, veraSize, 0, &face) )
        throw IllegalArgument( "FT_New_Face  error", PT_SOURCEINFO );
  
// TODO: Implment font locator
/*
#ifdef _WIN32_WCE
    if( FT_New_Face( ft, "\\WINDOWS\\tahoma.ttf", 0, &face ) )
        throw IllegalArgument( "FT_New_Face  error", PT_SOURCEINFO );

    FT_Attach_File( face, "\\WINDOWS\\tahoma" );
#elif defined(WIN32)
    if( FT_New_Face( ft, "c:\\WINDOWS\\fonts\\comic.ttf", 0, &face ) )
        throw IllegalArgument( "FT_New_Face  error", PT_SOURCEINFO );

    FT_Attach_File( face, "c:\\WINDOWS\\fonts\\comic" );
#else
    if( FT_New_Face( ft, "/usr/share/fonts/truetype/freefont/FreeSans.ttf", 0, &face ) )
        if( FT_New_Face( ft, "/usr/share/fonts/bitstream-vera/Vera.ttf", 0, &face ) )
            throw IllegalArgument( "FT_New_Face  error", PT_SOURCEINFO );

<<<<<<< .mine
    FT_Attach_File( face, "/usr/share/fonts/truetype/freefont/FreeSans" );
=======
    //FT_Attach_File( face, "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans" );
>>>>>>> .r427
#endif
*/
/*
    if( FT_Select_Charmap( face, FT_ENCODING_UNICODE ) )
        FT_Select_Charmap( face, FT_ENCODING_NONE );
<<<<<<< .mine
            
    FT_Set_Char_Size( face, size<< 6, size << 6, 72, 72 );
        
=======

    FT_Set_Char_Size( face, size<<6, size<< 6, 72, 72 );

>>>>>>> .r427
    FT_GlyphSlot slot     = face->glyph;
    FT_UInt      previous = 0;
    size_t border = 1;


    FT_Matrix matrix;
    FT_Vector pen;
    double angle = (-45.0 /360) * 3.14159 * 2 ;
    matrix.xx = (FT_Fixed) ( cos( angle )* 0x10000L );
    matrix.xy = (FT_Fixed) ( -sin( angle )* 0x10000L );
    matrix.yx = (FT_Fixed) ( sin( angle )* 0x10000L );
    matrix.yy = (FT_Fixed) ( cos( angle )* 0x10000L );   
 
    
    pen.x = to.x();
    pen.y = to.y();
    Pt::System::Clock   clock;
    FT_Set_Transform( face, &matrix, &pen );    
    
    clock.start();
    
    for( Text::String::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        {
            _pen = Pen(1,ARgbColor(0xffff,0xffff,0xffff));
            FT_UInt glyph_index = FT_Get_Char_Index( face, it->value() );

            if( !glyph_index )
                continue;

            if( FT_HAS_KERNING( face ) && previous )
            {
                FT_Vector delta;
                
                FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );       
                pen.x += delta.x >> 6;    
                pen.y -= delta.y >> 6;
            }

            if( FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER ) )
                continue;

            int incX = slot->advance.x >> 6;
            int incY = slot->advance.y >> 6;
                
            FT_Bitmap& bm = slot->bitmap;        
            
            drawGlyph(pen.x + slot->bitmap_left, pen.y - slot->bitmap_top, bm.buffer, bm.width, bm.rows, bm.pitch);               
            
            drawGlyph(pen.x + slot->bitmap_left + border, pen.y - slot->bitmap_top, bm.buffer, bm.width, bm.rows, bm.pitch);                
            drawGlyph(pen.x + slot->bitmap_left - border, pen.y - slot->bitmap_top, bm.buffer, bm.width, bm.rows, bm.pitch);                
            
            drawGlyph(pen.x + slot->bitmap_left , pen.y - slot->bitmap_top + border, bm.buffer, bm.width, bm.rows, bm.pitch);                
            drawGlyph(pen.x + slot->bitmap_left , pen.y - slot->bitmap_top - border, bm.buffer, bm.width, bm.rows, bm.pitch);                
            
            drawGlyph(pen.x + slot->bitmap_left + border , pen.y - slot->bitmap_top - border, bm.buffer, bm.width, bm.rows, bm.pitch);                
            drawGlyph(pen.x + slot->bitmap_left + border, pen.y - slot->bitmap_top + border, bm.buffer, bm.width, bm.rows, bm.pitch);                
            
            drawGlyph(pen.x + slot->bitmap_left  - border, pen.y - slot->bitmap_top - border, bm.buffer, bm.width, bm.rows, bm.pitch);                
            drawGlyph(pen.x + slot->bitmap_left  - border, pen.y - slot->bitmap_top + border, bm.buffer, bm.width, bm.rows, bm.pitch);
        }


        {
            _pen = Pen(1,ARgbColor(0,0,0));

            FT_Set_Char_Size( face, size<< 6, size << 6, 72, 72 );

            FT_UInt glyph_index = FT_Get_Char_Index( face, it->value() );

            if( !glyph_index )
                continue;

            if( FT_HAS_KERNING( face ) && previous )
            {
                FT_Vector delta;
                
                FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );       
                pen.x += delta.x >> 6;    
                pen.y -= delta.y >> 6;
            }

            if( FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER ) )
                continue;

            int incX = slot->advance.x >> 6;
            int incY = slot->advance.y >> 6;
                
            FT_Bitmap& bm = slot->bitmap;        
            
            drawGlyph(pen.x + slot->bitmap_left, pen.y - slot->bitmap_top , bm.buffer, bm.width, bm.rows, bm.pitch);            
            
            pen.x += incX;
            pen.y -= incY;
            previous = glyph_index;

        }


    }

    Pt::System::TimeValue time = clock.stop();        
    
    std::cerr<< "Font Time: " << (double) time.seconds() + time.microSeconds() / 1000000.0<< std::endl;
    FT_Done_Face( face );
    FT_Done_FreeType( ft );*/
}

void ImagePainter::drawGlyph( int penX, int penY, const Pt::uint8_t* bitmap, Pt::uint32_t bmWidth, Pt::uint32_t bmHeight, Pt::uint32_t bmPitch )
{
    //_image.pixel(

    Pt::ssize_t x1 = 0;
    Pt::ssize_t x2 = _image.width() - 1;
    Pt::ssize_t y1 = 0;
    Pt::ssize_t y2 = _image.height() - 1;

    if( bmPitch < bmWidth)
        bmPitch += bmWidth;

    int dsy = penY;

    for( Pt::uint32_t y = 0; y < bmHeight; y++, dsy++ )
    {
        if( dsy < y1 )
            continue;

        if( dsy > y2 )
            break;

        Pt::uint32_t yOffset = y * bmPitch;

        int dsx = penX;

        for( Pt::uint32_t x = 0; x < bmWidth; x++, dsx++ )
        {
            if( dsx < x1 )
                continue;

            if( dsx > x2 )
                break;

            Pt::uint8_t col = bitmap[ yOffset + x ];

            if( !col )
                continue;

            float af = col / 255.0f;

           // _image.pixel(dsx, dsy)  = _pen.color();
            mixColor( _image.pixel(dsx, dsy), _pen.color(), af );
        }
    }
}


void ImagePainter::drawRect(const  Math::Rect& rect)
{

}
void ImagePainter::fillRect(const  Math::Rect& rect)
{
}

void ImagePainter::drawEllipse(const  Math::Point& topLeft, const  Math::Size& size)
{
}

void ImagePainter::fillEllipse(const  Math::Point& topLeft, const  Math::Size& size)
{
}

void ImagePainter::drawPolyline(const  Math::Point* points, const size_t pointCount)
{
    if( _pen.size()  == 0 )
        return;

    for( size_t i = 1; i < pointCount; ++i)
        _drawLine->draw( _image, _pen, points[ i - 1], points[i] );

/*
    std::vector<Math::Point> p( pointCount );
    memcpy( &p[0], points, sizeof( Math::Point) * pointCount );

    _drawPolyline->draw(_image, _pen, p );
    */
}

void ImagePainter::fillPolygon(const  Math::Point* points, const size_t pointCount)
{
    std::vector<Math::Point> p( pointCount );
    memcpy( &p[0], points, sizeof( Math::Point) * pointCount );

    _fillConvexPolygon->draw( _image, _brush, p );
}

void ImagePainter::drawImage(const  Math::Point& to, const ARgbImage& image)
{
}

void ImagePainter::drawImage(const  Math::Point& to, const ARgbImage& image, const Region& imageRegion)
{
}

}
}
