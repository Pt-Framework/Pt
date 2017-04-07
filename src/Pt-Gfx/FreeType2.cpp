/* Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2017-2017 Aloysius Indrayanto

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

#include <limits>

#include <Pt/System/Directory.h>
#include <Pt/System/FileInfo.h>

#include <Pt/Gfx/Image.h>

#include "FreeType2.h"

#include "DejaVuSans.h"
#include "DejaVuSansBold.h"
#include "DejaVuSansItalic.h"
#include "DejaVuSansBoldItalic.h"


namespace Pt {
namespace Gfx {


static struct Init {
    Init()
    { FreeType2::instance(); }
} initFreeType2;


FreeType2::FreeType2()
{
    if( FT_Init_FreeType( &_ft ) )
        throw std::runtime_error("FT_Init_FreeType2");

    if( FTC_Manager_New( _ft, 0, 0, 0, &FreeType2::fontRequest, this, &_manager ) )
        throw std::runtime_error( "FTC_Manager_New" );

    if( FTC_CMapCache_New( _manager, &_charMapCache ) )
        throw std::runtime_error( "FTC_CMapCache_New" );

    if( FTC_SBitCache_New( _manager, &_bitmapCache ) )
        throw std::runtime_error( "FTC_SBitCache_New" );

    if( FTC_ImageCache_New( _manager, &_imageCache ) )
        throw std::runtime_error( "FTC_ImageCache_New" );

    const System::Path path = System::Path( System::Path::curdir()) / "fonts";
    const std::string  lp   = path.toLocal();
    setFontDir(path);
}

FreeType2::~FreeType2()
{
    FTC_Manager_Done( _manager );
    FT_Done_FreeType( _ft );
}

void FreeType2::setFontDir(const System::Path& path)
{
    System::MutexLock lock(_mutex);

    _fontDir = path;

    _fonts.clear();
    _files.clear();

    if( !System::FileInfo::exists(_fontDir) ) return;

    System::DirectoryIterator it(_fontDir);
    System::DirectoryIterator end;

    for(; it != end; ++it) {
        const System::Path fp = _fontDir / it->path();
        const std::string  gg = fp.toLocal();

        FT_Face  face;
        FT_Error err = FT_New_Face(_ft, fp.toLocal().c_str(), 0, &face);
        if(err != 0) continue;

        Font::Style style = Font::Normal;

        if( (face->style_flags & FT_STYLE_FLAG_BOLD) == FT_STYLE_FLAG_BOLD )
            style = Font::Bold;

        if( (face->style_flags & FT_STYLE_FLAG_ITALIC) == FT_STYLE_FLAG_ITALIC )
            style = Font::Italic;

        if( (face->style_flags & FT_STYLE_FLAG_BOLD) == FT_STYLE_FLAG_BOLD && (face->style_flags & FT_STYLE_FLAG_ITALIC) == FT_STYLE_FLAG_ITALIC )
            style = Font::BoldItalic;

        Font font(face->family_name, 12, style);

        System::Path& fontPath = _fonts[font];
        fontPath = fp;

        _files.insert(&fontPath);

        FT_Done_Face(face);
    }
}

std::vector<std::string> FreeType2::fontNames() const
{
    std::vector<std::string> names;
    names.push_back("DejaVu Sans");

    System::MutexLock lock(_mutex);

    Fonts::const_iterator it;
    for(it = _fonts.begin(); it != _fonts.end(); ++it) {
        if(std::find(names.begin(), names.end(), it->first.name()) == names.end())
            names.push_back( it->first.name() );
    }

    return names;
}

void FreeType2::setDefaultFont( const std::string& font )
{
    System::MutexLock lock(_mutex);

    _defaultFont = font;
}

std::string FreeType2::defaultFont() const
{
    System::MutexLock lock(_mutex);

    const std::string df = _defaultFont;

    return df;
}

FTC_FaceID FreeType2::findFaceId(const Font& font)
{
    System::MutexLock lock(_mutex);

    Fonts::iterator it = _fonts.find(font);
    if(it == _fonts.end()) return 0;

    System::Path* path = &it->second;
    return reinterpret_cast<FTC_FaceID>(path);
}

FontMetrics FreeType2::fontMetrics(const String& text, FTC_FaceID faceId, FTC_ImageType imageType)
{
    System::MutexLock lock(_mutex);

    FT_Face  face = 0;
    FT_Error ferr = FTC_Manager_LookupFace(_manager, faceId, &face);
    if(ferr && ferr != FT_Err_Out_Of_Memory) return FontMetrics(0, 0, 0, 0);

    FT_Int charMapIndex = 0;
    for(int n = 0; n < face->num_charmaps; ++n) {
        if(face->charmap[n].encoding != FT_ENCODING_UNICODE) continue;
        charMapIndex = n;
        break;
    }

    FTC_ScalerRec scaler;
    scaler.face_id = imageType->face_id;
    scaler.width   = imageType->width;
    scaler.height  = imageType->height;
    scaler.pixel   = 1; // Set to 1 to ignore scaler.x_res and scaler.y_res

    FT_Size size;
    FTC_Manager_LookupSize(_manager, &scaler, &size);

    int       pen_x    = 0;
    int       pen_y    = 0;
    FT_UInt   previous = 0;
    FT_Vector delta;
    FT_Glyph  glyph;
    FT_BBox   gbbox = { 0, 0, 0, 0 };
    FT_BBox   tbbox = { std::numeric_limits<FT_Pos>::max(),
                        std::numeric_limits<FT_Pos>::max(),
                        std::numeric_limits<FT_Pos>::min(),
                        std::numeric_limits<FT_Pos>::min()
                      };

    for(String::const_iterator it = text.begin(); it != text.end(); ++it) {
        FT_UInt glyph_index = FTC_CMapCache_Lookup(_charMapCache, faceId, charMapIndex, it->value());
        if(!glyph_index) continue;

        if(FT_HAS_KERNING(face) && previous) {
            FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);
            pen_x += delta.x;
            pen_y -= delta.y;
        }

        FTC_Node node;
        if(FTC_ImageCache_Lookup(_imageCache, imageType, glyph_index, &glyph, &node))
            continue;

        FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &gbbox);

        gbbox.xMin += ( pen_x >> 16 );
        gbbox.xMax += ( pen_x >> 16 );

        tbbox.xMin = std::min(gbbox.xMin, tbbox.xMin);
        tbbox.xMax = std::max(gbbox.xMax, tbbox.xMax);

        pen_x += glyph->advance.x;
        pen_y -= glyph->advance.y;

        previous = glyph_index;
    }

    return FontMetrics(
        size->metrics.ascender   >> 6,
      (-size->metrics.descender) >> 6,
      ( tbbox.xMax - tbbox.xMin),
        size->metrics.height     >> 6
    );
}

FT_Error FreeType2::fontRequest(FTC_FaceID faceId, FT_Library library, FT_Pointer data, FT_Face* face)
{
    FreeType2* ft = static_cast<FreeType2*>(data);
    return ft->onFontRequest(faceId, face);
}

FT_Error FreeType2::onFontRequest(FTC_FaceID faceId, FT_Face* face)
{
    System::Path* path = reinterpret_cast<System::Path*>(faceId);

    if(faceId == 0)
        return FT_New_Memory_Face(_ft, DejaVuSans, DejaVuSansSize, 0, face);

    // Check if path is still valid
    if(_files.find(path) == _files.end())
        return FT_New_Memory_Face(_ft, DejaVuSans, DejaVuSansSize, 0, face);

    return FT_New_Face(_ft, path->toLocal().c_str(), 0, face);
}

void FreeType2::getCharSpacing(
    Pt::int32_t& x, Pt::int32_t& y,
    const Char& from, const Char& to, FTC_FaceID faceId, FTC_ImageType imageType
)
{
    System::MutexLock lock(_mutex);

    FT_Face  face = 0;
    FT_Error ferr = FTC_Manager_LookupFace(_manager, faceId, &face);
    if(ferr && ferr != FT_Err_Out_Of_Memory) return;

    FT_Int charMapIndex = 0;
    for(int n = 0; n < face->num_charmaps; ++n) {
        if(face->charmap[n].encoding == FT_ENCODING_UNICODE) {
            charMapIndex = n;
            break;
        }
    }

    FT_UInt glyph_index0 = FTC_CMapCache_Lookup(_charMapCache, faceId, charMapIndex, from);
    if(!glyph_index0) return;

    FT_UInt glyph_index1 = FTC_CMapCache_Lookup(_charMapCache, faceId, charMapIndex, to);
    if(!glyph_index1) return;

    imageType->flags = FT_LOAD_TARGET_NORMAL | FT_LOAD_IGNORE_TRANSFORM;

    FT_Glyph glyph;
    FTC_Node node;
    FTC_ImageCache_Lookup(_imageCache, imageType, glyph_index0, &glyph, &node);

    x = glyph->advance.x >> 16;
    y = glyph->advance.y >> 16;

    if(FT_HAS_KERNING(face)) {
        FT_Vector delta;
        FT_Get_Kerning(face, glyph_index0, glyph_index1, FT_KERNING_DEFAULT, &delta);
        x += delta.x;
        y += delta.y;
    }
}

void FreeType2::pathFromChar(
    std::vector<PointF>& points, std::vector<Pt::uint8_t>& tags, std::vector<Pt::int32_t>& contours,
    const Char& chr, FTC_FaceID faceId, FTC_ImageType imageType
)
{
    points.clear();
    contours.clear();

    System::MutexLock lock(_mutex);

    FT_Face  face = 0;
    FT_Error ferr = FTC_Manager_LookupFace(_manager, faceId, &face);
    if(ferr && ferr != FT_Err_Out_Of_Memory) return;

    FT_Int charMapIndex = 0;
    for(int n = 0; n < face->num_charmaps; ++n) {
        if(face->charmap[n].encoding == FT_ENCODING_UNICODE) {
            charMapIndex = n;
            break;
        }
    }

    FT_UInt glyph_index = FTC_CMapCache_Lookup(_charMapCache, faceId, charMapIndex, chr);
    if(!glyph_index) return;

    imageType->flags = FT_LOAD_TARGET_NORMAL | FT_LOAD_IGNORE_TRANSFORM;

    FTC_SBit smalGlyphBitmap;
    FTC_Node node;
    if(FTC_SBitCache_Lookup(_bitmapCache, imageType, glyph_index, &smalGlyphBitmap, &node))
        return;

    points.resize(face->glyph->outline.n_points);
    for(int i = 0; i < face->glyph->outline.n_points; ++i) {
        points[i].set(
             face->glyph->outline.points[i].x * 0.015625, // (1.0 / 64.0)
            -face->glyph->outline.points[i].y * 0.015625
        );
    }
 
    tags.resize(face->glyph->outline.n_points);
    for(int i = 0; i < face->glyph->outline.n_points; ++i) {
        tags[i] = face->glyph->outline.tags[i];
    }

    contours.resize(face->glyph->outline.n_contours);
    for(int i = 0; i < face->glyph->outline.n_contours; ++i) {
        contours[i] = face->glyph->outline.contours[i];
    }
}

void FreeType2::draw(
    Image& image, const Rect& clip, const Point& pos, const Color& color, Pt::ssize_t fontAngle, const CompositionMode& mode,
    const String& text, FT_Matrix& matrix, FTC_FaceID faceId, FTC_ImageType imageType, bool mono
)
{
    System::MutexLock lock(_mutex);

    FT_Vector      glyphPos;
    FT_Vector      delta;
    FT_UInt        previous = 0;
    FT_Glyph       glyph;
    FT_Glyph       glyphCopy = 0;
    FTC_Node       node;
    FTC_SBit       smalGlyphBitmap;
    FT_BitmapGlyph glyphBitmap;

    // Glyph bitmap description
    int            incX;
    int            incY;
    int            left;
    int            top;
    int            pitch;
    int            height;
    int            width;
    unsigned char* buffer;

    FT_Face  face = 0;
    FT_Error ferr = FTC_Manager_LookupFace(_manager, faceId, &face);
    if(ferr && ferr != FT_Err_Out_Of_Memory) return;

    FT_Int charMapIndex = 0;
    for(int n = 0; n < face->num_charmaps; ++n) {
        if(face->charmap[n].encoding == FT_ENCODING_UNICODE) {
            charMapIndex = n;
            break;
        }
    }

    glyphPos.x = (int) pos.x() << 16;
    glyphPos.y = (int) pos.y() << 16;

    for(String::const_iterator it = text.begin(); it != text.end(); ++it) {

        FT_UInt glyph_index = FTC_CMapCache_Lookup(_charMapCache, faceId, charMapIndex, it->value());
        if(!glyph_index) continue;

        if(FT_HAS_KERNING(face) && previous) {
            FT_Get_Kerning(face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);
            glyphPos.x += delta.x;
            glyphPos.y -= delta.y;
        }

        if(!fontAngle) {
            if(mono) imageType->flags = FT_LOAD_RENDER | FT_LOAD_TARGET_MONO;
            else     imageType->flags = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;

            if(FTC_SBitCache_Lookup(_bitmapCache, imageType, glyph_index, &smalGlyphBitmap, &node))
                continue;

            incX   = smalGlyphBitmap->xadvance << 16;
            incY   = smalGlyphBitmap->yadvance << 16;

            left   = (glyphPos.x >> 16) + smalGlyphBitmap->left;
            top    = (glyphPos.y >> 16) - smalGlyphBitmap->top;
            pitch  = smalGlyphBitmap->pitch;
            width  = smalGlyphBitmap->width;
            height = smalGlyphBitmap->height;
            buffer = smalGlyphBitmap->buffer;
        }
        else {
            if(mono) {
                imageType->flags = FT_LOAD_TARGET_MONO;

                FTC_ImageCache_Lookup(_imageCache, imageType, glyph_index, &glyph, &node);
                FT_Glyph_Copy( glyph, &glyphCopy );
                FT_Glyph_Transform( glyphCopy, &matrix, 0);
                FT_Glyph_To_Bitmap( &glyphCopy, FT_RENDER_MODE_MONO, 0, 1 );
            }
            else {
                imageType->flags = FT_LOAD_TARGET_NORMAL;

                FTC_ImageCache_Lookup(_imageCache, imageType, glyph_index, &glyph, &node);
                FT_Glyph_Copy( glyph, &glyphCopy );
                FT_Glyph_Transform( glyphCopy, &matrix, 0);
                FT_Glyph_To_Bitmap( &glyphCopy, FT_RENDER_MODE_NORMAL, 0, 1 );
            }

            glyphBitmap = (FT_BitmapGlyph) glyphCopy;

            incX   = glyphCopy->advance.x;
            incY   = glyphCopy->advance.y;

            left   = (glyphPos.x >> 16) + glyphBitmap->left;
            top    = (glyphPos.y >> 16) - glyphBitmap->top;
            pitch  = glyphBitmap->bitmap.pitch;
            width  = glyphBitmap->bitmap.width;
            height = glyphBitmap->bitmap.rows;
            buffer = glyphBitmap->bitmap.buffer;
        }

        if( !isspace(*it) ) {
            drawGlyph(image, clip, left, top, color, mode, pitch, width, height, buffer, mono);
        }

        glyphPos.x += incX;
        glyphPos.y -= incY;
        previous    = glyph_index;

        if(glyphCopy) {
            FT_Done_Glyph(glyphCopy);
            glyphCopy = 0;
        }

    } // for
}

void FreeType2::drawGlyph(
    Image& image, const Rect& clip, int xpos, int ypos, const Color& color, const CompositionMode& mode,
    int pitch, int width, int height, const unsigned char* buffer, bool mono
)
{
    const Pt::ssize_t x1 = clip.x();
    const Pt::ssize_t y1 = clip.y();
    const Pt::ssize_t x2 = x1 + clip.width () - 1;
    const Pt::ssize_t y2 = y1 + clip.height() - 1;

    int ofsx = 0;
    if(xpos < x1) {
        ofsx = x1 - xpos;
        xpos = x1;
    }

    int ofsy = 0;
    if(ypos < y1) {
        ofsy = y1 - ypos;
        ypos = y1;
    }

    Color       pixelColor = color;
    Pt::ssize_t dsy        = ypos;

    for(Pt::int32_t y = ofsy; y < height; ++y, ++dsy) {
        if(dsy < y1) continue;
        if(dsy > y2) break;

        Pt::ssize_t dsx    = xpos;
        Pt::ssize_t yOffset = y * pitch;

        // Without anti-aliasing
        if(mono) {
            for(Pt::int32_t x = ofsx; x < pitch; ++x) {
                const unsigned char value = buffer[yOffset + x];
                      unsigned char mask  = 0x80;

                for(Pt::int32_t b = 0; b < 8; ++b, ++dsx) {
                    if(dsx < x1) continue;
                    if(dsx > x2) break;

                    const unsigned char valMono = value & mask;
                    mask >>= 1;

                    if(!valMono) continue;

                    Pixel pixel(image.view(), dsx, dsy);
                    image.format().setPixel(pixel, color, mode);
                }
            }
        }

        // With anti-aliasing
        else {
            for(Pt::int32_t x = ofsx; x < width; ++x, ++dsx) {
                if(dsx < x1) continue;
                if(dsx > x2) break;

                const unsigned char value = buffer[yOffset + x];
                if(!value) continue;

                Pixel pixel(image.view(), dsx, dsy);

                if(mode == CompositionMode::SourceOver) {
                    pixelColor.setAlpha(color.alpha() * value / 255);
                    image.format().setPixel(pixel, pixelColor, CompositionMode::SourceOver);
                }
                else { // SourceCopy
                    if(value != 255) {
                        pixelColor.setAlpha(value * 257);
                        image.format().setPixel(pixel, pixelColor, CompositionMode::SourceOver);
                    }
                    else {
                        image.format().setPixel(pixel, color, CompositionMode::SourceCopy);
                    }
                }
            }
        }

    } // for
}


} // namespace Gfx
} // namespace Pt

