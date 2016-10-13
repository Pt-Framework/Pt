/* Copyright (C) 2015 Marc Boris Duerner 
  
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

#include "FreeType.h"
#include "DejaVuSans.h"
#include "DejaVuSansBold.h"
#include "DejaVuSansItalic.h"
#include "DejaVuSansBoldItalic.h"
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Image.h>
#include <Pt/System/Directory.h>
#include <Pt/System/FileInfo.h>
#include <Pt/System/IOError.h>
#include <stdexcept>
#include <iostream>

namespace Pt {

namespace Gfx {

FreeType::FreeType()
{   
    if( FT_Init_FreeType( &_ft ) )
        throw std::runtime_error("FT_Init_FreeType");

    if( FTC_Manager_New( _ft, 0, 0, 0, &FreeType::fontRequest, this, &_manager ) )
        throw std::runtime_error( "FTC_Manager_New" );

    if( FTC_ImageCache_New( _manager, &_imageCache ) )
        throw std::runtime_error( "FTC_ImageCache_New" );

    if( FTC_CMapCache_New( _manager, &_charMapCache ) )
        throw std::runtime_error( "FTC_CMapCache_New" );

    if( FTC_SBitCache_New( _manager, &_bitmapCache ) )
        throw std::runtime_error( "FTC_SBitCache_New" );

    setFontDir(System::Path( System::Path::curdir()) / "fonts");
}


FreeType::~FreeType()
{
    FTC_Manager_Done( _manager );
    FT_Done_FreeType( _ft );
}


std::string FreeType::defaultFont() const
{
    // LOCK
    return _defaultFont;
    // UNLOCK
}


void FreeType::setDefaultFont( const std::string& font )
{
    // LOCK
    _defaultFont = font;
    // UNLOCK
}


std::vector<std::string> FreeType::fontNames() const
{
    // LOCK

    std::vector<std::string> names;
    names.push_back("DejaVu Sans");
    
    Fonts::const_iterator it; 
    for(it = _fonts.begin(); it != _fonts.end(); ++it)
    {
        if(std::find(names.begin(), names.end(), it->first.name()) == names.end())
            names.push_back( it->first.name() );
    }
    
    // UNLOCK

    return names;
}


void FreeType::setFontDir(const System::Path& path)
{
    // LOCK

    _fontDir = path;

    _fonts.clear();
    _files.clear();
    
    try
    {
        System::DirectoryIterator it(_fontDir);
        System::DirectoryIterator end;

        for( ; it != end; ++it)
        {
            std::string pathName = it->path().toLocal();

            FT_Face face;
            FT_Error err = FT_New_Face(_ft, pathName.c_str(), 0, &face);
            if(err != 0)
                continue;

            Font::FontStyle style = Font::NormalStyle;

            if( (face->style_flags & FT_STYLE_FLAG_BOLD) == FT_STYLE_FLAG_BOLD )
                style = Font::BoldStyle;

            if( (face->style_flags & FT_STYLE_FLAG_ITALIC) == FT_STYLE_FLAG_ITALIC )
                style = Font::ItalicStyle;

            if( (face->style_flags & FT_STYLE_FLAG_BOLD) == FT_STYLE_FLAG_BOLD && 
                (face->style_flags & FT_STYLE_FLAG_ITALIC) == FT_STYLE_FLAG_ITALIC )
                style = Font::BoldItalicStyle;

            Font font(face->family_name, 12, style);

            System::Path& fontPath = _fonts[font];
            fontPath = it->path();

            _files.insert(&fontPath);

            FT_Done_Face(face);
        }
    }
    catch(const Pt::System::AccessFailed&)
    {
    }

    // UNLOCK
}


FTC_FaceID FreeType::findFaceId(const Font& font)
{
    // LOCK

    Fonts::iterator it = _fonts.find(font);
    if( it == _fonts.end() )
        return 0;

    System::Path* path = &it->second;
    return reinterpret_cast<FTC_FaceID>(path);

    // UNLOCK
}


FT_Error FreeType::fontRequest( FTC_FaceID faceId, FT_Library library, 
                                FT_Pointer data, FT_Face* face )
{
    FreeType* ft = static_cast<FreeType*>(data);
    return ft->onFontRequest(faceId, face);
}


FT_Error FreeType::onFontRequest(FTC_FaceID faceId, FT_Face* face)
{
    System::Path* path = reinterpret_cast<System::Path*>(faceId);

    if(faceId == 0)
        return FT_New_Memory_Face(_ft, DejaVuSans, DejaVuSansSize, 0, face);

    // check if path is still valid
    if( _files.find(path) == _files.end() )
        return FT_New_Memory_Face(_ft, DejaVuSans, DejaVuSansSize, 0, face);

    return FT_New_Face(_ft, path->toLocal().c_str(), 0, face);
}


FontMetrics FreeType::fontMetrics(const String& text, 
                                  FTC_FaceID faceId, FTC_ImageType imageType)
{
    // LOCK

    FT_Face face = 0;
    FTC_Manager_LookupFace(_manager, faceId, &face);

    FT_Int charMapIndex = 0;
    for(int n = 0; n < face->num_charmaps; ++n)
    {
        if(face->charmap[n].encoding == FT_ENCODING_UNICODE)
        {
            charMapIndex = n;
        }
    }

    FTC_ScalerRec scaler;
    scaler.face_id = imageType->face_id;
    scaler.width   = imageType->width;
    scaler.height  = imageType->height;
    scaler.pixel   = 1; // 1 means TRUE and scaler.x_res and scaler.y_res are ignored
    
    FT_Size size;
    FTC_Manager_LookupSize(_manager, &scaler, &size);

    int pen_x = 0;
    int pen_y = 0;
    FT_UInt   previous = 0;
    FT_Vector delta;
    FT_Glyph  glyph;
    FT_BBox   gbbox = { 0 , 0, 0, 0 };
    FT_BBox   tbbox = { std::numeric_limits<FT_Pos>::max(), 
                        std::numeric_limits<FT_Pos>::max(),
                        std::numeric_limits<FT_Pos>::min(), 
                        std::numeric_limits<FT_Pos>::min() };

    for( String::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        FTC_Node  node;
        FT_UInt glyph_index = FTC_CMapCache_Lookup(_charMapCache, faceId, charMapIndex, it->value());

        if( ! glyph_index )
            continue;

        if( FT_HAS_KERNING( face ) && previous )
        {
            FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
            pen_x += delta.x; // << 16;
            pen_y -= delta.y; // << 16;
        }

        if( FTC_ImageCache_Lookup(_imageCache, imageType, glyph_index, &glyph, &node) )
            continue;

        FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &gbbox);

        gbbox.xMin += ( pen_x >> 16 );
        gbbox.xMax += ( pen_x >> 16 );

        tbbox.xMin = std::min( gbbox.xMin, tbbox.xMin );
        tbbox.xMax = std::max( gbbox.xMax, tbbox.xMax );

        pen_x += glyph->advance.x;
        pen_y -= glyph->advance.y;

        previous = glyph_index;
    }

    return FontMetrics( size->metrics.ascender >> 6, 
                       (-size->metrics.descender) >> 6,
                       tbbox.xMax - tbbox.xMin, 
                       size->metrics.height >> 6 );

    // UNLOCK
}


void FreeType::draw(Image& image, const Color& color, Pt::ssize_t fontAngle,
                    const Point& pos, const String& text, const Rect& clip, 
                    FT_Matrix& matrix, FTC_FaceID faceId, FTC_ImageType imageType)
{
    // LOCK

    FT_Vector      glyphPos;
    FT_Vector      delta;
    FT_UInt        previous = 0;
    FT_Glyph       glyph;
    FT_Glyph       glyphCopy = 0;
    FTC_Node       node;
    FTC_SBit       smalGlyphBitmap;
    FT_BitmapGlyph glyphBitmap;

    //Glyph bitmap description
    int            incX;
    int            incY;
    int            left;
    int            top;
    int            pitch;
    int            height;
    int            width;
    unsigned char* buffer;

    FT_Face face = 0;
    FTC_Manager_LookupFace(_manager, faceId, &face);

    FT_Int charMapIndex = 0;
    for(int n = 0; n < face->num_charmaps; ++n)
    {
        if(face->charmap[n].encoding == FT_ENCODING_UNICODE)
        {
            charMapIndex = n;
        }
    }

    glyphPos.x = (int) pos.x() << 16;
    glyphPos.y = (int) pos.y() << 16;

    for( String::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        FT_UInt glyph_index = FTC_CMapCache_Lookup(_charMapCache, faceId, charMapIndex, it->value());

        if( ! glyph_index )
            continue;

        if( FT_HAS_KERNING(face) && previous )
        {
            FT_Get_Kerning(face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);

            glyphPos.x += delta.x;
            glyphPos.y -= delta.y;
        }

        if( fontAngle == 0 )
        {
            if( FTC_SBitCache_Lookup( _bitmapCache, imageType, glyph_index, &smalGlyphBitmap, &node ) )
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
            FTC_ImageCache_Lookup(_imageCache, imageType, glyph_index, &glyph, &node);

            FT_Glyph_Copy( glyph, &glyphCopy );
            FT_Glyph_Transform( glyphCopy, &matrix, 0);
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

            drawGlyph(image, color, left, top, pitch, height, width, buffer, clip);
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

    // UNLOCK
}


void FreeType::drawGlyph(Image& image, const Color& color, int xpos, int ypos,
                         int bmPitch, int height, int width, 
                         const unsigned char* buffer, const Rect& clip)
{
    const int clipRight  = clip.x() + clip.width();
    const int clipBottom = clip.y() + clip.height();
    Pt::ssize_t yOffset  = 0;
    Pt::ssize_t dsy      = 0;
    Pt::ssize_t dsx      = 0;
    const Pt::ssize_t x2 = clipRight;
    const Pt::ssize_t y2 = clipBottom;

    if( bmPitch < width )
        bmPitch += width;

    int ofsx = 0;
            
    if(xpos < clip.x() ) 
    {
        ofsx = clip.x()  - xpos;
        xpos =  clip.x();
    }
            
    int ofsy = 0;
            
    if(ypos < clip.y()) 
    {
        ofsy = clip.y() - ypos;
        ypos = clip.y();
    }

    dsy = ypos;

    Color pixelColor = color;

    for( Pt::int32_t y = ofsy; y < height; ++y, ++dsy )
    {
        yOffset = y * bmPitch;

        if( dsy < clip.y() )
            continue;

        if( dsy > y2 )
            break;

        dsx   = xpos;

        for( Pt::int32_t x = ofsx; x < width; ++x, ++dsx )
        {
            if( dsx < clip.x() )
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

} // namespace Gfx

} // namespace Pt

