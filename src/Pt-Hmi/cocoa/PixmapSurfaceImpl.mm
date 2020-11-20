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

#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Gfx/Argb32Format.h>
#include <Pt/Utf8Codec.h>

namespace Pt {

namespace Hmi {

PixmapSurfaceImpl::PixmapSurfaceImpl()
: _size(10, 10)
, _painter(0)
, _context(0)
, _clipRect(CGRectNull)
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
    
    _context = CGBitmapContextCreate(0, 
                                     _size.width(), _size.height(), 
                                     8, 0, colorSpace, 
                                     kCGImageAlphaPremultipliedLast);

    CGColorSpaceRelease(colorSpace);

    //std::clog << "PIXMAP: " << _context << " " << _size.width() << "x" 
    //                        << _size.height() << std::endl;
}


void PixmapSurfaceImpl::destroy()
{
    if(_context)
        CGContextRelease(_context);
    
    _context = 0;
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


void PixmapSurfaceImpl::begin(Gfx::Painter& painter)
{
    _painter = &painter;

    Gfx::PaintData* pd = painter.paintData();
    _paintData = dynamic_cast<PaintData*>(pd);

    if (_paintData == 0)
    {
        delete pd;

        _paintData = new PaintData();
        painter.setPaintData(_paintData);
    }
}


void PixmapSurfaceImpl::finish()
{
    _paintData = 0;
    _painter = 0;
}


const Gfx::ImageFormat& PixmapSurfaceImpl::format() const
{
    return Gfx::ImageFormat::argb32();
}


void PixmapSurfaceImpl::setClip(const Gfx::RectF& clipRect)
{
    _clipRect = CGRectMake( clipRect.x(), 
                            _size.height() - clipRect.y() - clipRect.height(), 
                            clipRect.width(), 
                            clipRect.height() );
}


void PixmapSurfaceImpl::resetClip()
{
    _clipRect = CGRectNull;
}


void PixmapSurfaceImpl::beginClip()
{
    //return;
    //CGContextResetClip(_context);

    CGContextSaveGState(_context);

    if( ! CGRectIsNull(_clipRect) )
    {
        CGContextClipToRect(_context, _clipRect);

        //CGContextBeginPath(_context);
        //CGContextAddRect(_context, _clipRect);
        //CGContextClip(_context);
    }

    //CGRect boundingRect = CGContextGetClipBoundingBox(_context);

    //CGContextSetRGBFillColor (_context, 1, 0, 1, 1);
    //CGContextFillRect (_context, rect);
}


void PixmapSurfaceImpl::endClip()
{
    //return;
    CGContextRestoreGState(_context);
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
    
    CGContextSetLineWidth( _context, pen.size() );
    
    switch( pen.capStyle() )
    {
        default:
        case Pt::Gfx::Pen::FlatCap:
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
            CGContextSetLineDash(_context, 0, NULL, 0);
            break;
        }
            
        case Pt::Gfx::Pen::Dash:
        {
            if(pen.capStyle() == Gfx::Pen::RoundCap ||
               pen.capStyle() == Gfx::Pen::SquareCap)
            {
                double dashes[] = { 2.0 * pen.size(), 2.0 * pen.size() };
                CGContextSetLineDash(_context, 0, dashes, 2);
            }
            else
            {
                double dashes[] = { 3.0 * pen.size(), 1.0 * pen.size() };
                CGContextSetLineDash(_context, 0, dashes, 2);
            }
            break;
        }
 
        case Pt::Gfx::Pen::Dot:
        {
            if(pen.capStyle() == Gfx::Pen::RoundCap ||
               pen.capStyle() == Gfx::Pen::SquareCap)
            {
                double dashes[]  = {1.0, 2.0 * pen.size()};
                CGContextSetLineDash(_context, 0, dashes, 2);
            }
            else
            {
                double dashes[]  = {1.0 * pen.size(), 1.0 * pen.size()};
                CGContextSetLineDash(_context, 0, dashes, 2);
            }
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
    _paintData->setFont(font);
}


Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Pt::String& text) const
{
    if( ! _paintData )
        return Gfx::FontMetrics();

    CTFontRef font = _paintData->ctFont();
    return PaintData::fontMetrics(font, text);
}


void PixmapSurfaceImpl::drawText(const Gfx::PointF& to, 
                                 const Pt::String& text,
                                 const Gfx::Transform& trans)
{
    if( ! _paintData || ! _painter )
        return;

    CTFontRef font = _paintData->ctFont();
    const Gfx::Pen& pen = _painter->pen();

    CGColorRef textColor = CGColorCreateGenericRGB(pen.color().red() / 65535.0,
                                                   pen.color().green() / 65535.0,
                                                   pen.color().blue() / 65535.0,
                                                   pen.color().alpha() / 65535.0);

    CFTypeRef keys[] = { kCTFontAttributeName, kCTForegroundColorAttributeName };
    CFTypeRef values[] = { font, textColor };
    CFDictionaryRef attributes = CFDictionaryCreate(kCFAllocatorDefault, 
                                                    keys, values, 2, 
                                                    &kCFTypeDictionaryKeyCallBacks, 
                                                    &kCFTypeDictionaryValueCallBacks);

    std::string utf8String = Utf8Codec::encode(text);
    const UInt8* stringData = reinterpret_cast<const UInt8*>( utf8String.c_str() );
    CFStringRef string = CFStringCreateWithBytesNoCopy(kCFAllocatorDefault, 
                                                       stringData, 
                                                       utf8String.length(), 
                                                       kCFStringEncodingUTF8, 
                                                       false, 
                                                       kCFAllocatorNull);

    // OPTIMIZE: build attributed string on setFont and replace string in drawText
    // CFAttributedStringCreateMutable
    // CFAttributedStringReplaceString
    // CFAttributedStringSetAttribute
    CFAttributedStringRef attributedString = CFAttributedStringCreate(kCFAllocatorDefault, 
                                                                      string, attributes);

    CTLineRef line = CTLineCreateWithAttributedString(attributedString);

    // either flip the coordinate system for iOS, or...
    //CGContextTranslateCTM(_context, 0, _size.height());
    //CGContextScaleCTM(_context, 1.0, -1.0);

    // flip the text coordinate system for iOS
    //CGContextSetTextMatrix(_context, CGAffineTransformMakeScale(1.0, -1.0));
    
    beginClip();

    Gfx::Transform tt = trans;    
    tt.translate(to.x(), _size.height() - to.y());   

    CGAffineTransform tf;
    tf.a = tt.m11();
    tf.b = tt.m21();
    tf.c = tt.m12();
    tf.d = tt.m22();
    tf.tx = tt.dx();
    tf.ty = tt.dy();

    CGContextConcatCTM(_context, tf);
    CGContextSetTextPosition(_context, 0, 0);
    CTLineDraw(line, _context);
    
    endClip();

    CFRelease(line);
    CFRelease(attributedString);
    CFRelease(string);
    CFRelease(attributes);
    CFRelease(textColor);

    // ALTERNATIVE: CTRunDraw
}


void PixmapSurfaceImpl::drawLine(const Gfx::PointF& f, const Gfx::PointF& t)
{
    Gfx::PointF from = transform(f);
    Gfx::PointF to = transform(t);
    
    //std::clog << "drawLine: " << from.x() << ", " << from.y()
    //          << " -> " << to.x() << ", " << to.y() << std::endl;

    beginClip();
    CGContextMoveToPoint(_context, from.x(), from.y());
    CGContextAddLineToPoint(_context, to.x(), to.y());
    CGContextStrokePath(_context);
    endClip();
}


void PixmapSurfaceImpl::drawRect(const Gfx::RectF& rect)
{
    CGRect cgRect = CGRectMake(rect.x(), 
                               _size.height() - rect.y() - rect.height(), 
                               rect.width(), 
                               rect.height());
    
    beginClip();
    CGContextStrokeRect(_context, cgRect);
    endClip();
}


void PixmapSurfaceImpl::fillRect(const Gfx::RectF& rect)
{
    CGRect cgRect = CGRectMake(rect.x(), 
                               _size.height() - rect.y() - rect.height(), 
                               rect.width(), 
                               rect.height());

    beginClip();
    CGContextFillRect(_context, cgRect);
    endClip();

    //CGContextSetRGBFillColor (_context, 1, 0, 1, 1);
    //CGContextFillRect (_context, CGRectMake(0, 0, 100, 100));
}


Pt::Gfx::PointF PixmapSurfaceImpl::transform(const Pt::Gfx::PointF& p)
{
    return Pt::Gfx::PointF( p.x(), _size.height() - p.y() );
}


void PixmapSurfaceImpl::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    CGRect rect = CGRectMake(topLeft.x(), 
                             _size.height() - topLeft.y() - size.height(), 
                             size.width(), 
                             size.height());
    
    beginClip();
    CGContextAddEllipseInRect(_context, rect);
    CGContextStrokePath(_context);
    endClip();
}


void PixmapSurfaceImpl::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    CGRect rect = CGRectMake(topLeft.x(), 
                             _size.height() - topLeft.y() - size.height(), 
                             size.width(), 
                             size.height());
    
