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
#include "CocoaFontProvider.h"

#include <Pt/Forms/Pixmap.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Utf8Codec.h>

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////
// PixmapCanvas
///////////////////////////////////////////////////////////////////////

PixmapCanvas::PixmapCanvas()
: Gfx::Canvas()
, _pixmap(0)
, _compositionMode(Gfx::CompositionMode::SourceCopy)
, _clipRect(CGRectNull)
, _penColor(0)
, _penSize(1)
, _penCap(kCGLineCapButt)
, _penJoin(kCGLineJoinRound)
, _brushColor(0)
, _brushStyle(Gfx::Brush::Solid)
, _font(0)
, _fontAttributes(0)
, _attributedString(0)
, _cgPath(0)
{
}


PixmapCanvas::~PixmapCanvas()
{
    if(_pixmap)
        _pixmap->releaseCanvas();

    if(_font)
      CFRelease(_font);

    if(_penColor)
      CFRelease(_penColor);

    if(_attributedString)
        CFRelease(_attributedString);

    if(_fontAttributes)
      CFRelease(_fontAttributes);

    if(_cgPath)
      CGPathRelease(_cgPath);
}


void PixmapCanvas::setPixmap(PixmapImpl& pixmap)
{
    _pixmap = &pixmap;
}


void PixmapCanvas::reset()
{
    invalidate(DirtyAll);
}


void PixmapCanvas::onBeginPaint(const Gfx::Paint& paint)
{
}


void PixmapCanvas::onFinishPaint()
{
    // NOTE: this might be called from the attached surface base class destructor

    if(_pixmap)
        _pixmap = 0;
}


void PixmapCanvas::onSetCompositionMode(const Gfx::CompositionMode& mode) 
{
    _compositionMode = mode;
}


void PixmapCanvas::onSetTransform(const Gfx::Transform& tx)
{
    if(_pixmap && isActive())
    {
        CGContextRef context = _pixmap->context();
        CGContextRestoreGState(context);
        CGContextSaveGState(context);
        invalidate(DirtyAll & ~DirtyTransform);
    }

    _transform = tx;
}


void PixmapCanvas::onApplyTransform()
{
    if( ! _pixmap)
        return;

    CGContextRef context = _pixmap->context();

    double H = CGBitmapContextGetHeight(context);
    CGAffineTransform tf = CGAffineTransformMake( _transform.m11(), _transform.m12(), 
                                                  -_transform.m21(), -_transform.m22(),
                                                  _transform.dx(), H - _transform.dy() );
    CGContextConcatCTM(context, tf);
}


void PixmapCanvas::onApplyCompositionMode() 
{
}


void PixmapCanvas::onSetPen(const Gfx::Pen& pen)
{
    CGColorRef color = CGColorCreateGenericRGB(pen.color().red() / 65535.0,
                                                   pen.color().green() / 65535.0,
                                                   pen.color().blue() / 65535.0,
                                                   pen.color().alpha() / 65535.0);
    if(_penColor)
      CFRelease(_penColor);

    _penColor = color;
    _penSize = pen.size();
    
    switch( pen.capStyle() )
    {
        default:
        case Pt::Gfx::Pen::FlatCap:
            _penCap = kCGLineCapButt;
            break;
        
        case Pt::Gfx::Pen::SquareCap:
            _penCap = kCGLineCapSquare;
            break; 
        
        case Pt::Gfx::Pen::RoundCap:
            _penCap = kCGLineCapRound;
            break;
    }

    switch( pen.style() )
    {
        default:
        case Pt::Gfx::Pen::Solid:
        {
            _dashes.clear();
            break;
        }
            
        case Pt::Gfx::Pen::Dash:
        {
            if(pen.capStyle() == Gfx::Pen::RoundCap ||
               pen.capStyle() == Gfx::Pen::SquareCap)
            {
                _dashes = { 2.0 * pen.size(), 2.0 * pen.size() };
            }
            else
            {
                _dashes = { 3.0 * pen.size(), 1.0 * pen.size() };
            }
            break;
        }
 
        case Pt::Gfx::Pen::Dot:
        {
            if(pen.capStyle() == Gfx::Pen::RoundCap ||
               pen.capStyle() == Gfx::Pen::SquareCap)
            {
                _dashes  = { 1.0, 2.0 * pen.size() };
            }
            else
            {
                _dashes = { 1.0 * pen.size(), 1.0 * pen.size() };
            }
            break;
        }
    }

    switch( pen.joinStyle() )
    {
        default:
        case Pt::Gfx::Pen::RoundJoin:
            _penJoin = kCGLineJoinRound;
            break;
        
        case Pt::Gfx::Pen::BevelJoin:
            _penJoin = kCGLineJoinBevel;
            break;
            
        case Pt::Gfx::Pen::MiterJoin:
            _penJoin = kCGLineJoinMiter;
            break;
    }

    if(_fontAttributes)
    {
        CFDictionarySetValue(_fontAttributes, kCTForegroundColorAttributeName, _penColor);
    }
}


