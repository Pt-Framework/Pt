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

#include "PixmapImpl.h"
#include "PaintContext.h"

#include <Pt/Forms/Pixmap.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Utf8Codec.h>

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////
// PixmapCanvas
///////////////////////////////////////////////////////////////////////

PixmapCanvas::PixmapCanvas(Gfx::PaintSurface& surface)
: Gfx::Canvas(surface)
, _physicalSize(0, 0)
, _width(0)
, _height(0)
, _colorSpace(0)
, _context(0)
, _image(0)
, _imageModified(false)
, _paintContext(0)
, _compositionMode(Gfx::CompositionMode::SourceCopy)
, _clipRect(CGRectNull)
{
    create();
}


PixmapCanvas::~PixmapCanvas()
{
    destroy();
}


void PixmapCanvas::create()
{
    _colorSpace = CGColorSpaceCreateDeviceRGB();
    
    _context = CGBitmapContextCreate(0, _width, _height,
                                     8, 0, _colorSpace, 
                                     kCGImageAlphaPremultipliedLast);

    //CGContextSetShouldAntialias(_context, false);
    //CGContextSetBlendMode(_context, kCGBlendModeNormal);
    //CGContextSetBlendMode(_context, kCGBlendModeCopy)

    _imageModified = true;
    
    //std::clog << "PIXMAP: " << this << " " << _width << "x" 
    //                        << _height << std::endl;
}


void PixmapCanvas::destroy()
{
    if(_image)
        CGImageRelease(_image);

    if(_context)
        CGContextRelease(_context);
    
    if(_colorSpace)
        CGColorSpaceRelease(_colorSpace);

    _image = 0;
    _context = 0;
    _colorSpace = 0;
}


CGImageRef PixmapCanvas::getCGImage() const
{
    //return CGBitmapContextCreateImage(_context);
    if(_imageModified)
    {
        if(_image)
            CGImageRelease(_image);

        _image = CGBitmapContextCreateImage(_context);
        _imageModified = false;
    }

    return _image;
}


Gfx::Image PixmapCanvas::toImage() const
{
  return Gfx::Image();
}


void PixmapCanvas::set(const Gfx::Image& image)
{
    size_t width = image.width();
    size_t height = image.height();

    Gfx::SizeF size(width, height);

    resize(size);

    Gfx::PointF origin(0, 0);
    drawImage(origin, image);
}


const Gfx::SizeF& PixmapCanvas::physicalSize() const
{
    return _physicalSize;
}


const Gfx::SizeF& PixmapCanvas::logicalSize() const
{
    return _logicalSize;
}


void PixmapCanvas::resize(const Pt::Gfx::SizeF& size)
{
    size_t width = lround( size.width() );
    size_t height = lround( size.height() );
    
    if(width == 0)
        width = 10;
    
    if(height ==  0)
        height = 10;
    
    _width = width;
    _height = height;
    
    _physicalSize.set(width, height);
    _logicalSize = scaling().toLogical(_physicalSize);

    destroy();
    create();
}


void PixmapCanvas::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);
    _logicalSize = _scaling.toLogical(_physicalSize);
}


const Gfx::ImageFormat& PixmapCanvas::onGetFormat() const
{
    return Gfx::ImageFormat::argb32();
}


const Gfx::SizeF& PixmapCanvas::onGetSize() const
{
    return _logicalSize;
}


const Gfx::Scaling& PixmapCanvas::onGetScaling() const
{
    return _scaling;
}


Gfx::PaintContext* PixmapCanvas::onCreateContext(Gfx::PaintContext* context)
{
    PaintContext* paintContext = dynamic_cast<PaintContext*>(context);
    if( ! paintContext )
        paintContext  = new PaintContext();;
    
    paintContext->setPixmap(*this);

    _paintContext = paintContext;
    return _paintContext;
}


//Gfx::PaintContext* PixmapCanvas::onCreatePaint()
//{
//    PaintContext* paintContext  = new PaintContext();
//    paintContext->setPixmap(*this);
//
//    _paintContext = paintContext;
//    return paintContext;
//}


void PixmapCanvas::onReleasePaint()
{
    _paintContext = 0;
}


