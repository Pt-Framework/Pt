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

#include "RasterSurface.h"
#include "RasterContext.h"

#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/ImageSurface.h>
#include <Pt/Gfx/Algorithm.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// ImageSurface
///////////////////////////////////////////////////////////////////////

RasterSurface::RasterSurface()
: _context(0)
{
}


RasterSurface::RasterSurface(const Gfx::SizeF& size, std::size_t stride)
: _context(0)
{
    reset(size, stride);
}


RasterSurface::~RasterSurface()
{
}


const Gfx::Image& RasterSurface::image() const
{
    return _image;
}


void RasterSurface::reset(const Gfx::Image& image)
{
    if( image.format() != _image.format() )
    {
        _image.reset( format(), image.width(), image.height() );
        Pt::Gfx::copy( image.begin(), image.end(), _image.begin() );
    }
    else
    {
        _image = image;
    }

    _physicalSize.set( image.width(), image.height() );
    _logicalSize = _scaling.toLogical( Gfx::SizeF( image.width(), 
                                                   image.height() ) );

    //invalidate();
}


void RasterSurface::reset(const Gfx::SizeF& sizeF, std::size_t stride)
{
    long width = lround( sizeF.width() );
    long height = lround( sizeF.height() );

    _image.reset( _image.format(), width, height, stride );

    _physicalSize.set(width, height);
    _logicalSize = _scaling.toLogical( Gfx::SizeF(width, height) );

    //invalidate();
}


const SizeF& RasterSurface::physicalSize() const
{
    return _physicalSize;
}


const SizeF& RasterSurface::logicalSize() const
{
    return _logicalSize;
}


void RasterSurface::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);

    _physicalSize.set( _image.width(), _image.height() );
    _logicalSize = _scaling.toLogical( Gfx::SizeF( _image.width(), 
                                                   _image.height() ) );

    //invalidate();
}


const Gfx::ImageFormat& RasterSurface::format() const
{
    return Gfx::ImageFormat::argb32();
}


const Gfx::SizeF& RasterSurface::size() const
{
    return physicalSize();
}


const Scaling& RasterSurface::scaling() const
{
    return _scaling;
}


Gfx::PaintContext* RasterSurface::createContext(Gfx::PaintContext* context)
{
    RasterContext* paintContext = dynamic_cast<RasterContext*>(context);
    if( ! paintContext )
        paintContext = new RasterContext();

    paintContext->setImage(_image);
    
    _context = paintContext;
    return _context;
}


void RasterSurface::releaseContext()
{
    _context = 0;
}


void RasterSurface::sync()
{
}


void RasterSurface::drawImage(const Pt::Gfx::PointF& toF,
                              const ImageSurface& bitmap,
                              const Gfx::Paint& paint,
                              const Gfx::RectF* bitmapRect)
{
    const Scaling& scale = scaling();
    const Image& image = bitmap.image();

    Gfx::PointF toP = scale.toPhysical(toF);
    PointI to( toP.x(), toP.y() );

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


void RasterSurface::putImage(const PointI& to, const Image& image, 
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

    _image.view().copy(toRect.x(), toRect.y(), image.view(), 
                       fromRect.x(), fromRect.y(), 
                       fromRect.width(), fromRect.height(), paint.compositionMode());
}

} // namespace

} // namespace
