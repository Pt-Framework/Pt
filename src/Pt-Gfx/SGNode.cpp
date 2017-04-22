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

#include <stack>

#include <Pt/Gfx/SGNode.h>
#include <Pt/Gfx/ImagePainter2.h>


namespace Pt {
namespace Gfx {


SGNode::BasicExtendedData::BasicExtendedData()
{}

SGNode::BasicExtendedData::~BasicExtendedData()
{}


SGNode::~SGNode()
{
    // Delete all the child objects
    for(Children::iterator it = _nodeData->children.begin(); it != _nodeData->children.end(); ++it) {
        delete *it;
    }

    // Delete the extended data (if any)
    delete _extData;
}

void SGNode::clear()
{
    // Delete all the child objects
    for(Children::iterator it = _nodeData->children.begin(); it != _nodeData->children.end(); ++it) {
        delete *it;
    }

    // Make the transformation into an identity transformation
    _transform.identity();

    // Clear the children list
    _nodeData->children.clear();

    // Null the pen and brush
    _nodeData->pen   = Pen  ();
    _nodeData->brush = Brush();
}

SGNode* SGNode::clone() const
{
    SGNode* sgn = cloneGraph();
    sgn->_parent = 0;

    return sgn;
}

const Pen& SGNode::pen() const
{ return _nodeData->pen; }

const Pen& SGNode::effectivePen() const
{
    if(!_nodeData->pen.isNull()) return _nodeData->pen;

    const SGNode* p = _parent;
    while(p) {
        if(!p->_nodeData->pen.isNull()) return p->_nodeData->pen;
        p = p->_parent;
    }

    return _nodeData->pen;
}

const Brush& SGNode::brush() const
{ return _nodeData->brush; }

const Brush& SGNode::effectiveBrush() const
{
    if(!_nodeData->brush.isNull()) return _nodeData->brush;

    const SGNode* p = _parent;
    while(p) {
        if(!p->_nodeData->brush.isNull()) return p->_nodeData->brush;
        p = p->_parent;
    }

    return _nodeData->brush;
}

void SGNode::draw(ImagePainter2& painter, const TransformT* transform_)
{
    // Prepare the transformation
    TransformT transform = transform_ ? *transform_ : TransformT();

    // Prepare the stack
    std::stack<TraversalStack> nStack;
    nStack.push( TraversalStack(this, painter.pen(), painter.brush(), transform) );

    // Loop while the stack is not empty
    while(!nStack.empty()) {
        // Get the current stack element
        TraversalStack& eCur = nStack.top();
        // Get the pen and brush
        const Pen&   pen   = eCur.node->pen  ();
        const Brush& brush = eCur.node->brush();
        // If the flag is not set, then this is the "before" phase
        if(!eCur.after) {
            // Update the active transform object
            transform = transform * eCur.node->_transform;
            // Set this node's pen to the painter as needed
            if(!pen.isNull()) {
                // Scale the pen width
                const size_t orgPenSize = pen.size();
                const size_t sclPenSize = Gfx::Math::zrint( transform.transformSize((float) orgPenSize) );
                const size_t newPenSize = (sclPenSize >= 1) ? sclPenSize : 1;
                // Assign a new pen with the original width
                if(orgPenSize == newPenSize) {
                    painter.setPen(pen);
                }
                // Assign a new pen with the scaled width
                else {
                    Pen newPen = pen;
                    newPen.setSize(newPenSize);
                    painter.setPen(newPen);
                }
            }
            // Set this node's brush to the painter as needed
            if(!brush.isNull()) {
                // If the brush is a texture or a 2D gradient, adjust its rotation as needed
                if(brush.isTexture() || brush.isGradient()) {
                    // ### TODO: Texture scaling ??? ###
                    // Calculate the new gradient rotation and ensure that it is within the acceptable range
                    const ValueT orgRot = brush.rotation();
                          ValueT newRot = orgRot + transform.extractRotation();
                    while(newRot < -360) newRot += 360;
                    while(newRot >  360) newRot -= 360;
                    // Assign the original brush as the painter's brush
                    if(orgRot == newRot) {
                        painter.setBrush(brush);
                    }
                    // Assign a new brush with the updated texture rotation
                    else if(brush.isTexture()) {
                        Brush newBrush = brush;
                        newBrush.setTextureRotation(newRot, eCur.node->_nodeData->trCFil, eCur.node->_nodeData->trMode);
                        painter.setBrush(newBrush);
                    }
                    // Assign a new brush with the updated gradient rotation
                    else {
                        Brush newBrush = brush;
                        newBrush.setGradientRotation(newRot);
                        painter.setBrush(newBrush);
                    }
                }
                // Other brush types
                else {
                    painter.setBrush(brush);
                }
            }
            // Draw this node
            eCur.node->drawImpl(painter, transform, _DoNotOverrideRM);
            // Process the children of this node
            for(ConstReverseIterator it = eCur.node->rbegin(); it != eCur.node->rend(); ++it) {
                // Only store to stack if it is not hidden
                const SGNode* child = *it;
                if(child->_rm == RenderNone) continue;
                nStack.push( TraversalStack(child, painter.pen(), painter.brush(), transform) );
            }
            // Set the flag so that the next time this element is visited again,
            // the "after" phase will be processed instead
            eCur.after = true;
        }
        // If the flag is set, then this is the "after" phase
        else {
            // Restore the painter's original pen and/or brush as needed
            if(!pen  .isNull()) painter.setPen  (eCur.pen  );
            if(!brush.isNull()) painter.setBrush(eCur.brush);
            // Restore the transform object
            transform = eCur.transform;
            // Pop the stack element
            nStack.pop();
        }
    }
}

// ======================================================================================

SGNode* SGNode::cloneGraph() const
{ return cloneImpl(0); }

SGNode* SGNode::cloneImpl(SGNode* newInst) const
{
    // Cloning a node means cloning the whole graph, removing any proxy

    // The new instance
    SGNode* sgn = newInst ? newInst : new SGNode();

    // Set/copy this class' data
    sgn->_parent           = _parent;
    sgn->_rm               = _rm;

    sgn->_transform        = _transform;

    sgn->_nodeData         = SmartPtr<NodeData>(new NodeData());
    sgn->_nodeDataRO       = false;

    sgn->_nodeData->pen    = _nodeData->pen;
    sgn->_nodeData->brush  = _nodeData->brush;
    sgn->_nodeData->trCFil = _nodeData->trCFil;
    sgn->_nodeData->trMode = _nodeData->trMode;

    for(Children::const_iterator it = _nodeData->children.begin(); it != _nodeData->children.end(); ++it) {
        sgn->_nodeData->children.push_back((*it)->cloneGraph());
    }

    if(_extData) sgn->_extData = _extData->clone();

    // Return the new instance
    return sgn;
}

void SGNode::drawImpl(ImagePainter2& /*painter*/, const TransformT& /*transform*/, RenderMode /*overrideRM*/) const
{
    // Does nothing
}

void SGNode::checkForCircularChain(const SGNode* parent) const
{
    // Check for circular/recursive node chain
    const SGNode* p = parent;

    while(p) {
        if(p == this) throw std::logic_error("circular/recursive node chain detected");
        p = p->_parent;
    }
}


} // namespace
} // namespace
