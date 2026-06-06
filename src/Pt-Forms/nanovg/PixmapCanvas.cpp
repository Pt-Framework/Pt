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
#include "PixmapCanvas.h"
#include "PixmapImpl.h"
#include "NanoVGDevice.h"

#include <Pt/Forms/Pixmap.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Rgb32.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/TextMetrics.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/Gfx/Scaling.h>
#include <Pt/String.h>

#include <GLES2/gl2.h>
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"

#include <vector>
#include <string>
#include <cmath>
#include <cassert>

namespace {

NVGcolor toNVGColor(const Pt::Gfx::Color& c)
{
    return nvgRGBA(static_cast<unsigned char>(c.red()),
                   static_cast<unsigned char>(c.green()),
                   static_cast<unsigned char>(c.blue()),
                   static_cast<unsigned char>(c.alpha()));
}


// Encodes a Pt::String (sequence of Unicode code points) as UTF-8, as
// expected by nanovg text functions.
std::string toUtf8(const Pt::String& text)
{
    std::string out;
    out.reserve(text.size());

    for(std::size_t i = 0; i < text.size(); ++i)
    {
        unsigned int cp = text[i].value();

        if(cp < 0x80)
        {
            out += static_cast<char>(cp);
        }
        else if(cp < 0x800)
        {
            out += static_cast<char>(0xC0 | (cp >> 6));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        }
        else if(cp < 0x10000)
        {
            out += static_cast<char>(0xE0 | (cp >> 12));
            out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        }
        else
        {
            out += static_cast<char>(0xF0 | (cp >> 18));
            out += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (cp & 0x3F));
        }
    }

    return out;
}


// Converts a Gfx image to a tightly packed RGBA buffer (nanovg byte order).
// Pt rgb32 images store premultiplied BGRA, so the red and blue channels are
// swapped.
void buildRgba(const Pt::Gfx::Image& image,
               std::vector<unsigned char>& out,
               int& width,
               int& height)
{
    namespace Gfx = Pt::Gfx;

    width = static_cast<int>(image.width());
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

} // anonymous namespace

