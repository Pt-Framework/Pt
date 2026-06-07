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

#include <Pt/System/Logger.h>

#include <cmath>
#include <cassert>
#include <vector>

PT_LOG_DEFINE("Pt.Forms.PixmapImpl");

namespace {

// Converts a Pt::Gfx::Image to a tightly packed RGBA byte buffer (nanovg
// byte order). Pt rgb32 stores premultiplied BGRA, so R/B channels are
// swapped.
void buildRgba(const Pt::Gfx::Image& image,
               std::vector<unsigned char>& out,
               int& width,
               int& height)
{
    namespace Gfx = Pt::Gfx;

    width  = static_cast<int>(image.width());
    height = static_cast<int>(image.height());

    const Pt::uint8_t* data = image.data();
    Gfx::Rgb32Image rgb32;

    if(image.format() != Gfx::ImageFormat::rgb32() || image.padding() != 0)
    {
        rgb32.reset(image.width(), image.height());
        Gfx::copyView(image, rgb32);
        data = rgb32.data();
    }

    const std::size_t count = static_cast<std::size_t>(width) * height;
    out.resize(count * 4);

    for(std::size_t i = 0; i < count; ++i)
    {
        out[i * 4 + 0] = data[i * 4 + 2];
        out[i * 4 + 1] = data[i * 4 + 1];
        out[i * 4 + 2] = data[i * 4 + 0];
        out[i * 4 + 3] = data[i * 4 + 3];
    }
}


// Applies the brush as the current nanovg fill for the given shape bounds.
// Mirrors PixmapCanvas::applyFillForBounds but operates on a caller-supplied
// NVGcontext so it can be used during replay.
void replayFill(NVGcontext* vg,
                const Pt::Gfx::Brush& brush,
                int& textureImage,
                const Pt::Gfx::RectF& bounds)
{
    namespace Gfx = Pt::Gfx;

    if(brush.isTexture())
    {
        if(textureImage < 0)
        {
            const Gfx::Image& tex = brush.texture();
            if(tex.width() != 0 && tex.height() != 0)
            {
                std::vector<unsigned char> rgba;
                int tw = 0;
                int th = 0;
                buildRgba(tex, rgba, tw, th);
                textureImage = nvgCreateImageRGBA(vg, tw, th,
                    NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY | NVG_IMAGE_PREMULTIPLIED,
                    &rgba[0]);
            }
        }

        if(textureImage >= 0)
        {
            int tw = 0;
            int th = 0;
            nvgImageSize(vg, textureImage, &tw, &th);

            const Gfx::PointF& origin = brush.textureOrigin();
            NVGpaint paint = nvgImagePattern(vg,
                static_cast<float>(origin.x()), static_cast<float>(origin.y()),
                static_cast<float>(tw), static_cast<float>(th),
                0.0f, textureImage, 1.0f);
            nvgFillPaint(vg, paint);
            return;
        }

        NVGcolor c;
        c.r = brush.color().red()   / 255.0f;
        c.g = brush.color().green() / 255.0f;
        c.b = brush.color().blue()  / 255.0f;
        c.a = brush.color().alpha() / 255.0f;
        nvgFillColor(vg, c);
        return;
    }

    if(brush.isGradient())
    {
        const Gfx::ColorStops& stops = brush.gradientStops();

        NVGcolor inner;
        NVGcolor outer;
        if(stops.size() >= 2)
        {
            inner = nvgRGBA(
                static_cast<unsigned char>(stops.front().color().red()),
                static_cast<unsigned char>(stops.front().color().green()),
                static_cast<unsigned char>(stops.front().color().blue()),
                static_cast<unsigned char>(stops.front().color().alpha()));
            outer = nvgRGBA(
                static_cast<unsigned char>(stops.back().color().red()),
                static_cast<unsigned char>(stops.back().color().green()),
                static_cast<unsigned char>(stops.back().color().blue()),
                static_cast<unsigned char>(stops.back().color().alpha()));
        }
        else
        {
            inner = nvgRGBA(
                static_cast<unsigned char>(brush.color().red()),
                static_cast<unsigned char>(brush.color().green()),
                static_cast<unsigned char>(brush.color().blue()),
                static_cast<unsigned char>(brush.color().alpha()));
            outer = nvgRGBA(
                static_cast<unsigned char>(brush.gradientColor().red()),
                static_cast<unsigned char>(brush.gradientColor().green()),
                static_cast<unsigned char>(brush.gradientColor().blue()),
                static_cast<unsigned char>(brush.gradientColor().alpha()));
        }

        const bool relative = brush.positionMode() == Gfx::Brush::Relative;
        const float bx = static_cast<float>(bounds.x());
        const float by = static_cast<float>(bounds.y());
        const float bw = static_cast<float>(bounds.width());
        const float bh = static_cast<float>(bounds.height());

        const Gfx::Brush::GradientStyle style = brush.gradient();

        if(style == Gfx::Brush::Radial)
        {
            const Gfx::PointF& center = brush.gradientEnd();
            float cx;
            float cy;
            float radius = brush.gradientEndRadius();

            if(relative)
            {
                cx = bx + static_cast<float>(center.x()) * bw;
                cy = by + static_cast<float>(center.y()) * bh;
                radius *= (bw + bh) * 0.5f;
            }
            else
            {
                cx = static_cast<float>(center.x());
                cy = static_cast<float>(center.y());
            }

            NVGpaint paint = nvgRadialGradient(vg, cx, cy, 0.0f, radius, inner, outer);
            nvgFillPaint(vg, paint);
            return;
        }

        float sx;
        float sy;
        float ex;
        float ey;

        if(style == Gfx::Brush::Horizontal)
        {
            sx = bx;      sy = by;
            ex = bx + bw; ey = by;
        }
        else if(style == Gfx::Brush::Vertical)
        {
            sx = bx; sy = by;
            ex = bx; ey = by + bh;
        }
        else
        {
            const Gfx::PointF& begin = brush.gradientBegin();
            const Gfx::PointF& end   = brush.gradientEnd();

            if(relative)
            {
                sx = bx + static_cast<float>(begin.x()) * bw;
                sy = by + static_cast<float>(begin.y()) * bh;
                ex = bx + static_cast<float>(end.x())   * bw;
                ey = by + static_cast<float>(end.y())   * bh;
            }
            else
            {
                sx = static_cast<float>(begin.x());
                sy = static_cast<float>(begin.y());
                ex = static_cast<float>(end.x());
                ey = static_cast<float>(end.y());
            }
        }

        NVGpaint paint = nvgLinearGradient(vg, sx, sy, ex, ey, inner, outer);
        nvgFillPaint(vg, paint);
        return;
    }

    NVGcolor c;
    c.r = brush.color().red()   / 255.0f;
    c.g = brush.color().green() / 255.0f;
    c.b = brush.color().blue()  / 255.0f;
    c.a = brush.color().alpha() / 255.0f;
    nvgFillColor(vg, c);
}


// Builds an nvg path from a Gfx::Path.
void buildPath(NVGcontext* vg, const Pt::Gfx::Path& path)
{
    namespace Gfx = Pt::Gfx;

    nvgBeginPath(vg);

    for(Gfx::PathIterator it = path.begin(); it != path.end(); ++it)
    {
        switch(it->type())
        {
            default:
                break;

            case Gfx::Path::MoveTo:
            {
                const Gfx::PointF& to = it->point(0);
                nvgMoveTo(vg, static_cast<float>(to.x()), static_cast<float>(to.y()));
                break;
            }

            case Gfx::Path::LineTo:
            {
                const Gfx::PointF& to = it->point(0);
                nvgLineTo(vg, static_cast<float>(to.x()), static_cast<float>(to.y()));
                break;
            }

            case Gfx::Path::QuadTo:
            {
                const Gfx::PointF& cp = it->point(0);
                const Gfx::PointF& to = it->point(1);
                nvgQuadTo(vg,
                          static_cast<float>(cp.x()), static_cast<float>(cp.y()),
                          static_cast<float>(to.x()), static_cast<float>(to.y()));
                break;
            }

            case Gfx::Path::CubicTo:
            {
                const Gfx::PointF& c1 = it->point(0);
                const Gfx::PointF& c2 = it->point(1);
                const Gfx::PointF& to = it->point(2);
                nvgBezierTo(vg,
                            static_cast<float>(c1.x()), static_cast<float>(c1.y()),
                            static_cast<float>(c2.x()), static_cast<float>(c2.y()),
                            static_cast<float>(to.x()), static_cast<float>(to.y()));
                break;
            }

            case Gfx::Path::Close:
                nvgClosePath(vg);
                break;
        }
    }
}


// Returns the bounding box of all control points in a Gfx::Path.
Pt::Gfx::RectF pathBounds(const Pt::Gfx::Path& path)
{
    namespace Gfx = Pt::Gfx;

    bool  first = true;
    double minX = 0.0;
    double minY = 0.0;
    double maxX = 0.0;
    double maxY = 0.0;

    for(Gfx::PathIterator it = path.begin(); it != path.end(); ++it)
    {
        int pts = 0;
        switch(it->type())
        {
            case Gfx::Path::MoveTo:
            case Gfx::Path::LineTo:  pts = 1; break;
            case Gfx::Path::QuadTo:  pts = 2; break;
            case Gfx::Path::CubicTo: pts = 3; break;
            default:                 pts = 0; break;
        }

        for(int i = 0; i < pts; ++i)
        {
            const Gfx::PointF& p = it->point(i);
            if(first)
            {
                minX = maxX = p.x();
                minY = maxY = p.y();
                first = false;
            }
            else
            {
                if(p.x() < minX) minX = p.x();
                if(p.y() < minY) minY = p.y();
                if(p.x() > maxX) maxX = p.x();
                if(p.y() > maxY) maxY = p.y();
            }
        }
    }

    return Gfx::RectF::fromXYWH(minX, minY, maxX - minX, maxY - minY);
}

} // anonymous namespace

