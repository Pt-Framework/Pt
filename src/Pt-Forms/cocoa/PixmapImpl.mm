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

#include <Pt/Hmi/Pixmap.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Utf8Codec.h>

namespace Pt {

namespace Hmi {

///////////////////////////////////////////////////////////////////////
// PixmapCanvas
///////////////////////////////////////////////////////////////////////

PixmapCanvas::PixmapCanvas(Gfx::PaintSurface& surface)
: Gfx::Canvas(surface)
, _physicalSize(0, 0)
, _width(0)
, _height(0)
, _context(0)
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
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    _context = CGBitmapContextCreate(0, _width, _height,
                                     8, 0, colorSpace, 
                                     kCGImageAlphaPremultipliedLast);

    CGColorSpaceRelease(colorSpace);

    //std::clog << "PIXMAP: " << _context << " " << _width << "x" 
    //                        << _height << std::endl;
}


void PixmapCanvas::destroy()
{
    if(_context)
        CGContextRelease(_context);
    
    _context = 0;
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


bool PixmapCanvas::onSetPaint(Gfx::PaintContext* context)
{
    PaintContext* paintContext = dynamic_cast<PaintContext*>(context);
    if( ! paintContext )
        return false;
    
    _paintContext = paintContext;
    return true;
}


Gfx::PaintContext* PixmapCanvas::onCreatePaint()
{
    PaintContext* paintContext  = new PaintContext();

    _paintContext = paintContext;
    return paintContext;
}


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
        _clipRect = CGRectMake( clipRect->x(), 
                                _height - clipRect->y() - clipRect->height(), 
                                clipRect->width(), 
                                clipRect->height() );
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
}


void PixmapCanvas::onDrawPolyline(const Gfx::Polyline& line)
{
    std::size_t n = line.size();
    if (n < 2)
        return;
    
    beginClip();

    Gfx::PointF first = line.at(0);
    first = transform(first);
    CGContextMoveToPoint(_context, first.x(), first.y());

    for(unsigned i = 1; i < n; i++)
    {
        Gfx::PointF p = line.at(i);
        p = transform(p);
        CGContextAddLineToPoint(_context, p.x(), p.y());
    }

    CGContextStrokePath(_context);

    endClip();
}


void PixmapCanvas::onFillPolygon(const Gfx::Polyline& line)
{
    std::size_t n = line.size();
    if (n < 2)
        return;
    
    beginClip();

    Gfx::PointF first = line.at(0);
    first = transform(first);
    CGContextMoveToPoint(_context, first.x(), first.y());

    for(unsigned i = 1; i < n; i++)
    {
        Gfx::PointF p = line.at(i);
        p = transform(p);
        CGContextAddLineToPoint(_context, p.x(), p.y());
    }
    
    CGContextFillPath(_context);
    
    endClip();
}


void PixmapCanvas::onDrawRect(const Gfx::RectF& rect)
{
    CGFloat y = _height - rect.y() - rect.height();
    CGRect cgRect = CGRectMake( rect.x(), y, rect.width(), rect.height() );
    
    beginClip();
    CGContextStrokeRect(_context, cgRect);
    endClip();
}


void PixmapCanvas::onFillRect(const Gfx::RectF& rect)
{
    CGFloat y = _height - rect.y() - rect.height();
    CGRect cgRect = CGRectMake( rect.x(), y, rect.width(), rect.height() );

    beginClip();
    CGContextFillRect(_context, cgRect);
    endClip();

    //CGContextSetRGBFillColor (_context, 1, 0, 1, 1);
    //CGContextFillRect (_context, CGRectMake(0, 0, 100, 100));

}


void PixmapCanvas::onDrawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    CGFloat y = _height - topLeft.y() - size.height();
    CGRect rect = CGRectMake( topLeft.x(), y, size.width(), size.height() );
    
    beginClip();
    CGContextAddEllipseInRect(_context, rect);
    CGContextStrokePath(_context);
    endClip();
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
}


Gfx::FontMetrics PixmapCanvas::onGetFontMetrics(const Pt::String& text) const
{
    if( ! _paintContext )
        return Gfx::FontMetrics();

    CTFontRef font = _paintContext->font();
    return _paintContext->fontMetrics(font, text);
}


void PixmapCanvas::onDrawText(const Gfx::PointF& to, 
                                   const Pt::String& text, 
                                   const Gfx::Transform* trans)
{
    if( ! _paintContext )
        return;

    CTFontRef font = _paintContext->font();
    const Gfx::Pen& pen = _paintContext->pen();

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

    if(trans)
    {
        Gfx::Transform tt = *trans;    
        tt.translate( to.x(), _height - to.y() );   

        CGAffineTransform tf;
        tf.a = tt.m11();
        tf.b = tt.m21();
        tf.c = tt.m12();
        tf.d = tt.m22();
        tf.tx = tt.dx();
        tf.ty = tt.dy();

        CGContextConcatCTM(_context, tf);
    }

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

void PixmapCanvas::onDrawImage(const Gfx::PointF& to, 
                               const Gfx::Image& image,
                               const Gfx::RectF* rect)
{
    if( image.empty() )
        return;

    const Pt::uint8_t* data = image.data();
    std::size_t dataSize = image.format().imageSize( image.width(), 
                                                     image.height(), 
                                                     image.padding() );

    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, data, dataSize, NULL);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrder32Little|kCGImageAlphaFirst;
    
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


void PixmapCanvas::onDrawPixmap(const Gfx::PointF& to, 
                                const PixmapImpl& pixmap,
                                const Gfx::RectF* rect)
{
    CGImageRef image = CGBitmapContextCreateImage( pixmap.context() );

    if(rect)
    {
        CGRect subRect = CGRectMake(rect->left(), 
                                    rect->top(), 
                                    rect->size().width(), 
                                    rect->size().height());

        CGImageRef subImage = CGImageCreateWithImageInRect(image, subRect);

        CGRect destRect = CGRectMake(to.x(), 
                                     _height - to.y() - rect->height(), 
                                     rect->width(), 
                                     rect->height());
        beginClip();
        CGContextDrawImage(_context, destRect, subImage);
        endClip();

        CGImageRelease(subImage);
    }
    else
    {
        CGRect destRect = CGRectMake(to.x(), 
                                     _height - to.y() - pixmap.size().height(), 
                                     pixmap.size().width(), 
                                     pixmap.size().height());
        beginClip();
        CGContextDrawImage(_context, destRect, image);
        endClip();
    }

    CGImageRelease(image);
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
