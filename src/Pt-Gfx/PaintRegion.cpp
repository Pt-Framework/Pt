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

#include <Pt/Gfx/PaintRegion.h>

namespace Pt {

namespace Gfx {

PaintRegion::PaintRegion(PaintSurface& surface, const Gfx::RectF& rect)
: _surface(0)
{           
    _surface = &surface;
    _area = rect; 
}


PaintRegion::~PaintRegion()
{
    _surface->onFinish();
}


const Gfx::SizeF& PaintRegion::onSize() const
{
    return _area.size();
}


void PaintRegion::onBegin(Painter& painter)
{
    _surface->onBegin(painter);
}


void PaintRegion::onFinish()
{
    _surface->onFinish();
}


const Gfx::ImageFormat& PaintRegion::format() const
{
    return _surface->format();
}


void PaintRegion::setClip(const Gfx::RectF& clip)
{
    _surface->setClip( Gfx::RectF( clip.topLeft() +  _area.topLeft(), clip.size()));
}


void PaintRegion::resetClip()
{
    _surface->resetClip();
}


void PaintRegion::setCompositionMode(const Gfx::CompositionMode& mode)
{
    _surface->setCompositionMode(mode);
}


void PaintRegion::setPen(const Gfx::Pen& pen)
{
    _surface->setPen(pen);
}


void PaintRegion::setBrush(const Gfx::Brush& brush)
{
    _surface->setBrush(brush);
}


void PaintRegion::setFont(const Gfx::Font& font)
{
    _surface->setFont(font);
}


Gfx::FontMetrics PaintRegion::fontMetrics(const Pt::String& text) const
{
    return _surface->fontMetrics(text);
}


void PaintRegion::drawLine(const Gfx::PointF& fromF, const Gfx::PointF& toF)
{
    _surface->drawLine(fromF + _area.topLeft(),
                       toF + _area.topLeft() );
}


void PaintRegion::drawText(const Gfx::PointF& toF, const Pt::String& text)
{
    _surface->drawText(toF + _area.topLeft(), text);
}


void PaintRegion::drawText(const Gfx::PointF& to, const Pt::String& text, const Gfx::Transform& trans)
{
    _surface->drawText(to + _area.topLeft(), text, trans);
}

void PaintRegion::drawRect(const Gfx::RectF& r)
{
    Gfx::RectF rect(r);
    rect.setOrigin(r.topLeft() + _area.topLeft());

    _surface->drawRect(rect);
}


void PaintRegion::fillRect(const Gfx::RectF& r)
{
    Gfx::RectF rect(r);
    rect.setOrigin(r.topLeft() + _area.topLeft());

    _surface->fillRect(rect);
}


void PaintRegion::drawEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    _surface->drawEllipse(topLeftF + _area.topLeft(), sizeF);
}


void PaintRegion::fillEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    _surface->fillEllipse(topLeftF + _area.topLeft(), sizeF);
}


void PaintRegion::drawPolyline(const Gfx::PointF* points, size_t pointCount)
{
    std::vector<Gfx::PointF> translated(pointCount);

    for (size_t i = 0; i < pointCount; i++)
        translated[i] = points[i] + _area.topLeft();

    _surface->drawPolyline(&translated[0], pointCount);
}


void PaintRegion::fillPolygon(const Gfx::PointF* points, size_t pointCount)
{
    std::vector<Gfx::PointF> translated(pointCount);

    for (size_t i = 0; i < pointCount; i++)
      translated[i] = points[i] + _area.topLeft();

    _surface->fillPolygon(&translated[0], pointCount);
}


void PaintRegion::drawImage(const Gfx::PointF& toF, const Gfx::Image& image)
{
    _surface->drawImage(toF + _area.topLeft(), image);
}


void PaintRegion::drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& imgRect)
{
    _surface->drawImage(to + _area.topLeft(), image, imgRect);
}


void PaintRegion::drawPath(const Gfx::Path& path, float smoothness)
{
    Gfx::Path tpath = path;

    Gfx::Transform trans;
    trans.translate(_area.topLeft().x(), _area.topLeft().y());

    tpath.transform(trans);

    _surface->drawPath(tpath, smoothness);
}


void PaintRegion::fillPath(const Path& path, float smoothness)
{
    Gfx::Path tpath = path;

    Gfx::Transform trans;
    trans.translate(_area.topLeft().x(), _area.topLeft().y());

    tpath.transform(trans);

    _surface->fillPath(tpath, smoothness);
}

void PaintRegion::drawSurface(const Gfx::PointF& toF, const PaintSurface& surface)
{
    _surface->drawSurface(_area.topLeft() + toF, surface);
}

void PaintRegion::drawSurface(const Gfx::PointF& toF, const PaintSurface& surface, const Gfx::RectF& pmRect)
{
    _surface->drawSurface(_area.topLeft() + toF, surface, pmRect);;
}


Image PaintRegion::toImage(const Gfx::ImageFormat& format) const
{
    return _surface->toImage(format);
}

void PaintRegion::drawChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{

}

void PaintRegion::fillChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{

}

void PaintRegion::drawPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{

}

void PaintRegion::fillPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{

}

void PaintRegion::drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{

}


} // namespace

} // namespace
