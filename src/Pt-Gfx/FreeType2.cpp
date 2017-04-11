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

#include <iomanip>

#include <limits>
#include <utility>

#include <Pt/System/Directory.h>
#include <Pt/System/FileInfo.h>

#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Math.h>

#include "FreeType2.h"

#include "DejaVuSans.h"
#include "DejaVuSansBold.h"
#include "DejaVuSansItalic.h"
#include "DejaVuSansBoldItalic.h"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Normal Member Functions ========================================================
// ======================================================================================

FreeType2::FreeType2()
: _refCount(1)
{
    const bool initLib = !FreeType2::_ft;

    if( initLib && ( FT_Init_FreeType( &FreeType2::_ft ) || !FreeType2::_ft ) )
        throw std::runtime_error("FT_Init_FreeType");

    if( FTC_Manager_New( FreeType2::_ft, 0, 0, 0, &FreeType2::fontRequest, this, &_manager ) )
        throw std::runtime_error( "FTC_Manager_New" );

    if( FTC_CMapCache_New( _manager, &_charMapCache ) )
        throw std::runtime_error( "FTC_CMapCache_New" );

    if( FTC_SBitCache_New( _manager, &_bitmapCache ) )
        throw std::runtime_error( "FTC_SBitCache_New" );

    if( FTC_ImageCache_New( _manager, &_imageCache ) )
        throw std::runtime_error( "FTC_ImageCache_New" );

    if(!initLib) return;

    setFontDir_impl_noLock(
        FreeType2::_fontDir.empty()
        ? System::Path( System::Path::curdir()) / "fonts"
        : FreeType2::_fontDir
    );
}

FreeType2::~FreeType2()
{ FTC_Manager_Done(_manager); }

const FontMetrics FreeType2::fontMetrics(const String& text, FTC_FaceID faceId, FTC_ImageType imageType)
{
    System::MutexLock lock(FreeType2::_mutex);

    FT_Face  face = 0;
    FT_Error ferr = FTC_Manager_LookupFace(_manager, faceId, &face);
    if(ferr && ferr != FT_Err_Out_Of_Memory) return FontMetrics(0, 0, 0, 0);

    FT_Int charMapIndex = 0;
    for(Pt::int32_t n = 0; n < face->num_charmaps; ++n) {
        if(face->charmap[n].encoding != FT_ENCODING_UNICODE) continue;
        charMapIndex = n;
        break;
    }

    FTC_ScalerRec scaler;
    scaler.face_id = imageType->face_id;
    scaler.width   = imageType->width;
    scaler.height  = imageType->height;
    scaler.pixel   = 1; // Set to 1 to ignore "scaler.x_res" and "scaler.y_res"

    FT_Size size;
    FTC_Manager_LookupSize(_manager, &scaler, &size);

    Pt::int32_t penX     = 0;
    Pt::int32_t penY     = 0;
    FT_UInt     previous = 0;
    FT_Vector   delta;
    FT_Glyph    glyph;
    FT_BBox     gbbox = { 0, 0, 0, 0 };
    FT_BBox     tbbox = { std::numeric_limits<FT_Pos>::max(),
                          std::numeric_limits<FT_Pos>::max(),
                          std::numeric_limits<FT_Pos>::min(),
                          std::numeric_limits<FT_Pos>::min()
                        };

    for(String::const_iterator it = text.begin(); it != text.end(); ++it) {
        FT_UInt glyph_index = FTC_CMapCache_Lookup(_charMapCache, faceId, charMapIndex, it->value());
        if(!glyph_index) continue;

        FTC_Node node;
        if(FTC_ImageCache_Lookup(_imageCache, imageType, glyph_index, &glyph, &node))
            continue;

        FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &gbbox);

        gbbox.xMin += ( penX >> 16 );
        gbbox.xMax += ( penX >> 16 );

        tbbox.xMin = std::min(gbbox.xMin, tbbox.xMin);
        tbbox.xMax = std::max(gbbox.xMax, tbbox.xMax);

        penX += glyph->advance.x;
        penY -= glyph->advance.y;

        if(FT_HAS_KERNING(face) && previous) {
            FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);
            if(delta.x < glyph->advance.x && delta.y < glyph->advance.y) {
                penX += delta.x;
                penY -= delta.y;
            }
        }

        previous = glyph_index;
    }

    return FontMetrics(
        size->metrics.ascender   >> 6,
      (-size->metrics.descender) >> 6,
      ( tbbox.xMax - tbbox.xMin),
        size->metrics.height     >> 6
    );
}

