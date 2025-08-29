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
#include "ImageCanvas.h"

namespace Pt {

namespace Gfx {

RasterContext::RasterContext()
: PaintContext()
, _hasClip(false)
, _imageCanvas(0)
{
}


RasterContext::~RasterContext()
{
}


void RasterContext::setImage(ImageCanvas& imageCanvas)
{
    _imageCanvas = &imageCanvas;
}


void RasterContext::onBeginPaint(const Gfx::Paint& paint)
{
}


void RasterContext::onResetPaint()
{
    if(_imageCanvas)
        _imageCanvas = 0;
}


void RasterContext::onSetCompositionMode(const Gfx::CompositionMode& mode) 
{
    _compositionMode = mode;
}


void RasterContext::onSetPen(const Gfx::Pen& pen)
{
    double scaleFactor = scaling().scaleFactor();

    // keep pen size when downscaling
    size_t penSize = scaleFactor < 1.0 ? pen.size() 
                                       : static_cast<size_t>( pen.size() * scaleFactor );

    _pen = pen;
    _pen.setSize(penSize);
}


void RasterContext::onSetBrush(const Gfx::Brush& brush)
{
    _brush = brush;
}


void RasterContext::onSetFont(const Gfx::Font& font)
{
    _font = font;
}


void RasterContext::onSetClip(const Gfx::RectF* clip)
{
    _hasClip = clip != 0;

    if(clip)
        _clip = *clip;
    else
        _clip.clear();
}


const std::vector<Polygon>& RasterContext::flatPath()
{
    if( _flatPath.empty() )
    {
        _path.toPolygons(_flatPath);
    }

    return _flatPath;
}


void RasterContext::onSetPath(const Gfx::Path& path)
{
    _flatPath.clear();
    _path = path;
}


void RasterContext::onDrawPath()
{
    const std::vector<Polygon>& polygons = flatPath();

    if(_imageCanvas)
        _imageCanvas->drawPath(polygons);
}


void RasterContext::onFillPath()
{
    const std::vector<Polygon>& polygons = flatPath();

    if(_imageCanvas)
        _imageCanvas->fillPath(polygons);
}

} // namespace

} // namespace