void PixmapCanvas::onCompositionModeChanged()
{
    if( ! _paintContext )
        return;

    _compositionMode = _paintContext->compositionMode();
}


void PixmapCanvas::onPenChanged()
{
    if( ! _paintContext )
        return;

    const Gfx::Pen& pen = _paintContext->pen();

    CGContextSetRGBStrokeColor(_context, 
                               pen.color().red() / 65535.0,
                               pen.color().green() / 65535.0,
                               pen.color().blue() / 65535.0,
                               pen.color().alpha() / 65535.0);
    
    double scaleFactor = scaling().scaleFactor();
    CGContextSetLineWidth( _context, pen.size() * scaleFactor );
    
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


void PixmapCanvas::onBrushChanged()
{
    if( ! _paintContext )
        return;

    const Gfx::Brush& brush = _paintContext->brush();

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


void PixmapCanvas::onFontChanged()
{
    if( ! _paintContext )
        return;

    //_paintContext->font();
}


void PixmapCanvas::onClipChanged()
{
    if( ! _paintContext )
        return;
    
    const Gfx::RectF* clipRect = _paintContext->clipRect();
    if( ! clipRect )
    {
        _clipRect = CGRectNull;
    }
    else 
    {
        Gfx::RectF rect = scaling().toPhysical(*clipRect);

        _clipRect = CGRectMake( rect.x(), 
                                _height - rect.y() - rect.height(), 
                                rect.width(), 
                                rect.height() );
    }
}


void PixmapCanvas::beginClip()
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


void PixmapCanvas::endClip()
{
    //return;
    CGContextRestoreGState(_context);
}


Pt::Gfx::PointF PixmapCanvas::transform(const Pt::Gfx::PointF& p)
{
    return Pt::Gfx::PointF( p.x(), _height - p.y() );
}


void PixmapCanvas::onDrawLine(const Gfx::PointF& p0, const Gfx::PointF& p1)
{
    Gfx::PointF from = transform(p0);
    Gfx::PointF to = transform(p1);
    
    //std::clog << "drawLine: " << from.x() << ", " << from.y()
    //          << " -> " << to.x() << ", " << to.y() << std::endl;

    beginClip();
    CGContextMoveToPoint(_context, from.x(), from.y());
    CGContextAddLineToPoint(_context, to.x(), to.y());
    CGContextStrokePath(_context);
    endClip();

    _imageModified = true;
}


void PixmapCanvas::onDrawPolyline(const Gfx::Polyline& line)
{
    std::size_t n = line.size();
    if (n < 2)
        return;
    
    beginClip();

    Gfx::PointF first = line.at(0);
    first = transform(first);
    //std::clog << "DRAW POLY: " << first.x() << "," << first.y() << "  ";

    CGContextMoveToPoint(_context, first.x(), first.y());

    for(unsigned i = 1; i < n; i++)
    {
        Gfx::PointF p = line.at(i);
        p = transform(p);
        //std::clog << p.x() << "," << p.y() << "  ";

        CGContextAddLineToPoint(_context, p.x(), p.y());
    }

    //std::clog << std::endl;
    CGContextStrokePath(_context);

    endClip();

    _imageModified = true;
}


void PixmapCanvas::onFillPolygon(const Gfx::Polyline& line)
{
    std::size_t n = line.size();
    if (n < 2)
        return;
    
    beginClip();

    Gfx::PointF first = line.at(0);
    first = transform(first);
    //std::clog << "FILL POLY: " << first.x() << "," << first.y() << "  ";

    CGContextMoveToPoint(_context, first.x(), first.y());

    for(unsigned i = 1; i < n; i++)
    {
        Gfx::PointF p = line.at(i);
        p = transform(p);
        //std::clog << p.x() << "," << p.y() << "  ";

        CGContextAddLineToPoint(_context, p.x(), p.y());
    }
    
    //std::clog << std::endl;
    CGContextFillPath(_context);

    endClip();

    _imageModified = true;
}


void PixmapCanvas::onDrawRect(const Gfx::RectF& rect)
{
    CGFloat y = _height - rect.y() - rect.height();
    CGRect cgRect = CGRectMake( rect.x(), y, rect.width(), rect.height() );
    
    beginClip();
    CGContextStrokeRect(_context, cgRect);
    endClip();

    _imageModified = true;
}


void PixmapCanvas::onFillRect(const Gfx::RectF& rect)
{
    //std::clog << "CANVAS FILL RECT: " << rect.x() << "," << rect.y() << " " 
    //          << rect.width() << "x" << rect.height() << std::endl;

    CGFloat y = _height - rect.y() - rect.height();
    CGRect cgRect = CGRectMake( rect.x(), y, rect.width(), rect.height() );

    beginClip();
    CGContextFillRect(_context, cgRect);
    endClip();

    //CGContextSetRGBFillColor (_context, 1, 0, 1, 1);
    //CGContextFillRect (_context, CGRectMake(0, 0, 100, 100));

    _imageModified = true;
}


void PixmapCanvas::onDrawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    CGFloat y = _height - topLeft.y() - size.height();
    CGRect rect = CGRectMake( topLeft.x(), y, size.width(), size.height() );
    
    beginClip();
    CGContextAddEllipseInRect(_context, rect);
    CGContextStrokePath(_context);
    endClip();

    _imageModified = true;
}


void PixmapCanvas::onFillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    CGFloat y = _height - topLeft.y() - size.height();
    CGRect rect = CGRectMake( topLeft.x(), y, size.width(), size.height() );
    
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

    _imageModified = true;
}


