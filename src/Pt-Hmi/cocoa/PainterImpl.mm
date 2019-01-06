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

namespace Pt {

namespace Hmi {
/*

PainterImpl::PainterImpl(PaintSurfaceImpl* surface)
: _font("sans-serif")
, _surface(surface)
{
}


PainterImpl::~PainterImpl()
{
}


Pt::Gfx::PointF PainterImpl::tranPoint(const Pt::Gfx::PointF& p)
{
    return Pt::Gfx::PointF(p.x(), _surface->size().height() - p.y());
}


void PainterImpl::setPen(const Gfx::Pen& pen)
{
    _pen = pen;
  

}


const Gfx::Pen& PainterImpl::pen() const
{
    return _pen;
}


void PainterImpl::setSurface(PaintSurface& s)
{
    _surface = s.impl();
    
    setPen(_pen);
    setBrush(_brush);
    setFont(_font);
}


void PainterImpl::setBrush(const Gfx::Brush& brush)
{
    _brush = brush;
    

}


const Gfx::Brush& PainterImpl::brush() const
{
    return _brush;
}


const Gfx::Font& PainterImpl::font() const
{
    return _font;
}


void PainterImpl::setFont(const Gfx::Font& font)
{
    _font = font;

}


Gfx::FontMetrics PainterImpl::fontMetrics() const
{
	//TODO:
    return Gfx::FontMetrics(0, 0, 0, 0);
}


Gfx::FontMetrics PainterImpl::fontMetrics(const Pt::String& text) const
{
	//TODO:
    return Gfx::FontMetrics(0, 0, 0, 0);
}


const std::list<std::string>& PainterImpl::fontFamilyNames()
{
    static const std::list<std::string> _fontList;
    return _fontList;
}


void PainterImpl::drawLine(const Gfx::PointF& f, const Gfx::PointF& t)
{
    Gfx::PointF from = tranPoint(f);
    Gfx::PointF to  = tranPoint(t);
    CGContextMoveToPoint(_surface->context(), from.x(), from.y());
    CGContextAddLineToPoint(_surface->context(), to.x(), to.y());
    CGContextStrokePath(_surface->context());
}


void PainterImpl::drawRect(const Gfx::RectF& rect)
{
    CGRect cgRect = CGRectMake(rect.x(), _surface->size().height() - (rect.y() + rect.height()), rect.width(), rect.height());
    CGContextStrokeRect(_surface->context(),cgRect);
}


void PainterImpl::drawText(const Gfx::PointF& to, const Pt::String& text)
{
//TODO:
//  // Drawing code
//  CGContextRef  context = UIGraphicsGetCurrentContext();
//  CGContextSelectFont(context, "Arial", 24, kCGEncodingFontSpecific);
//  CGContextSetTextPosition(context,80,80);
//  CGContextShowText(context, "hello", 6);
  //not even this works
//  CGContextShowTextAtPoint(context, 1,1, "hello", 6);
}
        
void PainterImpl::drawPolyline(const Gfx::PointF* p, const size_t pointCount)
{
    std::vector<Gfx::PointF> points(pointCount);
    
    for( size_t i = 0; i < pointCount; ++i)
        points[i] = tranPoint(p[i]);
    
    CGContextMoveToPoint(_surface->context(), points[0].x(), points[0].y());
    
    for( size_t i = 1; i < pointCount; ++i)
        CGContextAddLineToPoint(_surface->context(), points[i].x(), points[i].y());
    
    CGContextStrokePath(_surface->context());
}


void PainterImpl::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
//TODO:
}


void PainterImpl::fillRect(const Gfx::RectF& rect)
{
    CGRect cgRect = CGRectMake(rect.x(), _surface->size().height() - (rect.y() + rect.height()) , rect.width(), rect.height());
    CGContextFillRect(_surface->context(), cgRect);
}


void PainterImpl::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
//TODO:
}


void PainterImpl::fillPolygon(const Gfx::PointF* p, const size_t pointCount)
{
    std::vector<Gfx::PointF> points(pointCount);
    
    for( size_t i = 0; i < pointCount; ++i)
        points[i] = tranPoint(p[i]);
    
    CGContextMoveToPoint(_surface->context(), points[0].x(), points[0].y());
    
    for( size_t i = 1; i < pointCount; ++i)
        CGContextAddLineToPoint(_surface->context(), points[i].x(), points[i].y());
    
    CGContextFillPath(_surface->context());
}

void PainterImpl::drawSurface(const Gfx::PointF& to, PaintSurface& pm)
{
    
    Pt::Hmi::PaintSurfaceImpl* impl = pm.impl();
    CGContextRef context = impl->context();
    
    CGImageRef image =  CGBitmapContextCreateImage(context);
    
    CGContextRef myContext = _surface->context();
    CGRect rect = CGRectMake(to.x(), _surface->size().height() - (to.y() + pm.size().height()), pm.size().width(), pm.size().height());
    
    CGContextDrawImage(myContext,rect,image);

    CGImageRelease(image);
    
}

void PainterImpl::drawSurface(const Gfx::PointF& to, PaintSurface& pm, const Gfx::Region& pmRegion)
{
    Pt::Hmi::PaintSurfaceImpl* impl = pm.impl();
    CGContextRef context = impl->context();
    
    CGImageRef image =  CGBitmapContextCreateImage(context);
    
    CGContextRef myContext = _surface->context();
    
    CGRect rect = CGRectMake(to.x(), _surface->size().height() - (to.y() + pm.size().height()), pm.size().width(), pm.size().height());
    
    CGContextDrawImage(myContext,rect,image);
    CGImageRelease(image);
}

void PainterImpl::drawImage(const Gfx::PointF& to, const Gfx::ARgbImage& image)
{
//TODO:
}


void PainterImpl::drawImage(const Gfx::PointF& to, const Gfx::ARgbImage& image, const Gfx::Region& imageRegion)
{
//TODO:
}
*/

} // namespace

} // namespace