void PixmapCanvas::onApplyPen()
{
    if( ! _pixmap)
        return;

    CGContextRef context = _pixmap->context();

    CGContextSetStrokeColorWithColor(context, _penColor);
    CGContextSetLineWidth(context, _penSize);
    CGContextSetLineCap(context, _penCap);
    CGContextSetLineJoin(context, _penJoin);

    if( _dashes.empty() )
    {
        CGContextSetLineDash(context, 0, NULL, 0);
    }
    else
    {
        CGContextSetLineDash( context, 0, &_dashes[0], _dashes.size() );
    }
}


void PixmapCanvas::onSetBrush(const Gfx::Brush& brush)
{
    _brushStyle = brush.fillStyle();

    switch(_brushStyle)
    {
        default:
        case Pt::Gfx::Brush::Solid:
        {
            CGColorRef color = CGColorCreateGenericRGB(brush.color().red() / 65535.0,
                                                       brush.color().green() / 65535.0,
                                                       brush.color().blue() / 65535.0,
                                                       brush.color().alpha() / 65535.0);
            if(_brushColor)
              CFRelease(_brushColor);

            _brushColor = color;
            break;
        }

        case Pt::Gfx::Brush::Texture:
            break;
    }
}


void PixmapCanvas::onApplyBrush()
{
    if( ! _pixmap)
        return;

    CGContextRef context = _pixmap->context();

    switch(_brushStyle)
    {
        default:
        case Pt::Gfx::Brush::Solid:
            CGContextSetFillColorWithColor(context, _brushColor);
            break;
            
        case Pt::Gfx::Brush::Texture:
            // CGContextSetFillPattern
            break;
    }
}


void PixmapCanvas::onSetFont(const Gfx::Font& font)
{
    if( ! _fontAttributes )
    {
        _fontAttributes = CFDictionaryCreateMutable(kCFAllocatorDefault, 2, 
                                                    &kCFTypeDictionaryKeyCallBacks, 
                                                    &kCFTypeDictionaryValueCallBacks);
        if( ! _fontAttributes )
            return;
    }

    if( _penColor )
    {
        CFDictionarySetValue(_fontAttributes, kCTForegroundColorAttributeName, _penColor);
    }
    CTFontRef fontRef = CocoaFontProvider::instance().lookupFont(font);
    if( ! fontRef )
        return;

    if(_font)
        CFRelease(_font);

    _font = fontRef;
    CFDictionarySetValue(_fontAttributes, kCTFontAttributeName, _font);
}


void PixmapCanvas::onApplyFont()
{
}


void PixmapCanvas::onSetClip(const Gfx::RectF* clipRect)
{
    // Restore to the clean createCanvas state and save a new checkpoint,
    // because CGContext clipping is cumulative and cannot be undone
    // without restoring the graphics state.
    if(_pixmap && isActive())
    {
        CGContextRef context = _pixmap->context();
        CGContextRestoreGState(context);
        CGContextSaveGState(context);
        invalidate(DirtyAll & ~DirtyClip);
    }

    if( ! clipRect )
    {
        _clipRect = CGRectNull;
    }
    else 
    {
        _clipRect = CGRectMake( clipRect->x(), clipRect->y(), 
                                clipRect->width(), clipRect->height() );
    }
}