void FreeType2::getCharSpacing(
    Pt::int32_t& x, Pt::int32_t& y,
    const Char& from, const Char& to, FTC_FaceID faceId, FTC_ImageType imageType
)
{
    x = y = 0;

    System::MutexLock lock(FreeType2::_mutex);

    FT_Face  face = 0;
    FT_Error ferr = FTC_Manager_LookupFace(_manager, faceId, &face);
    if(ferr && ferr != FT_Err_Out_Of_Memory) return;

    FT_Int charMapIndex = 0;
    for(Pt::int32_t n = 0; n < face->num_charmaps; ++n) {
        if(face->charmap[n].encoding == FT_ENCODING_UNICODE) {
            charMapIndex = n;
            break;
        }
    }

    FT_UInt glyph_index0 = FTC_CMapCache_Lookup(_charMapCache, faceId, charMapIndex, from);
    if(!glyph_index0) return;

    FT_UInt glyph_index1 = FTC_CMapCache_Lookup(_charMapCache, faceId, charMapIndex, to);
    if(!glyph_index1) return;

    imageType->flags = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL | FT_LOAD_IGNORE_TRANSFORM;

    FT_Glyph glyph = 0;
    FTC_Node node  = 0;
    if(FTC_ImageCache_Lookup(_imageCache, imageType, glyph_index0, &glyph, &node))
        return;

    x = glyph->advance.x >> 16;
    y = glyph->advance.y >> 16;

    //std::clog << "getCharSpacing() : from '" << (char) from << "' to '" << (char) to << "' : G[ " << std::setw(5) << glyph_index0 << " | " << std::setw(5) << glyph_index1 << " ]" << std::endl;
    //std::clog << "getCharSpacing() : from '" << (char) from << "' to '" << (char) to << "' : S( " << std::setw(5) << x << " , " << std::setw(5) << y << " )" << std::endl;

    if(FT_HAS_KERNING(face)) {
        FT_Vector delta;
        FT_Get_Kerning(face, glyph_index0, glyph_index1, FT_KERNING_DEFAULT, &delta);
        if(delta.x < x && delta.y < y) {
            x += delta.x;
            y -= delta.y;
        }
        //std::clog << "getCharSpacing() : from '" << (char) from << "' to '" << (char) to << "' : K( " << std::setw(5) << delta.x << " , " << std::setw(5) << delta.y << " )" << std::endl;
    }

    //std::clog << "getCharSpacing() : from '" << (char) from << "' to '" << (char) to << "' : S( " << std::setw(5) << x << " , " << std::setw(5) << y << " )" << std::endl;
    //std::clog << std::endl;
}

void FreeType2::pathFromChar(
    std::vector<PointF>& points, std::vector<Pt::uint8_t>& tags, std::vector<Pt::int32_t>& contours,
    const Char& chr, FTC_FaceID faceId
)
{
    points.clear();
    contours.clear();

    System::MutexLock lock(FreeType2::_mutex);

    FT_Face  face = 0;
    FT_Error ferr = FTC_Manager_LookupFace(_manager, faceId, &face);
    if(ferr && ferr != FT_Err_Out_Of_Memory) return;

    FT_Int charMapIndex = 0;
    for(Pt::int32_t n = 0; n < face->num_charmaps; ++n) {
        if(face->charmap[n].encoding == FT_ENCODING_UNICODE) {
            charMapIndex = n;
            break;
        }
    }

    FT_UInt glyph_index = FTC_CMapCache_Lookup(_charMapCache, faceId, charMapIndex, chr);
    if(!glyph_index) return;

    FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL | FT_LOAD_IGNORE_TRANSFORM);
    FT_GlyphSlot glyph = face->glyph;

    points.resize(glyph->outline.n_points);
    for(Pt::int32_t i = 0; i < glyph->outline.n_points; ++i) {
        points[i].set(
             glyph->outline.points[i].x * Gfx::Math::dPSFi,
            -glyph->outline.points[i].y * Gfx::Math::dPSFi
        );
    }

    tags.resize(glyph->outline.n_points);
    for(Pt::int32_t i = 0; i < glyph->outline.n_points; ++i) {
        tags[i] = glyph->outline.tags[i];
    }

    contours.resize(glyph->outline.n_contours);
    for(Pt::int32_t i = 0; i < glyph->outline.n_contours; ++i) {
        contours[i] = glyph->outline.contours[i];
    }
}