namespace Pt {

namespace Forms {

PixmapCanvas::PixmapCanvas()
: Gfx::Canvas()
, _pixmap(0)
, _vg(0)
, _painting(false)
, _penColor(nvgRGBA(0, 0, 0, 255))
, _penWidth(1.0f)
, _lineCap(NVG_BUTT)
, _lineJoin(NVG_MITER)
, _textureImage(-1)
, _fontFace(-1)
, _fontSize(12.0f)
, _clipSet(false)
{
    NanoVGDevice* device = NanoVGDevice::instance();
    if(device)
        _vg = device->context();
}


PixmapCanvas::~PixmapCanvas()
{
    if(_vg && _textureImage >= 0)
        nvgDeleteImage(_vg, _textureImage);
}


void PixmapCanvas::setPixmap(PixmapImpl& pixmap)
{
    _pixmap = &pixmap;
}


void PixmapCanvas::onBeginPaint(const Gfx::Paint& /*paint*/)
{
    if( ! _pixmap)
        return;

    NanoVGDevice* device = NanoVGDevice::instance();
    if( ! device || ! device->isValid())
        return;

    NVGLUframebuffer* fb = _pixmap->framebuffer();
    if( ! fb)
        return;

    _vg = device->context();

    const int width = _pixmap->width();
    const int height = _pixmap->height();

    device->makeCurrentOffscreen();
    nvgluBindFramebuffer(fb);
    glViewport(0, 0, width, height);

    nvgBeginFrame(_vg, static_cast<float>(width), static_cast<float>(height), 1.0f);
    _painting = true;
}


void PixmapCanvas::onFinishPaint()
{
    if( ! _vg || ! _painting)
        return;

    nvgEndFrame(_vg);
    glFlush();
    nvgluBindFramebuffer(0);

    _painting = false;
}


void PixmapCanvas::onSetTransform(const Gfx::Transform& /*tx*/)
{
}


void PixmapCanvas::onApplyTransform()
{
    if( ! _vg)
        return;

    const Gfx::Transform& tx = transform();

    nvgResetTransform(_vg);
    nvgTransform(_vg,
                 static_cast<float>(tx.m11()), static_cast<float>(tx.m12()),
                 static_cast<float>(tx.m21()), static_cast<float>(tx.m22()),
                 static_cast<float>(tx.dx()),  static_cast<float>(tx.dy()));
}


void PixmapCanvas::onSetCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;
}


void PixmapCanvas::onApplyCompositionMode()
{
    if( ! _vg)
        return;

    if(_compositionMode == Gfx::CompositionMode::SourceCopy)
        nvgGlobalCompositeOperation(_vg, NVG_COPY);
    else
        nvgGlobalCompositeOperation(_vg, NVG_SOURCE_OVER);
}


void PixmapCanvas::onSetPen(const Gfx::Pen& pen)
{
    _pen = pen;

    _penColor = toNVGColor(pen.color());

    _penWidth = static_cast<float>(pen.size());
    if(_penWidth < 1.0f)
        _penWidth = 1.0f;

    switch(pen.capStyle())
    {
        case Gfx::Pen::FlatCap:
            _lineCap = NVG_BUTT;
            break;
        case Gfx::Pen::RoundCap:
            _lineCap = NVG_ROUND;
            break;
        case Gfx::Pen::SquareCap:
            _lineCap = NVG_SQUARE;
            break;
    }

    switch(pen.joinStyle())
    {
        case Gfx::Pen::NoJoin:
        case Gfx::Pen::MiterJoin:
            _lineJoin = NVG_MITER;
            break;
        case Gfx::Pen::RoundJoin:
            _lineJoin = NVG_ROUND;
            break;
        case Gfx::Pen::BevelJoin:
            _lineJoin = NVG_BEVEL;
            break;
    }

    // nanovg has no native dash support; dash and dot styles fall back to a
    // solid stroke.
}


void PixmapCanvas::onApplyPen()
{
    if( ! _vg)
        return;

    nvgStrokeColor(_vg, _penColor);
    nvgStrokeWidth(_vg, _penWidth);
    nvgLineCap(_vg, _lineCap);
    nvgLineJoin(_vg, _lineJoin);
}


void PixmapCanvas::onSetBrush(const Gfx::Brush& brush)
{
    _brush = brush;

    if(_vg && _textureImage >= 0)
    {
        nvgDeleteImage(_vg, _textureImage);
        _textureImage = -1;
    }
}


void PixmapCanvas::onApplyBrush()
{
    // nanovg fills are configured per shape in applyFillForBounds(), so the
    // gradient and texture coordinates can be computed from the shape bounds.
}


void PixmapCanvas::applyFillForBounds(const Gfx::RectF& bounds)
{
    if( ! _vg)
        return;

    if(_brush.isTexture())
    {
        if(_textureImage < 0)
        {
            const Gfx::Image& texture = _brush.texture();
            if(texture.width() != 0 && texture.height() != 0)
            {
                std::vector<unsigned char> rgba;
                int tw = 0;
                int th = 0;
                buildRgba(texture, rgba, tw, th);

                _textureImage = nvgCreateImageRGBA(_vg, tw, th,
                    NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY | NVG_IMAGE_PREMULTIPLIED,
                    &rgba[0]);
            }
        }

        if(_textureImage >= 0)
        {
            int tw = 0;
            int th = 0;
            nvgImageSize(_vg, _textureImage, &tw, &th);

            const Gfx::PointF& origin = _brush.textureOrigin();
            NVGpaint paint = nvgImagePattern(_vg,
                static_cast<float>(origin.x()), static_cast<float>(origin.y()),
                static_cast<float>(tw), static_cast<float>(th),
                0.0f, _textureImage, 1.0f);
            nvgFillPaint(_vg, paint);
            return;
        }

        nvgFillColor(_vg, toNVGColor(_brush.color()));
        return;
    }

    if(_brush.isGradient())
    {
        // nanovg gradients support only two colors. Multi-stop gradients are
        // approximated using the first and last stop.
        const Gfx::ColorStops& stops = _brush.gradientStops();

        NVGcolor inner;
        NVGcolor outer;
        if(stops.size() >= 2)
        {
            inner = toNVGColor(stops.front().color());
            outer = toNVGColor(stops.back().color());
        }
        else
        {
            inner = toNVGColor(_brush.color());
            outer = toNVGColor(_brush.gradientColor());
        }

        const bool relative = _brush.positionMode() == Gfx::Brush::Relative;
        const float bx = static_cast<float>(bounds.x());
        const float by = static_cast<float>(bounds.y());
        const float bw = static_cast<float>(bounds.width());
        const float bh = static_cast<float>(bounds.height());

        Gfx::Brush::GradientStyle style = _brush.gradient();

        if(style == Gfx::Brush::Radial)
        {
            const Gfx::PointF& center = _brush.gradientEnd();
            float cx;
            float cy;
            float radius = _brush.gradientEndRadius();

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

            NVGpaint paint = nvgRadialGradient(_vg, cx, cy, 0.0f, radius, inner, outer);
            nvgFillPaint(_vg, paint);
            return;
        }

        float sx;
        float sy;
        float ex;
        float ey;

        if(style == Gfx::Brush::Horizontal)
        {
            sx = bx;          sy = by;
            ex = bx + bw;     ey = by;
        }
        else if(style == Gfx::Brush::Vertical)
        {
            sx = bx;          sy = by;
            ex = bx;          ey = by + bh;
        }
        else
        {
            const Gfx::PointF& begin = _brush.gradientBegin();
            const Gfx::PointF& end = _brush.gradientEnd();

            if(relative)
            {
                sx = bx + static_cast<float>(begin.x()) * bw;
                sy = by + static_cast<float>(begin.y()) * bh;
                ex = bx + static_cast<float>(end.x()) * bw;
                ey = by + static_cast<float>(end.y()) * bh;
            }
            else
            {
                sx = static_cast<float>(begin.x());
                sy = static_cast<float>(begin.y());
                ex = static_cast<float>(end.x());
                ey = static_cast<float>(end.y());
            }
        }

        NVGpaint paint = nvgLinearGradient(_vg, sx, sy, ex, ey, inner, outer);
        nvgFillPaint(_vg, paint);
        return;
    }

    nvgFillColor(_vg, toNVGColor(_brush.color()));
}


void PixmapCanvas::onSetFont(const Gfx::Font& font)
{
    _font = font;

    NanoVGDevice* device = NanoVGDevice::instance();
    _fontFace = device ? device->fontFace(font) : -1;

    float scale = device ? device->fontSizeScale(_fontFace) : 1.0f;
    double size = font.size();
    if(size <= 0.0)
        size = 12.0;

    _fontSize = static_cast<float>(size * scale);

    _fontMetrics = Gfx::FontMetrics();

    NVGcontext* vg = device ? device->context() : 0;
    if(vg && _fontFace >= 0)
    {
        nvgFontFaceId(vg, _fontFace);
        nvgFontSize(vg, _fontSize);

        float ascender = 0.0f;
        float descender = 0.0f;
        float lineHeight = 0.0f;
        nvgTextMetrics(vg, &ascender, &descender, &lineHeight);

        // nanovg reports a negative descender below the baseline.
        _fontMetrics.setAscent(ascender);
        _fontMetrics.setDescent(-descender);
        _fontMetrics.setLeading(lineHeight - (ascender - descender));
    }
}


void PixmapCanvas::onApplyFont()
{
    // The font face and size are set per text operation.
}


void PixmapCanvas::onSetClip(const Gfx::RectF* clip)
{
    if(clip)
    {
        _clip = *clip;
        _clipSet = true;
    }
    else
    {
        _clipSet = false;
    }
}


void PixmapCanvas::onApplyClip()
{
    if( ! _vg)
        return;

    if(_clipSet)
    {
        nvgScissor(_vg,
                   static_cast<float>(_clip.x()), static_cast<float>(_clip.y()),
                   static_cast<float>(_clip.width()), static_cast<float>(_clip.height()));
    }
    else
    {
        nvgResetScissor(_vg);
    }
}


void PixmapCanvas::onDrawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( ! _vg)
        return;

