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

#include "PainterImpl.h"
#include "PaintSurfaceImpl.h"

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Painter.h>
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Application.h>

namespace Pt {

namespace Hmi {

Painter::Painter(PaintSurface& surface)
: _impl( new PainterImpl )
, _surface(0)
, _pen()
, _brush( Gfx::Color(0, 0, 0, 0) )
, _font()
{
    begin(surface);
}


Painter::~Painter()
{
    finish();

    delete _impl;
}


void Painter::begin(PaintSurface& surface)
{
    finish();

    _surface = &surface;
    _surface->begin(*this);
    
    _surface->setPen(_pen);
    _surface->setBrush(_brush);
    _surface->setFont(_font);
    _surface->setClip(_clip);
    _surface->setCompositionMode(_compositionMode); 
}


void Painter::finish()
{
    if(_surface)
    {
        _surface->finish(*this);
        _surface = 0;
    }
}


void Painter::onDetach()
{
    _surface = 0;
}


const Gfx::ImageFormat& Painter::format() const
{
  return _surface->format();
}


void Painter::setCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;

    _impl->setCompositionMode(mode);

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

    // NOTE: cannot cache clip in _impl, because of surface transformation

    if(_surface)
        _surface->setClip(_clip);
}


void Painter::setPen(const Gfx::Pen& pen)
{
    double scaleFactor = Application::instance().screen().scaleFactor();

    // keep pen size when downscaling
    double scaledSize = scaleFactor < 1.0 ? pen.size() 
                                          : scaleFactor * pen.size();

    size_t penSize = static_cast<size_t>(scaledSize);

    Gfx::Pen scaledPen = pen;
    scaledPen.setSize(penSize);

    _impl->setPen(scaledPen);

    if(_surface)
        _surface->setPen(scaledPen);

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

    _impl->setBrush(_brush);

    if(_surface)
        _surface->setBrush(_brush);
}


const Gfx::Brush& Painter::brush() const
{
    return _brush;
}


void Painter::setFont(const Gfx::Font& font)
{
    _font = font;
    _impl->setFont(_font);

    if(_surface)
        _surface->setFont(_font);
}


const Gfx::Font& Painter::font() const
{
    return _font;
}


Gfx::FontMetrics Painter::fontMetrics(const Pt::String& text) const
{
    if( ! _surface)
        return fontMetrics(_font, text);

    return _surface->fontMetrics(text);
}


Gfx::FontMetrics Painter::fontMetrics(const Gfx::Font& font, const Pt::String& text)
{
    return PainterImpl::fontMetrics(font, text);
}


std::vector<std::string> Painter::fontNames()
{
    return PaintSurfaceImpl::fontNames();
}


void Painter::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( _pen.size() == 0 )
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
    if( _pen.size() == 0 )
       return;

    _surface->drawPolyline(points, pointCount);
}


void Painter::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    _surface->fillPolygon(points, pointCount);
}


void Painter::drawImage(const Gfx::PointF& to, const Gfx::Image& image)
{
    _surface->drawImage(to, image);
}


void Painter::drawPicture(const Gfx::PointF& to, const Picture& pic)
{
    _surface->drawPicture( to, pic );
}


void Painter::drawSurface(const Gfx::PointF& to, const PixmapSurface& surface)
{
    _surface->drawSurface(to, surface);
}


void Painter::drawSurface(const Gfx::PointF& to, 
                          const PixmapSurface& pm, const Gfx::RectF& pmRect)
{
    _surface->drawSurface(to, pm, pmRect);
}


void Painter::drawImage(const Gfx::PointF& to, 
                        const Gfx::Image& image, const Gfx::RectF& imageRect)
{
    _surface->drawImage(to, image, imageRect);
}


void Painter::setDefaultFont(std::string f)
{
    PaintSurface::setDefaultFont(f);
}


double Painter::toPhysical(double n) const
{
    return _surface ? _surface->toPhysical(n) : n;
}


Gfx::PointF Painter::toPhysical(const Gfx::PointF& p) const
{
    return _surface ?_surface->toPhysical(p) : p;
}


Gfx::SizeF Painter::toPhysical(const Gfx::SizeF& s) const
{
    return _surface ? _surface->toPhysical(s) : s;
}


Gfx::RectF Painter::toPhysical(const Gfx::RectF& r) const
{
    return _surface ? _surface->toPhysical(r) : r;
}


double Painter::toLogical(double n) const
{
    return _surface ? _surface->toLogical(n) : n;
}


Gfx::PointF Painter::toLogical(const Gfx::PointF& p) const
{
    return _surface ? _surface->toLogical(p) : p;
}


Gfx::SizeF Painter::toLogical(const Gfx::SizeF& s) const
{
    return _surface ? _surface->toLogical(s) : s;
}


Gfx::RectF Painter::toLogical(const Gfx::RectF& r) const
{
    return _surface ? _surface->toLogical(r) : r;
}


double Painter::alignContour(size_t n) const
{
    return _surface ? _surface->alignContour(n) : n;
}


double Painter::align(double n) const
{
    return _surface ? _surface->align(n) : n;
}


Gfx::PointF Painter::align(const Gfx::PointF& p) const
{
    return _surface ? _surface->align(p) : p;
}


Gfx::RectF Painter::align(const Gfx::RectF& rect) const
{
    return _surface ? _surface->align(rect) : rect;
}

} // namespace

} // namespace
