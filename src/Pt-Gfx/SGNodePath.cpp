/* Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2017-2017 Aloysius Indrayanto

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


#include <Pt/Gfx/SGNodePath.h>
#include <Pt/Gfx/ImagePainter2.h>


namespace Pt {
namespace Gfx {


SGNodePath::~SGNodePath()
{}

void SGNodePath::clear()
{
    // Clear the path and reset the smoothness
    _path.clear();
    _smoothness = 1.0f;

    // Clear the base class' data
    SGNode::clear();
}

void SGNodePath::drawImpl(ImagePainter2& painter, const TransformT& transform, RenderMode overrideRM) const
{
    // Return if the path is null
    if(_path.isNull()) return;

    // Generate points
    std::vector<PointT> points;
    _path.generatePoints(points, _smoothness);

    // TransformT points
    transform.transformPoints(points.data(), points.size());

    // Draw based on the effective mode
    switch(effectiveRenderMode(overrideRM)) {
        case RenderFill            : painter.fillPolygon (points.data(), points.size()       ); break;
        case RenderStroke          : painter.drawPolyline(points.data(), points.size(), false); break;
        case RenderStrokeAutoClose : painter.drawPolyline(points.data(), points.size(), true ); break;
        default                    :                                                            break;
    }
}


} // namespace
} // namespace