    beginClip();
    CGContextAddEllipseInRect(_context, rect);
    CGContextFillPath(_context);
    endClip();

    // double y = _size.height() - topLeft.y() - size.height();

    // double scaleX = size.width() / size.height();
    // double centerX = (topLeft.x() + size.width() / 2.0) / scaleX;
    // double centerY = y + size.height() / 2;
    // double radius = size.height() / 2.0;

    // CGMutablePathRef path = CGPathCreateMutable();
    // CGAffineTransform transform = CGAffineTransformMakeScale(scaleX, 1);
    
    // CGPathAddArc(path, &transform,
    //              centerX, centerY, radius, 
    //              0, 2 * 3.1415927, false);
   
    // CGContextBeginPath(_context);
    // CGContextAddPath(_context, path);
    // CGContextFillPath(_context);
    // CGPathRelease(path);
}


void PixmapSurfaceImpl::drawPolyline(const Gfx::PointF* p, size_t pointCount)
{
    std::vector<Gfx::PointF> points(pointCount);
    
    for( size_t i = 0; i < pointCount; ++i)
        points[i] = transform(p[i]);
    
    beginClip();
    CGContextMoveToPoint(_context, points[0].x(), points[0].y());
    
    for( size_t i = 1; i < pointCount; ++i)
        CGContextAddLineToPoint(_context, points[i].x(), points[i].y());
    
    CGContextStrokePath(_context);
    endClip();
}