namespace Pt {

namespace Forms {

PixmapImpl::PixmapImpl()
: _physicalSize(0, 0)
, _width(0)
, _height(0)
, _image(-1)
, _canvas(0)
{
}


PixmapImpl::~PixmapImpl()
{
    destroyTexture();
}


void PixmapImpl::createTexture(int width, int height)
{
    destroyTexture();

    if(width <= 0 || height <= 0)
        return;

    NanoVGDevice* device = NanoVGDevice::instance();
    if( ! device || ! device->isValid())
        return;

    device->makeCurrentOffscreen();

    _image = device->createImage(width, height);
    if(_image < 0)
        return;

    _width  = width;
    _height = height;
    _physicalSize.set(width, height);

    // Clear the new texture to fully transparent.
    if( device->bindRenderTarget(_image, width, height) )
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        device->unbindRenderTarget();
    }
}


void PixmapImpl::destroyTexture()
{
    if(_image >= 0)
    {
        NanoVGDevice* device = NanoVGDevice::instance();
        if(device && device->isValid())
        {
            device->makeCurrentOffscreen();
            nvgDeleteImage(device->context(), _image);
        }

        _image = -1;
    }

    _commands.clear();
    _width  = 0;
    _height = 0;
    _physicalSize.set(0, 0);
}


void PixmapImpl::reset(const Gfx::SizeF& size)
{
    int width  = static_cast<int>( std::lround(size.width()) );
    int height = static_cast<int>( std::lround(size.height()) );

    if(_width == width && _height == height)
        return;

    createTexture(width, height);
}


void PixmapImpl::reset()
{
    destroyTexture();
}


void PixmapImpl::reset(const Gfx::Image& image)
{
    const std::size_t width  = image.width();
    const std::size_t height = image.height();

    reset(Gfx::SizeF(width, height));

    if(_image < 0)
        return;

    NanoVGDevice* device = NanoVGDevice::instance();
    if( ! device || ! device->isValid())
        return;

    NVGcontext* vg = device->context();

    // Convert Pt rgb32 (premultiplied BGRA) to nanovg RGBA.
    std::vector<unsigned char> rgba;
    int iw = 0;
    int ih = 0;
    buildRgba(image, rgba, iw, ih);

    device->makeCurrentOffscreen();

    int srcImage = nvgCreateImageRGBA(vg,
                                      static_cast<int>(width),
                                      static_cast<int>(height),
                                      NVG_IMAGE_PREMULTIPLIED,
                                      rgba.data());
    if(srcImage < 0)
        return;

    if( device->bindRenderTarget(_image,
                                  static_cast<int>(width),
                                  static_cast<int>(height)) )
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        nvgBeginFrame(vg,
                      static_cast<float>(width),
                      static_cast<float>(height),
                      1.0f);

        NVGpaint paint = nvgImagePattern(vg,
                                         0.0f, 0.0f,
                                         static_cast<float>(width),
                                         static_cast<float>(height),
                                         0.0f, srcImage, 1.0f);
        nvgBeginPath(vg);
        nvgRect(vg, 0.0f, 0.0f,
                static_cast<float>(width), static_cast<float>(height));
        nvgFillPaint(vg, paint);
        nvgFill(vg);
        nvgEndFrame(vg);

        device->unbindRenderTarget();
    }

