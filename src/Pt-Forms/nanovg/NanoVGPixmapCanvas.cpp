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
#include "NanoVGPixmapCanvas.h"
#include "NanoVGPixmapImpl.h"
#include "NanoVGDevice.h"
#include "PaintCommand.h"

#include <Pt/Forms/Pixmap.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Rgb32.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/TextMetrics.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/Gfx/Scaling.h>
#include <Pt/System/Logger.h>
#include <Pt/String.h>

#include <vector>
#include <string>
#include <cmath>
#include <cassert>

PT_LOG_DEFINE("Pt.Forms.NanoVGPixmapCanvas");

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

} // anonymous namespace

namespace Pt {

namespace Forms {

NanoVGPixmapCanvas::NanoVGPixmapCanvas()
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


NanoVGPixmapCanvas::~NanoVGPixmapCanvas()
{
    // _textureImage is not used in recording mode; brush textures are created
    // and destroyed during flush() inside PixmapImpl.
}


void NanoVGPixmapCanvas::setPixmap(NanoVGPixmapImpl& pixmap)
{
    _pixmap = &pixmap;
}


void NanoVGPixmapCanvas::onBeginPaint(const Gfx::Paint& /*paint*/)
{
    // Recording mode: no nanovg frame is opened here. Commands are appended
    // to _pixmap->commands() and replayed lazily in PixmapImpl::flush().
    _painting = true;
}


void NanoVGPixmapCanvas::onFinishPaint()
{
    // Recording mode: no nanovg frame to close. The command buffer is flushed
    // when the pixmap texture is actually needed (commitFrame, toImage, blit).
    _painting = false;
}


void NanoVGPixmapCanvas::onSetTransform(const Gfx::Transform& /*tx*/)
{
}


void NanoVGPixmapCanvas::onApplyTransform()
{
    if( ! _pixmap)
        return;

    PaintCommand c;
    c.type = PaintCommand::SetTransform;
    c.transform = transform();
    _pixmap->commands().push_back(c);
}


void NanoVGPixmapCanvas::onSetCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;
}


void NanoVGPixmapCanvas::onApplyCompositionMode()
{
    if( ! _pixmap)
        return;

    // Always use NVG_SOURCE_OVER for pixmap rendering, regardless of Pt's
    // CompositionMode. nanovg's stencil-based text rendering and anti-aliasing
    // require proper alpha blending; NVG_COPY would write partial-coverage
    // pixels without blending, causing visual artifacts.
    PaintCommand c;
    c.type = PaintCommand::SetCompositionMode;
    c.compositionMode = NVG_SOURCE_OVER;

    _pixmap->commands().push_back(c);
}


void NanoVGPixmapCanvas::onSetPen(const Gfx::Pen& pen)
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


void NanoVGPixmapCanvas::onApplyPen()
{
    if( ! _pixmap)
        return;

    PaintCommand c;
    c.type     = PaintCommand::SetPen;
    c.penColor = _penColor;
    c.penWidth = _penWidth;
    c.lineCap  = _lineCap;
    c.lineJoin = _lineJoin;
    _pixmap->commands().push_back(c);
}


void NanoVGPixmapCanvas::onSetBrush(const Gfx::Brush& brush)
{
    _brush = brush;
    _textureImage = -1;
}


void NanoVGPixmapCanvas::onApplyBrush()
{
    if( ! _pixmap)
        return;

    PaintCommand c;
    c.type  = PaintCommand::SetBrush;
    c.brush = _brush;
    _pixmap->commands().push_back(c);
}


void NanoVGPixmapCanvas::onSetFont(const Gfx::Font& font)
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
        // Metric queries must use identity transform so that nvgTextMetrics
        // computes fonsSetSize(fontSize * scale) with scale=1. After flush()
        // the NVG state retains the last SetTransform (m11=scaleFactor), which
        // would cause fontstash's integer isize = (short)(size*scale*10) to
        // truncate differently and produce unstable metric values.
        nvgSave(vg);
        nvgResetTransform(vg);

        nvgFontFaceId(vg, _fontFace);
        nvgFontSize(vg, _fontSize);

        float ascender = 0.0f;
        float descender = 0.0f;
        float lineHeight = 0.0f;
        nvgTextMetrics(vg, &ascender, &descender, &lineHeight);