void PixmapSurfaceImpl::fillPolygon(const Gfx::PointF* p, size_t pointCount)
{
    std::vector<Gfx::PointF> points(pointCount);
    
    for( size_t i = 0; i < pointCount; ++i)
        points[i] = transform(p[i]);
    
    beginClip();
    CGContextMoveToPoint(_context, points[0].x(), points[0].y());
    
    for( size_t i = 1; i < pointCount; ++i)
        CGContextAddLineToPoint(_context, points[i].x(), points[i].y());
    
    CGContextFillPath(_context);
    endClip();
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& to, const PixmapSurface& pm)
{
    //std::clog << "drawSurface: " <<  pm.pixmapImpl()->context()
    //          << " to: " << _context << std::endl;
    
    CGImageRef image =  CGBitmapContextCreateImage( pm.pixmapImpl()->context() );
    
    CGRect rect = CGRectMake(to.x(), 
                             _size.height() - to.y() - pm.size().height(), 
                             pm.size().width(), 
                             pm.size().height());
    
    //CGContextSetRGBFillColor (_context, 1, 0, 1, 1);
    //CGContextFillRect(_context, rect);
    //CGContextFillRect (_context, CGRectMake(0, 0, 100, 100));

    beginClip();
    CGContextDrawImage(_context, rect, image);
    endClip();

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
    
    beginClip();
    CGContextDrawImage(_context, rect, subImage);
    endClip();

    CGImageRelease(image);
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& to, const Gfx::Image& image)
{
    const Pt::uint8_t* data = image.data();
    std::size_t dataSize = image.format().imageSize( image.size(), image.padding() );

    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, 
                                                              data, 
                                                              dataSize, 
                                                              NULL);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrder32Little|kCGImageAlphaFirst;
    
    CGImageRef imageRef = CGImageCreate(image.width(), image.height(), 
                                        8, 32, 4 * image.width(), 
                                        colorSpace, bitmapInfo, provider, 
                                        NULL, false, kCGRenderingIntentDefault);

    CGRect contextRect = CGRectMake( to.x(), 
                                     _size.height() - to.y() - image.height(), 
                                     image.width(), 
                                     image.height() );

    beginClip();
    CGContextDrawImage(_context, contextRect, imageRef);
    endClip();
    
    CFRelease(imageRef);
    CFRelease(colorSpace);
    CFRelease(provider);
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& to, 
                                  const Gfx::Image& image, 
                                  const Gfx::RectF& imgRect)
{
    //TODO
}


Gfx::Image PixmapSurfaceImpl::toImage(const Gfx::ImageFormat& iformat) const
{
  return Gfx::Image();
}


void PixmapSurfaceImpl::set(const Gfx::Image& image)
{
    resize( Gfx::SizeF( image.size().width(), 
                        image.size().height() ) );

    Gfx::PointF origin(0, 0);
    drawImage(origin, image);
}


std::string PixmapSurfaceImpl::defaultFont()
{
    return getDefaultFont();
}


void PixmapSurfaceImpl::setDefaultFont(const std::string& f)
{
    getDefaultFont() = f;
}


std::string& PixmapSurfaceImpl::getDefaultFont()
{ 
    #if PT_IOS
        //"Helvetica"
        //"Times New Roman"
        //"Courier New"
        static std::string _defaultFont = "Helvetica";
    #else
        //"Lucida Grande"
        //"Times New Roman"
        //"Monaco"
        static std::string _defaultFont = "Helvetica";
    #endif
            
    return _defaultFont; 
}


std::vector<std::string> PixmapSurfaceImpl::fontNames()
{
    std::vector<std::string> fonts;

#if PT_IOS
    NSArray* fonts = [UIFont familyNames];
#else
    NSArray* families = [[NSFontManager sharedFontManager] availableFontFamilies];
#endif

    for (unsigned int i = 0; i < [families count]; ++i)
    {
        NSString* font = (NSString*)[families objectAtIndex: i];
        fonts.push_back( [font UTF8String] );
    }
    
    return fonts;
}


void PixmapSurfaceImpl::setFontDir(const System::Path& path)
{
}


Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Gfx::Font& font, const Pt::String& text)
{
    return PaintData::fontMetrics(font, text);
}

} // namespace

} // namespace
