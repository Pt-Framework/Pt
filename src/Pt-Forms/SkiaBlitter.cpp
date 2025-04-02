/* Copyright (C) 2020 Marc Boris Duerner
Copyright (C) 2020 Laurentiu-Gheorghe Crisan

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

#include "SkiaBlitter.h"

namespace Pt {
namespace Forms {

SkiaBlitter::SkiaBlitter(const SkPixmap& device, const SkPaint& paint, Gfx::Image& image)
: _device(device)
, _image(image)
{
    SkColor color = paint.getColor();

    unsigned fSrcA = SkColorGetA(color);

    unsigned scale = SkAlpha255To256(fSrcA);

    unsigned fSrcR = SkAlphaMul(SkColorGetR(color), scale);
    unsigned fSrcG = SkAlphaMul(SkColorGetG(color), scale);
    unsigned fSrcB = SkAlphaMul(SkColorGetB(color), scale);
    
    _color = Pt::Gfx::Color::fromRgb8(SkColorGetR(color), SkColorGetG(color), SkColorGetB(color), fSrcA);

    _pmColor = SkPackARGB32(fSrcA, fSrcR, fSrcG, fSrcB);
}


void SkiaBlitter::blitH(int x, int y, int width)
{
    _image.format().setPixels(*_image.pixel(x, y), _color, width, Pt::Gfx::CompositionMode::Mode::SourceCopy);
}

void SkiaBlitter::blitAntiH(int x, int y, const SkAlpha antialias[], const int16_t runs[])
{
    if (_color.alpha() == 0) 
        return;

    uint32_t    color = _pmColor;
    Pt::Gfx::Image::PixelIterator device = _image.pixel(x, y);
    unsigned    opaqueMask = _color.alpha() / 256;
   
    size_t pos = 0;

    while(runs[pos] > 0)
    {
        const unsigned aa = antialias[pos];
        int count = runs[pos];
       
        if (aa)
        {
            for (size_t i = 0; i < count; ++i)
            {
                if ((opaqueMask & aa) == 255)
                    _image.format().setPixel(*device, _color,Pt::Gfx::CompositionMode::Mode::SourceCopy, aa);
                else
                    _image.format().setPixel(*device, _color, Pt::Gfx::CompositionMode::Mode::SourceOver, aa);

                ++device;
            }
        }
        else
        {
            device += count;
        }

        pos += count;
    }
}


void SkiaBlitter::blitV(int x, int y, int height, SkAlpha alpha)
{
    if (alpha == 0 || _color.alpha() == 0) 
        return;
    

    Pt::Gfx::Image::PixelIterator device = _image.pixel(x, y);

    Pt::Gfx::Color color = _color;

    color.setAlpha(alpha* 256);
    
    size_t width = _image.width();

    while (--height >= 0) 
    {
        _image.format().setPixel(*device, color, Pt::Gfx::CompositionMode::Mode::SourceOver);

         device  +=  width;
    }
}


void SkiaBlitter::blitRect(int x, int y, int width, int height)
{
    if (_color.alpha() == 0)
        return;

    Pt::Gfx::Image::PixelIterator device = _image.pixel(x, y);

    size_t  row= _image.width();

    while (--height >= 0) 
    {
        _image.format().setPixels(*device, _color, width, Pt::Gfx::CompositionMode::Mode::SourceOver);
        device += row;
    }
}

void SkiaBlitter::blitMask(const SkMask& mask, const SkIRect& clip)
{
    if (_color.alpha() == 0)
        return;

    int x = clip.fLeft;
    int y = clip.fTop;
    int width = clip.width();
    int height = clip.height();

    Pt::Gfx::Image::PixelIterator dstRow = _image.pixel(x, y);

    switch (mask.fFormat)
    {
        case SkMask::kA8_Format:
        {
            const uint8_t* srcRow = reinterpret_cast<const uint8_t*>(mask.getAddr8(x, y));

            do
            {

                Pt::Gfx::Image::PixelIterator p2 = dstRow;

                for (size_t i = 0; i < mask.fRowBytes; ++i)
                {
                    _image.format().setPixel(*p2, _color, Pt::Gfx::CompositionMode::Mode::SourceOver, srcRow[i]);
                    ++p2;
                }


                dstRow += _image.width();
                srcRow += mask.fRowBytes;

            } while (--height != 0);
        }
        break;
    }

}

const SkPixmap* SkiaBlitter::justAnOpaqueColor(uint32_t* value)
{
    if (_color.alpha() != 0xFFFF)
        return 0;
    
    *value = _pmColor;
    return &_device;
}

void SkiaBlitter::blitAntiH2(int x, int y, U8CPU a0, U8CPU a1)
{
    Pt::Gfx::Image::PixelIterator p = _image.pixel(x, y);

    _image.format().setPixel(*p, _color, Pt::Gfx::CompositionMode::Mode::SourceCopy, a0);
    ++p;
    _image.format().setPixel(*p, _color, Pt::Gfx::CompositionMode::Mode::SourceCopy, a1);
}

void SkiaBlitter::blitAntiV2(int x, int y, U8CPU a0, U8CPU a1)
{
    Pt::Gfx::Image::PixelIterator p = _image.pixel(x, y);

    _image.format().setPixel(*p, _color, Pt::Gfx::CompositionMode::Mode::SourceCopy, a0);
     p += _image.width();
    _image.format().setPixel(*p, _color, Pt::Gfx::CompositionMode::Mode::SourceCopy, a1);
}


SkBlitter* SkCreateBlitter(const SkPixmap& device, const SkPaint& paint, SkArenaAlloc* alloc, void* context)
{
    Pt::Gfx::Image* image = (Pt::Gfx::Image*) context;
    return  alloc->make<Pt::Forms::SkiaBlitter>(device, paint, *image);
}


}}
