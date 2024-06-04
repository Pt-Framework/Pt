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

PaintData::PaintData()
: _painter(0)
, _canvas(0)
, _isDirty(false)
, _scaleFactor(1.0)
{
}


PaintData::~PaintData()
{
    if(_canvas)
    {
        _canvas->detachPaint();
        _canvas = 0;
    }
}


void PaintData::onDetach(Canvas& canvas)
{
    if(_canvas)
    {
        onFinish();
        _canvas = 0;
    }
}


void PaintData::attachPainter(Painter& painter)
{
    _painter = &painter;
    _isDirty = true;
}


void PaintData::begin(PaintSurface& surface)
{
    finish();

    double scaleFactor = surface.scaling().scaleFactor();

    if(_scaleFactor != scaleFactor)
    {
        _scaleFactor = scaleFactor;
        _isDirty = true;
    }

    if(_isDirty)
    {
        onSetPen( _painter->pen() );
        onSetCompositionMode( _painter->compositionMode() );
        onSetBrush( _painter->brush() );
        onSetFont( _painter->font() );

        Gfx::RectF clip = _painter->clip();
        if( clip.isNull() )
        {
            onResetClip();
        }
        else
        {
            clip.shift( origin().x(), origin().y() );
            onSetClip(clip);
        }

        _isDirty = false;
    }
    
    _region = surface.region();

    Canvas* canvas = surface.canvas();
    if(canvas)
    {
        canvas->attachPaint(*this);
        _canvas = canvas;
    }
    
    if(_canvas && _painter)
    {
        _canvas->setCompositionMode( _painter->compositionMode() );
        _canvas->setPen( _painter->pen() );
        _canvas->setBrush( _painter->brush() );
        _canvas->setFont( _painter->font() );

        Gfx::RectF clip = _painter->clip();
        if( clip.isNull() )
        {
            _canvas->resetClip();
        }
        else
        {
          clip.shift( origin().x(), origin().y() );
          _canvas->setClip(clip);
        }
    }
}


void PaintData::finish()
{
    if(_canvas)
    {
        onFinish();

        _canvas->detachPaint();
        _canvas = 0;
    }
}


Canvas* PaintData::canvas()
{
    return _canvas;
}


const RectF& PaintData::region() const
{
    return _region;
}


const PointF& PaintData::origin() const
{
    return _region.topLeft();
}


void PaintData::setCompositionMode(const Gfx::CompositionMode& mode)
{
    if(_canvas)
        _canvas->setCompositionMode(mode);
}


void PaintData::setPen(const Pen& pen)
{
    onSetPen(pen);

    if(_canvas)
        _canvas->setPen(pen);
}


void PaintData::setBrush(const Brush& brush)
{
    onSetBrush(brush);

    if(_canvas)
        _canvas->setBrush(brush);
}


void PaintData::setFont(const Gfx::Font& font)
{
    onSetFont(font);

    if(_canvas)
        _canvas->setFont(font);
}


void PaintData::setClip(const RectF& rect)
{ 
    Gfx::RectF clip = rect;
    clip.shift( origin().x(), origin().y() );

    onSetClip(clip);

    if(_canvas)
        _canvas->setClip(clip);
}


void PaintData::resetClip()
{
    onResetClip();

    if(_canvas)
        _canvas->resetClip();
}


void PaintData::drawLine(const PointF& from, const PointF& to)
{   
    if(_canvas)
    {
        Gfx::Line line(*this, from, to);
        _canvas->drawLine(line);
    }
}


void PaintData::drawRect(const Gfx::RectF& rect)
{
    Gfx::RectF r = rect;
    
    r.shift( origin().x(), 
             origin().y() );

    if(_canvas)
        _canvas->drawRect(r);
}


void PaintData::fillRect(const Gfx::RectF& rect)
{
    Gfx::RectF r = rect;
    
    r.shift( origin().x(), 
             origin().y() );

    if(_canvas)
        _canvas->fillRect(r);
}


void PaintData::drawPolyline(const Gfx::PointF* ps, const size_t n)
{
    if(_canvas)
    {
        Polyline line(*this, ps, n);
        _canvas->drawPolyline(line);
    }
}


void PaintData::fillPolygon(const Gfx::PointF* ps, const size_t n)
{
    if(_canvas)
    {
        Polyline line(*this, ps, n);
        _canvas->fillPolygon(line);
    }
}


void PaintData::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    Pt::Gfx::PointF p = topLeft + origin();

    if(_canvas)
        _canvas->drawEllipse(p, size);
}


void PaintData::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    Pt::Gfx::PointF p = topLeft + origin();

    if(_canvas)
        _canvas->fillEllipse(p, size);
}


FontMetrics PaintData::fontMetrics(const Pt::String& text) const
{
    if(_canvas)
        return _canvas->fontMetrics(text);

    return FontMetrics();
}


void PaintData::drawText(const PointF& to, const Pt::String& text)
{
    Pt::Gfx::PointF p = to + origin(); 
    if(_canvas)
        _canvas->drawText(p, text);
}


void PaintData::drawText(const PointF& to, const Pt::String& text, const Transform& t)
{
    Pt::Gfx::PointF p = to + origin(); 
    if(_canvas)
        _canvas->drawText(p, text, t);
}


void PaintData::drawImage(const Gfx::PointF& to, 
                          const Gfx::Image& image)
{
    Pt::Gfx::PointF p = to + origin(); 
    if(_canvas)
        _canvas->drawImage(p, image);
}