void PixmapCanvas::drawPath(CGMutablePathRef path)
{
    beginClip();

    CGAffineTransform axisTransform = CGAffineTransformMakeTranslation(0.0, _height);
    axisTransform = CGAffineTransformScale(axisTransform, 1.0, -1.0);
    CGContextConcatCTM(_context, axisTransform);

    const Gfx::Transform& tx = _paintContext->transform();
    CGAffineTransform transform = CGAffineTransformMake( tx.m11(), tx.m12(),
                                                         tx.m21(), tx.m22(),
                                                         tx.dx(),  tx.dy() );
    CGContextConcatCTM(_context, transform);

    CGContextAddPath(_context, path);
    CGContextStrokePath(_context);

    endClip();
}


void PixmapCanvas::fillPath(CGMutablePathRef path)
{
    beginClip();

    CGAffineTransform axisTransform = CGAffineTransformMakeTranslation(0.0, _height);
    axisTransform = CGAffineTransformScale(axisTransform, 1.0, -1.0);
    CGContextConcatCTM(_context, axisTransform);

    const Gfx::Transform& tx = _paintContext->transform();
    CGAffineTransform transform = CGAffineTransformMake( tx.m11(), tx.m12(),
                                                         tx.m21(), tx.m22(),
                                                         tx.dx(),  tx.dy() );
    CGContextConcatCTM(_context, transform);

    CGContextAddPath(_context, path);
    CGContextFillPath(_context);

    endClip();
}


Gfx::TextMetrics PixmapCanvas::onGetTextMetrics(const Pt::String& text) const
{
    if( ! _paintContext )
        return Gfx::TextMetrics();

    CTFontRef font = _paintContext->font();
    CFDictionaryRef fontAttributes = _paintContext->fontAttributes();

    CFStringRef string = CFStringCreateWithBytesNoCopy(0, (const UInt8*) text.c_str(), 
                                                       text.length() * 4, 
                                                       kCFStringEncodingUTF32LE, 
                                                       false, kCFAllocatorNull);
    if( ! string )
        return Gfx::TextMetrics();

    CFMutableAttributedStringRef attributedString = 
        CFAttributedStringCreateMutable(kCFAllocatorDefault, 0);

    CFIndex length = CFAttributedStringGetLength(attributedString);
    CFAttributedStringReplaceString(attributedString, CFRangeMake(0, length), string);
    CFRelease(string);

    length = CFAttributedStringGetLength(attributedString);
    CFAttributedStringSetAttributes(attributedString, CFRangeMake(0, length), 
                                    fontAttributes, false);

    CTLineRef line = CTLineCreateWithAttributedString(attributedString);
    CFRelease(attributedString);

    double width = CTLineGetTypographicBounds(line, NULL, NULL, NULL);
    CGFloat ascent = CTFontGetAscent(font);
    CGFloat descent = CTFontGetDescent(font);
    CGFloat leading = CTFontGetLeading(font);
    CGFloat capHeight = CTFontGetCapHeight(font);
    CFRelease(line);

    Gfx::TextMetrics fm;
    fm.setAscent(ascent);
    fm.setDescent(descent);
    fm.setCapHeight(capHeight);
    fm.setLeading(leading);
    fm.setWidth(width);
    return fm;
}