void PixmapCanvas::onApplyClip()
{
    if( ! _pixmap)
        return;

    if( ! CGRectIsNull(_clipRect) )
        CGContextClipToRect(_pixmap->context(), _clipRect);
}


void PixmapCanvas::onDrawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    CGContextRef context = _pixmap->context();

    CGContextMoveToPoint(context, from.x(), from.y());
    CGContextAddLineToPoint(context, to.x(), to.y());
    CGContextStrokePath(context);

    _pixmap->setModified();
}


void PixmapCanvas::onDrawPolyline(const Gfx::PointF* pts, const size_t n)
{
    if(n < 2)
        return;

    CGContextRef context = _pixmap->context();
    
    const Gfx::PointF& first = pts[0];
    CGContextMoveToPoint(context, first.x(), first.y());

    for(unsigned i = 1; i < n; i++)
    {
        const Gfx::PointF& p = pts[i];
        CGContextAddLineToPoint(context, p.x(), p.y());
    }

    CGContextStrokePath(context);

    _pixmap->setModified();
}


void PixmapCanvas::onFillPolygon(const Gfx::PointF* pts, const size_t n)
{
    if(n < 2)
        return;

    CGContextRef context = _pixmap->context();

    const Gfx::PointF&first = pts[0];
    CGContextMoveToPoint(context, first.x(), first.y());

    for(unsigned i = 1; i < n; i++)
    {
        const Gfx::PointF& p = pts[i];
        CGContextAddLineToPoint(context, p.x(), p.y());
    }
 
    CGContextFillPath(context);

    _pixmap->setModified();
}


void PixmapCanvas::onDrawRect(const Gfx::RectF& r)
{
    CGContextRef context = _pixmap->context();
    
    CGRect rect = CGRectMake( r.x(), r.y(), r.width(), r.height() );
    CGContextStrokeRect(context, rect);

    _pixmap->setModified();
}


void PixmapCanvas::onFillRect(const Gfx::RectF& r)
{
    CGContextRef context = _pixmap->context();

    CGRect rect = CGRectMake( r.x(), r.y(), r.width(), r.height() );
    CGContextFillRect(context, rect);

    _pixmap->setModified();
}


void PixmapCanvas::onDrawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    CGContextRef context = _pixmap->context();

    CGRect rect = CGRectMake( topLeft.x(), topLeft.y(), 
                              size.width(), size.height() );

    CGContextAddEllipseInRect(context, rect);
    CGContextStrokePath(context);

    _pixmap->setModified();
}


void PixmapCanvas::onFillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    CGContextRef context = _pixmap->context();

    CGRect rect = CGRectMake( topLeft.x(), topLeft.y(), 
                              size.width(), size.height() ); 

    CGContextAddEllipseInRect(context, rect);
    CGContextFillPath(context);

    _pixmap->setModified();
}