void PaintData::drawImage(const Gfx::PointF& to, 
                          const Gfx::Image& image, 
                          const Gfx::RectF& rect)
{
    Pt::Gfx::PointF p = to + origin();

    if(_canvas)
        _canvas->drawImage(p, image, rect);
}


void PaintData::drawSurface(const Gfx::PointF& to, 
                            const Gfx::PaintSurface& surface)
{
    Pt::Gfx::PointF p = to + origin();
    if(_canvas)
        _canvas->drawSurface(p, surface);
}


void PaintData::drawSurface(const Gfx::PointF& to,
                            const Gfx::PaintSurface& surface,
                            const Gfx::RectF& rect)
{
    Pt::Gfx::PointF p = to + origin();

    if(_canvas)
        _canvas->drawSurface(p, surface, rect);
}

//
// Painter
//

Painter::Painter()
: _surface(0)
, _paint(0)
{
}


Painter::Painter(PaintSurface& surface)
: _surface(0)
, _paint(0)
{
    begin(surface);
}


Painter::~Painter()
{
    if( _surface )
    {
        _surface->detachPainter();
        _surface = 0;
    }

    delete _paint;
}


void Painter::begin(PaintSurface& surface)
{
    finish();

    surface.attachPainter(*this);
    _surface = &surface;

    PaintData* paint = surface.getPaint(_paint);
    if(_paint != paint)
    {
        delete _paint;
        _paint = paint;

        _paint->attachPainter(*this);
    }
    
    if(_paint)
        _paint->begin(*_surface);
}


void Painter::finish()
{
    if(_paint)
    {
        _paint->finish();
        // keep paint
    }

    if( _surface )
    {
        _surface->detachPainter();
        _surface = 0;
    }
}


void Painter::onDetach(PaintSurface& surface)
{
    _surface = 0;
}


const Gfx::ImageFormat& Painter::format() const
{
    Gfx::PaintSurface* surface = _surface;

    return surface ? surface->format() : ImageFormat::argb32();
}


const Gfx::CompositionMode& Painter::compositionMode() const
{
    return _compositionMode;
}


void Painter::setCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;

    if(_paint)
        _paint->setCompositionMode(_compositionMode);
}


void Painter::setPen(const Gfx::Pen& pen)
{
    _pen = pen;

    if(_paint)
        _paint->setPen(pen);
}


const Gfx::Pen& Painter::pen() const
{
    return _pen;
}


void Painter::setClip(const Gfx::RectF& clip)
{
    _clip = clip;

    if(_paint)
    {
        if( _clip.isNull() )
            _paint->resetClip();
        else
            _paint->setClip(_clip);
    }
}


void Painter::resetClip()
{
    // TODO: RECT-NULL
    _clip = Gfx::RectF();

    if(_paint)
        _paint->resetClip();
}


void Painter::setBrush(const Gfx::Brush& brush)
{
    _brush = brush;

    if(_paint)
        _paint->setBrush(_brush);
}


const Gfx::Brush& Painter::brush() const
{
    return _brush;
}


void Painter::setFont(const Gfx::Font& font)
{
    _font = font;

    if(_paint)
        _paint->setFont(_font);
}


const Gfx::Font& Painter::font() const
{
    return _font;
}


Gfx::FontMetrics Painter::fontMetrics(const Pt::String& text) const
{
    if(_paint)
        return _paint->fontMetrics(text);

    return Gfx::FontMetrics();
}


void Painter::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    if( _pen.size() == 0 )
        return;

    if(_paint)
        _paint->drawLine(from, to);
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& text)
{
    if(_paint)
        _paint->drawText(to, text);
}


void Painter::drawText(const Gfx::PointF& to, const Pt::String& text, 
                       const Gfx::Transform& trans)
{
    if(_paint)
        _paint->drawText(to, text, trans);
}


void Painter::drawRect(const Gfx::RectF& rect)
{
    if(_paint)
        _paint->drawRect(rect);
}


void Painter::fillRect(const Gfx::RectF& rect)
{
    if(_paint)
        _paint->fillRect(rect);
}


void Painter::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_paint)
        _paint->drawEllipse(topLeft, size);
}


void Painter::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if(_paint)
        _paint->fillEllipse(topLeft, size);
}


void Painter::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{
    if (_pen.size() == 0)
        return;

    if(_paint)
        _paint->drawPolyline(points, pointCount);
}


void Painter::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    if(_paint)
        _paint->fillPolygon(points, pointCount);
}


void Painter::drawSurface(const Gfx::PointF& toF, 
                          const PaintSurface& surface)
{
    if(_paint)
        _paint->drawSurface(toF, surface);
}


void Painter::drawSurface(const Gfx::PointF& toF, 
                          const PaintSurface& surface, 
                          const Gfx::RectF& surfaceRect)
{
    if(_paint)
        _paint->drawSurface(toF, surface, surfaceRect);
}


void Painter::drawImage(const Gfx::PointF& to, 
                        const Gfx::Image& image)
{
    if(_paint)
        _paint->drawImage(to, image);
}


void Painter::drawImage(const Gfx::PointF& to,
                        const Gfx::Image& image, 
                        const Gfx::RectF& imageRect)
{
    if(_paint)
        _paint->drawImage(to, image, imageRect);
}


void Painter::drawPath(const Gfx::Path& path, float smoothness)
{
}


void Painter::fillPath(const Path& path, float smoothness)
{
}


void Painter::drawArc(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void Painter::drawChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void Painter::drawPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void Painter::fillPie(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
}


void Painter::fillChord(const PointF& topLeft, const SizeF& size, float degBegin, float degEnd)
{
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