void PixmapCanvas::onDrawText(const Gfx::PointF& to, 
                              const Pt::String& text, 
                              const Gfx::Transform* transform)
{
    if( ! _paintContext )
        return;

    //std::clog << "DRAW TEXT: " << text.narrow() << std::endl;

    CFMutableDictionaryRef fontAttributes = _paintContext->fontAttributes();

    CFStringRef string = CFStringCreateWithBytesNoCopy(NULL, (const UInt8*) text.c_str(), 
                                                       text.length() * 4, 
                                                       kCFStringEncodingUTF32LE, 
                                                       false, kCFAllocatorNull);
    if( ! string )
        return;

    CFMutableAttributedStringRef attributedString = 
        CFAttributedStringCreateMutable(kCFAllocatorDefault, 0);

    CFIndex length = CFAttributedStringGetLength(attributedString);
    CFAttributedStringReplaceString(attributedString, CFRangeMake(0, length), string);
    CFRelease(string);
    
    length = CFAttributedStringGetLength(attributedString);
    CFAttributedStringSetAttributes(attributedString, CFRangeMake(0, length), 
                                    fontAttributes, false);

    beginClip();

    //CGContextSetTextMatrix( _context, CGAffineTransformMake( 1.0, 0.0, 0.0,
    //                                                        -1.0, 0.0, 0.0) );
    //CGContextTranslateCTM(_context, 0, _height);
    //CGContextScaleCTM(_context, 1.0, -1.0);

    Gfx::Transform xform = transform ? *transform : Gfx::Transform();
    xform.translate( to.x(), _height - to.y() );

    CGAffineTransform tf = CGAffineTransformMake( xform.m11(), xform.m21(), 
                                                  xform.m12(), xform.m22(),
                                                  xform.dx(),  xform.dy());
    CGContextConcatCTM(_context, tf);
    CGContextSetTextPosition(_context, 0, 0);

    CTLineRef line = CTLineCreateWithAttributedString(attributedString);
    CFRelease(attributedString);

    CTLineDraw(line, _context);
    CFRelease(line);

    endClip();

    // ALTERNATIVE: CTRunDraw

    _imageModified = true;
}

void PixmapCanvas::onDrawImage(const Gfx::PointF& toL, 
                               const Gfx::Image& image,
                               const Gfx::RectF* rect)
{
    if( image.empty() )
        return;

    Gfx::PointF to = _scaling.toPhysical(toL);

    //std::clog << "onDrawImage: " << this << " "
    //          << to.x() << "," << to.y() << " "
    //          << image.width() << "," << image.height() << std::endl;

    const Pt::uint8_t* data = image.data();
    std::size_t dataSize = image.format().imageSize( image.width(), 
                                                     image.height(), 
                                                     image.padding() );

    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, data, dataSize, NULL);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrder32Host|kCGImageAlphaFirst;
    
    CGImageRef imageRef = CGImageCreate(image.width(), image.height(), 
                                        8, 32, 4 * image.width(), 
                                        colorSpace, bitmapInfo, provider, 
                                        NULL, false, kCGRenderingIntentDefault);

    CGRect contextRect = CGRectMake( to.x(), 
                                     _height - to.y() - image.height(), 
                                     image.width(), 
                                     image.height() );

    beginClip();
    CGContextDrawImage(_context, contextRect, imageRef);
    endClip();
    
    CFRelease(imageRef);
    CFRelease(colorSpace);
    CFRelease(provider);

    _imageModified = true;
}


