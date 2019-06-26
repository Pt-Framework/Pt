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

#include <Pt/Hmi/Picture.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Gfx/Argb32Format.h>
#include <Pt/Utf8Codec.h>

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
    std::clog << "pixmap: " << _size.width() << "x" 
                            << _size.height() << std::endl;

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
}


void PixmapSurfaceImpl::setClip(const Gfx::RectF& clipRect)
{
    CGRect cgRect = CGRectMake(clipRect.x(), 
                               _size.height() - clipRect.y() - clipRect.height(), 
                               clipRect.width(), 
                               clipRect.height());
    
    CGContextResetClip(_context);
    CGContextClipToRect(_context, cgRect);
}


void PixmapSurfaceImpl::setCompositionMode(const Gfx::CompositionMode& mode)
{
}


void PixmapSurfaceImpl::setPen(const Gfx::Pen& pen)
{
    CGContextSetRGBStrokeColor(_context, 
                               pen.color().red() / 65535.0,
                               pen.color().green() / 65535.0,
                               pen.color().blue() / 65535.0,
                               pen.color().alpha() / 65535.0);
    
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
                                     brush.color().red() / 65535.0, 
                                     brush.color().green() / 65535.0, 
                                     brush.color().blue() / 65535.0, 
                                     brush.color().alpha() / 65535.0);
            break;
            
        case Pt::Gfx::Brush::Texture:
            // CGContextSetFillPattern
            break;
    }
}


void PixmapSurfaceImpl::setFont(const Gfx::Font& font)
{
    //CTFontCreateCopyWithSymbolicTraits
    CFStringRef fname = CFStringCreateWithCString(0, font.name().c_str(), kCFStringEncodingUTF8);
    CGFontRef fontRef = CGFontCreateWithFontName(fname);
    CGContextSetFont(_context, fontRef);
    CGContextSetFontSize(_context, font.size());
    CFRelease(fname);
    CGFontRelease(fontRef);
}


Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Pt::String& text) const
{
    CGFloat ascent = 10.0;
    CGFloat descent = 10.0;
    double width = 10.0;
    //width = CTLineGetTypographicBounds(line, &ascent, &descent, NULL);
    return Gfx::FontMetrics(static_cast<unsigned>(ascent), 
                            static_cast<unsigned>(descent), 
                            static_cast<unsigned>(width), 
                            static_cast<unsigned>(ascent + descent) );
}


void PixmapSurfaceImpl::drawLine(const Gfx::PointF& f, const Gfx::PointF& t)
{
    Gfx::PointF from = transform(f);
    Gfx::PointF to = transform(t);
    
    CGContextMoveToPoint(_context, from.x(), from.y());
    CGContextAddLineToPoint(_context, to.x(), to.y());
    CGContextStrokePath(_context);
}


void PixmapSurfaceImpl::drawText(const Gfx::PointF& to, const Pt::String& text)
{
  //CGContextSetTextDrawingMode(_context, kCGTextFill);

  // Core Text uses a reference coordinate system with the origin on the bottom-left
  // flip the coordinate system before drawing or the text will appear upside down
  //CGContextTranslateCTM(context, 0, self.bounds.size.height);
  //CGContextScaleCTM(context, 1.0, -1.0);

  CGAffineTransform matrix = CGAffineTransformMakeScale(1, -1);
  //CGAffineTransform matrix = &CGAffineTransformIdentity;

  // iOS
  //CFStringRef fontName = CFSTR("Courier");
  //CGFloat fontSize = 10;

  CFStringRef fontName = CFSTR("Menlo");
  CGFloat fontSize = 11;
  CTFontRef font = CTFontCreateWithName(fontName, fontSize, &matrix);
  CGColorRef textColor = CGColorCreateGenericRGB(0.0, 0.0, 0.0, 1.0);

  CFTypeRef keys[] = { kCTFontAttributeName, kCTForegroundColorAttributeName };
  CFTypeRef values[] = { font, textColor };
  CFDictionaryRef attributes = CFDictionaryCreate(kCFAllocatorDefault, 
                                                  keys, values, 2, 
                                                  &kCFTypeDictionaryKeyCallBacks, 
                                                  &kCFTypeDictionaryValueCallBacks);

  std::string cstr = Utf8Codec::encode(text);

  CFStringRef string = CFStringCreateWithBytesNoCopy(kCFAllocatorDefault, 
                                                     reinterpret_cast<const UInt8*>( cstr.data() ), 
                                                     cstr.length(), 
                                                     kCFStringEncodingUTF8, 
                                                     false, kCFAllocatorNull);

  // CFAttributedStringCreateMutable
  // CFAttributedStringReplaceString
  // CFAttributedStringSetAttribute
  CFAttributedStringRef attributedString = CFAttributedStringCreate(kCFAllocatorDefault, 
                                                                    string, attributes);
  CFRelease(string);
  CFRelease(attributes);
  CFRelease(textColor);
  CFRelease(font);

  // standard view coordinates
  //CGContextSetTextMatrix(context, CGAffineTransformIdentity);  
  // flipped coordinates
  //CGContextSetTextMatrix(context, CGAffineTransformMakeScale(1.0, -1.0)); 

  CTLineRef line = CTLineCreateWithAttributedString(attributedString);
  CFRelease(attributedString);
  //CGPoint textPosition = CGContextGetTextPosition(_context);
  
  CGContextSetTextPosition(_context, to.x(), to.y());
  CTLineDraw(line, _context);
  CFRelease(line);

  //CGContextSetTextPosition(_context, textPosition.x, textPosition.y);
  //CGContextRestoreGState(_context);

  // ALTERNATIVE: CTRunDraw


  //CTFontRef font = CTFontCreateWithName((__bridge CFStringRef)_fontName, _fontSize, 
  //                                      &CGAffineTransformIdentity);
  //CFStringRef keys[] = { kCTFontAttributeName, kCTForegroundColorAttributeName };
  //CFTypeRef values[] = { font, _fillColor.CGColor };
  
  //CFDictionaryRef attributes = CFDictionaryCreate(kCFAllocatorDefault, 
  //                                                (const void**)&keys, (const void**)&values, 2, 
  //                                                &kCFTypeDictionaryKeyCallBacks, 
  //                                                &kCFTypeDictionaryValueCallBacks);
  //CFAttributedStringRef attrString = CFAttributedStringCreate(kCFAllocatorDefault, 
  //                                                            (__bridge CFStringRef)text, attributes);
  //CFRelease(attributes);
  //CFRelease(font);
  //
  //CTLineRef line = CTLineCreateWithAttributedString(attrString);
  //CFRelease(attrString);
  //CTLineDraw(line, context);

  //float textWidth = CTLineGetTypographicBounds(line, NULL, NULL, NULL);
  //CGContextSetTextPosition(context, prevPosition.x + textWidth, prevPosition.y);
}