Gfx::TextMetrics PixmapCanvas::onGetTextMetrics(const Pt::String& text) const
{
    CGContextRef context = _pixmap->context();

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
                                    _fontAttributes, false);

    CTLineRef line = CTLineCreateWithAttributedString(attributedString);
    CFRelease(attributedString);

    double width = CTLineGetTypographicBounds(line, NULL, NULL, NULL);
    CGFloat ascent = CTFontGetAscent(_font);
    CGFloat descent = CTFontGetDescent(_font);
    CGFloat leading = CTFontGetLeading(_font);
    CGFloat capHeight = CTFontGetCapHeight(_font);
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
                              const Gfx::Transform* tx)
{
    CGContextRef context = _pixmap->context();

    CFStringRef string = CFStringCreateWithBytesNoCopy(NULL, (const UInt8*) text.c_str(), 
                                                       text.length() * 4, 
                                                       kCFStringEncodingUTF32LE, 
                                                       false, kCFAllocatorNull);
    if( ! string )
        return;

    if( ! _attributedString )
        _attributedString = CFAttributedStringCreateMutable(kCFAllocatorDefault, 0);

    CFIndex length = CFAttributedStringGetLength(_attributedString);
    CFAttributedStringReplaceString(_attributedString, CFRangeMake(0, length), string);
    CFRelease(string);
    
    length = CFAttributedStringGetLength(_attributedString);
    CFAttributedStringSetAttributes(_attributedString, CFRangeMake(0, length), 
                                    _fontAttributes, false);

    //CGContextSetTextMatrix( context, CGAffineTransformMake( 1.0, 0.0, 0.0,
    //                                                        -1.0, 0.0, 0.0) );

    Gfx::PointF p = to;

    // Gfx::Transform xform = transform();
    // xform.translate(to.x(), to.y());

    // if (tx)
    //     xform *= *tx;

    // // CGAffineTransform: a, b, c, d, tx, ty
    // CGAffineTransform tf = CGAffineTransformMake( xform.m11(), xform.m12(), 
    //                                               xform.m21(), xform.m22(),
    //                                               xform.dx(),  xform.dy() );
    
    CGContextSaveGState(context);

    // undo y-axis flip to prevent drawing upside down
    CGFloat H = CGBitmapContextGetHeight(context);
    CGFloat height = scaling().toLogical(H);
    CGContextScaleCTM(context, 1.0, -1.0);
    CGContextTranslateCTM( context, 0, -height );

    //CGContextConcatCTM(context, tf);
    //CGContextSetTextPosition(context, 0, 0);

    CGFloat y = height - p.y();
    CGContextSetTextPosition(context, p.x(), y);

    CTLineRef line = CTLineCreateWithAttributedString(_attributedString);
    CTLineDraw(line, context);   
    CFRelease(line);

    CGContextRestoreGState(context);

    // ALTERNATIVE: CTRunDraw

    _pixmap->setModified();
}


void PixmapCanvas::onDrawImage(const Gfx::PointF& to, 
                               const Gfx::Image& image,
                               const Gfx::RectF* rect)
{
    if( image.empty() )
        return;

    CGContextRef context = _pixmap->context();

    CGContextSaveGState(context);

    // undo y-axis flip to prevent drawing upside down
    CGFloat H = CGBitmapContextGetHeight(context);
    CGFloat height = scaling().toLogical(H);
    CGContextScaleCTM(context, 1.0, -1.0);
    CGContextTranslateCTM(context, 0, -height);

    const Pt::uint8_t* data = image.data();
    std::size_t dataSize = image.size();

    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, data, dataSize, NULL);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrder32Host|kCGImageAlphaPremultipliedFirst;
    
    CGImageRef imageRef = CGImageCreate(image.width(), image.height(), 
                                        8, 32, 4 * image.width(), 
                                        colorSpace, bitmapInfo, provider, 
                                        NULL, false, kCGRenderingIntentDefault);

    double imageWidth = scaling().toLogical( image.width() );
    double imageHeight = scaling().toLogical( image.height() );
    Gfx::RectF imageRect( to, Gfx::SizeF(imageWidth, imageHeight) );
    
    CGRect targetRect = CGRectMake( imageRect.x(), imageRect.y(), 
                                    imageRect.width(), imageRect.height() );
    targetRect.origin.y = height - (targetRect.origin.y + targetRect.size.height);

    CGContextDrawImage(context, targetRect, imageRef);
    
    CFRelease(imageRef);
    CFRelease(colorSpace);
    CFRelease(provider);

    CGContextRestoreGState(context);

    _pixmap->setModified();
}


