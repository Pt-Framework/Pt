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

#include "PixmapSurfaceImpl.h"

namespace Pt {

namespace Hmi {

PixmapSurfaceImpl::PixmapSurfaceImpl()
: _size(10, 10)
{
    create();
}


PixmapSurfaceImpl::~PixmapSurfaceImpl()
{
    destroy();
}





void PixmapSurfaceImpl::create()
{
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    _context = CGBitmapContextCreate(nullptr, 
                                     _size.width(), _size.height(), 
                                     8, 0, colorSpace, 
                                     kCGImageAlphaPremultipliedLast);

    CGColorSpaceRelease(colorSpace);
}


void PixmapSurfaceImpl::destroy()
{
    if(_context == nullptr)
        return;
    
    CGContextRelease(_context);
    _context = nullptr;
}


void PixmapSurfaceImpl::clear(const Gfx::Color& c)
{
}


const Gfx::SizeF& PixmapSurfaceImpl::size() const
{
    return _size;
}


void PixmapSurfaceImpl::resize(const Pt::Gfx::SizeF& size)
{
	_size = size;
    
    if(_size.width() ==  0)
        _size.setWidth(10);
    
    if(_size.height() ==  0)
        _size.setHeight(10);
    
    destroy();
    create();
}


const Gfx::ImageFormat& PixmapSurfaceImpl::format() const
{
    return Gfx::ImageFormat::argb32();
}


void PixmapSurfaceImpl::begin(Painter& painter)
{
    _painter = &painter;
}


void PixmapSurfaceImpl::finish()
{
    _painter = 0;

    //SelectObject(_dc, _oldPen);
    //SelectObject(_dc, _oldBrush);
    //SelectObject(_dc, _oldFont);
}


void PixmapSurfaceImpl::setClip(const Gfx::RectF& clipRect)
{
    // _painter->impl()->setClip(clipRect);

    // HRGN hrgn = _painter->impl()->clipRect();

    // if(hrgn)
    //     SelectClipRgn(_dc, hrgn);
    // else
    //     SelectClipRgn(_dc, NULL);
}


void PixmapSurfaceImpl::setCompositionMode(const Gfx::CompositionMode& mode)
{
}


void PixmapSurfaceImpl::setPen(const Gfx::Pen& pen)
{
    CGContextSetRGBStrokeColor(_context, pen.color().red() / 257,
                               pen.color().green() / 257,
                               pen.color().blue() / 257,
                               pen.color().alpha() / 257);
    
    CGContextSetLineWidth(_context, pen.size());
    
    switch( pen.capStyle() )
    {
        default:
        case Pt::Gfx::Pen::ButtCap:
            CGContextSetLineCap(_context, kCGLineCapButt);
            break;
        
        case Pt::Gfx::Pen::SquareCap:
            CGContextSetLineCap(_context, kCGLineCapSquare );
            break; 
        
        case Pt::Gfx::Pen::RoundCap:
            CGContextSetLineCap(_context, kCGLineCapRound);
            break;
    }
    
    switch( pen.style() )
    {
        default:
        case Pt::Gfx::Pen::Solid:
        {
            double dash[1] = {1.0};
            CGContextSetLineDash(_context,0, dash, 0);
            break;
        }
            
        case Pt::Gfx::Pen::Dash:
        {
            double dash[2] = {3.0, 2.0};
            CGContextSetLineDash(_context,0, dash, 2);
            break;
        }
 
        case Pt::Gfx::Pen::DoubleDash:
        {
            double dash[3]  = {3.0, 2.0, 2.0};
            CGContextSetLineDash(_context,0, dash, 3);
            break;
        }
    }
    
    switch( pen.joinStyle() )
    {
        default:
        case Pt::Gfx::Pen::RoundJoin:
            CGContextSetLineJoin(_context, kCGLineJoinRound);
            break;
        
        case Pt::Gfx::Pen::BevelJoin:
         CGContextSetLineJoin(_context, kCGLineJoinBevel );
            break;
            
        case Pt::Gfx::Pen::MiterJoin:
            CGContextSetLineJoin(_context, kCGLineJoinMiter );
            break;
    }
}

void PixmapSurfaceImpl::setBrush(const Gfx::Brush& brush)
{
    switch( brush.fillStyle() )
    {
        default:
        case Pt::Gfx::Brush::Solid:
            CGContextSetRGBFillColor(_context, 
                                     brush.color().red() / 255.0, 
                                     brush.color().green() / 255.0, 
                                     brush.color().blue() / 255.0, 
                                     brush.color().alpha() / 255.0);
            break;
            
        case Pt::Gfx::Brush::Texture:
            break;
    }
}


void PixmapSurfaceImpl::setFont(const Gfx::Font& font)
{
    CFStringRef fname = CFStringCreateWithCString(0, font.name().c_str(), kCFStringEncodingUTF8);
    CGFontRef fontRef = CGFontCreateWithFontName(fname);
    CGContextSetFont(_context, fontRef);
    CGContextSetFontSize(_context, font.size());
    CFRelease(fname);
    CGFontRelease(fontRef);
}


Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Pt::String& text) const
{
    return Gfx::FontMetrics(10, 10, 10, 10);
}

} // namespace

} // namespace