        nvgRestore(vg);
        // fontstash adds lineGap to the ascender before normalizing, producing
        // a smaller value than the true typographic ascender. Use the FreeType
        // face->ascender / units_per_EM ratio directly so that fm.ascent()
        // matches what the raster renderer reports and widget baseline
        // calculations place text at the correct position.
        const float trueAscender = device->fontAscenderRatio(_fontFace) * _fontSize;

        // nanovg reports a negative descender below the baseline.
        _fontMetrics.setAscent(trueAscender);
        _fontMetrics.setDescent(-descender);
        _fontMetrics.setLeading(lineHeight - (trueAscender + (-descender)));

        // Use FreeType OS/2 table values (sCapHeight, sxHeight) for exact
        // typographic heights relative to the ascender.
        _fontMetrics.setCapHeight(device->fontCapHeightRatio(_fontFace) * trueAscender);
        _fontMetrics.setXHeight(device->fontXHeightRatio(_fontFace) * trueAscender);
    }
}


void NanoVGPixmapCanvas::onApplyFont()
{
    if( ! _pixmap)
        return;

    PaintCommand c;
    c.type     = PaintCommand::SetFont;
    c.fontFace = _fontFace;
    c.fontSize = _fontSize;
    _pixmap->commands().push_back(c);
}


void NanoVGPixmapCanvas::onSetClip(const Gfx::RectF* clip)
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


void NanoVGPixmapCanvas::onApplyClip()
{
    if( ! _pixmap)
        return;

    PaintCommand c;
    c.type    = PaintCommand::SetClip;
    c.hasClip = _clipSet;
    if(_clipSet)
        c.clip = _clip;
    _pixmap->commands().push_back(c);
}


void NanoVGPixmapCanvas::onDrawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( ! _pixmap)
        return;

    PaintCommand c;
    c.type = PaintCommand::DrawLine;
    c.p0   = from;
    c.p1   = to;
    _pixmap->commands().push_back(c);
}


void NanoVGPixmapCanvas::onDrawPolyline(const Gfx::PointF* pts, const size_t n)
{
    if( ! _pixmap || n < 2)
        return;

    PaintCommand c;
    c.type = PaintCommand::DrawPolyline;
    c.points.assign(pts, pts + n);
    _pixmap->commands().push_back(std::move(c));
}


void NanoVGPixmapCanvas::onFillPolygon(const Gfx::PointF* ps, const size_t n)
{
    if( ! _pixmap || n < 3)
        return;

    PaintCommand c;
    c.type = PaintCommand::FillPolygon;
    c.points.assign(ps, ps + n);
    _pixmap->commands().push_back(std::move(c));
}


void NanoVGPixmapCanvas::onDrawRect(const Gfx::RectF& rectangle)
{
    if( ! _pixmap)
        return;

    PaintCommand c;
    c.type = PaintCommand::DrawRect;
    c.rect = rectangle;
    _pixmap->commands().push_back(c);
}


void NanoVGPixmapCanvas::onFillRect(const Gfx::RectF& rectangle)
{
    if( ! _pixmap)
        return;

    PaintCommand c;
    c.type = PaintCommand::FillRect;
    c.rect = rectangle;
    _pixmap->commands().push_back(c);
}


void NanoVGPixmapCanvas::onDrawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if( ! _pixmap)
        return;

    PaintCommand c;
    c.type        = PaintCommand::DrawEllipse;
    c.p0          = topLeft;
    c.ellipseSize = size;
    _pixmap->commands().push_back(c);
}


void NanoVGPixmapCanvas::onFillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if( ! _pixmap)
        return;

    PaintCommand c;
    c.type        = PaintCommand::FillEllipse;
    c.p0          = topLeft;
    c.ellipseSize = size;
    _pixmap->commands().push_back(c);
}