bool PixmapCanvas::onDrawLayer(const Gfx::PointF& to,
                               const Gfx::PaintLayer& layer,
                               const Gfx::RectF* rect)
{
    const Gfx::PaintSurface* layerSurface = layer.surface();
    const PixmapImpl* pixmap = dynamic_cast<const PixmapImpl*>(layerSurface);
    if(pixmap)
    {
        onDrawPixmap(to, *pixmap, rect);
        return true;
    }

    return false;
}


void PixmapCanvas::onDrawPixmap(const Gfx::PointF& toL, 
                                const PixmapImpl& pixmap,
                                const Gfx::RectF* rectL)
{
    Gfx::PointF to = _scaling.toPhysical(toL);
    CGImageRef image = pixmap.getCGImage();

    if(rectL)
    {
        Gfx::RectF rect = _scaling.toPhysical(*rectL);

        //std::clog << "onDrawPixmap: " << this << " "
        //          << to.x() << "," << to.y() << " "
        //          << rect.x() << "," << rect.y() << " "
        //          << rect.width() << "x" << rect.height() << std::endl;

        CGRect subRect = CGRectMake(rect.left(), 
                                    rect.top(), 
                                    rect.size().width(), 
                                    rect.size().height());

        CGImageRef subImage = CGImageCreateWithImageInRect(image, subRect);

        CGRect destRect = CGRectMake(to.x(), 
                                     _height - to.y() - rect.height(), 
                                     rect.width(), 
                                     rect.height());
        beginClip();
        CGContextDrawImage(_context, destRect, subImage);
        endClip();

        CGImageRelease(subImage);
    }
    else
    {
        //std::clog << "onDrawPixmap: " << this << " " << to.x() << "," << to.y() << " FULL" << std::endl;

        CGRect destRect = CGRectMake(to.x(), 
                                     _height - to.y() - pixmap.size().height(), 
                                     pixmap.size().width(), 
                                     pixmap.size().height());

        //std::clog << "dest: " << this << " " << destRect.origin.x << ", " << destRect.origin.y << std::endl;
        beginClip();
        CGContextDrawImage(_context, destRect, image);
        endClip();
    }

    _imageModified = true;
}

///////////////////////////////////////////////////////////////////////
// PixmapImpl
///////////////////////////////////////////////////////////////////////

PixmapImpl::PixmapImpl()
: _canvas(0)
{
    _canvas = new PixmapCanvas(*this);
    setCanvas(_canvas);
}


PixmapImpl::~PixmapImpl()
{
    delete _canvas;
}


CGImageRef PixmapImpl::getCGImage() const
{
    return _canvas->getCGImage();
}


void PixmapImpl::set(const Gfx::Image& image)
{
    _canvas->set(image);
}


Gfx::Image PixmapImpl::toImage() const
{
    return _canvas->toImage();
}


void PixmapImpl::clear(const Gfx::Color& c)
{
}


const Gfx::SizeF& PixmapImpl::size() const
{
    return _canvas->physicalSize();
}


void PixmapImpl::resize(const Gfx::SizeF& size)
{
    _canvas->resize(size);
}


void PixmapImpl::setScaleFactor(double scaleFactor)
{
    _canvas->setScaleFactor(scaleFactor);
}


void PixmapImpl::draw(Gfx::PaintSurface& surface,
                      const Gfx::Paint& paint,
                      const Gfx::PointF& to,
                      const Gfx::RectF* rect) const
{
    Gfx::Painter painter(surface);
    painter.setCompositionMode( paint.compositionMode() );
    
    Gfx::Image pixmapImage = toImage();
    if(rect)
    {
        Gfx::RectF imageRect = _canvas->scaling().toPhysical(*rect);
        painter.drawImage(to, pixmapImage, imageRect);
    }
    else
    {
        painter.drawImage(to, pixmapImage);
    }
}


CGContextRef PixmapImpl::context() const
{
    return _canvas->context();
}


const std::string& PixmapImpl::defaultFont()
{
    return getDefaultFont();
}


void PixmapImpl::setDefaultFont(const std::string& f)
{
    getDefaultFont() = f;
}


std::vector<std::string> PixmapImpl::fontNames()
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


void PixmapImpl::setFontDir(const System::Path& path)
{
}


std::string& PixmapImpl::getDefaultFont()
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

} // namespace

} // namespace
