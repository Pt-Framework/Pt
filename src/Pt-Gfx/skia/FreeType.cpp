/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2019 Laurentiu-Gheorghe Crisan

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

#include <Pt/Gfx/TextMetrics.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/Gfx/Image.h>
#include <Pt/System/Directory.h>
#include <Pt/System/FileInfo.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Logger.h>

#include <stdexcept>
#include <limits>
#include <iostream>

PT_LOG_DEFINE("Pt.Gfx.FreeType");

namespace {

static const unsigned DefaultFontSize = 12;

// some versions of freetype do not work with NULL
static const FTC_FaceID DefaultFaceId = reinterpret_cast<FTC_FaceID>(1);

} // namespace

namespace Pt {

namespace Gfx {

FreeType::FreeType()
: _defaultFace(DefaultFaceId)
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

    System::Path  path = System::Path( System::Path::curdir()) / "fonts";
    std::string lp = path.toLocal();
    setFontDir(path);
}


FreeType::~FreeType()
{
    FTC_Manager_Done( _manager );
    FT_Done_FreeType( _ft );
}


const std::string& FreeType::defaultFont() const
{
    // LOCK

    return _defaultFont;

    // UNLOCK
}


FTC_FaceID FreeType::defaultFace() const
{
    // LOCK

    return _defaultFace;

    // UNLOCK
}


void FreeType::setDefaultFont(const std::string& font)
{
    // LOCK

    _defaultFont = font;
    _defaultFace = findFaceId(Font(_defaultFont, DefaultFontSize));

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

    if( ! System::FileInfo::exists(_fontDir) )
        return;

    System::DirectoryIterator it(_fontDir);
    System::DirectoryIterator end;

    for( ; it != end; ++it)
    {
        if( it->path().fileName() == "." || it->path().fileName() == ".." )
          continue;

        System::Path fontFile = _fontDir / it->path();

        FT_Face face;
        FT_Error err = FT_New_Face(_ft, fontFile.toLocal().c_str(), 0, &face);
        if(err != 0)
        {
            PT_LOG_WARN( "font error: " << fontFile.toLocal() << ' ' << err );
            continue;
        }

        if ((face->face_flags & FT_FACE_FLAG_SCALABLE) == 0)
        {
            PT_LOG_WARN( "font skipped: " << fontFile.toLocal() << " not scalable");
            continue;
        }

        Font font(face->family_name, DefaultFontSize, face->style_name);
        PT_LOG_INFO( "loaded font: " << font.name() << ' ' << font.style() );

        System::Path& fontPath = _fonts[font];
        fontPath = fontFile;

        _files.insert(&fontPath);

        FT_Done_Face(face);
    }

    // UNLOCK
}


FTC_FaceID FreeType::findFaceId(const Font& font)
{
    // LOCK
    const std::string fontName = font.name().empty() ? _defaultFont : font.name();

    Fonts::iterator it = _fonts.begin();

    for (; it != _fonts.end(); ++it)
    {
        if (it->first.name() == fontName && it->first.style() == font.style())
        {
            System::Path* path = &it->second;
            return reinterpret_cast<FTC_FaceID>(path);
        }
    }

    return DefaultFaceId;
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
    if(faceId == DefaultFaceId)
    {
        return FT_New_Memory_Face(_ft, DejaVuSans, DejaVuSansSize, 0, face);
    }

    System::Path* path = reinterpret_cast<System::Path*>(faceId);

    // check if path is still valid
    if( _files.find(path) == _files.end() )
        return FT_New_Memory_Face(_ft, DejaVuSans, DejaVuSansSize, 0, face);

    return FT_New_Face(_ft, path->toLocal().c_str(), 0, face);
}


TextMetrics FreeType::textMetrics(const String& text,
                                  FTC_FaceID faceId,
                                  std::size_t fontSize)
{
    // LOCK

    FT_Face face = 0;
    FT_Error ferr = FTC_Manager_LookupFace(_manager, faceId, &face);
    if(ferr)
        return TextMetrics();

    // calculate total font height
    double fontHeight = (face->height / double(face->ascender)) * fontSize;

    FTC_ImageTypeRec imageType;
    imageType.face_id = faceId;
    imageType.height  = static_cast<unsigned>(fontHeight + 0.1);
    imageType.width   = imageType.height;
    imageType.flags   = FT_LOAD_DEFAULT;

    FT_Int charMapIndex = 0;
    for(int n = 0; n < face->num_charmaps; ++n)
    {
        if(face->charmap[n].encoding == FT_ENCODING_UNICODE)
        {
            charMapIndex = n;
            break;
        }
    }

    FTC_ScalerRec scaler;
    scaler.face_id = imageType.face_id;
    scaler.width   = imageType.width;
    scaler.height  = imageType.height;
    scaler.pixel   = 1; // 1 means TRUE and scaler.x_res and scaler.y_res are ignored

    FT_Size size;
    FTC_Manager_LookupSize(_manager, &scaler, &size);

    FT_UInt   previous = 0;
    FT_Vector delta;
    FT_Glyph  glyph;

    int width = 0;

    for( String::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        FT_UInt glyph_index = FTC_CMapCache_Lookup( _charMapCache, faceId,
                                                    charMapIndex, it->value() );

        if( ! glyph_index )
            glyph_index = FTC_CMapCache_Lookup(_charMapCache, faceId,
                                               charMapIndex, 63);

        if( ! glyph_index )
            continue;

        FTC_Node node;
        if( FTC_ImageCache_Lookup(_imageCache, &imageType, glyph_index, &glyph, &node) )
            continue;

        // width is the sum of the advance of each character
        width += glyph->advance.x >> 16;

        // add kerning between previous and this character
        if( FT_HAS_KERNING(face) && previous )
        {
            FT_Get_Kerning(face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);

            width += delta.x >> 16;
        }

        previous = glyph_index;
    }

    double scaleY = size->metrics.y_scale / 65536.0;
    double asc = (face->ascender * scaleY) / 64.0;
    double des = (-face->descender * scaleY) / 64.0;
    double emh = (face->units_per_EM * scaleY) / 64.0;
    double lih = (face->height * scaleY) / 64.0;
    double cap = emh - des;
    double exl = lih - (asc + des);

    Gfx::TextMetrics fm;
    fm.setAscent(asc);
    fm.setDescent(des);
    fm.setCapHeight(cap);
    fm.setLeading(exl);
    fm.setWidth(width);
    return fm;

    // UNLOCK
}


void FreeType::draw(Image& image, Pt::ssize_t x, Pt::ssize_t y, 
                    const String& text, const Color& color, const Rect& clip,
                    const CompositionMode& mode, const Transform& tf, 
                    FTC_FaceID faceId, std::size_t fontSize)
{
    // LOCK

    // apply translation here, FT uses a 2x2 matrix for the other transformations
    PointF translatedPos( x, y );
    translatedPos.addX( tf.dx() );
    translatedPos.addY( tf.dy() );
    
    Point pos = round(translatedPos);

    FT_Matrix matrix;
    matrix.xx = tf.m11() * 0x10000L;
    matrix.xy = tf.m12() * 0x10000L;
    matrix.yx = tf.m21() * 0x10000L;
    matrix.yy = tf.m22() * 0x10000L;

    FT_Face face = 0;
    FT_Error ferr = FTC_Manager_LookupFace(_manager, faceId, &face);
    if(ferr)
        return;

    // calculate total font height
    double fontHeight = (face->height / double(face->ascender)) * fontSize;
    
    FTC_ImageTypeRec imageType;
    imageType.face_id = faceId;
    imageType.height  = static_cast<unsigned>(fontHeight + 0.1);
    imageType.width   = imageType.height;
    imageType.flags   = FT_LOAD_DEFAULT;

    FT_Int charMapIndex = 0;
    for(int n = 0; n < face->num_charmaps; ++n)
    {
        if(face->charmap[n].encoding == FT_ENCODING_UNICODE)
        {
            charMapIndex = n;
            break;
        }
    }

    FT_Vector glyphPos;
    glyphPos.x = (int) pos.x() << 16;
    glyphPos.y = (int) pos.y() << 16;

    FT_UInt previousIndex = 0;
    
    for( String::const_iterator it = text.begin(); it != text.end(); ++it )
    {
        FT_UInt glyphIndex = FTC_CMapCache_Lookup(_charMapCache, faceId,
                                                   charMapIndex, it->value());
        
        if( ! glyphIndex )
            glyphIndex = FTC_CMapCache_Lookup(_charMapCache, faceId,
                                              charMapIndex, 63);

        if( ! glyphIndex )
            continue;

        int incX = 0;
        int incY = 0;
        FT_Glyph glyphCopy = 0;

        int            left = 0;
        int            top = 0;
        int            pitch = 0;
        int            height = 0;
        int            width = 0;
        unsigned char* buffer = 0;

        if( tf.isIdentity() )
        {
            FTC_Node node = 0;
            FTC_SBit glyphBitmap = 0;
            if( FTC_SBitCache_Lookup( _bitmapCache, &imageType, glyphIndex,
                                      &glyphBitmap, &node ) )
                continue;

            incX = glyphBitmap->xadvance << 16;
            incY = glyphBitmap->yadvance << 16;

            if( FT_HAS_KERNING(face) && previousIndex && face->size )
            {
                FT_Vector delta;
                FT_Get_Kerning(face, previousIndex, glyphIndex, FT_KERNING_DEFAULT, &delta);

                glyphPos.x += delta.x;
                glyphPos.y -= delta.y;
            }

            left        = (glyphPos.x >> 16) + glyphBitmap->left;
            top         = (glyphPos.y >> 16) - glyphBitmap->top;
            pitch       = glyphBitmap->pitch;
            height      = glyphBitmap->height;
            width       = glyphBitmap->width;
            buffer      = glyphBitmap->buffer;
        }
        else
        {
            FTC_Node node = 0;
            FT_Glyph glyph = 0;
            FT_Error err = FTC_ImageCache_Lookup(_imageCache, &imageType, glyphIndex, &glyph, &node);
            err += FT_Glyph_Copy(glyph, &glyphCopy);
            
            err += FT_Glyph_Transform(glyphCopy, &matrix, 0);
            err += FT_Glyph_To_Bitmap(&glyphCopy, FT_RENDER_MODE_NORMAL,  0, 1);
            if( err != 0)
                continue;

            FT_BitmapGlyph glyphBitmap = reinterpret_cast<FT_BitmapGlyph>(glyphCopy);

            incX = glyphCopy->advance.x;
            incY = glyphCopy->advance.y;

            if(FT_HAS_KERNING(face) && previousIndex && face->size)
            {
                FT_Vector delta;
                FT_Get_Kerning(face, previousIndex, glyphIndex, FT_KERNING_DEFAULT, &delta);

                glyphPos.x += delta.x;
                glyphPos.y -= delta.y;
            }

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

            drawGlyph(image, left, top, color, pitch, height, width, buffer, 
                      clip, mode);
        }

        glyphPos.x  += incX;
        glyphPos.y  -= incY;

        previousIndex = glyphIndex;

        if(glyphCopy)
        {
            FT_Done_Glyph( glyphCopy );
            glyphCopy = 0;
        }
    }

    // UNLOCK
}


void FreeType::drawGlyph(Image& image, int xpos, int ypos, const Color& color,
                         int bmPitch, int height, int width, const unsigned char* buffer, 
                         const Rect& clip, const CompositionMode& mode)
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

    if( xpos < clip.x() )
    {
        ofsx = clip.x() - xpos;
        xpos =  clip.x();
    }

    int ofsy = 0;

    if( ypos < clip.y() )
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

        if( dsy >= y2 )
            break;

        dsx = xpos;

        for( Pt::int32_t x = ofsx; x < width; ++x, ++dsx )
        {            
            if( dsx < clip.x())
                continue;

            if( dsx >= x2 )
                break;

            Pixel pixel(image.view(), dsx, dsy);

            const int px = yOffset + x;
            unsigned char value = buffer[px];

            switch(mode)
            {
                default:
                case CompositionMode::SourceCopy:
                    if(value != 255)
                    {
                        pixelColor.setAlpha(value * 257);
                        image.format().setPixel(pixel, pixelColor, CompositionMode::SourceOver);
                    }
                    else
                    {
                        image.format().setPixel(pixel, color, CompositionMode::SourceCopy);
                    }
                    break;

                case CompositionMode::SourceOver:
                    pixelColor.setAlpha(color.alpha() * value / 255);
                    image.format().setPixel(pixel, pixelColor, CompositionMode::SourceOver);
                    break;
            }
        }
    }
}

} // namespace Gfx

} // namespace Pt