Gfx::TextMetrics NanoVGPixmapCanvas::onGetTextMetrics(const Pt::String& text) const
{
    NanoVGDevice* device = NanoVGDevice::instance();
    NVGcontext* vg = device ? device->context() : 0;

    if( ! vg || _fontFace < 0)
        return Gfx::TextMetrics();

    nvgFontFaceId(vg, _fontFace);
    nvgFontSize(vg, _fontSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);

    // Same reasoning as onSetFont: use identity transform so the
    // fontstash isize truncation is consistent regardless of NVG state.
    nvgSave(vg);
    nvgResetTransform(vg);

    std::string utf8 = toUtf8(text);

    float bounds[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    float advance = nvgTextBounds(vg, 0.0f, 0.0f, utf8.c_str(), 0, bounds);

    nvgRestore(vg);
    Gfx::TextMetrics tm;
    tm.setAdvance(advance);
    tm.setBoundingWidth(bounds[2] - bounds[0]);
    tm.setBoundingHeight(bounds[3] - bounds[1]);
    tm.setBearingY(-bounds[1]);

    return tm;
}


const Gfx::FontMetrics& NanoVGPixmapCanvas::onGetFontMetrics() const
{
    return _fontMetrics;
}


void NanoVGPixmapCanvas::onDrawText(const Gfx::PointF& to,
                              const Pt::String& text,
                              const Gfx::Transform* tform)
{
    if( ! _pixmap || _fontFace < 0)
        return;

    PaintCommand c;
    c.type = PaintCommand::DrawText;
    c.p0   = to;
    c.text = toUtf8(text);

    if(tform)
    {
        c.hasTextTransform = true;
        c.textTransform    = *tform;
    }

    _pixmap->commands().push_back(std::move(c));
}


void NanoVGPixmapCanvas::onDrawImage(const Gfx::PointF& toF,
                               const Gfx::Image& image,
                               const Gfx::RectF* rect)
{
    if( ! _pixmap)
        return;

    if(image.width() == 0 || image.height() == 0)
        return;

    PaintCommand c;
    c.type  = PaintCommand::DrawImage;
    c.p0    = toF;
    c.image = image;

    if(rect)
    {
        c.hasSrcRect = true;
        c.srcRect    = *rect;
    }

    _pixmap->commands().push_back(std::move(c));
}


void NanoVGPixmapCanvas::onSetPath(const Gfx::Path& path)
{
    _ptPath = path;
}


void NanoVGPixmapCanvas::onDrawPath()
{
    if( ! _pixmap)
        return;

    PaintCommand c;
    c.type = PaintCommand::DrawPath;
    c.path = _ptPath;
    _pixmap->commands().push_back(std::move(c));
}


void NanoVGPixmapCanvas::onFillPath()
{
    if( ! _pixmap)
        return;

    PaintCommand c;
    c.type = PaintCommand::FillPath;
    c.path = _ptPath;
    _pixmap->commands().push_back(std::move(c));
}


void NanoVGPixmapCanvas::onDrawPath(const Gfx::Path& path)
{
    if( ! _pixmap)
        return;

    PaintCommand c;
    c.type = PaintCommand::DrawPath;
    c.path = path;
    _pixmap->commands().push_back(std::move(c));
}


void NanoVGPixmapCanvas::onFillPath(const Gfx::Path& path)
{
    if( ! _pixmap)
        return;

    PaintCommand c;
    c.type = PaintCommand::FillPath;
    c.path = path;
    _pixmap->commands().push_back(std::move(c));
}


void NanoVGPixmapCanvas::drawPixmap(const Gfx::PointF& to,
                              const Pixmap& pm,
                              const Gfx::RectF* rect)
{
    if( ! _pixmap || ! _painting)
        return;

    applyState();

    const NanoVGPixmapImpl* srcConstImpl = static_cast<const NanoVGPixmapImpl*>( pm.impl() );
    NanoVGPixmapImpl* srcImpl = const_cast<NanoVGPixmapImpl*>(srcConstImpl);
    if( ! srcImpl)
        return;

    // Flush the source so its texture is current before we record the blit.
    srcImpl->flush();

    int img = srcImpl->framebufferImage();
    if(img < 0)
        return;

    const int srcW = srcImpl->width();
    const int srcH = srcImpl->height();
    if(srcW == 0 || srcH == 0)
        return;

    PaintCommand c;
    c.type           = PaintCommand::DrawPixmap;
    c.p0             = to;
    c.srcPixmapImage = img;
    c.srcPixmapW     = srcW;
    c.srcPixmapH     = srcH;

    if(rect)
    {
        const Gfx::Scaling& srcScaling = srcImpl->scaling();
        Gfx::RectF physRect = srcScaling.toPhysical(*rect);
        c.hasSrcRect = true;
        c.srcRect    = physRect;
        // Record logical destination dimensions separately; the srcRect holds
        // physical pixel coordinates for texture sampling, but nanovg draw
        // calls need logical units so the canvas transform is not applied twice.
        c.destW = static_cast<float>(rect->width());
        c.destH = static_cast<float>(rect->height());
    }

    _pixmap->commands().push_back(std::move(c));
}

} // namespace

} // namespace