    nvgBeginPath(_vg);
    nvgMoveTo(_vg, static_cast<float>(from.x()), static_cast<float>(from.y()));
    nvgLineTo(_vg, static_cast<float>(to.x()), static_cast<float>(to.y()));
    nvgStroke(_vg);
}


void PixmapCanvas::onDrawPolyline(const Gfx::PointF* pts, const size_t n)
{
    if( ! _vg || n < 2)
        return;

    nvgBeginPath(_vg);
    nvgMoveTo(_vg, static_cast<float>(pts[0].x()), static_cast<float>(pts[0].y()));
    for(size_t i = 1; i < n; ++i)
        nvgLineTo(_vg, static_cast<float>(pts[i].x()), static_cast<float>(pts[i].y()));
    nvgStroke(_vg);
}


void PixmapCanvas::onFillPolygon(const Gfx::PointF* ps, const size_t n)
{
    if( ! _vg || n < 3)
        return;

    double minX = ps[0].x();
    double minY = ps[0].y();
    double maxX = minX;
    double maxY = minY;

    nvgBeginPath(_vg);
    nvgMoveTo(_vg, static_cast<float>(ps[0].x()), static_cast<float>(ps[0].y()));
    for(size_t i = 1; i < n; ++i)
    {
        nvgLineTo(_vg, static_cast<float>(ps[i].x()), static_cast<float>(ps[i].y()));

        if(ps[i].x() < minX) minX = ps[i].x();
        if(ps[i].y() < minY) minY = ps[i].y();
        if(ps[i].x() > maxX) maxX = ps[i].x();
        if(ps[i].y() > maxY) maxY = ps[i].y();
    }
    nvgClosePath(_vg);

    applyFillForBounds(Gfx::RectF::fromXYWH(minX, minY, maxX - minX, maxY - minY));
    nvgFill(_vg);
}


