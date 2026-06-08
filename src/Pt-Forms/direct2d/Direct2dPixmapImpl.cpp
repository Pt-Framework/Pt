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

#include "win32.h"
#include "Direct2dPixmapImpl.h"
#include "Direct2dPixmapCanvas.h"
#include "D2DDevice.h"
#include "ApplicationImpl.h"

#include <Pt/Forms/Application.h>
#include <Pt/Forms/View.h>
#include <Pt/Forms/Pixmap.h>

#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Canvas.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/PaintContext.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Rgb32.h>
#include <Pt/Gfx/Argb32.h>

#include <cassert>

namespace Pt {

namespace Forms {

Direct2dPixmapImpl::Direct2dPixmapImpl()
: _physicalSize(0, 0)
, _width(0)
, _height(0)
, _d2dBitmap(0)
, _canvas(0)
{
}


Direct2dPixmapImpl::~Direct2dPixmapImpl()
{
    destroyBitmap();
}


void Direct2dPixmapImpl::createBitmap(LONG width, LONG height)
{
    destroyBitmap();

    if(width == 0 || height == 0)
        return;

    ID2D1DeviceContext* ctx = 0;
    Application::instance().impl()->d2d().d2dDevice()->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &ctx);
    if( ! ctx)
        return;

    D2D1_BITMAP_PROPERTIES1 bmpProps = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0);

    HRESULT hr = ctx->CreateBitmap(
        D2D1::SizeU(static_cast<UINT32>(width), static_cast<UINT32>(height)),
        nullptr, 0, bmpProps, &_d2dBitmap);

    ctx->Release();

    if(FAILED(hr))
    {
        _d2dBitmap = 0;
        return;
    }

    _width = width;
    _height = height;
    _physicalSize.set(width, height);
}


void Direct2dPixmapImpl::destroyBitmap()
{
    if(_d2dBitmap)
    {
        _d2dBitmap->Release();
        _d2dBitmap = 0;
    }

    _width = 0;
    _height = 0;
    _physicalSize.set(0, 0);
}


void Direct2dPixmapImpl::reset(const Gfx::SizeF& size)
{
    LONG width = lround( size.width() );
    LONG height = lround( size.height() );

    if(_width == width && _height == height)
        return;

    createBitmap(width, height);
}


void Direct2dPixmapImpl::reset()
{
    destroyBitmap();
}


void Direct2dPixmapImpl::reset(const Gfx::Image& image)
{
    size_t width = image.width();
    size_t height = image.height();

    Gfx::SizeF size(width, height);
    reset(size);

    if( ! _d2dBitmap)
        return;

    // Upload image pixels into D2D bitmap (BGRA premultiplied)
    const Pt::uint8_t* src = image.data();

    Gfx::Rgb32Image rgb32Image;
    if(image.format() != Gfx::ImageFormat::rgb32() || image.padding() != 0)
    {
        rgb32Image.reset(width, height);
        Gfx::copyView(image, rgb32Image);
        src = rgb32Image.data();
    }

    D2D1_RECT_U destRect = D2D1::RectU(0, 0,
        static_cast<UINT32>(width), static_cast<UINT32>(height));
    _d2dBitmap->CopyFromMemory(&destRect, src,
        static_cast<UINT32>(width * 4));
}


Gfx::Image Direct2dPixmapImpl::toImage() const
{
    if(_width == 0 || _height == 0 || ! _d2dBitmap)
        return Gfx::Image();

    // Map the bitmap to read pixels (create a CPU-readable copy)
    ID2D1DeviceContext* ctx = 0;
    Application::instance().impl()->d2d().d2dDevice()->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &ctx);
    if( ! ctx)
        return Gfx::Image();

    // Create a CPU-readable bitmap
    D2D1_BITMAP_PROPERTIES1 readProps = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0);

    ID2D1Bitmap1* readBitmap = 0;
    HRESULT hr = ctx->CreateBitmap(
        D2D1::SizeU(static_cast<UINT32>(_width), static_cast<UINT32>(_height)),
        nullptr, 0, readProps, &readBitmap);

    if(FAILED(hr) || ! readBitmap)
    {
        ctx->Release();
        return Gfx::Image();
    }

    D2D1_POINT_2U destPoint = D2D1::Point2U(0, 0);
    D2D1_RECT_U srcRect = D2D1::RectU(0, 0,
        static_cast<UINT32>(_width), static_cast<UINT32>(_height));
    hr = readBitmap->CopyFromBitmap(&destPoint, _d2dBitmap, &srcRect);

    Gfx::Image image;
    if(SUCCEEDED(hr))
    {
        D2D1_MAPPED_RECT mapped;
        hr = readBitmap->Map(D2D1_MAP_OPTIONS_READ, &mapped);
        if(SUCCEEDED(hr))
        {
            image = Gfx::Image(_width, _height, Gfx::Rgb32());
            for(LONG y = 0; y < _height; ++y)
            {
                std::memcpy(image.data() + y * _width * 4,
                            mapped.bits + y * mapped.pitch,
                            _width * 4);
            }
            readBitmap->Unmap();
        }
    }

    readBitmap->Release();
    ctx->Release();
    return image;
}


void Direct2dPixmapImpl::getBitmap(Gfx::Bitmap& bitmap, const Gfx::RectF& rect) const
{
    bitmap.reset( rect.size() );

    Gfx::Image image = this->toImage();

    Gfx::PaintContext ctx(bitmap);
    Gfx::Painter painter(ctx);
    painter.drawImage(Gfx::PointF(0, 0), image, rect);
}


const Gfx::SizeF& Direct2dPixmapImpl::size() const
{
    return _physicalSize;
}


void Direct2dPixmapImpl::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);
}


const Gfx::ImageFormat& Direct2dPixmapImpl::format() const
{
    return Gfx::ImageFormat::rgb32();
}


const Gfx::Scaling& Direct2dPixmapImpl::scaling() const
{
    return _scaling;
}


Gfx::Canvas* Direct2dPixmapImpl::createCanvas(Gfx::Canvas* reuse)
{
    Direct2dPixmapCanvas* canvas = dynamic_cast<Direct2dPixmapCanvas*>(reuse);
    if( ! canvas)
        canvas = new Direct2dPixmapCanvas();

    canvas->setPixmap(*this);
    _canvas = canvas;
    return _canvas;
}


void Direct2dPixmapImpl::releaseCanvas()
{
    _canvas = 0;
}


void Direct2dPixmapImpl::sync()
{
}


void Direct2dPixmapImpl::finish()
{
}


void Direct2dPixmapImpl::drawPixmap(Gfx::Canvas& canvas,
                                    const Gfx::PointF& to,
                                    const Pixmap& pm,
                                    const Gfx::RectF* rect)
{
    assert(_canvas == &canvas);

    if(_canvas == &canvas)
        _canvas->drawPixmap(to, pm, rect);
}

} // namespace

} // namespace
