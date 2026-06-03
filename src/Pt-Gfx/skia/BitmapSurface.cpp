/* Copyright (C) 2024 Marc Boris Duerner

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
#include "../freetype/FreeTypeFontProvider.h"

#include <Pt/Gfx/Rgb32.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Bitmap.h>
#include <Pt/Gfx/Image.h>

#include <SkImageInfo.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// BitmapSurface
///////////////////////////////////////////////////////////////////////

BitmapSurface::BitmapSurface()
: _image( Rgb32::get() )
{
}


BitmapSurface::BitmapSurface(const Gfx::SizeF& size, std::size_t stride)
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


void BitmapSurface::initSkSurface()
{
    _skSurface.reset();

    if( _rgb32Image.width() == 0 || _rgb32Image.height() == 0 )
        return;

    SkImageInfo info = SkImageInfo::Make( _rgb32Image.width(),
                                          _rgb32Image.height(),
                                          kBGRA_8888_SkColorType,
                                          kPremul_SkAlphaType );

    _skSurface = SkSurface::MakeRasterDirect( info,
                                               _rgb32Image.data(),
                                               _rgb32Image.stride() );
}


void BitmapSurface::reset(const Gfx::Image& image)
{
    _rgb32Image.reset( image.width(), image.height() );
    copyView(image, _rgb32Image);

    _image.reset( _rgb32Image.data(), _rgb32Image.width(), _rgb32Image.height(),
                  _rgb32Image.padding() );

    _physicalSize.set( image.width(), image.height() );

    initSkSurface();
}


void BitmapSurface::reset(const Gfx::SizeF& sizeF, std::size_t stride)
{
    long width = lround( sizeF.width() );
    long height = lround( sizeF.height() );

    _rgb32Image.reset( width, height, stride );

    _image.reset( _rgb32Image.data(), _rgb32Image.width(), _rgb32Image.height(),
                  _rgb32Image.padding() );

    _physicalSize.set(width, height);

    initSkSurface();
}


void BitmapSurface::clear()
{
    _skSurface.reset();
    _rgb32Image.clear();
    _image.clear();
    _physicalSize.set(0, 0);
}


void BitmapSurface::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);

    _physicalSize.set( _rgb32Image.width(), _rgb32Image.height() );
}


const Gfx::ImageFormat& BitmapSurface::format() const
{
    return Gfx::ImageFormat::rgb32();
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

    canvas->init(*this);
    return canvas;
}


void BitmapSurface::releaseCanvas()
{
}


void BitmapSurface::sync()
{
    if( _skSurface )
        _skSurface->getCanvas()->flush();
}


void BitmapSurface::finish()
{
}


const std::string& BitmapSurface::defaultFont()
{
    return FreeTypeFontProvider::instance().defaultFont();
}


void BitmapSurface::setDefaultFont(const std::string& family)
{
    FreeTypeFontProvider::instance().setDefaultFont(family);
}


std::vector<std::string> BitmapSurface::fontFamilies()
{
    return FreeTypeFontProvider::instance().fontFamilies();
}


std::vector<FontFace> BitmapSurface::fontFaces(const std::string& family)
{
    return FreeTypeFontProvider::instance().fontFaces(family);
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

    RectI fullClip;
    fullClip.setWidth( _rgb32Image.width() );
    fullClip.setHeight( _rgb32Image.height() );

    if(bitmapRect)
    {
        Gfx::RectF imageRect = bitmap.scaling().toPhysical(*bitmapRect);

        RectI srcRect( PointI( lround( imageRect.x() ),
                               lround( imageRect.y() ) ),
                       SizeI( lround( imageRect.width() ),
                              lround( imageRect.height() ) ) );

        putImage(to, image, srcRect, fullClip, paint.compositionMode());
    }
    else
    {
        RectI srcRect;
        srcRect.setWidth( image.width() );
        srcRect.setHeight( image.height() );

        putImage(to, image, srcRect, fullClip, paint.compositionMode());
    }
}


void BitmapSurface::putImage(const PointI& to, const Image& image,
                             const RectI& imageRect, const RectI& clip,
                             const CompositionMode& mode)
{
    // clip against source boundaries
    RectI fromRect( image.width(), image.height() );
    fromRect = fromRect.toIntersected(imageRect);

    // update target position if rect got smaller
    PointI toPos = to;
    toPos += fromRect.topLeft() - imageRect.topLeft();

    // clip against target boundaries
    RectI toRect( toPos, fromRect.size() );
    toRect = toRect.toIntersected(clip);

    // update source position if rect got smaller
    PointI fromPos = fromRect.topLeft();
    fromPos += toRect.topLeft() - toPos;
    fromRect.setOrigin(fromPos);

    // update source size if rect got smaller
    fromRect.setSize( toRect.size() );

    auto toView = view<Rgb32>(_rgb32Image.data(), _rgb32Image.width(), _rgb32Image.height(), _rgb32Image.padding());
    const auto fromView = view<Rgb32>(image.data(), image.width(), image.height(), image.padding());

    auto toLines = lineView(toView, toRect.x(), toRect.y(), toRect.width(), toRect.height());
    auto fromLines = lineView(fromView, fromRect.x(), fromRect.y(), fromRect.width(), fromRect.height());
    auto fromIt = fromLines.begin();

    switch( mode )
    {
        default:
        case CompositionMode::SourceCopy:
        {
            for(auto& toSpan : toLines)
            {
                sourceCopy(toSpan.front(), fromIt->front(), fromIt->length());
                ++fromIt;
            }
            break;
        }

        case CompositionMode::SourceOver:
        {
            for(auto& toSpan : toLines)
            {
                sourceOver(toSpan.front(), fromIt->front(), fromIt->length());
                ++fromIt;
            }
            break;
        }
    }
}

} // namespace

} // namespace
