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
#include <stack>

#include <Pt/Gfx/SGNode.h>
#include <Pt/Gfx/ImagePainter2.h>


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== SGNode Class ===================================================================
// ======================================================================================

SGNode::~SGNode()
{
    // Delete all the child objects
    for(Children::iterator it = _children.begin(); it != _children.end(); ++it) {
        delete *it;
    }
}

void SGNode::clear()
{
    // Delete all the child objects
    for(Children::iterator it = _children.begin(); it != _children.end(); ++it) {
        delete *it;
    }

    // Clear the children list
    _children.clear();

    // Make the transformation into an identity transformation
    _transform.identity();

    // Null the pen and brush
    _pen   = Pen  ();
    _brush = Brush();
}

void SGNode::draw(ImagePainter2& painter, const TransformT* transform_)
{
    // Prepare the transformation object
    TransformT transform = transform_ ? *transform_ : TransformT();

    // Prepare the stack
    std::stack<TraversalStack> nStack;
    nStack.push(TraversalStack(this, painter.pen(), painter.brush(), transform));

    // Loop while the stack is not empty
    while(!nStack.empty()) {
        // Get the current stack element
        TraversalStack& eCur = nStack.top();
        // If the flag is not set, then this is the "before" phase
        if(!eCur.after) {
            // Update the active transform object
            transform = eCur.node->_transform * transform;
            // Set this node's pen and/or brush to the painter as needed
            if(!eCur.node->_pen  .isNull()) painter.setPen  (eCur.node->_pen  );
            if(!eCur.node->_brush.isNull()) painter.setBrush(eCur.node->_brush);
            // Draw this node
            eCur.node->drawImpl(painter, transform);
            // Process the children of this node
            for(Children::const_reverse_iterator it = eCur.node->_children.rbegin(); it != eCur.node->_children.rend(); ++it) {
                // Only store to stack if it is not hidden
                const SGNode* child = *it;
                if(child->_rm == RenderNone) continue;
                nStack.push(TraversalStack(child, painter.pen(), painter.brush(), transform));
            }
            // Set the flag so that the next time this element is visited again,
            // the "after" phase will be processed instead
            eCur.after = true;
        }
        // If the flag is set, then this is the "after" phase
        else {
            // Restore the painter's original pen and/or brush as needed
            if(!eCur.node->_pen  .isNull()) painter.setPen  (eCur.pen  );
            if(!eCur.node->_brush.isNull()) painter.setBrush(eCur.brush);
            // Restore the transform object
            transform = eCur.transform;
            // Pop the stack element
            nStack.pop();
        }
    }
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

void SGNodePath::drawImpl(ImagePainter2& painter, const TransformT& transform) const
{
    // Return if the path is null
    if(_path.isNull()) return;

    // Generate points
    std::vector<PointF> pointsF;
    _path.generatePoints(pointsF, _smoothness);

    // TransformT points
    transform.transformPoints(pointsF.data(), pointsF.size());

    // Draw based on the mode
    const RenderMode rm = renderMode();
    switch(rm) {
        case RenderFill            : painter.fillPolygon (pointsF.data(), pointsF.size()       ); break;
        case RenderStroke          : painter.drawPolyline(pointsF.data(), pointsF.size(), false); break;
        case RenderStrokeAutoClose : painter.drawPolyline(pointsF.data(), pointsF.size(), true ); break;
        default                    :                                                              break;
    }
}


// ======================================================================================
// ===== SGNodeLine Class ===============================================================
// ======================================================================================
SGNodeLine::~SGNodeLine()
{}

void SGNodeLine::drawImpl(ImagePainter2& painter, const TransformT& transform) const
{
    // TransformT the coordinates
    PointF f, t;

    transform.transformPoint(f, _from);
    transform.transformPoint(t, _to  );

    // Draw the line based on the mode
    switch(renderMode()) {
        case RenderFill            : /* Fallthrough */
        case RenderStroke          : /* Fallthrough */
        case RenderStrokeAutoClose : painter.drawLine(f, t); break;
        default                    :                         break;
    }
}


} // namespace
} // namespace