void PixmapCanvas::onDrawRect(const Gfx::RectF& rectangle)
{
    if( ! _vg)
        return;

    nvgBeginPath(_vg);
    nvgRect(_vg,
            static_cast<float>(rectangle.x()), static_cast<float>(rectangle.y()),
            static_cast<float>(rectangle.width()), static_cast<float>(rectangle.height()));
    nvgStroke(_vg);
}


void PixmapCanvas::onFillRect(const Gfx::RectF& rectangle)
{
    if( ! _vg)
        return;

    nvgBeginPath(_vg);
    nvgRect(_vg,
            static_cast<float>(rectangle.x()), static_cast<float>(rectangle.y()),
            static_cast<float>(rectangle.width()), static_cast<float>(rectangle.height()));

    applyFillForBounds(rectangle);
    nvgFill(_vg);
}


void PixmapCanvas::onDrawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if( ! _vg)
        return;

    nvgBeginPath(_vg);
    nvgEllipse(_vg,
               static_cast<float>(topLeft.x() + size.width() / 2.0),
               static_cast<float>(topLeft.y() + size.height() / 2.0),
               static_cast<float>(size.width() / 2.0),
               static_cast<float>(size.height() / 2.0));
    nvgStroke(_vg);
}


void PixmapCanvas::onFillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if( ! _vg)
        return;

    nvgBeginPath(_vg);
    nvgEllipse(_vg,
               static_cast<float>(topLeft.x() + size.width() / 2.0),
               static_cast<float>(topLeft.y() + size.height() / 2.0),
               static_cast<float>(size.width() / 2.0),
               static_cast<float>(size.height() / 2.0));

    applyFillForBounds(Gfx::RectF::fromXYWH(topLeft.x(), topLeft.y(),
                                            size.width(), size.height()));
    nvgFill(_vg);
}


