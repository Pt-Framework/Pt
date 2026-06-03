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

#include "PixmapCanvas.h"
#include "PixmapImpl.h"
#include "CocoaFontProvider.h"

#include <Pt/Forms/Pixmap.h>
#include <Pt/Gfx/FontMetrics.h>

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
, _brushGradient(0)
, _font(0)
, _fontAttributes(0)
, _attributedString(0)
, _cgPath(0)
{
}


PixmapCanvas::~PixmapCanvas()
{
    if(_font)
      CFRelease(_font);

    if(_penColor)
      CFRelease(_penColor);

    if(_brushColor)
      CFRelease(_brushColor);

    if(_brushGradient)
      CGGradientRelease(_brushGradient);

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


void PixmapCanvas::onBeginPaint(const Gfx::Paint& paint)
{
    if(_pixmap)
    {
        _pixmap->invalidateImage();

        CGContextRef context = _pixmap->context();
        if(context)
            CGContextSaveGState(context);
    }
}


void PixmapCanvas::onFinishPaint()
{
    if(_pixmap)
    {
        CGContextRef context = _pixmap->context();
        if(context)
            CGContextRestoreGState(context);
    }

    _pixmap = 0;
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


void PixmapCanvas::onSetCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;
}


void PixmapCanvas::onApplyCompositionMode()
{
    if( ! _pixmap)
        return;

    CGContextRef context = _pixmap->context();
    if( ! context)
        return;

    CGBlendMode blendMode = kCGBlendModeNormal;

    switch(_compositionMode)
    {
        case Gfx::CompositionMode::SourceCopy:
            blendMode = kCGBlendModeCopy;
            break;

        case Gfx::CompositionMode::SourceOver:
        default:
            blendMode = kCGBlendModeNormal;
            break;
    }

    CGContextSetBlendMode(context, blendMode);
}


void PixmapCanvas::onSetPen(const Gfx::Pen& pen)
{
    CGColorRef color = CGColorCreateGenericRGB( pen.color().red() / 255.0,
                                                pen.color().green() / 255.0,
                                                pen.color().blue() / 255.0,
                                                pen.color().alpha() / 255.0 );
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
    _brush = brush;

    switch(_brush.fillStyle())
    {
        default:
        case Pt::Gfx::Brush::Solid:
        {
            CGColorRef color = CGColorCreateGenericRGB( brush.color().red() / 255.0,
                                                        brush.color().green() / 255.0,
                                                        brush.color().blue() / 255.0,
                                                        brush.color().alpha() / 255.0 );
            if(_brushColor)
              CFRelease(_brushColor);

            _brushColor = color;
            break;
        }

        case Pt::Gfx::Brush::Gradient:
        {
            if(_brushGradient)
            {
                CGGradientRelease(_brushGradient);
                _brushGradient = 0;
            }

            const Gfx::ColorStops& stops = brush.gradientStops();
            const std::size_t count = stops.size();

            if(count > 0)
            {
                std::vector<CGFloat> components;
                std::vector<CGFloat> locations;
                components.reserve(count * 4);
                locations.reserve(count);

                for(std::size_t i = 0; i < count; ++i)
                {
                    const Gfx::ColorStop& s = stops[i];
                    components.push_back( s.color().red()   / 255.0 );
                    components.push_back( s.color().green() / 255.0 );
                    components.push_back( s.color().blue()  / 255.0 );
                    components.push_back( s.color().alpha() / 255.0 );
                    locations.push_back( s.position() );
                }

                CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
                _brushGradient = CGGradientCreateWithColorComponents(colorSpace,
                                                                components.data(),
                                                                locations.data(),
                                                                count);
                CGColorSpaceRelease(colorSpace);
            }
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

    switch(_brush.fillStyle())
    {
        default:
        case Pt::Gfx::Brush::Solid:
            CGContextSetFillColorWithColor(context, _brushColor);
            break;

        case Pt::Gfx::Brush::Gradient:
            // gradient is drawn at fill time via fillGradient()
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

    CGFloat ascent = CTFontGetAscent(_font);
    CGFloat descent = CTFontGetDescent(_font);
    CGFloat leading = CTFontGetLeading(_font);
    CGFloat capHeight = CTFontGetCapHeight(_font);
    CGFloat xHeight = CTFontGetXHeight(_font);
    CGFloat underlinePos = CTFontGetUnderlinePosition(_font);
    CGFloat underlineThickness = CTFontGetUnderlineThickness(_font);

    _fontMetrics.setAscent(ascent);
    _fontMetrics.setDescent(descent);
    _fontMetrics.setCapHeight(capHeight);
    _fontMetrics.setXHeight(xHeight);
    _fontMetrics.setLeading(leading);
    _fontMetrics.setUnderlinePos(underlinePos);
    _fontMetrics.setUnderlineThickness(underlineThickness);
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
    if( ! _pixmap)
        return;

    CGContextRef context = _pixmap->context();

    CGContextMoveToPoint(context, from.x(), from.y());
    CGContextAddLineToPoint(context, to.x(), to.y());
    CGContextStrokePath(context);
}


void PixmapCanvas::onDrawPolyline(const Gfx::PointF* pts, const size_t n)
{
    if(n < 2 || ! _pixmap)
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
}


void PixmapCanvas::onFillPolygon(const Gfx::PointF* pts, const size_t n)
{
    if(n < 2 || ! _pixmap)
        return;

    CGContextRef context = _pixmap->context();

    const Gfx::PointF& first = pts[0];
    CGContextMoveToPoint(context, first.x(), first.y());

    for(unsigned i = 1; i < n; i++)
    {
        const Gfx::PointF& p = pts[i];
        CGContextAddLineToPoint(context, p.x(), p.y());
    }

    if(_brush.fillStyle() == Gfx::Brush::Gradient)
    {
        CGContextSaveGState(context);
        CGRect bbox = CGContextGetPathBoundingBox(context);
        CGContextClip(context);
        fillGradient(context, bbox);
        CGContextRestoreGState(context);
    }
    else
    {
        CGContextFillPath(context);
    }
}


void PixmapCanvas::onDrawRect(const Gfx::RectF& r)
{
    if( ! _pixmap)
        return;

    CGContextRef context = _pixmap->context();

    CGRect rect = CGRectMake( r.x(), r.y(), r.width(), r.height() );
    CGContextStrokeRect(context, rect);
}


void PixmapCanvas::onFillRect(const Gfx::RectF& r)
{
    if( ! _pixmap)
        return;

    CGContextRef context = _pixmap->context();

    CGRect rect = CGRectMake( r.x(), r.y(), r.width(), r.height() );

    if(_brush.fillStyle() == Gfx::Brush::Gradient)
    {
        CGContextSaveGState(context);
        CGContextClipToRect(context, rect);
        fillGradient(context, rect);
        CGContextRestoreGState(context);
    }
    else
    {
        CGContextFillRect(context, rect);
    }
}


void PixmapCanvas::onDrawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if( ! _pixmap)
        return;

    CGContextRef context = _pixmap->context();

    CGRect rect = CGRectMake( topLeft.x(), topLeft.y(),
                              size.width(), size.height() );

    CGContextAddEllipseInRect(context, rect);
    CGContextStrokePath(context);
}


void PixmapCanvas::onFillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    if( ! _pixmap)
        return;

    CGContextRef context = _pixmap->context();

    CGRect rect = CGRectMake( topLeft.x(), topLeft.y(),
                              size.width(), size.height() );

    if(_brush.fillStyle() == Gfx::Brush::Gradient)
    {
        CGContextSaveGState(context);
        CGContextAddEllipseInRect(context, rect);
        CGContextClip(context);
        fillGradient(context, rect);
        CGContextRestoreGState(context);
    }
    else
    {
        CGContextAddEllipseInRect(context, rect);
        CGContextFillPath(context);
    }
}


Gfx::TextMetrics PixmapCanvas::onGetTextMetrics(const Pt::String& text) const
{
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

    double advance = CTLineGetTypographicBounds(line, NULL, NULL, NULL);

    CGRect bounds = CTLineGetBoundsWithOptions(line, 0);
    CFRelease(line);

    Gfx::TextMetrics fm;
    fm.setAdvance(advance);
    fm.setBearingX(bounds.origin.x);
    fm.setBearingY(bounds.origin.y + bounds.size.height);
    fm.setBoundingWidth(bounds.size.width);
    fm.setBoundingHeight(bounds.size.height);
    return fm;
}


const Gfx::FontMetrics& PixmapCanvas::onGetFontMetrics() const
{
    return _fontMetrics;
}


void PixmapCanvas::onDrawText(const Gfx::PointF& to,
                              const Pt::String& text,
                              const Gfx::Transform* /*tx*/)
{
    if( ! _pixmap)
        return;

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

    Gfx::PointF p = to;

    CGContextSaveGState(context);

    // undo y-axis flip to prevent drawing upside down
    CGFloat H = CGBitmapContextGetHeight(context);
    CGFloat height = scaling().toLogical(H);
    CGContextScaleCTM(context, 1.0, -1.0);
    CGContextTranslateCTM( context, 0, -height );

    CGFloat y = height - p.y();
    CGContextSetTextPosition(context, p.x(), y);

    CTLineRef line = CTLineCreateWithAttributedString(_attributedString);
    CTLineDraw(line, context);
    CFRelease(line);

    CGContextRestoreGState(context);
}


void PixmapCanvas::onDrawImage(const Gfx::PointF& to,
                               const Gfx::Image& image,
                               const Gfx::RectF* /*rect*/)
{
    if( image.empty() || ! _pixmap)
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
}


void PixmapCanvas::fillGradient(CGContextRef context, CGRect bbox)
{
    if( ! _brushGradient)
        return;

    const Gfx::Brush::GradientStyle gs = _brush.gradient();
    const Gfx::Brush::PositionMode  pm = _brush.positionMode();

    CGPoint start;
    CGPoint end;

    if(gs == Gfx::Brush::Horizontal)
    {
        start = CGPointMake(bbox.origin.x, bbox.origin.y);
        end   = CGPointMake(bbox.origin.x + bbox.size.width, bbox.origin.y);
    }
    else if(gs == Gfx::Brush::Vertical)
    {
        start = CGPointMake(bbox.origin.x, bbox.origin.y);
        end   = CGPointMake(bbox.origin.x, bbox.origin.y + bbox.size.height);
    }
    else if(pm == Gfx::Brush::Relative)
    {
        const Gfx::PointF& gb = _brush.gradientBegin();
        const Gfx::PointF& ge = _brush.gradientEnd();
        start = CGPointMake(bbox.origin.x + gb.x() * bbox.size.width,
                            bbox.origin.y + gb.y() * bbox.size.height);
        end   = CGPointMake(bbox.origin.x + ge.x() * bbox.size.width,
                            bbox.origin.y + ge.y() * bbox.size.height);
    }
    else
    {
        start = CGPointMake(_brush.gradientBegin().x(), _brush.gradientBegin().y());
        end   = CGPointMake(_brush.gradientEnd().x(),   _brush.gradientEnd().y());
    }

    const CGGradientDrawingOptions opts = kCGGradientDrawsBeforeStartLocation
                                        | kCGGradientDrawsAfterEndLocation;

    if(gs == Gfx::Brush::Radial)
    {
        CGFloat r0 = _brush.gradientBeginRadius();
        CGFloat r1 = _brush.gradientEndRadius();

        if(pm == Gfx::Brush::Relative)
        {
            CGFloat scale = std::min(bbox.size.width, bbox.size.height);
            r0 *= scale;
            r1 *= scale;
        }

        CGContextDrawRadialGradient(context, _brushGradient, start, r0, end, r1, opts);
    }
    else
    {
        CGContextDrawLinearGradient(context, _brushGradient, start, end, opts);
    }
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
                                          to.x(), to.y());
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
    if( ! _pixmap)
        return;

    CGContextRef context = _pixmap->context();

    CGContextAddPath(context, _cgPath);
    CGContextStrokePath(context);
}


void PixmapCanvas::onFillPath()
{
    if( ! _pixmap)
        return;

    CGContextRef context = _pixmap->context();

    if(_brush.fillStyle() == Gfx::Brush::Gradient)
    {
        CGContextSaveGState(context);
        CGContextAddPath(context, _cgPath);
        CGRect bbox = CGContextGetPathBoundingBox(context);
        CGContextClip(context);
        fillGradient(context, bbox);
        CGContextRestoreGState(context);
    }
    else
    {
        CGContextAddPath(context, _cgPath);
        CGContextFillPath(context);
    }
}


void PixmapCanvas::onDrawPath(const Gfx::Path& path)
{
    if( ! _pixmap)
        return;

    CGMutablePathRef cgPath = makePath(path);

    CGContextRef context = _pixmap->context();
    CGContextAddPath(context, cgPath);
    CGContextStrokePath(context);

    if(cgPath)
        CGPathRelease(cgPath);
}


void PixmapCanvas::onFillPath(const Gfx::Path& path)
{
    if( ! _pixmap)
        return;

    CGMutablePathRef cgPath = makePath(path);

    CGContextRef context = _pixmap->context();

    if(_brush.fillStyle() == Gfx::Brush::Gradient)
    {
        CGContextSaveGState(context);
        CGContextAddPath(context, cgPath);
        CGRect bbox = CGContextGetPathBoundingBox(context);
        CGContextClip(context);
        fillGradient(context, bbox);
        CGContextRestoreGState(context);
    }
    else
    {
        CGContextAddPath(context, cgPath);
        CGContextFillPath(context);
    }

    if(cgPath)
        CGPathRelease(cgPath);
}


void PixmapCanvas::drawPixmap(const Gfx::PointF& toF,
                              const Pixmap& pm,
                              const Gfx::RectF* rectF)
{
    if( ! _pixmap )
        return;

    applyState();

    const PixmapImpl* pixmap = pm.impl();
    CGContextRef ctx = _pixmap->context();
    CGContextRef srcCtx = pixmap->context();

    if( ! ctx || ! srcCtx )
        return;

    CGContextSaveGState(ctx);

    CGImageRef image = pixmap->getCGImage();

    if(rectF)
    {
        const Gfx::Scaling& srcScaling = pixmap->scaling();
        Gfx::RectF  physSrc = srcScaling.toPhysical(*rectF);
        CGRect sourceRect = CGRectMake(physSrc.left(),
                                       physSrc.top(),
                                       physSrc.size().width(),
                                       physSrc.size().height());
        CGRect drawRect = CGRectMake(0, 0, rectF->width(), rectF->height());

        CGImageRef subImage = CGImageCreateWithImageInRect(image, sourceRect);

        CGContextTranslateCTM(ctx, toF.x(), toF.y() + rectF->height());
        CGContextScaleCTM(ctx, 1.0, -1.0);

    #ifdef PT_FORMS_WARN_UNALIGNED_BLIT
        CGRect destRect = CGContextConvertRectToDeviceSpace(ctx, drawRect);
        Detail::warnIfExpensiveBlit("PixmapCanvas::drawPixmap",
                                    sourceRect, destRect);
    #endif

        CGContextDrawImage(ctx, drawRect, subImage);
        CGImageRelease(subImage);
    }
    else
    {
        double logW = scaling().toLogical( pixmap->size().width() );
        double logH = scaling().toLogical( pixmap->size().height() );
        CGRect drawRect = CGRectMake(0, 0, logW, logH);

        CGContextTranslateCTM(ctx, toF.x(), toF.y() + logH);
        CGContextScaleCTM(ctx, 1.0, -1.0);

#ifdef PT_FORMS_WARN_UNALIGNED_BLIT
        CGRect sourceRect = CGRectMake(0, 0,
                           CGImageGetWidth(image),
                           CGImageGetHeight(image));
        CGRect destRect = CGContextConvertRectToDeviceSpace(ctx, drawRect);
        Detail::warnIfExpensiveBlit("PixmapCanvas::drawPixmap",
                                    sourceRect, destRect);
#endif

        CGContextDrawImage(ctx, drawRect, image);
    }

    CGContextRestoreGState(ctx);
}

} // namespace Forms

} // namespace Pt
