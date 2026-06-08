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

#include "CocoaPixmapImpl.h"
#include "CocoaPixmapCanvas.h"

#include <Pt/Forms/Pixmap.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintContext.h>

#include <cassert>
#include <cmath>
#include <iostream>

namespace Pt {

namespace Forms {

#ifdef PT_FORMS_WARN_UNALIGNED_BLIT

namespace {

bool isNearlyEqual(double left, double right)
{
    return std::fabs(left - right) <= 0.0001;
}


bool isPhysicalPixelAligned(const CGRect& rect)
{
    return isNearlyEqual(CGRectGetMinX(rect), std::round(CGRectGetMinX(rect))) &&
           isNearlyEqual(CGRectGetMinY(rect), std::round(CGRectGetMinY(rect))) &&
           isNearlyEqual(CGRectGetMaxX(rect), std::round(CGRectGetMaxX(rect))) &&
           isNearlyEqual(CGRectGetMaxY(rect), std::round(CGRectGetMaxY(rect)));
}

} // namespace

namespace Detail {

void warnIfExpensiveBlit(const char* tag,
                         const CGRect& sourceRectValue,
                         const CGRect& destRectValue)
{
    CGRect sourceRect = CGRectStandardize(sourceRectValue);
    CGRect destRect   = CGRectStandardize(destRectValue);

    double sourcePixels = std::fabs(sourceRect.size.width * sourceRect.size.height);
    double destPixels   = std::fabs(destRect.size.width   * destRect.size.height);
    double pixelCount   = sourcePixels > destPixels ? sourcePixels : destPixels;

    if(pixelCount < 16384.0)
        return;

    if( isPhysicalPixelAligned(sourceRect) && isPhysicalPixelAligned(destRect) )
        return;

    std::clog << "Warning: " << tag
              << " has an unaligned expensive blit."
              << std::endl;
}

} // namespace Detail

#endif

///////////////////////////////////////////////////////////////////////
// CocoaPixmapImpl
///////////////////////////////////////////////////////////////////////

CocoaPixmapImpl::CocoaPixmapImpl()
: _physicalSize(0, 0)
, _width(0)
, _height(0)
, _context(0)
, _image(0)
, _canvas(0)
{}

CocoaPixmapImpl::~CocoaPixmapImpl()
{
    destroy();
}

void CocoaPixmapImpl::create()
{
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();

    _context = CGBitmapContextCreate(0, _width, _height, 8, 0, colorSpace,
                                     kCGBitmapByteOrder32Host|kCGImageAlphaPremultipliedFirst);

    CGColorSpaceRelease(colorSpace);
}

void CocoaPixmapImpl::destroy()
{
    if(_image)   CGImageRelease(_image);
    if(_context) CGContextRelease(_context);
    _image   = 0;
    _context = 0;
}

CGContextRef CocoaPixmapImpl::context() const
{
    return _context;
}

void CocoaPixmapImpl::invalidateImage()
{
    if(_image)
    {
        CGImageRelease(_image);
        _image = 0;
    }
}

CGImageRef CocoaPixmapImpl::getCGImage() const
{
    if( ! _image && _context)
    {
        void*  data        = CGBitmapContextGetData(_context);
        size_t bytesPerRow = CGBitmapContextGetBytesPerRow(_context);
        size_t dataSize    = bytesPerRow * _height;

        CGDataProviderRef provider   = CGDataProviderCreateWithData(NULL, data, dataSize, NULL);
        CGColorSpaceRef   colorSpace = CGBitmapContextGetColorSpace(_context);
        CGBitmapInfo      bitmapInfo = CGBitmapContextGetBitmapInfo(_context);

        _image = CGImageCreate(_width, _height, 8, 32,
                               bytesPerRow, colorSpace, bitmapInfo, provider,
                               NULL, false, kCGRenderingIntentDefault);

        CGDataProviderRelease(provider);
    }

    return _image;
}

void CocoaPixmapImpl::reset(const Gfx::Image& image)
{
    size_t width  = image.width();
    size_t height = image.height();

    Gfx::SizeF size(width, height);
    reset(size);

    if( image.empty() )
        return;

    Gfx::PointF to(0, 0);

    const Pt::uint8_t* data     = image.data();
    std::size_t        dataSize = image.size();

    CGDataProviderRef provider   = CGDataProviderCreateWithData(NULL, data, dataSize, NULL);
    CGColorSpaceRef   colorSpace = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo      bitmapInfo = kCGBitmapByteOrder32Host|kCGImageAlphaPremultipliedFirst;

    CGImageRef imageRef = CGImageCreate(image.width(), image.height(),
                                        8, 32, 4 * image.width(),
                                        colorSpace, bitmapInfo, provider,
                                        NULL, false, kCGRenderingIntentDefault);

    CGRect contextRect = CGRectMake( to.x(),
                                     _height - to.y() - image.height(),
                                     image.width(),
                                     image.height() );

    CGContextDrawImage(_context, contextRect, imageRef);

    CFRelease(imageRef);
    CFRelease(colorSpace);
    CFRelease(provider);
}

void CocoaPixmapImpl::reset(const Gfx::SizeF& size)
{
    size_t width  = lround( size.width() );
    size_t height = lround( size.height() );

    _width  = width;
    _height = height;

    _physicalSize.set(width, height);

    destroy();
    create();
}

void CocoaPixmapImpl::reset()
{
    destroy();
    _width  = 0;
    _height = 0;
    _physicalSize.set(0, 0);
}

void CocoaPixmapImpl::getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const
{
    bitmap.reset( rect.size() );

    Gfx::Image image;

    Gfx::PaintContext ctx(bitmap);
    Gfx::Painter painter(ctx);
    painter.drawImage(Gfx::PointF(0, 0), image, rect);
}

void CocoaPixmapImpl::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);
}

const Gfx::ImageFormat& CocoaPixmapImpl::format() const
{
    return Gfx::ImageFormat::rgb32();
}

const Gfx::SizeF& CocoaPixmapImpl::size() const
{
    return _physicalSize;
}

const Gfx::Scaling& CocoaPixmapImpl::scaling() const
{
    return _scaling;
}

Gfx::Canvas* CocoaPixmapImpl::createCanvas(Gfx::Canvas* reuse)
{
    CocoaPixmapCanvas* canvas = dynamic_cast<CocoaPixmapCanvas*>(reuse);
    if( ! canvas )
        canvas = new CocoaPixmapCanvas();

    canvas->setPixmap(*this);

    _canvas = canvas;
    return _canvas;
}

void CocoaPixmapImpl::releaseCanvas()
{
    _canvas = 0;
}

void CocoaPixmapImpl::sync()   {}
void CocoaPixmapImpl::finish() {}

void CocoaPixmapImpl::drawPixmap(Gfx::Canvas& canvas,
                                 const Gfx::PointF& to,
                                 const Pixmap& pm,
                                 const Gfx::RectF* rect)
{
    assert(_canvas == &canvas);

    if(_canvas == &canvas)
        _canvas->drawPixmap(to, pm, rect);
}

} // namespace Forms

} // namespace Pt
