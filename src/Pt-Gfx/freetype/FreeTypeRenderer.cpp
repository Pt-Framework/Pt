/* Copyright (C) 2024 Marc Boris Duerner

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

#include "FreeTypeRenderer.h"

#include "FreeTypeFontProvider.h"

#include FT_TRUETYPE_TABLES_H

#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/TextMetrics.h>
#include <Pt/Gfx/Transform.h>

#include <cctype>
#include <cmath>

namespace {

static const std::size_t DefaultFontSize = 12;

} // namespace

namespace Pt {

namespace Gfx {

FreeTypeRenderer::FreeTypeRenderer()
: _faceId(FreeTypeFontProvider::instance().defaultFace())
, _fontSize(DefaultFontSize)
{
}


void FreeTypeRenderer::setFont(const Font& font)
{
    _faceId = FreeTypeFontProvider::instance().findFaceId(font);
    _fontSize = font.size() == 0 ? DefaultFontSize : font.size();
}


FontMetrics FreeTypeRenderer::fontMetrics() const
{
    FreeTypeFontProvider& freetype = FreeTypeFontProvider::instance();

    FT_Face face = 0;
    FT_Error ferr = FTC_Manager_LookupFace(freetype._manager, _faceId, &face);
    if(ferr)
        return FontMetrics();

    double fontHeight = (face->height / double(face->ascender)) * _fontSize;

    FTC_ScalerRec scaler;
    scaler.face_id = _faceId;
    scaler.width = static_cast<unsigned>(fontHeight + 0.1);
    scaler.height = scaler.width;
    scaler.pixel = 1;

    FT_Size size;
    FTC_Manager_LookupSize(freetype._manager, &scaler, &size);

    double scaleY = size->metrics.y_scale / 65536.0;
    double asc = (face->ascender * scaleY) / 64.0;
    double des = (-face->descender * scaleY) / 64.0;
    double emh = (face->units_per_EM * scaleY) / 64.0;
    double lih = (face->height * scaleY) / 64.0;
    double cap = emh - des;
    double exl = lih - (asc + des);

    FontMetrics fm;
    fm.setAscent(asc);
    fm.setDescent(des);
    fm.setCapHeight(cap);
    fm.setLeading(exl);

    TT_OS2* os2 = static_cast<TT_OS2*>(FT_Get_Sfnt_Table(face, FT_SFNT_OS2));
    if(os2)
    {
        fm.setXHeight((os2->sxHeight * scaleY) / 64.0);

        if(os2->yStrikeoutSize > 0)
        {
            fm.setStrikeoutPos((os2->yStrikeoutPosition * scaleY) / 64.0);
            fm.setStrikeoutThickness((os2->yStrikeoutSize * scaleY) / 64.0);
        }
    }

    TT_Postscript* post = static_cast<TT_Postscript*>(FT_Get_Sfnt_Table(face, FT_SFNT_POST));
    if(post)
    {
        fm.setUnderlinePos((-post->underlinePosition * scaleY) / 64.0);
        fm.setUnderlineThickness((post->underlineThickness * scaleY) / 64.0);
    }

    return fm;
}


TextMetrics FreeTypeRenderer::textMetrics(const String& text) const
{
    FreeTypeFontProvider& freetype = FreeTypeFontProvider::instance();

    FT_Face face = 0;
    FT_Error ferr = FTC_Manager_LookupFace(freetype._manager, _faceId, &face);
    if(ferr)
        return TextMetrics();

    double fontHeight = (face->height / double(face->ascender)) * _fontSize;

    FTC_ImageTypeRec imageType;
    imageType.face_id = _faceId;
    imageType.height = static_cast<unsigned>(fontHeight + 0.1);
    imageType.width = imageType.height;
    imageType.flags = FT_LOAD_DEFAULT;

    FT_Int charMapIndex = 0;
    for(int n = 0; n < face->num_charmaps; ++n)
    {
        if(face->charmap[n].encoding == FT_ENCODING_UNICODE)
        {
            charMapIndex = n;
            break;
        }
    }

    FT_UInt previous = 0;
    FT_Vector delta;
    FT_Glyph glyph;

    int penX = 0;
    int bboxLeft = 0;
    int bboxRight = 0;
    int bboxTop = 0;
    int bboxBottom = 0;
    bool firstGlyph = true;

    for(String::const_iterator it = text.begin(); it != text.end(); ++it)
    {
        FT_UInt glyphIndex = FTC_CMapCache_Lookup(freetype._charMapCache,
                                                  _faceId,
                                                  charMapIndex,
                                                  it->value());

        if(!glyphIndex)
            glyphIndex = FTC_CMapCache_Lookup(freetype._charMapCache,
                                              _faceId,
                                              charMapIndex,
                                              63);

        if(!glyphIndex)
            continue;

        FTC_Node node;
        if(FTC_ImageCache_Lookup(freetype._imageCache, &imageType, glyphIndex,
                                 &glyph, &node))
            continue;

        if(FT_HAS_KERNING(face) && previous)
        {
            FT_Get_Kerning(face, previous, glyphIndex, FT_KERNING_DEFAULT, &delta);
            penX += delta.x >> 16;
        }

        FT_BBox cbox;
        FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &cbox);

        int glyphLeft = penX + static_cast<int>(cbox.xMin);
        int glyphRight = penX + static_cast<int>(cbox.xMax);
        int glyphTop = static_cast<int>(cbox.yMax);
        int glyphBottom = static_cast<int>(cbox.yMin);

        if(firstGlyph)
        {
            bboxLeft = glyphLeft;
            bboxRight = glyphRight;
            bboxTop = glyphTop;
            bboxBottom = glyphBottom;
            firstGlyph = false;
        }
        else
        {
            if(glyphLeft < bboxLeft) bboxLeft = glyphLeft;
            if(glyphRight > bboxRight) bboxRight = glyphRight;
            if(glyphTop > bboxTop) bboxTop = glyphTop;
            if(glyphBottom < bboxBottom) bboxBottom = glyphBottom;
        }

        penX += glyph->advance.x >> 16;
        previous = glyphIndex;
    }

    Gfx::TextMetrics fm;
    fm.setAdvance(penX);
    fm.setBearingX(bboxLeft);
    fm.setBearingY(bboxTop);
    fm.setBoundingWidth(bboxRight - bboxLeft);
    fm.setBoundingHeight(bboxTop - bboxBottom);
    return fm;
}


void FreeTypeRenderer::draw(Rgb32Image& image, Pt::ssize_t x, Pt::ssize_t y,
                            const String& text, const ColorF& color,
                            const RectI& clip, const CompositionMode& mode,
                            const Transform* tf) const
{
    FreeTypeFontProvider& freetype = FreeTypeFontProvider::instance();

    PointF translatedPos(static_cast<Float>(x), static_cast<Float>(y));
    if(tf)
    {
        translatedPos.addX(tf->dx());
        translatedPos.addY(tf->dy());
    }

    int xpos = lround(translatedPos.x());
    int ypos = lround(translatedPos.y());

    FT_Face face = 0;
    FT_Error ferr = FTC_Manager_LookupFace(freetype._manager, _faceId, &face);
    if(ferr)
        return;

    double fontHeight = (face->height / double(face->ascender)) * _fontSize;

    FTC_ImageTypeRec imageType;
    imageType.face_id = _faceId;
    imageType.height = static_cast<unsigned>(fontHeight + 0.1);
    imageType.width = imageType.height;
    imageType.flags = FT_LOAD_DEFAULT;

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
    glyphPos.x = xpos << 16;
    glyphPos.y = ypos << 16;

    FT_UInt previousIndex = 0;

    for(String::const_iterator it = text.begin(); it != text.end(); ++it)
    {
        FT_UInt glyphIndex = FTC_CMapCache_Lookup(freetype._charMapCache,
                                                  _faceId,
                                                  charMapIndex,
                                                  it->value());

        if(!glyphIndex)
            glyphIndex = FTC_CMapCache_Lookup(freetype._charMapCache,
                                              _faceId,
                                              charMapIndex,
                                              63);

        if(!glyphIndex)
            continue;

        int incX = 0;
        int incY = 0;
        FT_Glyph glyphCopy = 0;

        int left = 0;
        int top = 0;
        int pitch = 0;
        int height = 0;
        int width = 0;
        unsigned char* buffer = 0;

        if(!tf)
        {
            FTC_Node node = 0;
            FTC_SBit glyphBitmap = 0;
            if(FTC_SBitCache_Lookup(freetype._bitmapCache, &imageType, glyphIndex,
                                    &glyphBitmap, &node))
                continue;

            incX = glyphBitmap->xadvance << 16;
            incY = glyphBitmap->yadvance << 16;

            if(FT_HAS_KERNING(face) && previousIndex && face->size)
            {
                FT_Vector delta;
                FT_Get_Kerning(face, previousIndex, glyphIndex,
                               FT_KERNING_DEFAULT, &delta);

                glyphPos.x += delta.x;
                glyphPos.y -= delta.y;
            }

            left = (glyphPos.x >> 16) + glyphBitmap->left;
            top = (glyphPos.y >> 16) - glyphBitmap->top;
            pitch = glyphBitmap->pitch;
            height = glyphBitmap->height;
            width = glyphBitmap->width;
            buffer = glyphBitmap->buffer;
        }
        else
        {
            FTC_Node node = 0;
            FT_Glyph glyph = 0;

            FT_Matrix matrix;
            matrix.xx = static_cast<FT_Fixed>(tf->m11() * 0x10000L);
            matrix.xy = static_cast<FT_Fixed>(tf->m12() * 0x10000L);
            matrix.yx = static_cast<FT_Fixed>(tf->m21() * 0x10000L);
            matrix.yy = static_cast<FT_Fixed>(tf->m22() * 0x10000L);

            FT_Error err = FTC_ImageCache_Lookup(freetype._imageCache, &imageType,
                                                 glyphIndex, &glyph, &node);
            err += FT_Glyph_Copy(glyph, &glyphCopy);

            err += FT_Glyph_Transform(glyphCopy, &matrix, 0);
            err += FT_Glyph_To_Bitmap(&glyphCopy, FT_RENDER_MODE_NORMAL, 0, 1);
            if(err != 0)
                continue;

            FT_BitmapGlyph glyphBitmap = reinterpret_cast<FT_BitmapGlyph>(glyphCopy);

            incX = glyphCopy->advance.x;
            incY = glyphCopy->advance.y;

            if(FT_HAS_KERNING(face) && previousIndex && face->size)
            {
                FT_Vector delta;
                FT_Get_Kerning(face, previousIndex, glyphIndex,
                               FT_KERNING_DEFAULT, &delta);

                glyphPos.x += delta.x;
                glyphPos.y -= delta.y;
            }

            left = (glyphPos.x >> 16) + glyphBitmap->left;
            top = (glyphPos.y >> 16) - glyphBitmap->top;
            pitch = glyphBitmap->bitmap.pitch;
            height = glyphBitmap->bitmap.rows;
            width = glyphBitmap->bitmap.width;
            buffer = glyphBitmap->bitmap.buffer;
        }

        if(!isspace(*it))
            drawGlyph(image, left, top, color, pitch, height, width, buffer, clip, mode);

        glyphPos.x += incX;
        glyphPos.y -= incY;

        previousIndex = glyphIndex;

        if(glyphCopy)
            FT_Done_Glyph(glyphCopy);
    }
}


void FreeTypeRenderer::drawGlyph(Rgb32Image& image, int xpos, int ypos,
                                 const ColorF& color, int bmPitch, int height,
                                 int width, const unsigned char* buffer,
                                 const RectI& clip,
                                 const CompositionMode& mode) const
{
    const int clipRight = static_cast<int>(clip.x() + clip.width());
    const int clipBottom = static_cast<int>(clip.y() + clip.height());
    Pt::ssize_t yOffset = 0;
    Pt::ssize_t dsy = 0;
    Pt::ssize_t dsx = 0;
    const Pt::ssize_t x2 = clipRight;
    const Pt::ssize_t y2 = clipBottom;

    if(bmPitch < width)
        bmPitch += width;

    int ofsx = 0;

    if(xpos < clip.x())
    {
        ofsx = static_cast<int>(clip.x()) - xpos;
        xpos = static_cast<int>(clip.x());
    }

    int ofsy = 0;

    if(ypos < clip.y())
    {
        ofsy = static_cast<int>(clip.y()) - ypos;
        ypos = static_cast<int>(clip.y());
    }

    dsy = ypos;

    const Pt::uint8_t ca = static_cast<Pt::uint8_t>(color.alpha() >> 8);
    const Pt::uint8_t cr = static_cast<Pt::uint8_t>(color.red() >> 8);
    const Pt::uint8_t cg = static_cast<Pt::uint8_t>(color.green() >> 8);
    const Pt::uint8_t cb = static_cast<Pt::uint8_t>(color.blue() >> 8);
    const Rgb32Color penColor(ca,
                              static_cast<Pt::uint8_t>((cr * ca + 127) / 255),
                              static_cast<Pt::uint8_t>((cg * ca + 127) / 255),
                              static_cast<Pt::uint8_t>((cb * ca + 127) / 255));

    Rgb32View rgb32View(image);

    for(Pt::int32_t y = ofsy; y < height; ++y, ++dsy)
    {
        yOffset = y * bmPitch;

        if(dsy < clip.y())
            continue;

        if(dsy >= y2)
            break;

        dsx = xpos;

        for(Pt::int32_t x = ofsx; x < width; ++x, ++dsx)
        {
            if(dsx < clip.x())
                continue;

            if(dsx >= x2)
                break;

            Rgb32Pixel pixel(rgb32View, dsx, dsy);

            const Int px = yOffset + x;
            unsigned char value = buffer[px];

            if(value == 0)
                continue;

            Rgb32Color glyphColor(
                static_cast<Pt::uint8_t>(penColor.alpha() * value / 255),
                static_cast<Pt::uint8_t>(penColor.red() * value / 255),
                static_cast<Pt::uint8_t>(penColor.green() * value / 255),
                static_cast<Pt::uint8_t>(penColor.blue() * value / 255));

            switch(mode)
            {
                default:
                case CompositionMode::SourceCopy:
                    if(value == 255)
                        sourceCopy(pixel, 1, glyphColor);
                    else
                        sourceOver(pixel, 1, glyphColor);
                    break;

                case CompositionMode::SourceOver:
                    sourceOver(pixel, 1, glyphColor);
                    break;
            }
        }
    }
}

} // namespace

} // namespace