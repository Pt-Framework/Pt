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

// Just for debugging ;)
//#warning "Just for debugging ;)"
//#include <stdio.h>

#include <stdexcept>

#include <Pt/Gfx/SGNode.h>
#include <Pt/Gfx/ImagePainter2.h>


namespace Pt {
namespace Gfx {


/*
template <typename T>
static void sgDumpTransformMatrix(const BasicTransform<T>& transform)
{
    T r[3][3];
    transform.getRaw(r);

    printf("    | %7.3f %7.3f %7.3f |\n", r[0][0], r[0][1], r[0][2]);
    printf("    | %7.3f %7.3f %7.3f |\n", r[1][0], r[1][1], r[1][2]);
    printf("    | %7.3f %7.3f %7.3f |\n", r[2][0], r[2][1], r[2][2]);
    printf("\n");
}
//*/


// ======================================================================================
// ===== SGNode Class ===================================================================
// ======================================================================================

SGNode::~SGNode()
{}

void SGNode::clear()
{
    // Delete the children
    for(Children::iterator it = _children.begin(); it != _children.end(); ++it) {
        delete *it;
    }

    // Null the pen and brush
    _pen   = Pen  ();
    _brush = Brush();

    // Clear the transformation and children list
    _transform.identity();
    _children.clear();
}

const Transform SGNode::begDrawSeq(ImagePainter2& painter, const Transform* transform)
{
    // Combine the transformations
    const Transform& thisTransform = transform ? ( _transform * (*transform) ) : _transform;

    // Save the original pen and brush as needed
    if(!_pen  .isNull()) _savePen   = painter.pen  ();
    if(!_brush.isNull()) _saveBrush = painter.brush();

    // Set the current pen and/or brush as needed
    if(!_pen  .isNull()) painter.setPen  (_pen  );
    if(!_brush.isNull()) painter.setBrush(_brush);

    // Draw the children
    for(Children::iterator it = _children.begin(); it != _children.end(); ++it) {
        (*it)->draw(painter, &thisTransform);
    }

    // Combine the combined transformations
    return thisTransform;
}

const void SGNode::endDrawSeq(ImagePainter2& painter)
{
    // Restore the original pen and/or brush as needed
    if(!_pen  .isNull()) painter.setPen  (_savePen  );
    if(!_brush.isNull()) painter.setBrush(_saveBrush);
}


// ======================================================================================
// ===== SGNodePath Class ===============================================================
// ======================================================================================

SGNodePath::~SGNodePath()
{ clear(); }

void SGNodePath::clear()
{
    // Clear the path
    _path.clear();

    // Clear the base class' data
    SGNode::clear();
}

void SGNodePath::draw(ImagePainter2& painter, const Transform* transform)
{
    // Check if this node and all its children must not be drawn
    if(_rm == RenderNone) return;

    // Begin the drawing sequence
    const Transform& thisTransform = begDrawSeq(painter, transform);

    // Draw the path only if it is not null
    if(!_path.isNull()) {
        // Generate points
        std::vector<PointF> pointsF;
        _path.generatePoints(pointsF, _smoothness);
        // Transform points
        thisTransform.transformPoints(pointsF.data(), pointsF.size());
        // Draw based on the mode
        const RenderMode rm = renderMode();
        switch(rm) {
            case RenderFill            : painter.fillPolygon (pointsF.data(), pointsF.size()       ); break;
            case RenderStroke          : painter.drawPolyline(pointsF.data(), pointsF.size(), false); break;
            case RenderStrokeAutoClose : painter.drawPolyline(pointsF.data(), pointsF.size(), true ); break;
            default                    :                                                              break;
        }
    }

    // End the drawing sequence
    endDrawSeq(painter);
}


// ======================================================================================
// ===== SGNodeLine Class ===============================================================
// ======================================================================================
SGNodeLine::~SGNodeLine()
{}

void SGNodeLine::draw(ImagePainter2& painter, const Transform* transform)
{
    // Check if this node and all its children must not be drawn
    if(_rm == RenderNone) return;

    // Begin the drawing sequence
    const Transform& thisTransform = begDrawSeq(painter, transform);

    // Transform the coordinates
    PointF f, t;

    thisTransform.transformPoint(f, _from);
    thisTransform.transformPoint(t, _to  );

    Pen npen = _pen;

    float w = _pen.size() * 0.5f;
    float z = 0.0f;

    thisTransform.transformPoint(w, z);

    std::clog << w << std::endl;

    // TODO: Transform the pen width too!

    // Draw the line based on the mode
    const RenderMode rm = renderMode();
    switch(rm) {
        case RenderFill            : /* Fallthrough */
        case RenderStroke          : /* Fallthrough */
        case RenderStrokeAutoClose : painter.drawLine(f, t); break;
        default                    :                         break;
    }

    // End the drawing sequence
    endDrawSeq(painter);
}


} // namespace
} // namespace
