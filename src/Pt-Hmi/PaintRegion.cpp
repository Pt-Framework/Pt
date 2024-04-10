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

#include <Pt/Hmi/PaintRegion.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Hmi/Painter.h>

namespace Pt {

namespace Hmi {

PaintRegion::PaintRegion()
: _surface(0)
{
}


PaintRegion::PaintRegion(PaintSurface& surface, const Gfx::RectF& rect)
: _surface(&surface)
, _area(rect)
{
}


PaintRegion::~PaintRegion()
{
    if(_surface)
        _surface->onFinish();
}


void PaintRegion::reset(Hmi::PaintSurface& surface, const Gfx::RectF& rect)
{
    _surface = &surface;
    _area = rect;

    // restart currently attached painter
    Gfx::Painter* painter = this->painter();
    if(painter)
        painter->begin(*this);
}


void PaintRegion::reset()
{
    if(_surface)
        _surface->onFinish();

    _surface = 0;
    _area.clear(); 
}


const Gfx::SizeF& PaintRegion::onSize() const
{
    return _area.size();
}


double PaintRegion::onScaleFactor() const
{
    if(_surface)
        return _surface->onScaleFactor();

    return 1.0;
}


void PaintRegion::onBegin(Gfx::Painter& painter)
{
    if(_surface)
        _surface->onBegin(painter);
}


void PaintRegion::onFinish()
{
    if(_surface)
        _surface->onFinish();
}


void PaintRegion::drawPixmap(const Gfx::PointF& toF, 
                             const PixmapSurface& surface) 
{
    if(_surface)
        _surface->drawPixmap(_area.topLeft() + toF, surface);
}


void PaintRegion::drawPixmap(const Gfx::PointF& toF, 
                             const PixmapSurface& surface, 
                             const Gfx::RectF& surfaceRect) 
{
    if(_surface)
        _surface->drawPixmap(_area.topLeft() + toF, surface, surfaceRect);
}


const Gfx::ImageFormat& PaintRegion::format() const
{
    return _surface ? _surface->format()
                    : Gfx::ImageFormat::argb32();
}


void PaintRegion::setClip(const Gfx::RectF& clip)
{
    if(_surface)
        _surface->setClip( Gfx::RectF( clip.topLeft() +  _area.topLeft(), clip.size()));
}


void PaintRegion::resetClip()
{
    if(_surface)
        _surface->resetClip();
}


void PaintRegion::setCompositionMode(const Gfx::CompositionMode& mode)
{
    if(_surface)
        _surface->setCompositionMode(mode);
}


void PaintRegion::setPen(const Gfx::Pen& pen)
{
    if(_surface)
        _surface->setPen(pen);
}


void PaintRegion::setBrush(const Gfx::Brush& brush)
{
    if(_surface)
        _surface->setBrush(brush);
}


void PaintRegion::setFont(const Gfx::Font& font)
{
    if(_surface)
        _surface->setFont(font);
}


Gfx::FontMetrics PaintRegion::fontMetrics(const Pt::String& text) const
{
    return _surface ? _surface->fontMetrics(text)
                    : Gfx::FontMetrics();
}


void PaintRegion::drawLine(const Gfx::PointF& fromF, const Gfx::PointF& toF)
{
    if(_surface)
        _surface->drawLine(fromF + _area.topLeft(),
                           toF + _area.topLeft() );
}


void PaintRegion::drawText(const Gfx::PointF& toF, const Pt::String& text)
{
    if(_surface)
        _surface->drawText(toF + _area.topLeft(), text);
}


void PaintRegion::drawText(const Gfx::PointF& to, const Pt::String& text, const Gfx::Transform& trans)
{
    if(_surface)
        _surface->drawText(to + _area.topLeft(), text, trans);
}


void PaintRegion::drawRect(const Gfx::RectF& r)
{
    Gfx::RectF rect(r);
    rect.setOrigin(r.topLeft() + _area.topLeft());

    if(_surface)
        _surface->drawRect(rect);
}


void PaintRegion::fillRect(const Gfx::RectF& r)
{
    Gfx::RectF rect(r);
    rect.setOrigin(r.topLeft() + _area.topLeft());

    if(_surface)
        _surface->fillRect(rect);
}


void PaintRegion::drawEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    if(_surface)
        _surface->drawEllipse(topLeftF + _area.topLeft(), sizeF);
}


void PaintRegion::fillEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    if(_surface)
        _surface->fillEllipse(topLeftF + _area.topLeft(), sizeF);
}


void PaintRegion::drawPolyline(const Gfx::PointF* points, size_t pointCount)
{
    std::vector<Gfx::PointF> translated(pointCount);

    for (size_t i = 0; i < pointCount; i++)
        translated[i] = points[i] + _area.topLeft();

    if(_surface)
        _surface->drawPolyline(&translated[0], pointCount);
}


void PaintRegion::fillPolygon(const Gfx::PointF* points, size_t pointCount)
{
    std::vector<Gfx::PointF> translated(pointCount);

    for (size_t i = 0; i < pointCount; i++)
      translated[i] = points[i] + _area.topLeft();

    if(_surface)
        _surface->fillPolygon(&translated[0], pointCount);
}


void PaintRegion::drawImage(const Gfx::PointF& toF, const Gfx::Image& image)
{
    if(_surface)
        _surface->drawImage(toF + _area.topLeft(), image);
}


void PaintRegion::drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& imgRect)
{
    if(_surface)
        _surface->drawImage(to + _area.topLeft(), image, imgRect);
}


void PaintRegion::drawPath(const Gfx::Path& path, float smoothness)
{
    Gfx::Path tpath = path;

    Gfx::Transform trans;
    trans.translate(_area.topLeft().x(), _area.topLeft().y());

    tpath.transform(trans);

    if(_surface)
        _surface->drawPath(tpath, smoothness);
}


void PaintRegion::fillPath(const Gfx::Path& path, float smoothness)
{
    Gfx::Path tpath = path;

    Gfx::Transform trans;
    trans.translate(_area.topLeft().x(), _area.topLeft().y());

    tpath.transform(trans);

    if(_surface)
        _surface->fillPath(tpath, smoothness);
}


void PaintRegion::drawSurface(const Gfx::PointF& toF, const Gfx::PaintSurface& surface)
{
    if(_surface)
        _surface->drawSurface(_area.topLeft() + toF, surface);
}


void PaintRegion::drawSurface(const Gfx::PointF& toF, const Gfx::PaintSurface& surface, const Gfx::RectF& pmRect)
{
    if(_surface)
        _surface->drawSurface(_area.topLeft() + toF, surface, pmRect);;
}


Gfx::Image PaintRegion::toImage() const
{
    return _surface ? _surface->toImage()
                    : Gfx::Image();
}


void PaintRegion::drawChord(const Gfx::PointF& topLeft, const Gfx::SizeF& size, float degBegin, float degEnd)
{
}


void PaintRegion::fillChord(const Gfx::PointF& topLeft, const Gfx::SizeF& size, float degBegin, float degEnd)
{
}


void PaintRegion::drawPie(const Gfx::PointF& topLeft, const Gfx::SizeF& size, float degBegin, float degEnd)
{
}


void PaintRegion::fillPie(const Gfx::PointF& topLeft, const Gfx::SizeF& size, float degBegin, float degEnd)
{
}


void PaintRegion::drawArc(const Gfx::PointF& topLeft, const Gfx::SizeF& size, float degBegin, float degEnd)
{
}

} // namespace

} // namespace
