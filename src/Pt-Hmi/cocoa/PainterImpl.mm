/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2005-2007 Aloysius Indrayanto                           *
 *   Copyright (C) 2014-2007 Laurentiu-Gheorghe Crisan                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "PainterImpl.h"
#include "PaintSurfaceImpl.h"
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/FontMetrics.h>
#include <iostream>

namespace Pt {
namespace Hmi {

PainterImpl::PainterImpl(PaintSurfaceImpl* surface)
: _font("sans-serif")
, _surface(surface)
{
}


PainterImpl::~PainterImpl()
{
}

void PainterImpl::setPen(const Gfx::Pen& pen)
{
    _pen = pen;
  
    CGContextSetRGBStrokeColor(_surface->context(), _pen.color().red()/255.0,
                               _pen.color().green()/255.0,
                               _pen.color().blue()/255.0,
                               _pen.color().alpha()/255.0);
    
    CGContextSetLineWidth(_surface->context(), _pen.size());
    
    switch(_pen.capStyle())
    {
        case Pt::Gfx::Pen::FlatCap:
            CGContextSetLineCap(_surface->context(), kCGLineCapSquare);
        break;
            
        case Pt::Gfx::Pen::RoundCap:
            CGContextSetLineCap(_surface->context(), kCGLineCapRound);
        break;
        
        case Pt::Gfx::Pen::TriangularCap:
            CGContextSetLineCap(_surface->context(), kCGLineCapButt ); //Todo:
        break;
            
        case Pt::Gfx::Pen::ProjectingCap:
            CGContextSetLineCap(_surface->context(), kCGLineCapSquare ); //Todo:
        break;
        
        case Pt::Gfx::Pen::ButtCap:
            CGContextSetLineCap(_surface->context(), kCGLineCapButt );
        break;
        
        case Pt::Gfx::Pen::NotLastCap:
            CGContextSetLineCap(_surface->context(), kCGLineCapButt ); //Todo:
        break;
    }
    
    switch(_pen.style())
    {
        case Pt::Gfx::Pen::SolidStyle:
        {
            double dash[1] = {1.0};
            CGContextSetLineDash(_surface->context(),0, dash, 0);
        }
        break;
            
        case Pt::Gfx::Pen::DashStyle:
        {
            double dash[2] = {3.0,2.0};
            CGContextSetLineDash(_surface->context(),0, dash, 2);
        }
        
        break;
        
        case Pt::Gfx::Pen::DoubleDash:
        {
            double dash[3]  = {3.0,2.0, 2.0};
            CGContextSetLineDash(_surface->context(),0, dash, 3);
        }
        break;
    }
    
    switch(_pen.joinStyle())
    {
        case Pt::Gfx::Pen::RoundJoin:
            CGContextSetLineJoin(_surface->context(), kCGLineJoinRound);
        break;
        
        case Pt::Gfx::Pen::BevelJoin:
         CGContextSetLineJoin(_surface->context(),  kCGLineJoinBevel );
        break;
            
        case Pt::Gfx::Pen::MiterJoin:
            CGContextSetLineJoin(_surface->context(), kCGLineJoinMiter );
        break;
        
        case Pt::Gfx::Pen::TriangularJoin:
        break;
    }
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
    
    switch(_brush.fillStyle())
    {
        case Pt::Gfx::Brush::SolidFill:
            CGContextSetRGBFillColor(_surface->context(), _brush.color().red()/255.0, _brush.color().green()/255.0,    _brush.color().blue()/255.0,       _brush.color().alpha()/255.0);
        break;
            
        case Pt::Gfx::Brush::TextureFill:
        //CG
        break;
    }
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
    CFStringRef fname = CFStringCreateWithCString(0, _font.name().c_str(), kCFStringEncodingUTF8);
    CGFontRef fontRef = CGFontCreateWithFontName(fname);
    CGContextSetFont(_surface->context(), fontRef);
    CGContextSetFontSize(_surface->context(), _font.size());
    CFRelease(fname);
    CGFontRelease(fontRef);
}


Gfx::FontMetrics PainterImpl::fontMetrics() const
{
    return Gfx::FontMetrics(0, 0, 0, 0);
}


Gfx::FontMetrics PainterImpl::fontMetrics(const Pt::String& text) const
{
    return Gfx::FontMetrics(0, 0, 0, 0);
}


const std::list<std::string>& PainterImpl::fontFamilyNames()
{
    static const std::list<std::string> _fontList;
    return _fontList;
}

void PainterImpl::drawPixel(const Gfx::PointF& to)
{
    drawLine(to, to);
}

void PainterImpl::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    CGContextMoveToPoint(_surface->context(), from.x(), from.y());
    CGContextAddLineToPoint(_surface->context(), to.x(), to.y());
    CGContextStrokePath(_surface->context());
}


void PainterImpl::drawRect(const Gfx::RectF& rect)
{
    CGRect cgRect = CGRectMake(rect.x(), rect.y(), rect.width(), rect.height());
    CGContextStrokeRect(_surface->context(),cgRect);
}


void PainterImpl::drawText(const Gfx::PointF& to, const Pt::String& text)
{

}
    
void PainterImpl::drawText( const Gfx::PointF& to, const Pt::String& text, const Gfx::ARgbColor* outline )
{
}
    
void PainterImpl::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{    
    CGContextMoveToPoint(_surface->context(), points[0].x(), points[0].y());
    
    for( size_t i = 1; i < pointCount; ++i)
        CGContextAddLineToPoint(_surface->context(), points[i].x(), points[i].y());
    
    CGContextStrokePath(_surface->context());
}


void PainterImpl::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{

}


void PainterImpl::fillRect(const Gfx::RectF& rect)
{
    CGRect cgRect = CGRectMake(rect.x(), rect.y(), rect.width(), rect.height());
    CGContextFillRect(_surface->context(), cgRect);
}


void PainterImpl::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{

}


void PainterImpl::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
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
    CGRect rect = CGRectMake(to.x(), to.y(), pm.size().width(), pm.size().height());
    
    CGContextDrawImage(myContext,rect,image);
    CGImageRelease(image);
}

void PainterImpl::drawSurface(const Gfx::PointF& to, PaintSurface& pm, const Gfx::Region& pmRegion)
{
    Pt::Hmi::PaintSurfaceImpl* impl = pm.impl();
    CGContextRef context = impl->context();
    
    CGImageRef image =  CGBitmapContextCreateImage(context);
    
    CGContextRef myContext = _surface->context();
    
    CGRect rect = CGRectMake(to.x(), to.y(), pm.size().width(), pm.size().height());
    
    CGContextDrawImage(myContext,rect,image);
    CGImageRelease(image);
}

void PainterImpl::drawImage(const Gfx::PointF& to, const Gfx::ARgbImage& image)
{

}


void PainterImpl::drawImage(const Gfx::PointF& to, const Gfx::ARgbImage& image, const Gfx::Region& imageRegion)
{

}

}}


