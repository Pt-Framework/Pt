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
#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Algorithm.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// RasterSurface
///////////////////////////////////////////////////////////////////////

RasterSurface::RasterSurface()
: _rasterImage()
, _rasterContext()
{
}


RasterSurface::RasterSurface(const Gfx::SizeF& size, std::size_t stride)
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

    if( _rasterContext.target_image() )
        _rasterContext.end();
    
    std::size_t lineSize = _image.format().imageSize(image.width(), 1, image.padding());

    _rasterImage.create_from_data( image.width(), image.height(), 
                                   BL_FORMAT_PRGB32, _image.data(), lineSize );
}


void RasterSurface::reset(const Gfx::SizeF& sizeF, std::size_t stride)
{
    long width = lround( sizeF.width() );
    long height = lround( sizeF.height() );

    _image.reset( _image.format(), width, height, stride );

    _physicalSize.set(width, height);

    std::size_t lineSize = _image.format().imageSize(width, 1, stride);

    if( _rasterContext.target_image() )
        _rasterContext.end();
    
    _rasterImage.create_from_data( _image.width(), _image.height(),
                                   BL_FORMAT_PRGB32, _image.data(), lineSize );
}


void RasterSurface::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);

    _physicalSize.set( _image.width(), _image.height() );
}


const Gfx::ImageFormat& RasterSurface::format() const
{
    return Gfx::ImageFormat::argb32();
}


const Gfx::SizeF& RasterSurface::size() const
{
    return _physicalSize;
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

    if( ! _rasterContext.target_image() )
        _rasterContext.begin(_rasterImage);

    _rasterContext.save(_stateCookie);

    paintContext->init(_rasterContext, _image);
    return paintContext;
}


void RasterSurface::releaseContext()
{
    _rasterContext.restore(_stateCookie);
}


void RasterSurface::finish()
{
    if( _rasterContext.target_image() )
        _rasterContext.end();
}

#if USE_BLEND2D_BLIT

void toPRGB(const Pt::Gfx::Image& image, 
            std::vector<Pt::uint8_t>& bitmapData)
{
    size_t _width = image.width();
    size_t _height = image.height();

    for (std::size_t y = 0; y < image.height(); ++y)
    {
        for (std::size_t x = 0; x < image.width(); ++x)
        {
            Pt::Gfx::ConstPixel pixel(image.view(), x, y);
            Pt::Gfx::Color color = pixel.getColor();

            const Pt::uint8_t r = color.red() / 257;
            const Pt::uint8_t g = color.green() / 257;
            const Pt::uint8_t b = color.blue() / 257;
            const Pt::uint8_t a = color.alpha() / 257;

            bitmapData.push_back((Pt::uint8_t) (a * b / 255));
            bitmapData.push_back((Pt::uint8_t) (a * g / 255));
            bitmapData.push_back((Pt::uint8_t) (a * r / 255));
            bitmapData.push_back((Pt::uint8_t) (a));
        }
    }
}


void RasterSurface::drawBitmap(const Pt::Gfx::PointF& toF,
                               const Bitmap& bitmap,
                               const Gfx::Paint& paint,
                               const Gfx::RectF* bitmapRect)
{
    if( ! _rasterContext.target_image() )
        _rasterContext.begin(_rasterImage);

    _rasterContext.save();
    _rasterContext.reset_transform();

    const Scaling& scale = scaling();
    const Image& image = bitmap.image();

    Gfx::PointF toP = scale.toPhysical(toF);
    BLPoint pos( toP.x(), toP.y() );

    if( image.empty() )
        return;

    void* data = const_cast<Pt::uint8_t*>( image.data() );
    std::size_t stride = image.format().imageSize( image.width(), 1, image.padding() );

    BLCompOp compOp = BL_COMP_OP_SRC_OVER;
    
    if(paint.compositionMode() == CompositionMode::SourceOver)
    {
        compOp = BL_COMP_OP_SRC_OVER;
    }
    else // CompositionMode::SourceCopy
    {
        compOp = BL_COMP_OP_SRC_COPY;
    }

    _rasterContext.set_comp_op(compOp);

    BLImage view;
    std::vector<Pt::uint8_t> bitmapData;

    if(paint.compositionMode() == CompositionMode::SourceCopy)
    {
        view.create_from_data(image.width(), image.height(), BL_FORMAT_XRGB32,
                              data, stride, BL_DATA_ACCESS_READ);
    }
    else
    {
        toPRGB(image, bitmapData);

        view.create_from_data(image.width(), image.height(), BL_FORMAT_PRGB32,
                              bitmapData.data(), stride, BL_DATA_ACCESS_READ);
    }

    if(bitmapRect)
    {

        Gfx::RectF imageRect = bitmap.scaling().toPhysical(*bitmapRect);
        BLRectI srcRect(lround( imageRect.x() ),
                        lround( imageRect.y() ), 
                        lround( imageRect.width() ),
                        lround( imageRect.height() ) );

        _rasterContext.blit_image(pos, view, srcRect);
    }
    else
    {
        _rasterContext.blit_image(pos, view);
    }

    _rasterContext.restore();
}

#else

void RasterSurface::drawBitmap(const Pt::Gfx::PointF& toF,
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

#endif

} // namespace

} // namespace