    nvgDeleteImage(vg, srcImage);
}


void PixmapImpl::flush()
{
    if(_commands.empty() || _image < 0)
        return;

    NanoVGDevice* device = NanoVGDevice::instance();
    if( ! device || ! device->isValid())
        return;

    if( ! device->bindRenderTarget(_image, _width, _height) )
        return;

    NVGcontext* vg = device->context();

    nvgBeginFrame(vg, static_cast<float>(_width), static_cast<float>(_height), 1.0f);

    // Current state tracked across state commands for fill/text replay.
    Gfx::Brush currentBrush;
    NVGcolor   currentPenColor = nvgRGBA(0, 0, 0, 255);

    // Transient texture image created for the current brush; deleted after
    // each SetBrush that changes the brush, and at the end of the frame.
    int brushTexImage = -1;

    for(std::size_t i = 0; i < _commands.size(); ++i)
    {
        const PaintCommand& c = _commands[i];

        switch(c.type)
        {
            case PaintCommand::SetTransform:
            {
                const Gfx::Transform& tx = c.transform;
                nvgResetTransform(vg);
                nvgTransform(vg,
                             static_cast<float>(tx.m11()), static_cast<float>(tx.m12()),
                             static_cast<float>(tx.m21()), static_cast<float>(tx.m22()),
                             static_cast<float>(tx.dx()),  static_cast<float>(tx.dy()));
                break;
            }

            case PaintCommand::SetCompositionMode:
                nvgGlobalCompositeOperation(vg, c.compositionMode);
                break;

            case PaintCommand::SetClip:
                if(c.hasClip)
                {
                    nvgScissor(vg,
                               static_cast<float>(c.clip.x()),
                               static_cast<float>(c.clip.y()),
                               static_cast<float>(c.clip.width()),
                               static_cast<float>(c.clip.height()));
                }
                else
                {
                    nvgResetScissor(vg);
                }
                break;

            case PaintCommand::SetPen:
                nvgStrokeColor(vg, c.penColor);
                nvgStrokeWidth(vg, c.penWidth);
                nvgLineCap(vg, c.lineCap);
                nvgLineJoin(vg, c.lineJoin);
                currentPenColor = c.penColor;
                break;

            case PaintCommand::SetBrush:
                if(brushTexImage >= 0)
                {
                    nvgDeleteImage(vg, brushTexImage);
                    brushTexImage = -1;
                }
                currentBrush = c.brush;
                break;

            case PaintCommand::SetFont:
                nvgFontFaceId(vg, c.fontFace);
                nvgFontSize(vg, c.fontSize);
                break;

            case PaintCommand::DrawLine:
                nvgBeginPath(vg);
                nvgMoveTo(vg, static_cast<float>(c.p0.x()), static_cast<float>(c.p0.y()));
                nvgLineTo(vg, static_cast<float>(c.p1.x()), static_cast<float>(c.p1.y()));
                nvgStroke(vg);
                break;

            case PaintCommand::DrawPolyline:
                if(c.points.size() >= 2)
                {
                    nvgBeginPath(vg);
                    nvgMoveTo(vg,
                              static_cast<float>(c.points[0].x()),
                              static_cast<float>(c.points[0].y()));
                    for(std::size_t j = 1; j < c.points.size(); ++j)
                    {
                        nvgLineTo(vg,
                                  static_cast<float>(c.points[j].x()),
                                  static_cast<float>(c.points[j].y()));
                    }
                    nvgStroke(vg);
                }
                break;

            case PaintCommand::FillPolygon:
                if(c.points.size() >= 3)
                {
                    double minX = c.points[0].x();
                    double minY = c.points[0].y();
                    double maxX = minX;
                    double maxY = minY;

                    nvgBeginPath(vg);
                    nvgMoveTo(vg,
                              static_cast<float>(c.points[0].x()),
                              static_cast<float>(c.points[0].y()));
                    for(std::size_t j = 1; j < c.points.size(); ++j)
                    {
                        nvgLineTo(vg,
                                  static_cast<float>(c.points[j].x()),
                                  static_cast<float>(c.points[j].y()));
                        if(c.points[j].x() < minX) minX = c.points[j].x();
                        if(c.points[j].y() < minY) minY = c.points[j].y();
                        if(c.points[j].x() > maxX) maxX = c.points[j].x();
                        if(c.points[j].y() > maxY) maxY = c.points[j].y();
                    }
                    nvgClosePath(vg);
                    replayFill(vg, currentBrush, brushTexImage,
                               Gfx::RectF::fromXYWH(minX, minY,
                                                    maxX - minX, maxY - minY));
                    nvgFill(vg);
                }
                break;

            case PaintCommand::DrawRect:
                nvgBeginPath(vg);
                nvgRect(vg,
                        static_cast<float>(c.rect.x()),
                        static_cast<float>(c.rect.y()),
                        static_cast<float>(c.rect.width()),
                        static_cast<float>(c.rect.height()));
                nvgStroke(vg);
                break;

            case PaintCommand::FillRect:
                nvgBeginPath(vg);
                nvgRect(vg,
                        static_cast<float>(c.rect.x()),
                        static_cast<float>(c.rect.y()),
                        static_cast<float>(c.rect.width()),
                        static_cast<float>(c.rect.height()));
                replayFill(vg, currentBrush, brushTexImage, c.rect);
                nvgFill(vg);
                break;

            case PaintCommand::DrawEllipse:
                nvgBeginPath(vg);
                nvgEllipse(vg,
                           static_cast<float>(c.p0.x() + c.ellipseSize.width()  / 2.0),
                           static_cast<float>(c.p0.y() + c.ellipseSize.height() / 2.0),
                           static_cast<float>(c.ellipseSize.width()  / 2.0),
                           static_cast<float>(c.ellipseSize.height() / 2.0));
                nvgStroke(vg);
                break;

            case PaintCommand::FillEllipse:
                nvgBeginPath(vg);
                nvgEllipse(vg,
                           static_cast<float>(c.p0.x() + c.ellipseSize.width()  / 2.0),
                           static_cast<float>(c.p0.y() + c.ellipseSize.height() / 2.0),
                           static_cast<float>(c.ellipseSize.width()  / 2.0),
                           static_cast<float>(c.ellipseSize.height() / 2.0));
                replayFill(vg, currentBrush, brushTexImage,
                           Gfx::RectF::fromXYWH(c.p0.x(), c.p0.y(),
                                                c.ellipseSize.width(),
                                                c.ellipseSize.height()));
                nvgFill(vg);
                break;

            case PaintCommand::DrawPath:
                buildPath(vg, c.path);
                nvgStroke(vg);
                break;

            case PaintCommand::FillPath:
                buildPath(vg, c.path);
                replayFill(vg, currentBrush, brushTexImage, pathBounds(c.path));
                nvgFill(vg);
                break;

            case PaintCommand::DrawText:
            {
                nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
                nvgFillColor(vg, currentPenColor);

                if(c.hasTextTransform)
                {
                    nvgSave(vg);
                    nvgTranslate(vg,
                                 static_cast<float>(c.p0.x()),
                                 static_cast<float>(c.p0.y()));
                    const Gfx::Transform& tx = c.textTransform;
                    nvgTransform(vg,
                                 static_cast<float>(tx.m11()), static_cast<float>(tx.m12()),
                                 static_cast<float>(tx.m21()), static_cast<float>(tx.m22()),
                                 static_cast<float>(tx.dx()),  static_cast<float>(tx.dy()));
                    nvgText(vg, 0.0f, 0.0f, c.text.c_str(), 0);
                    nvgRestore(vg);
                }
                else
                {
                    nvgText(vg,
                            static_cast<float>(c.p0.x()),
                            static_cast<float>(c.p0.y()),
                            c.text.c_str(), 0);
                }
                break;
            }

            case PaintCommand::DrawImage:
            {
                if(c.image.width() == 0 || c.image.height() == 0)
                    break;

                std::vector<unsigned char> rgba;
                int iw = 0;
                int ih = 0;
                buildRgba(c.image, rgba, iw, ih);

                int img = nvgCreateImageRGBA(vg, iw, ih,
                                             NVG_IMAGE_PREMULTIPLIED, &rgba[0]);
                if(img <= 0)
                    break;

                if(c.hasSrcRect)
                {
                    const float rx = static_cast<float>(c.srcRect.x());
                    const float ry = static_cast<float>(c.srcRect.y());
                    const float rw = static_cast<float>(c.srcRect.width());
                    const float rh = static_cast<float>(c.srcRect.height());

                    if(rw > 0.0f && rh > 0.0f)
                    {
                        const float dx = static_cast<float>(c.p0.x());
                        const float dy = static_cast<float>(c.p0.y());
                        const float dw = rw;
                        const float dh = rh;
                        const float tileW = dw * static_cast<float>(iw) / rw;
                        const float tileH = dh * static_cast<float>(ih) / rh;
                        const float ox = dx - rx * (dw / rw);
                        const float oy = dy - ry * (dh / rh);

                        nvgSave(vg);
                        nvgIntersectScissor(vg, dx, dy, dw, dh);
                        nvgBeginPath(vg);
                        nvgRect(vg, dx, dy, dw, dh);
                        NVGpaint paint = nvgImagePattern(vg, ox, oy, tileW, tileH,
                                                         0.0f, img, 1.0f);
                        nvgFillPaint(vg, paint);
                        nvgFill(vg);
                        nvgRestore(vg);
                    }
                }
                else
                {
                    const float dx   = static_cast<float>(c.p0.x());
                    const float dy   = static_cast<float>(c.p0.y());
                    const float logW = static_cast<float>(
                        _scaling.toLogical(static_cast<double>(iw)));
                    const float logH = static_cast<float>(
                        _scaling.toLogical(static_cast<double>(ih)));

                    nvgBeginPath(vg);
                    nvgRect(vg, dx, dy, logW, logH);
                    NVGpaint paint = nvgImagePattern(vg, dx, dy, logW, logH,
                                                     0.0f, img, 1.0f);
                    nvgFillPaint(vg, paint);
                    nvgFill(vg);
                }

                nvgDeleteImage(vg, img);
                break;
            }

            case PaintCommand::DrawPixmap:
            {
                // Source was flushed at record time; its texture is current.
                if(c.srcPixmapImage < 0 || c.srcPixmapW == 0 || c.srcPixmapH == 0)
                    break;

                if(c.hasSrcRect)
                {
                    const float rx = static_cast<float>(c.srcRect.x());
                    const float ry = static_cast<float>(c.srcRect.y());
                    const float rw = static_cast<float>(c.srcRect.width());
                    const float rh = static_cast<float>(c.srcRect.height());

                    if(rw > 0.0f && rh > 0.0f)
                    {
                        const float dx  = static_cast<float>(c.p0.x());
                        const float dy  = static_cast<float>(c.p0.y());
                        // destW/destH are the logical destination dimensions;
                        // srcRect holds physical pixel coordinates for texture
                        // sampling. Using rw/rh here would double-apply the
                        // canvas scale transform.
                        const float dw  = c.destW;
                        const float dh  = c.destH;
                        const float tileW = dw * static_cast<float>(c.srcPixmapW) / rw;
                        const float tileH = dh * static_cast<float>(c.srcPixmapH) / rh;
                        const float ox  = dx - rx * (dw / rw);
                        const float oy  = dy - ry * (dh / rh);

                        nvgSave(vg);
                        nvgIntersectScissor(vg, dx, dy, dw, dh);
                        nvgBeginPath(vg);
                        nvgRect(vg, dx, dy, dw, dh);
                        NVGpaint paint = nvgImagePattern(vg, ox, oy, tileW, tileH,
                                                         0.0f, c.srcPixmapImage, 1.0f);
                        nvgFillPaint(vg, paint);
                        nvgFill(vg);
                        nvgRestore(vg);
                    }
                }
                else
                {
                    const float dx   = static_cast<float>(c.p0.x());
                    const float dy   = static_cast<float>(c.p0.y());
                    const float logW = static_cast<float>(
                        _scaling.toLogical(static_cast<double>(c.srcPixmapW)));
                    const float logH = static_cast<float>(
                        _scaling.toLogical(static_cast<double>(c.srcPixmapH)));

                    nvgBeginPath(vg);
                    nvgRect(vg, dx, dy, logW, logH);
                    NVGpaint paint = nvgImagePattern(vg, dx, dy, logW, logH,
                                                     0.0f, c.srcPixmapImage, 1.0f);
                    nvgFillPaint(vg, paint);
                    nvgFill(vg);
                }
                break;
            }
        }
    }

    if(brushTexImage >= 0)
        nvgDeleteImage(vg, brushTexImage);

    nvgEndFrame(vg);
    device->unbindRenderTarget();

    _commands.clear();
}


Gfx::Image PixmapImpl::toImage() const
{
    if(_width == 0 || _height == 0 || _image < 0)
        return Gfx::Image();

    const_cast<PixmapImpl*>(this)->flush();

    NanoVGDevice* device = NanoVGDevice::instance();
    if( ! device || ! device->isValid())
        return Gfx::Image();

    if( ! device->bindRenderTarget(_image, _width, _height) )
        return Gfx::Image();

    std::vector<unsigned char> buffer(_width * _height * 4);
    glReadPixels(0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());

    device->unbindRenderTarget();

    // GL framebuffers have a bottom-left origin, Pt images a top-left origin.
    // Flip rows and swizzle RGBA to Pt rgb32 (BGRA).
    Gfx::Image image(_width, _height, Gfx::Rgb32());
    Pt::uint8_t* dst = image.data();

    for(int y = 0; y < _height; ++y)
    {
        const unsigned char* srcRow =
            buffer.data() + (_height - 1 - y) * _width * 4;
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

} // namespace Forms

} // namespace Pt