CGMutablePathRef PixmapCanvas::makePath(const Gfx::Path& path)
{
    CGMutablePathRef cgPath = CGPathCreateMutable();

    for(Gfx::PathIterator it = path.begin(); it != path.end(); ++it)
    {
        switch( it->type() )
        {
            default:
                break;

            case Gfx::Path::Close:
                CGPathCloseSubpath(cgPath);
                break;

            case Gfx::Path::MoveTo:
            {
                const Gfx::PointF& to = it->point(0);
                CGPathMoveToPoint(cgPath, NULL, to.x(), to.y());
                break;
            }

            case Gfx::Path::LineTo:
            {
                const Gfx::PointF& to = it->point(0);
                CGPathAddLineToPoint(cgPath, NULL, to.x(), to.y());
                break;
            }

            case Gfx::Path::QuadTo:
            {
                const Gfx::PointF& c1 = it->point(0);
                const Gfx::PointF& to = it->point(1);

                CGPathAddQuadCurveToPoint(cgPath, NULL, c1.x(), c1.y(),
                                          to.x(), to.y() );
                break;
            }

            case Gfx::Path::CubicTo:
            {
                const Gfx::PointF& c1 = it->point(0);
                const Gfx::PointF& c2 = it->point(1);
                const Gfx::PointF& to = it->point(2);

                CGPathAddCurveToPoint(cgPath, NULL, c1.x(), c1.y(), 
                                      c2.x(), c2.y(), to.x(), to.y());
                break;
            }
        }
    }

    return cgPath;
}


void PixmapCanvas::onSetPath(const Gfx::Path& path)
{
    if(_cgPath)
        CGPathRelease(_cgPath);

    _cgPath = makePath(path);
}


void PixmapCanvas::onDrawPath()
{
    CGContextRef context = _pixmap->context();

    CGContextAddPath(context, _cgPath);
    CGContextStrokePath(context);

    _pixmap->setModified();
}


void PixmapCanvas::onFillPath()
{
    CGContextRef context = _pixmap->context();

    CGContextAddPath(context, _cgPath);
    CGContextFillPath(context);

    _pixmap->setModified();
}


void PixmapCanvas::onDrawPath(const Gfx::Path& path)
{
    CGMutablePathRef cgPath = makePath(path);

    CGContextRef context = _pixmap->context();
    CGContextAddPath(context, cgPath);
    CGContextStrokePath(context);

    _pixmap->setModified();

    if(cgPath)
        CGPathRelease(cgPath);
}


void PixmapCanvas::onFillPath(const Gfx::Path& path)
{
    CGMutablePathRef cgPath = makePath(path);

    CGContextRef context = _pixmap->context();
    CGContextAddPath(context, cgPath);
    CGContextFillPath(context);

    _pixmap->setModified();

    if(cgPath)
        CGPathRelease(cgPath);
}

///////////////////////////////////////////////////////////////////////
// PixmapImpl
///////////////////////////////////////////////////////////////////////

PixmapImpl::PixmapImpl()
: _physicalSize(0, 0)
, _width(0)
, _height(0)
, _context(0)
, _image(0)
, _imageModified(false)
, _canvas(0)
{
    create();
}


PixmapImpl::~PixmapImpl()
{
    destroy();
}


void PixmapImpl::create()
{
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    _context = CGBitmapContextCreate(0, _width, _height,
                                     8, 0, colorSpace, 
                                     kCGImageAlphaPremultipliedLast);

    CGColorSpaceRelease(colorSpace);

    //CGContextSetShouldAntialias(_context, false);
    //CGContextSetBlendMode(_context, kCGBlendModeNormal);
    //CGContextSetBlendMode(_context, kCGBlendModeCopy)

    _imageModified = true;
}


void PixmapImpl::destroy()
{
    if(_image)
        CGImageRelease(_image);

    if(_context)
        CGContextRelease(_context);

    _image = 0;
    _context = 0;
}


CGContextRef PixmapImpl::context() const
{
    return _context;
}


CGImageRef PixmapImpl::getCGImage() const
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


void PixmapImpl::setModified()
{
    _imageModified = true;
}


