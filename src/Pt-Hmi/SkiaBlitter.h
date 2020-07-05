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

#ifndef PT_HMI_SKIAPBLITTER_H
#define PT_HMI_SKIAPBLITTER_H

#include <SkSurface.h>
#include <SkImage.h>
#include <SkCanvas.h>
#include <SkFont.h>
#include <SkBlitter.h>
#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/System/Path.h>

namespace Pt {

namespace Hmi {


class SkiaBlitter : public SkBlitter {
public:
    SkiaBlitter(const SkPixmap& device, const SkPaint& paint, Gfx::Image& image);


    void blitH(int x, int y, int width) override;
    void blitAntiH(int x, int y, const SkAlpha antialias[], const int16_t runs[]) override;
    void blitV(int x, int y, int height, SkAlpha alpha) override;
    void blitRect(int x, int y, int width, int height) override;
    void blitMask(const SkMask&, const SkIRect&) override;
    const SkPixmap* justAnOpaqueColor(uint32_t*) override;
    void blitAntiH2(int x, int y, U8CPU a0, U8CPU a1) override;
    void blitAntiV2(int x, int y, U8CPU a0, U8CPU a1) override;

protected:
    const SkPixmap& _device;

    Pt::Gfx::Image& _image;
    Pt::Gfx::Color _color;
    uint32_t _pmColor;
};

SkBlitter* SkCreateBlitter(const SkPixmap& device, const SkPaint& paint, SkArenaAlloc* alloc, void* context);

}
}

#endif