/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
 * Copyright (C) 2010 Aloysius Indrayanto
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/

#include <stdexcept>

#include <Pt/Gfx/SGNode.h>
#include <Pt/Gfx/ImagePainter2.h>


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== SGNode Class ===================================================================
// ======================================================================================

SGNode::~SGNode()
{}

void SGNode::clear()
{
    for(Children::iterator it = _children.begin(); it != _children.end(); ++it) {
        delete *it;
    }

    _pen   = Pen  ();
    _brush = Brush();

    _transform.identity();
    _children.clear();
}


// ======================================================================================
// ===== SGNodePath Class ===============================================================
// ======================================================================================

SGNodePath::~SGNodePath()
{ clear(); }

void SGNodePath::clear()
{
    _path.clear();

    SGNode::clear();
}

void SGNodePath::draw(ImagePainter2& painter, const Transform* transform)
{
    // Check if this node and all its children must not be drawn
    if(_rm == RenderNone) return;

    // Combine the transformations
    const Transform& thisTransform = transform ? ( _transform * (*transform) ) : _transform;

    // Save the original pen and brush
    const Pen   pen   = painter.pen  ();
    const Brush brush = painter.brush();

    // Set the current pen and/or brush as needed
    if(!_pen  .isNull()) painter.setPen  (_pen  );
    if(!_brush.isNull()) painter.setBrush(_brush);

    // Draw the children
    for(Children::iterator it = _children.begin(); it != _children.end(); ++it) {
        (*it)->draw(painter, &thisTransform);
    }

    // Draw the path only if it is not null
    if(!_path.isNull()) {
        // Generate points
        std::vector<PointF> pointsF;
        _path.generatePoints(pointsF, _smoothness);
        // Transform points
        thisTransform.transformPoints(pointsF.data(), pointsF.size());
        // Draw based on the mode
        const RenderMode rm = (_rm == RenderInherit) ? _parent->renderMode() : _rm;
        switch(rm) {
            case RenderFill            : painter.fillPolygon (pointsF.data(), pointsF.size()       ); break;
            case RenderStroke          : painter.drawPolyline(pointsF.data(), pointsF.size(), false); break;
            case RenderStrokeAutoClose : painter.drawPolyline(pointsF.data(), pointsF.size(), true ); break;
            default                    :                                                              break;
        }
    }

    // Restore the original pen and/or brush as needed
    if(!_pen  .isNull()) painter.setPen  (pen  );
    if(!_brush.isNull()) painter.setBrush(brush);
}


} // namespace
} // namespace