void PixmapImpl::reset(const Gfx::Image& image)
{
    size_t width = image.width();
    size_t height = image.height();

    Gfx::SizeF size(width, height);
    reset(size);

    if( image.empty() )
        return;

    Gfx::PointF to(0, 0);

    const Pt::uint8_t* data = image.data();
    std::size_t dataSize = image.size();

    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, data, dataSize, NULL);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrder32Host|kCGImageAlphaPremultipliedFirst;
    
    CGImageRef imageRef = CGImageCreate(image.width(), image.height(), 
                                        8, 32, 4 * image.width(), 
                                        colorSpace, bitmapInfo, provider, 
                                        NULL, false, kCGRenderingIntentDefault);

    CGRect contextRect = CGRectMake( to.x(), 
                                     _height - to.y() - image.height(), 
                                     image.width(), 
                                     image.height() );

    //CGContextClipToRect(_context, CGRectNull);
    CGContextDrawImage(_context, contextRect, imageRef);
   
    CFRelease(imageRef);
    CFRelease(colorSpace);
    CFRelease(provider);

    _imageModified = true;
}


void PixmapImpl::reset(const Gfx::SizeF& size)
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

    destroy();
    create();
}


void PixmapImpl::getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const
{
    bitmap.reset( rect.size() );

    Gfx::Image image;

    Gfx::Painter painter(bitmap);
    painter.drawImage(Gfx::PointF(0, 0), image, rect);
}


void PixmapImpl::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);
}


const Gfx::ImageFormat& PixmapImpl::format() const
{
    return Gfx::ImageFormat::rgb32();
}


const Gfx::SizeF& PixmapImpl::size() const
{
    return _physicalSize;
}


const Gfx::Scaling& PixmapImpl::scaling() const
{
    return _scaling;
}


Gfx::Canvas* PixmapImpl::createCanvas(Gfx::Canvas* reuse)
{
    PixmapCanvas* canvas = dynamic_cast<PixmapCanvas*>(reuse);
    if( ! canvas ) 
        canvas  = new PixmapCanvas();

    CGContextSaveGState(_context);

    canvas->setPixmap(*this);

    _canvas = canvas;
    return _canvas;
}


void PixmapImpl::releaseCanvas()
{
    // NOTE: this might be called from the attached canvas base class destructor

    CGContextRestoreGState(_context);

    _canvas = 0;
}


void PixmapImpl::sync()
{
}


void PixmapImpl::finish()
{
}


void PixmapImpl::drawPixmap(const Gfx::PointF& toF,
                            const Pixmap& pm,
                            const Gfx::Paint& paint,
                            const Gfx::RectF* rectF)
{
    const PixmapImpl* pixmap = pm.impl();

    Gfx::PointF to = _scaling.toPhysical(toF);
    CGImageRef image = pixmap->getCGImage();
   
    if(_canvas)
    {
        _canvas->reset();

        // Restore the Save from createCanvas to get a clean context
        CGContextRestoreGState(_context);
    }

    if(rectF)
    {
        Gfx::RectF rect = _scaling.toPhysical(*rectF);

        CGRect subRect = CGRectMake(rect.left(), 
                                    rect.top(), 
                                    rect.size().width(), 
                                    rect.size().height());

        CGImageRef subImage = CGImageCreateWithImageInRect(image, subRect);

        CGRect destRect = CGRectMake(to.x(), 
                                     _height - to.y() - rect.height(), 
                                     rect.width(), 
                                     rect.height());

        CGContextDrawImage(_context, destRect, subImage);
        CGImageRelease(subImage);
    }
    else
    {
        CGRect destRect = CGRectMake(to.x(), 
                                     _height - to.y() - pm.size().height(), 
                                     pixmap->size().width(), 
                                     pixmap->size().height());

        CGContextDrawImage(_context, destRect, image);
    }

    // Re-Save so releaseCanvas can still Restore its own Save
    if(_canvas)
        CGContextSaveGState(_context);

    _imageModified = true;
}


const std::string& PixmapImpl::defaultFont()
{
    return CocoaFontProvider::instance().defaultFont();
}


void PixmapImpl::setDefaultFont(const std::string& family)
{
    CocoaFontProvider::instance().setDefaultFont(family);
}


std::vector<std::string> PixmapImpl::fontFamilies()
{
    return CocoaFontProvider::instance().fontFamilies();
}


std::vector<Gfx::FontFace> PixmapImpl::fontFaces(const std::string& family)
{
    return CocoaFontProvider::instance().fontFaces(family);
}

} // namespace

} // namespace