void PixmapSurfaceImpl::drawText(const Gfx::PointF& to, const Pt::String& text, 
                                 const Gfx::Transform& trans)
{
    drawText(to, text);
}


void PixmapSurfaceImpl::drawRect(const Gfx::RectF& rect)
{
    CGRect cgRect = CGRectMake(rect.x(), 
                               _size.height() - rect.y() - rect.height(), 
                               rect.width(), 
                               rect.height());
    
    CGContextStrokeRect(_context, cgRect);
}


void PixmapSurfaceImpl::fillRect(const Gfx::RectF& rect)
{
    CGRect cgRect = CGRectMake(rect.x(), 
                               _size.height() - rect.y() - rect.height(), 
                               rect.width(), 
                               rect.height());

    CGContextFillRect(_context, cgRect);

    //std::clog << cgRect.size.width << "x" << cgRect.size.height << std::endl;

    //CGContextSetRGBFillColor (_context, 1, 0, 1, 1);
    //CGContextFillRect (_context, CGRectMake(0, 0, 100, 100));
}


Pt::Gfx::PointF PixmapSurfaceImpl::transform(const Pt::Gfx::PointF& p)
{
    return Pt::Gfx::PointF( p.x(), _size.height() - p.y() );
}


void PixmapSurfaceImpl::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    //TODO
}


void PixmapSurfaceImpl::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    //TODO
}


void PixmapSurfaceImpl::drawPolyline(const Gfx::PointF* p, size_t pointCount)
{
    std::vector<Gfx::PointF> points(pointCount);
    
    for( size_t i = 0; i < pointCount; ++i)
        points[i] = transform(p[i]);
    
    CGContextMoveToPoint(_context, points[0].x(), points[0].y());
    
    for( size_t i = 1; i < pointCount; ++i)
        CGContextAddLineToPoint(_context, points[i].x(), points[i].y());
    
    CGContextStrokePath(_context);
}


void PixmapSurfaceImpl::fillPolygon(const Gfx::PointF* p, size_t pointCount)
{
    std::vector<Gfx::PointF> points(pointCount);
    
    for( size_t i = 0; i < pointCount; ++i)
        points[i] = transform(p[i]);
    
    CGContextMoveToPoint(_context, points[0].x(), points[0].y());
    
    for( size_t i = 1; i < pointCount; ++i)
        CGContextAddLineToPoint(_context, points[i].x(), points[i].y());
    
    CGContextFillPath(_context);
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& to, const PixmapSurface& pm)
{
    CGImageRef image =  CGBitmapContextCreateImage( pm.pixmapImpl()->context() );
    
    CGRect rect = CGRectMake(to.x(), 
                             _size.height() - to.y() - pm.size().height(), 
                             pm.size().width(), 
                             pm.size().height());
    
    CGContextDrawImage(_context, rect, image);

    CGImageRelease(image);
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& to, 
                                    const PixmapSurface& pm,
                                    const Gfx::RectF& pmRect)
{
    CGImageRef image =  CGBitmapContextCreateImage( pm.pixmapImpl()->context() );

    CGRect subRect = CGRectMake(pmRect.left(), 
                                pmRect.top(), 
                                pmRect.size().width(), 
                                pmRect.size().height());

    CGImageRef subImage = CGImageCreateWithImageInRect(image, subRect);

    CGRect rect = CGRectMake(to.x(), 
                             _size.height() - to.y() - pmRect.size().height(), 
                             pmRect.size().width(), 
                             pmRect.size().height());
    
    CGContextDrawImage(_context, rect, subImage);
    CGImageRelease(image);
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& to, const Gfx::Image& image)
{
    //TODO
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& to, 
                const Gfx::Image& image, 
                const Gfx::RectF& imgRect)
{
    //TODO
}


void PixmapSurfaceImpl::drawPicture(const Gfx::PointF& to, const Picture& pic)
{
    //TODO
}

} // namespace

} // namespace
