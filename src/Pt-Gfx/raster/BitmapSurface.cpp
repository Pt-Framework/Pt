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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include "BitmapSurface.h"
#include "BitmapCanvas.h"

#include <Pt/Gfx/Argb32.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Algorithm.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// BitmapSurface
///////////////////////////////////////////////////////////////////////

BitmapSurface::BitmapSurface()
: _canvas(0)
{
}


BitmapSurface::BitmapSurface(const Gfx::SizeF& size, std::size_t stride)
: _canvas(0)
{
    reset(size, stride);
}


BitmapSurface::~BitmapSurface()
{
}


const Gfx::Image& BitmapSurface::image() const
{
    return _image;
}


void BitmapSurface::reset(const Gfx::Image& image)
{
    if( image.format() != _image.format() )
    {
        _image.reset( image.width(), image.height() );
        copyArea(image, _image);
    }
    else
    {
        _image = image;
    }

    _physicalSize.set( image.width(), image.height() );
}


void BitmapSurface::reset(const Gfx::SizeF& sizeF, std::size_t stride)
{
    long width = lround( sizeF.width() );
    long height = lround( sizeF.height() );

    _image.reset( _image.format(), width, height, stride );

    _physicalSize.set(width, height);
}


void BitmapSurface::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);

    _physicalSize.set( _image.width(), _image.height() );
}


const Gfx::ImageFormat& BitmapSurface::format() const
{
    return Gfx::ImageFormat::argb32();
}


const Gfx::SizeF& BitmapSurface::size() const
{
    return _physicalSize;
}


const Scaling& BitmapSurface::scaling() const
{
    return _scaling;
}


Gfx::Canvas* BitmapSurface::createCanvas(Gfx::Canvas* reuse)
{
    BitmapCanvas* canvas = dynamic_cast<BitmapCanvas*>(reuse);
    if( ! canvas )
        canvas = new BitmapCanvas();

    canvas->init(_image);
    
    _canvas = canvas;
    return canvas;
}


void BitmapSurface::releaseCanvas()
{
    _canvas = 0;
}


void BitmapSurface::sync()
{
}


void BitmapSurface::finish()
{
}


void BitmapSurface::drawBitmap(const Pt::Gfx::PointF& toF,
                               const Bitmap& bitmap,
                               const Gfx::Paint& paint,
                               const Gfx::RectF* bitmapRect)
{
    const Scaling& scale = scaling();
    const Image& image = bitmap.image();

    Gfx::PointF toP = scale.toPhysical(toF);
    PointI to( lround( toP.x() ), 
               lround( toP.y() ) );

    if( image.empty() )
        return;

    if(bitmapRect)
    {
        Gfx::RectF imageRect = bitmap.scaling().toPhysical(*bitmapRect);

        RectI srcRect( PointI( lround( imageRect.x() ),
                               lround( imageRect.y() ) ), 
                       SizeI( lround( imageRect.width() ),
                              lround( imageRect.height() ) ) );

        putImage(to, image, paint, srcRect);
    }
    else
    {
        RectI srcRect;
        srcRect.setWidth( image.width() );
        srcRect.setHeight( image.height() );

        putImage(to, image, paint, srcRect);
    }
}


void BitmapSurface::putImage(const PointI& to, const Image& image, 
                             const Gfx::Paint& paint, const RectI& imageRect)
{
    // clip against source boundaries
    RectI fromRect( image.width(), image.height() );
    fromRect = fromRect.intersect(imageRect);

    // update target position if rect got smaller
    PointI toPos = to;
    toPos += fromRect.topLeft() - imageRect.topLeft();

    // clip against target boundaries
    RectI currentClip;
    currentClip.setWidth( _image.width() );
    currentClip.setHeight( _image.height() );

    RectI toRect( toPos, fromRect.size() );
    toRect = toRect.intersect(currentClip);

    // update source position if rect got smaller
    PointI fromPos = fromRect.topLeft();
    fromPos += toRect.topLeft() - toPos;
    fromRect.setOrigin(fromPos);

    // update source size if rect got smaller
    fromRect.setSize( toRect.size() );

    //std::clog << "BLIT to: " << toRect.x() << ", " << toRect.y() << " "
    //          << "from: " << fromRect.x() << ", " << fromRect.y() << " "
    //          << fromRect.width() << "x" << fromRect.height() << std::endl;

    Gfx::PixelView toView(_image);
    Gfx::PixelView::Iterator toIter = toView.pixel( toRect.x(), toRect.y() );

    Gfx::ConstPixelView fromView(image);
    Gfx::ConstPixelView::Iterator fromIter = fromView.pixel( fromRect.x(), fromRect.y() );

    switch( paint.compositionMode() )
    {
        default:
        case CompositionMode::SourceCopy:
            Argb32::sourceCopy(toIter->base(), toView.stride(),
                               fromIter->base(), fromView.stride(), 
                               fromRect.width(), fromRect.height());

            //Argb32::sourceCopy(toView.base(), toRect.x(), toRect.y(),
            //                   fromView, fromRect.x(), fromRect.y(), 
            //                   fromRect.width(), fromRect.height());
            break;

        case CompositionMode::SourceOver:
            Argb32::sourceOver(toIter->base(), toView.stride(),
                               fromIter->base(), fromView.stride(), 
                               fromRect.width(), fromRect.height());

            //Argb32::sourceOver(toView.base(), toRect.x(), toRect.y(),
            //                   fromView, fromRect.x(), fromRect.y(), 
            //                   fromRect.width(), fromRect.height());
            break;
    }
}

} // namespace

} // namespace
