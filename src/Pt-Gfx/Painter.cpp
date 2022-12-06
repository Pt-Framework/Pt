/* Copyright (C) 2015 Marc Boris Duerner
Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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


#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintSurface.h>

namespace Pt {

namespace Gfx {

Painter::Painter()
: _surface(0)
, _paintData(0)
{
}


Painter::Painter(PaintSurface& surface)
: _surface(0)
, _paintData(0)
{
    begin(surface);
}


Painter::~Painter()
{
    finish();
    delete _paintData;
}


void Painter::begin(PaintSurface& surface)
{
    finish();

    _surface = &surface;
    _surface->begin(*this);

    _surface->setPen(_pen);
    _surface->setBrush(_brush);
    _surface->setFont(_font);

    // TODO: RECT-NULL
    if( _clip.isNull() )
        _surface->resetClip();
    else
        _surface->setClip(_clip);

    _surface->setCompositionMode(_compositionMode);
}


void Painter::finish()
{
    if( _surface )
    {
        _surface->finish();
        _surface = 0;
    }
}


const Gfx::ImageFormat& Painter::format() const
{
    return _surface ? _surface->format() : ImageFormat::argb32();
}


void Painter::setCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;

    if(_surface)
        _surface->setCompositionMode(mode);
}


const Gfx::CompositionMode& Painter::compositionMode() const
{
    return _compositionMode;
}


void Painter::setClip(const Gfx::RectF& clip)
{
    _clip = clip;

    if (_clip.isNull())
    {
        if(_surface)
            _surface->resetClip();

        return;
    }

    if (_surface)
        _surface->setClip(_clip);
}


void Painter::resetClip()
{
    // TODO: RECT-NULL
    _clip = Gfx::RectF();

    if (_surface)
        _surface->resetClip();
}


void Painter::setPen(const Gfx::Pen& pen)
{
    if (_surface)
    {
        _surface->setPen(pen);
    }

    // keep pen in logical size
    _pen = pen;
}


const Gfx::Pen& Painter::pen() const
{
    return _pen;
}


void Painter::setBrush(const Gfx::Brush& brush)
{
    _brush = brush;

    if (_surface)
        _surface->setBrush(_brush);
}


const Gfx::Brush& Painter::brush() const
{
    return _brush;
}


void Painter::setFont(const Gfx::Font& font)
{
    _font = font;

    if (_surface)
        _surface->setFont(_font);
}


const Gfx::Font& Painter::font() const
{
    return _font;
}


Gfx::FontMetrics Painter::fontMetrics(const Pt::String& text) const
{
    if (!_surface)
        return Gfx::FontMetrics();

    return _surface->fontMetrics(text);
}


void Painter::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if (_pen.size() == 0)
        return;

    _surface->drawLine(from, to);
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    _surface->drawText(to, text);
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& text, const Gfx::Transform& trans)
{
    _surface->drawText(to, text, trans);
}


void Painter::drawRect(const Gfx::RectF& rect)
{
    _surface->drawRect(rect);
}


void Painter::fillRect(const Gfx::RectF& rect)
{
    _surface->fillRect(rect);
}


void Painter::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    _surface->drawEllipse(topLeft, size);
}


void Painter::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    _surface->fillEllipse(topLeft, size);
}


void Painter::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{
    if (_pen.size() == 0)
        return;

    _surface->drawPolyline(points, pointCount);
}


void Painter::fillPath(const Path& path, float smoothness)
{
    _surface->fillPath(path, smoothness);
}


void Painter::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    _surface->fillPolygon(points, pointCount);
}


void Painter::drawImage(const Gfx::PointF& to, const Gfx::Image& image)
{
    _surface->drawImage(to, image);
}


void Painter::drawImage(const Gfx::PointF& to,
    const Gfx::Image& image, const Gfx::RectF& imageRect)
{
    _surface->drawImage(to, image, imageRect);
}


void Painter::drawPath(const Gfx::Path& path, float smoothness)
{
    _surface->drawPath(path, smoothness);
}


void Painter::drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    _surface->drawArc(topLeft, size, degBegin, degEnd);
}


void Painter::drawChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    _surface->drawChord(topLeft, size, degBegin, degEnd);
}


void Painter::drawPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    _surface->drawPie(topLeft, size, degBegin, degEnd);
}


void Painter::fillPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    _surface->fillPie(topLeft, size, degBegin, degEnd);
}


void Painter::fillChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
    _surface->fillChord(topLeft, size, degBegin, degEnd);
}


void Painter::drawSurface(const Gfx::PointF& toF, const PaintSurface& surface)
{
    _surface->drawSurface(toF, surface);
}


void Painter::drawSurface(const Gfx::PointF& toF, const PaintSurface& pm, const Gfx::RectF& pmRect)
{
    _surface->drawSurface(toF, pm, pmRect);
}


Image Painter::toImage() const
{
    return _surface->toImage();
}


double Painter::scaleFactor() const
{
    return _surface ? _surface->scaleFactor() : 1.0;
}


double Painter::toPhysical(double n) const
{
    return _surface->toPhysical(n);
}


Gfx::PointF Painter::toPhysical(const Gfx::PointF& p) const
{
    return _surface->toPhysical(p);
}


Gfx::SizeF Painter::toPhysical(const Gfx::SizeF& s) const
{
    return _surface->toPhysical(s);
}


Gfx::RectF Painter::toPhysical(const Gfx::RectF& r) const
{
    return _surface->toPhysical(r);
}


double Painter::toLogical(double n) const
{
    return _surface->toLogical(n);
}


Gfx::PointF Painter::toLogical(const Gfx::PointF& p) const
{
    return _surface->toLogical(p);
}


Gfx::SizeF Painter::toLogical(const Gfx::SizeF& s) const
{
    return _surface->toLogical(s);
}


Gfx::RectF Painter::toLogical(const Gfx::RectF& r) const
{
    return _surface->toLogical(r);
}


double Painter::align(double n) const
{
    return _surface->align(n);
}


double Painter::alignPixel(double n) const
{
    return _surface->alignPixel(n);
}


double Painter::alignContour(size_t n) const
{
    return _surface->alignContour(n);
}


Gfx::PointF Painter::align(const Gfx::PointF& p) const
{
    return _surface->align(p);
}


Gfx::SizeF Painter::align(const Gfx::SizeF& s) const
{
    return _surface->align(s);
}


Gfx::RectF Painter::align(const Gfx::RectF& rect) const
{
    return _surface->align(rect);
}

} // namespace

} // namespace
