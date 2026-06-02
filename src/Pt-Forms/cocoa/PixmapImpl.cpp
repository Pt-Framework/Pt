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
#include "PixmapCanvas.h"
#include "CocoaFontProvider.h"

#include <Pt/Forms/Pixmap.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintContext.h>

#include <cassert>

namespace Pt {

namespace Forms {

///////////////////////////////////////////////////////////////////////
// PixmapImpl
///////////////////////////////////////////////////////////////////////

PixmapImpl::PixmapImpl()
: _physicalSize(0, 0)
, _width(0)
, _height(0)
, _context(0)
, _image(0)
, _canvas(0)
{
}


PixmapImpl::~PixmapImpl()
{
    destroy();
}


void PixmapImpl::create()
{
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();

    _context = CGBitmapContextCreate(0, _width, _height, 8, 0, colorSpace,
                                     kCGBitmapByteOrder32Host|kCGImageAlphaPremultipliedFirst);

    CGColorSpaceRelease(colorSpace);
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


void PixmapImpl::invalidateImage()
{
    if(_image)
    {
        CGImageRelease(_image);
        _image = 0;
    }
}


CGImageRef PixmapImpl::getCGImage() const
{
    if( ! _image && _context)
    {
        void* data = CGBitmapContextGetData(_context);
        size_t bytesPerRow = CGBitmapContextGetBytesPerRow(_context);
        size_t dataSize = bytesPerRow * _height;

        CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, data, dataSize, NULL);
        CGColorSpaceRef colorSpace = CGBitmapContextGetColorSpace(_context);
        CGBitmapInfo bitmapInfo = CGBitmapContextGetBitmapInfo(_context);

        _image = CGImageCreate(_width, _height, 8, 32,
                               bytesPerRow, colorSpace, bitmapInfo, provider,
                               NULL, false, kCGRenderingIntentDefault);

        CGDataProviderRelease(provider);
    }

    return _image;
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

    CGContextDrawImage(_context, contextRect, imageRef);

    CFRelease(imageRef);
    CFRelease(colorSpace);
    CFRelease(provider);
}


void PixmapImpl::reset(const Gfx::SizeF& size)
{
    size_t width = lround( size.width() );
    size_t height = lround( size.height() );

    _width = width;
    _height = height;

    _physicalSize.set(width, height);

    destroy();
    create();
}


void PixmapImpl::reset()
{
    destroy();

    _width = 0;
    _height = 0;

    _physicalSize.set(0, 0);
}


void PixmapImpl::getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const
{
    bitmap.reset( rect.size() );

    Gfx::Image image;

    Gfx::PaintContext ctx(bitmap);
    Gfx::Painter painter(ctx);
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

    canvas->setPixmap(*this);

    _canvas = canvas;
    return _canvas;
}


void PixmapImpl::releaseCanvas()
{
    // NOTE: this might be called from the attached canvas base class destructor

    _canvas = 0;
}


void PixmapImpl::sync()
{
}


void PixmapImpl::finish()
{
}


void PixmapImpl::drawPixmap(Gfx::Canvas& canvas,
                            const Gfx::PointF& to,
                            const Pixmap& pm,
                            const Gfx::RectF* rect)
{
    assert(_canvas == &canvas);

    if(_canvas == &canvas)
        _canvas->drawPixmap(to, pm, rect);
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

} // namespace Forms

} // namespace Pt