Gfx::TextMetrics PixmapCanvas::onGetTextMetrics(const Pt::String& text) const
{
    NanoVGDevice* device = NanoVGDevice::instance();
    NVGcontext* vg = device ? device->context() : 0;

    if( ! vg || _fontFace < 0)
        return Gfx::TextMetrics();

    nvgFontFaceId(vg, _fontFace);
    nvgFontSize(vg, _fontSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);

    std::string utf8 = toUtf8(text);

    float bounds[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    float advance = nvgTextBounds(vg, 0.0f, 0.0f, utf8.c_str(), 0, bounds);

    Gfx::TextMetrics tm;
    tm.setAdvance(advance);
    tm.setBoundingWidth(bounds[2] - bounds[0]);
    tm.setBoundingHeight(bounds[3] - bounds[1]);
    tm.setBearingY(-bounds[1]);

    return tm;
}


const Gfx::FontMetrics& PixmapCanvas::onGetFontMetrics() const
{
    return _fontMetrics;
}


void PixmapCanvas::onDrawText(const Gfx::PointF& to,
                              const Pt::String& text,
                              const Gfx::Transform* tform)
{
    if( ! _vg || _fontFace < 0)
        return;

    std::string utf8 = toUtf8(text);

    if(tform)
    {
        nvgSave(_vg);
        nvgFontFaceId(_vg, _fontFace);
        nvgFontSize(_vg, _fontSize);
        nvgTextAlign(_vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
        nvgFillColor(_vg, _penColor);

        // The current transform already maps logical to physical coordinates.
        // Translate to the text origin and apply the extra text transform.
        nvgTranslate(_vg, static_cast<float>(to.x()), static_cast<float>(to.y()));
        nvgTransform(_vg,
                     static_cast<float>(tform->m11()), static_cast<float>(tform->m12()),
                     static_cast<float>(tform->m21()), static_cast<float>(tform->m22()),
                     static_cast<float>(tform->dx()),  static_cast<float>(tform->dy()));

        nvgText(_vg, 0.0f, 0.0f, utf8.c_str(), 0);
        nvgRestore(_vg);
    }
    else
    {
        nvgFontFaceId(_vg, _fontFace);
        nvgFontSize(_vg, _fontSize);
        nvgTextAlign(_vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
        nvgFillColor(_vg, _penColor);

        nvgText(_vg,
                static_cast<float>(to.x()), static_cast<float>(to.y()),
                utf8.c_str(), 0);
    }
}


void PixmapCanvas::onDrawImage(const Gfx::PointF& toF,
                               const Gfx::Image& image,
                               const Gfx::RectF* rect)
{
    if( ! _vg)
        return;

    if(image.width() == 0 || image.height() == 0)
        return;

    std::vector<unsigned char> rgba;
    int width = 0;
    int height = 0;
    buildRgba(image, rgba, width, height);

    int img = nvgCreateImageRGBA(_vg, width, height,
                                 NVG_IMAGE_PREMULTIPLIED, &rgba[0]);
    if(img <= 0)
        return;

    if(rect)
    {
        const float rx = static_cast<float>(rect->x());
        const float ry = static_cast<float>(rect->y());
        const float rw = static_cast<float>(rect->width());
        const float rh = static_cast<float>(rect->height());

        if(rw > 0.0f && rh > 0.0f)
        {
            const float dx = static_cast<float>(toF.x());
            const float dy = static_cast<float>(toF.y());
            const float dw = rw;
            const float dh = rh;

            const float tileW = dw * static_cast<float>(width) / rw;
            const float tileH = dh * static_cast<float>(height) / rh;
            const float ox = dx - rx * (dw / rw);
            const float oy = dy - ry * (dh / rh);

            nvgSave(_vg);
            nvgIntersectScissor(_vg, dx, dy, dw, dh);
            nvgBeginPath(_vg);
            nvgRect(_vg, dx, dy, dw, dh);
            NVGpaint paint = nvgImagePattern(_vg, ox, oy, tileW, tileH, 0.0f, img, 1.0f);
            nvgFillPaint(_vg, paint);
            nvgFill(_vg);
            nvgRestore(_vg);
        }
    }
    else
    {
        const Gfx::Scaling& dstScaling = scaling();
        const float logW = static_cast<float>(dstScaling.toLogical(static_cast<double>(width)));
        const float logH = static_cast<float>(dstScaling.toLogical(static_cast<double>(height)));
        const float dx = static_cast<float>(toF.x());
        const float dy = static_cast<float>(toF.y());

        nvgBeginPath(_vg);
        nvgRect(_vg, dx, dy, logW, logH);
        NVGpaint paint = nvgImagePattern(_vg, dx, dy, logW, logH, 0.0f, img, 1.0f);
        nvgFillPaint(_vg, paint);
        nvgFill(_vg);
    }

    nvgDeleteImage(_vg, img);
}


void PixmapCanvas::buildPath(const Gfx::Path& path)
{
    nvgBeginPath(_vg);

    for(Gfx::PathIterator it = path.begin(); it != path.end(); ++it)
    {
        switch(it->type())
        {
            default:
                break;

            case Gfx::Path::MoveTo:
            {
                const Gfx::PointF& to = it->point(0);
                nvgMoveTo(_vg, static_cast<float>(to.x()), static_cast<float>(to.y()));
                break;
            }

            case Gfx::Path::LineTo:
            {
                const Gfx::PointF& to = it->point(0);
                nvgLineTo(_vg, static_cast<float>(to.x()), static_cast<float>(to.y()));
                break;
            }

            case Gfx::Path::QuadTo:
            {
                const Gfx::PointF& cp = it->point(0);
                const Gfx::PointF& to = it->point(1);
                nvgQuadTo(_vg,
                          static_cast<float>(cp.x()), static_cast<float>(cp.y()),
                          static_cast<float>(to.x()), static_cast<float>(to.y()));
                break;
            }

            case Gfx::Path::CubicTo:
            {
                const Gfx::PointF& c1 = it->point(0);
                const Gfx::PointF& c2 = it->point(1);
                const Gfx::PointF& to = it->point(2);
                nvgBezierTo(_vg,
                            static_cast<float>(c1.x()), static_cast<float>(c1.y()),
                            static_cast<float>(c2.x()), static_cast<float>(c2.y()),
                            static_cast<float>(to.x()), static_cast<float>(to.y()));
                break;
            }

            case Gfx::Path::Close:
            {
                nvgClosePath(_vg);
                break;
            }
        }
    }
}


namespace {

Pt::Gfx::RectF pathBounds(const Pt::Gfx::Path& path)
{
    namespace Gfx = Pt::Gfx;

    bool first = true;
    double minX = 0.0;
    double minY = 0.0;
    double maxX = 0.0;
    double maxY = 0.0;

    for(Gfx::PathIterator it = path.begin(); it != path.end(); ++it)
    {
        int points = 0;
        switch(it->type())
        {
            case Gfx::Path::MoveTo:
            case Gfx::Path::LineTo:
                points = 1;
                break;
            case Gfx::Path::QuadTo:
                points = 2;
                break;
            case Gfx::Path::CubicTo:
                points = 3;
                break;
            default:
                points = 0;
                break;
        }

        for(int i = 0; i < points; ++i)
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


void PixmapCanvas::onSetPath(const Gfx::Path& path)
{
    _ptPath = path;
}


void PixmapCanvas::onDrawPath()
{
    if( ! _vg)
        return;

    buildPath(_ptPath);
    nvgStroke(_vg);
}


void PixmapCanvas::onFillPath()
{
    if( ! _vg)
        return;

    buildPath(_ptPath);
    applyFillForBounds(pathBounds(_ptPath));
    nvgFill(_vg);
}


void PixmapCanvas::onDrawPath(const Gfx::Path& path)
{
    if( ! _vg)
        return;

    buildPath(path);
    nvgStroke(_vg);
}


void PixmapCanvas::onFillPath(const Gfx::Path& path)
{
    if( ! _vg)
        return;

    buildPath(path);
    applyFillForBounds(pathBounds(path));
    nvgFill(_vg);
}


void PixmapCanvas::drawPixmap(const Gfx::PointF& to,
                              const Pixmap& pm,
                              const Gfx::RectF* rect)
{
    if( ! _vg || ! _painting)
        return;

    applyState();

    const PixmapImpl* srcImpl = pm.impl();
    if( ! srcImpl)
        return;

    int img = srcImpl->framebufferImage();
    if(img < 0)
        return;

    const int srcW = srcImpl->width();
    const int srcH = srcImpl->height();
    if(srcW == 0 || srcH == 0)
        return;

    if(rect)
    {
        const Gfx::Scaling& srcScaling = srcImpl->scaling();
        Gfx::RectF physRect = srcScaling.toPhysical(*rect);

        const float rx = static_cast<float>(physRect.x());
        const float ry = static_cast<float>(physRect.y());
        const float rw = static_cast<float>(physRect.width());
        const float rh = static_cast<float>(physRect.height());

        if(rw > 0.0f && rh > 0.0f)
        {
            const float dx = static_cast<float>(to.x());
            const float dy = static_cast<float>(to.y());
            const float dw = static_cast<float>(rect->width());
            const float dh = static_cast<float>(rect->height());

            const float tileW = dw * static_cast<float>(srcW) / rw;
            const float tileH = dh * static_cast<float>(srcH) / rh;
            const float ox = dx - rx * (dw / rw);
            const float oy = dy - ry * (dh / rh);

            nvgSave(_vg);
            nvgIntersectScissor(_vg, dx, dy, dw, dh);
            nvgBeginPath(_vg);
            nvgRect(_vg, dx, dy, dw, dh);
            NVGpaint paint = nvgImagePattern(_vg, ox, oy, tileW, tileH, 0.0f, img, 1.0f);
            nvgFillPaint(_vg, paint);
            nvgFill(_vg);
            nvgRestore(_vg);
        }
    }
    else
    {
        const Gfx::Scaling& dstScaling = scaling();
        const float logW = static_cast<float>(dstScaling.toLogical(static_cast<double>(srcW)));
        const float logH = static_cast<float>(dstScaling.toLogical(static_cast<double>(srcH)));
        const float dx = static_cast<float>(to.x());
        const float dy = static_cast<float>(to.y());

        nvgBeginPath(_vg);
        nvgRect(_vg, dx, dy, logW, logH);
        NVGpaint paint = nvgImagePattern(_vg, dx, dy, logW, logH, 0.0f, img, 1.0f);
        nvgFillPaint(_vg, paint);
        nvgFill(_vg);
    }
}

} // namespace

} // namespace
