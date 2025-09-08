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

#include "RasterContext.h"
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/PaintLayer.h>
#include <Pt/Gfx/ImageSurface.h>

namespace Pt {

namespace Gfx {

void testBlend2d()
{
  // Use constructor or `create()` function to
  // allocate a new image data of the required
  // format.
  BLImage img(480, 480, BL_FORMAT_PRGB32);

  // Attach a rendering context into `img`.
  BLContext ctx(img);

  // Clearing the image would make it transparent.
  ctx.clear_all();

  // Create a path having cubic curves.
  BLPath path;
  path.move_to(26, 31);
  path.cubic_to(642, 132, 587, -136, 25, 464);
  path.cubic_to(882, 404, 144, 267, 27, 31);

  // Fill a path with opaque white - 0xAARRGGBB.
  ctx.fill_path(path, BLRgba32(0xFFFF0000));

  // Detach the rendering context from `img`.
  ctx.end();

  // Let's use some built-in codecs provided by Blend2D.
  img.write_to_file("bl_sample_1.png");
}


RasterContext::RasterContext()
: PaintContext()
{
  static int nnn = 0;
  if(nnn == 0)
  {
    testBlend2d();
    ++nnn;
  }
}


RasterContext::~RasterContext()
{

}


void RasterContext::setImage(Image& image)
{
    _image = &image;
}


void RasterContext::onBeginPaint(const Gfx::Paint& paint)
{

}


void RasterContext::onResetPaint()
{
    // NOTE: this might be called from the attached canvas base class destructor

    if(_image)
        _image = 0;
}


void RasterContext::onSetCompositionMode(const Gfx::CompositionMode& mode) 
{

}


void RasterContext::onApplyCompositionMode(const Gfx::CompositionMode& mode)
{

}


void RasterContext::onSetPen(const Gfx::Pen& pen)
{

}


void RasterContext::onApplyPen(const Gfx::Pen& pen)
{

}


void RasterContext::onSetBrush(const Gfx::Brush& brush)
{

}


void RasterContext::onApplyBrush(const Gfx::Brush& brush)
{

}


void RasterContext::onSetFont(const Gfx::Font& font)
{
}


void RasterContext::onApplyFont(const Gfx::Font& font)
{
}


void RasterContext::onSetClip(const Gfx::RectF* clip)
{
}


void RasterContext::onApplyClip(const Gfx::RectF* clip) 
{
}


void RasterContext::onDrawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
}


void RasterContext::onDrawPolyline(const Gfx::PointF* pts, const size_t n)
{
}


void RasterContext::onFillPolygon(const Gfx::PointF* pts, const size_t n)
{
}


void RasterContext::onDrawRect(const Gfx::RectF& r)
{
}


void RasterContext::onFillRect(const Gfx::RectF& r)
{
}


void RasterContext::onDrawEllipse(const PointF& topLeftF, const SizeF& sizeF)
{
}


void RasterContext::onFillEllipse(const PointF& topLeftF, const SizeF& sizeF)
{
}


void RasterContext::onBeginPath()
{
}


void RasterContext::onMoveTo(const PointF& to)
{
}


void RasterContext::onLineTo(const PointF& to)
{
}


void RasterContext::onCurveTo(const PointF &cp, const PointF& to)
{
}


void RasterContext::onCurveTo(const PointF &cp1, const PointF &cp2, const PointF& to)
{
}


void RasterContext::onClosePath()
{
}


void RasterContext::onSetPath(const Gfx::Path& path)
{
}


void RasterContext::onDrawPath(const Path& path)
{
}


void RasterContext::onFillPath(const Path& path)
{
}


TextMetrics RasterContext::onGetTextMetrics(const String& text) const
{
    return TextMetrics();
}


void RasterContext::onDrawText(const PointF& to, const Pt::String& text, 
                               const Transform* tform)
{
}


void RasterContext::onDrawImage(const PointF& toF, const Image& image, 
                               const RectF* imageRect)
{
}


bool RasterContext::onDrawLayer(const Gfx::PointF& to,
                                const Gfx::PaintLayer& layer,
                                const Gfx::RectF* rect)
{
    const PaintSurface* layerSurface = layer.surface();
    const ImageSurface* imageSurface = dynamic_cast<const ImageSurface*>(layerSurface);
    if(imageSurface)
    {
        const Gfx::Image& image = imageSurface->image();
        
        if(rect)
        {
            Gfx::RectF imageRect = scaling().toPhysical(*rect);
            drawImage(to, image, &imageRect);
        }
        else
        {
            drawImage(to, image);
        }
        
        return true;
    }

    return false;
}

} // namespace

} // namespace