void FreeType2::draw(
    Image& image, const Rect& clip, const Point& pos, const Color& color, Pt::ssize_t fontAngle, const CompositionMode& mode,
    const String& text, FT_Matrix& matrix, FTC_FaceID faceId, FTC_ImageType imageType, bool mono
)
{
    System::MutexLock lock(FreeType2::_mutex);

    FT_Face  face = 0;
    FT_Error ferr = FTC_Manager_LookupFace(_manager, faceId, &face);
    if(ferr && ferr != FT_Err_Out_Of_Memory) return;

    FT_Int charMapIndex = 0;
    for(Pt::int32_t n = 0; n < face->num_charmaps; ++n) {
        if(face->charmap[n].encoding == FT_ENCODING_UNICODE) {
            charMapIndex = n;
            break;
        }
    }

    FT_Vector glyphPos;
    glyphPos.x = (Pt::int32_t) pos.x() << 16;
    glyphPos.y = (Pt::int32_t) pos.y() << 16;

    // Glyph data
    FTC_Node       node;
    FT_Glyph       glyph;
    FT_Glyph       glyphCopy = 0;
    FT_UInt        previous = 0;
    FTC_SBit       smalGlyphBitmap;
    FT_BitmapGlyph glyphBitmap;

    // Glyph bitmap data
    Pt::int32_t    incX;
    Pt::int32_t    incY;
    Pt::int32_t    left;
    Pt::int32_t    top;
    Pt::int32_t    pitch;
    Pt::int32_t    height;
    Pt::int32_t    width;
    unsigned char* buffer;

    for(String::const_iterator it = text.begin(); it != text.end(); ++it) {

        FT_UInt glyph_index = FTC_CMapCache_Lookup(_charMapCache, faceId, charMapIndex, it->value());
        if(!glyph_index) continue;

        if(!fontAngle) {
            if(mono) imageType->flags = FT_LOAD_RENDER | FT_LOAD_TARGET_MONO;
            else     imageType->flags = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;
            if(FTC_SBitCache_Lookup(_bitmapCache, imageType, glyph_index, &smalGlyphBitmap, &node))
                continue;

            incX = smalGlyphBitmap->xadvance << 16;
            incY = smalGlyphBitmap->yadvance << 16;

            if(FT_HAS_KERNING(face) && previous) {
                FT_Vector delta;
                FT_Get_Kerning(face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);
                if(delta.x < incX && delta.y < incY) {
                     glyphPos.x += delta.x;
                     glyphPos.y -= delta.y;
                }
            }

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

                if(FTC_ImageCache_Lookup(_imageCache, imageType, glyph_index, &glyph, &node))
                    continue;

                FT_Glyph_Copy( glyph, &glyphCopy );
                FT_Glyph_Transform( glyphCopy, &matrix, 0);
                FT_Glyph_To_Bitmap( &glyphCopy, FT_RENDER_MODE_MONO, 0, 1 );
            }
            else {
                imageType->flags = FT_LOAD_TARGET_NORMAL;

                if(FTC_ImageCache_Lookup(_imageCache, imageType, glyph_index, &glyph, &node))
                    continue;

                FT_Glyph_Copy( glyph, &glyphCopy );
                FT_Glyph_Transform( glyphCopy, &matrix, 0);
                FT_Glyph_To_Bitmap( &glyphCopy, FT_RENDER_MODE_NORMAL, 0, 1 );
            }

            glyphBitmap = (FT_BitmapGlyph) glyphCopy;

            incX = glyphCopy->advance.x;
            incY = glyphCopy->advance.y;

            if(FT_HAS_KERNING(face) && previous) {
                FT_Vector delta;
                FT_Get_Kerning(face, previous, glyph_index, FT_KERNING_DEFAULT, &delta);
                if(delta.x < incX && delta.y < incY) {
                     glyphPos.x += delta.x;
                     glyphPos.y -= delta.y;
                }
            }

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
    Image& image, const Rect& clip, Pt::int32_t xpos, Pt::int32_t ypos, const Color& color, const CompositionMode& mode,
    Pt::int32_t pitch, Pt::int32_t width, Pt::int32_t height, const unsigned char* buffer, bool mono
)
{
    // No need to lock the mutex here because this function is called by the
    // draw() function that already locked the mutex

    const Pt::ssize_t x1 = clip.x();
    const Pt::ssize_t y1 = clip.y();
    const Pt::ssize_t x2 = x1 + clip.width () - 1;
    const Pt::ssize_t y2 = y1 + clip.height() - 1;

    Pt::int32_t ofsx = 0;
    if(xpos < x1) {
        ofsx = x1 - xpos;
        xpos = x1;
    }

    Pt::int32_t ofsy = 0;
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

FT_Error FreeType2::onFontRequest(FTC_FaceID faceId, FT_Face* face)
{
    const System::Path* path = reinterpret_cast<const System::Path*>(faceId);

    // No need to lock the mutex here because this function is called by the
    // fontRequest() function, which is called by the FTC_Manager instance, which
    // in turn is being used by other functions that already locked the mutex

    if(faceId == 0)
        return FT_New_Memory_Face(FreeType2::_ft, DejaVuSans, DejaVuSansSize, 0, face);

    // Check if path is still valid
    if(FreeType2::_files.find(path->toString()) == FreeType2::_files.end()) {
        return FT_New_Memory_Face(FreeType2::_ft, DejaVuSans, DejaVuSansSize, 0, face);
    }

    return FT_New_Face(FreeType2::_ft, path->toLocal().c_str(), 0, face);
}


// ======================================================================================
// ===== Static Member Variables ========================================================
// ======================================================================================

System::Mutex        FreeType2::_mutex;

FT_Library           FreeType2::_ft = 0;
FreeType2::Instances FreeType2::_instances;

System::Path         FreeType2::_fontDir;
FreeType2::Fonts     FreeType2::_fonts;
FreeType2::Files     FreeType2::_files;

std::string          FreeType2::_defaultFont;

FreeType2::InitFT2   FreeType2::_initFT2;


// ======================================================================================
// ===== Static Member Functions ========================================================
// ======================================================================================

void FreeType2::reserveInstance(FTC_FaceID faceID)
{
    // Ensure that the specified face ID is not zero
    if(!faceID)
        throw std::logic_error( "FreeType2::reserveInstance()" );

    // Reserve the instance
    reserveInstance_impl(faceID);
}

void FreeType2::reserveInstance_impl(FTC_FaceID faceID)
{
    System::MutexLock lock(FreeType2::_mutex);

    // If the instance for the specified face ID already exists,
    // simply increment the reference counter
    FreeType2::Instances::iterator it = FreeType2::_instances.find(faceID);
    if(it != FreeType2::_instances.end()) {
        ++it->second->_refCount;
        //std::clog << "reserveInstance_impl(" << std::hex << std::setw(10) << faceID << std::dec << ") : INC : refCount = " << it->second->_refCount << std::endl;
        return;
    }

    // Create a new instance and store it to the map
    FreeType2* inst = new FreeType2;
    FreeType2::_instances.insert(std::make_pair(faceID, inst));

    //std::clog << "reserveInstance_impl(" << std::hex << std::setw(10) << faceID << std::dec << ") : NEW : refCount = " << inst->_refCount << std::endl;
}

void FreeType2::releaseInstance(FTC_FaceID faceID)
{
    // Ensure that the specified face ID is not zero
    if(!faceID)
        throw std::logic_error( "FreeType2::releaseInstance()" );

    // Release the instance
    releaseInstance_impl(faceID);
}

void FreeType2::releaseInstance_impl(FTC_FaceID faceID)
{
    System::MutexLock lock(FreeType2::_mutex);

    // Check if the instance for the specified face ID does actually exists
    FreeType2::Instances::iterator it = FreeType2::_instances.find(faceID);
    if(it == FreeType2::_instances.end())
        throw std::logic_error( "FreeType2::releaseInstance_impl()" );

    // Check the reference counter
    if(it->second->_refCount > 1) {
        --it->second->_refCount;
        //std::clog << "releaseInstance_impl(" << std::hex << std::setw(10) << faceID << std::dec << ") : DEC : refCount = " << it->second->_refCount << std::endl;
        return;
    }

    // The instance can be deleted now
    delete it->second;
    FreeType2::_instances.erase(it);
    //std::clog << "releaseInstance_impl(" << std::hex << std::setw(10) << faceID << std::dec << ") : DEL : refCount = 0\n";

    // If there are no more instances, delete the library too
    if(FreeType2::_instances.empty()) {
        FT_Done_FreeType(FreeType2::_ft);
        FreeType2::_ft = 0;
        //std::clog << "reserveInstance_impl(" << std::hex << std::setw(10) << faceID << std::dec << ") : END : closing FreeType\n";
    }
}

FreeType2& FreeType2::instance(FTC_FaceID faceID)
{
    System::MutexLock lock(FreeType2::_mutex);

    // Check if the instance for the specified face ID does actually exists
    FreeType2::Instances::iterator it = FreeType2::_instances.find(faceID);
    if(it == FreeType2::_instances.end())
        throw std::logic_error( "FreeType2::instance()" );

    //std::clog << "            instance(" << std::hex << std::setw(10) << faceID << std::dec << ") : GET : refCount = " << it->second->_refCount << std::endl;

    // Return the instance
    return *it->second;
}

void FreeType2::setFontDir(const System::Path& path)
{
    System::MutexLock lock(FreeType2::_mutex);

    setFontDir_impl_noLock(path);
}

void FreeType2::setFontDir_impl_noLock(const System::Path& path)
{
    FreeType2::_fontDir = path;

    FreeType2::_fonts.clear();
    FreeType2::_files.clear();

    if( !System::FileInfo::exists(FreeType2::_fontDir) ) return;

    // Iterate through the files in the given path
    System::DirectoryIterator it(FreeType2::_fontDir);
    System::DirectoryIterator end;

    for(; it != end; ++it) {
        // Try to load the font file
        const System::Path& fontPath = FreeType2::_fontDir / it->path();

        //std::clog << "setFontDir_impl_noLock() : FOUND : " << fontPath.toLocal() << std::endl;

        FT_Face face;
        if(FT_New_Face(FreeType2::_ft, fontPath.toLocal().c_str(), 0, &face)) continue;

        // Determine the font's style
        Font::Style style = Font::Normal;

        if( (face->style_flags & FT_STYLE_FLAG_BOLD ) == FT_STYLE_FLAG_BOLD &&
            (face->style_flags & FT_STYLE_FLAG_ITALIC) == FT_STYLE_FLAG_ITALIC
          ) {
            style = Font::BoldItalic;
        }

        else if( (face->style_flags & FT_STYLE_FLAG_BOLD) == FT_STYLE_FLAG_BOLD ) {
            style = Font::Bold;
        }

        else if( (face->style_flags & FT_STYLE_FLAG_ITALIC) == FT_STYLE_FLAG_ITALIC ) {
            style = Font::Italic;
        }

        //std::clog << "setFontDir_impl_noLock() : VALID : [" << style << "] " << std::endl;

        // Generate a font object so that font information can be stored
        Font font(face->family_name, 0, style);
        FT_Done_Face(face);

        // Store the font information
        FreeType2::_fonts[font] = fontPath;

        // Store the font file name
        FreeType2::_files.insert(fontPath.toString());
    }

    //for(FreeType2::Fonts::const_iterator it = FreeType2::_fonts.begin(); it != FreeType2::_fonts.end(); ++it) {
    //    std::clog << "setFontDir_impl_noLock() : " << std::left << std::setw(80) << it->second.toLocal() << std::right << " : [" << it->first.style() << "] " << it->first.name() << std::endl;
    //}
    //std::clog << std::endl;
}

const std::vector<std::string> FreeType2::fontNames()
{
    std::vector<std::string> names;
    names.push_back("DejaVu Sans");

    System::MutexLock lock(FreeType2::_mutex);

    Fonts::const_iterator it;
    for(it = FreeType2::_fonts.begin(); it != FreeType2::_fonts.end(); ++it) {
        if(std::find(names.begin(), names.end(), it->first.name()) != names.end()) continue;
        names.push_back(it->first.name());
    }

    return names;
}

void FreeType2::setDefaultFont( const std::string& font )
{
    System::MutexLock lock(FreeType2::_mutex);

    FreeType2::_defaultFont = font.empty() ? "DejaVu Sans" : font;
}

const std::string FreeType2::defaultFont()
{
    System::MutexLock lock(FreeType2::_mutex);

    const std::string df = FreeType2::_defaultFont;

    return df;
}

FTC_FaceID FreeType2::findFaceId(const Font& font)
{
    System::MutexLock lock(FreeType2::_mutex);

    Fonts::iterator it = FreeType2::_fonts.find(font);
    if(it == FreeType2::_fonts.end()) {
        //std::clog << "findFaceId() : " << 0 << std::endl;
        return 0;
    }

    //std::clog << "findFaceId() : " << it->second.toLocal() << std::endl;
    return reinterpret_cast<FTC_FaceID>(&it->second);
}

FT_Error FreeType2::fontRequest(FTC_FaceID faceId, FT_Library library, FT_Pointer data, FT_Face* face)
{
    FreeType2* ft = static_cast<FreeType2*>(data);

    //const System::Path* path = reinterpret_cast<const System::Path*>(faceId);
    //if(path) std::clog << "fontRequest() : " << path->toLocal() << std::endl;
    //else     std::clog << "fontRequest() : " << 0 << std::endl;

    return ft->onFontRequest(faceId, face);
}


} // namespace Gfx
} // namespace Pt

