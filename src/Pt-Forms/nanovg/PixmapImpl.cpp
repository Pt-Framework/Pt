/* Copyright (C) 2026 Marc Boris Duerner

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include "PixmapImpl.h"
#include "PixmapCanvas.h"
#include "NanoVGDevice.h"

#include <Pt/Forms/Pixmap.h>

#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Rgb32.h>
#include <Pt/Gfx/PaintContext.h>
#include <Pt/Gfx/Painter.h>

#include <GLES2/gl2.h>

#include "nanovg.h"
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"

#include <cmath>
#include <cassert>
#include <vector>

namespace Pt {

namespace Forms {

PixmapImpl::PixmapImpl()
: _physicalSize(0, 0)
, _width(0)
, _height(0)
, _fb(0)
, _canvas(0)
{
}


PixmapImpl::~PixmapImpl()
{
    destroyFramebuffer();
}


void PixmapImpl::createFramebuffer(int width, int height)
{
    destroyFramebuffer();

    if(width <= 0 || height <= 0)
        return;

    NanoVGDevice* device = NanoVGDevice::instance();
    if( ! device || ! device->isValid())
        return;

    device->makeCurrentOffscreen();

    _fb = nvgluCreateFramebuffer(device->context(), width, height, 0);
    if( ! _fb)
        return;

    _width = width;
    _height = height;
    _physicalSize.set(width, height);

    // Clear the new framebuffer to fully transparent.
    nvgluBindFramebuffer(_fb);
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    nvgluBindFramebuffer(0);
}


void PixmapImpl::destroyFramebuffer()
{
    if(_fb)
    {
        NanoVGDevice* device = NanoVGDevice::instance();
        if(device && device->isValid())
        {
            device->makeCurrentOffscreen();
            nvgluDeleteFramebuffer(_fb);
        }

        _fb = 0;
    }

    _width = 0;
    _height = 0;
    _physicalSize.set(0, 0);
}


void PixmapImpl::reset(const Gfx::SizeF& size)
{
    int width = static_cast<int>( std::lround(size.width()) );
    int height = static_cast<int>( std::lround(size.height()) );

    if(_width == width && _height == height)
        return;

    createFramebuffer(width, height);
}


void PixmapImpl::reset()
{
    destroyFramebuffer();
}


void PixmapImpl::reset(const Gfx::Image& image)
{
    const std::size_t width = image.width();
    const std::size_t height = image.height();

    reset(Gfx::SizeF(width, height));

    if( ! _fb)
        return;

    NanoVGDevice* device = NanoVGDevice::instance();
    if( ! device || ! device->isValid())
        return;

    NVGcontext* vg = device->context();

    // Convert Pt rgb32 (premultiplied BGRA) to nanovg RGBA.
    const Pt::uint8_t* src = image.data();
    Gfx::Rgb32Image rgb32Image;
    if(image.format() != Gfx::ImageFormat::rgb32() || image.padding() != 0)
    {
        rgb32Image.reset(width, height);
        Gfx::copyView(image, rgb32Image);
        src = rgb32Image.data();
    }

    std::vector<unsigned char> rgba(width * height * 4);
    for(std::size_t i = 0; i < width * height; ++i)
    {
        rgba[i * 4 + 0] = src[i * 4 + 2]; // R <- B
        rgba[i * 4 + 1] = src[i * 4 + 1]; // G
        rgba[i * 4 + 2] = src[i * 4 + 0]; // B <- R
        rgba[i * 4 + 3] = src[i * 4 + 3]; // A
    }

    device->makeCurrentOffscreen();

    int srcImage = nvgCreateImageRGBA(vg, static_cast<int>(width),
                                      static_cast<int>(height),
                                      NVG_IMAGE_PREMULTIPLIED, rgba.data());
    if(srcImage < 0)
        return;

    nvgluBindFramebuffer(_fb);
    glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(vg, static_cast<float>(width), static_cast<float>(height), 1.0f);
    NVGpaint paint = nvgImagePattern(vg, 0.0f, 0.0f,
                                     static_cast<float>(width),
                                     static_cast<float>(height),
                                     0.0f, srcImage, 1.0f);
    nvgBeginPath(vg);
    nvgRect(vg, 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    nvgFillPaint(vg, paint);
    nvgFill(vg);
    nvgEndFrame(vg);

    nvgluBindFramebuffer(0);
    nvgDeleteImage(vg, srcImage);
}


Gfx::Image PixmapImpl::toImage() const
{
    if(_width == 0 || _height == 0 || ! _fb)
        return Gfx::Image();

    NanoVGDevice* device = NanoVGDevice::instance();
    if( ! device || ! device->isValid())
        return Gfx::Image();

    device->makeCurrentOffscreen();
    nvgluBindFramebuffer(_fb);

    std::vector<unsigned char> buffer(_width * _height * 4);
    glReadPixels(0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());

    nvgluBindFramebuffer(0);

    // GL framebuffers have a bottom-left origin, Pt images a top-left origin.
    // Flip rows and swizzle RGBA to Pt rgb32 (BGRA).
    Gfx::Image image(_width, _height, Gfx::Rgb32());
    Pt::uint8_t* dst = image.data();

    for(int y = 0; y < _height; ++y)
    {
        const unsigned char* srcRow = buffer.data() + (_height - 1 - y) * _width * 4;
        Pt::uint8_t* dstRow = dst + y * _width * 4;

        for(int x = 0; x < _width; ++x)
        {
            dstRow[x * 4 + 0] = srcRow[x * 4 + 2]; // B <- R
            dstRow[x * 4 + 1] = srcRow[x * 4 + 1]; // G
            dstRow[x * 4 + 2] = srcRow[x * 4 + 0]; // R <- B
            dstRow[x * 4 + 3] = srcRow[x * 4 + 3]; // A
        }
    }

    return image;
}


void PixmapImpl::getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const
{
    bitmap.reset( rect.size() );

    Gfx::Image image = this->toImage();

    Gfx::PaintContext ctx(bitmap);
    Gfx::Painter painter(ctx);
    painter.drawImage(Gfx::PointF(0, 0), image, rect);
}


int PixmapImpl::framebufferImage() const
{
    return _fb ? _fb->image : -1;
}


const Gfx::SizeF& PixmapImpl::size() const
{
    return _physicalSize;
}


void PixmapImpl::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);
}


const Gfx::ImageFormat& PixmapImpl::format() const
{
    return Gfx::ImageFormat::rgb32();
}


const Gfx::Scaling& PixmapImpl::scaling() const
{
    return _scaling;
}


Gfx::Canvas* PixmapImpl::createCanvas(Gfx::Canvas* reuse)
{
    PixmapCanvas* canvas = dynamic_cast<PixmapCanvas*>(reuse);
    if( ! canvas)
        canvas = new PixmapCanvas();

    canvas->setPixmap(*this);
    _canvas = canvas;
    return _canvas;
}


void PixmapImpl::releaseCanvas()
{
    _canvas = 0;
}


void PixmapImpl::drawPixmap(Gfx::Canvas& canvas,
                            const Gfx::PointF& to,
                            const Pixmap& pm,
                            const Gfx::RectF* rect)
{
    assert(_canvas == &canvas);

    if(_canvas == &canvas)
        _canvas->drawPixmap(to, pm, rect);
}

} // namespace

} // namespace
