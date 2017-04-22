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

#include <Pt/Gfx/SGNodeProxy.h>


namespace Pt {
namespace Gfx {


SGNodeProxy::~SGNodeProxy()
{
    delete _penOverride;
    delete _brushOverride;
}

void SGNodeProxy::setPenOverride(Pen* pen)
{
    delete _penOverride;
    _penOverride = pen;
}

void SGNodeProxy::setBrushOverride(Brush* brush)
{
    delete _brushOverride;
    _brushOverride = brush;
}

const Pen& SGNodeProxy::pen() const
{
    if(_penOverride) return *_penOverride;
    return SGNode::pen();
}

const Pen& SGNodeProxy::effectivePen() const
{
    if(_penOverride) return *_penOverride;
    return SGNode::effectivePen();
}

const Brush& SGNodeProxy::brush() const
{
    if(_brushOverride) return *_brushOverride;
    return SGNode::brush();
}

const Brush& SGNodeProxy::effectiveBrush() const
{
    if(_brushOverride) return *_brushOverride;
    return SGNode::effectiveBrush();
}

// ======================================================================================

SGNode* SGNodeProxy::cloneImpl(SGNode* newInst) const
{
    // Cloning a proxy node means cloning the whole target graph, removing the proxy
    SGNode* sgn = _target.cloneGraph();

    // Assign the pen and brush as needed
    if(_penOverride  ) sgn->setPen  (*_penOverride  );
    if(_brushOverride) sgn->setBrush(*_brushOverride);

    // Have we given an instance?
    if(newInst) {
        // Check the object type
        if(!dynamic_cast<SGNodeProxy*>(newInst))
            throw std::runtime_error("SGNodeProxy: invalid clone operation");
        // Call the base class implementation so that it can copy its data
        SGNode::cloneImpl(newInst);
        // Add the newly created instance as the child of the given instance
        newInst->addChild(sgn);
        // Assign the given instance as the newly created instance
        sgn = newInst;
    }

    // Return the new instance
    return sgn;
}

void SGNodeProxy::drawImpl(ImagePainter2& painter, const TransformT& transform, RenderMode overrideRM) const
{
    // Determine the render mode
    SGNode::RenderMode rm = _rm;

    if(rm == SGNode::RenderInherit) {
        rm = _target.renderMode();
        if(rm == SGNode::RenderNone) rm = SGNode::RenderInherit;
    }

    // Call the target's implementation usig this node's render mode
    _target.drawImpl(painter, transform * _target._transform, rm);
}

void SGNodeProxy::checkForCircularChain(const SGNode* parent) const
{
    // Call the base implementation
    SGNode::checkForCircularChain(parent);

    // Check for circular/recursive proxy chain
    const SGNode* p = parent;

    while(p) {
        if(p == &_target) throw std::logic_error("circular/recursive proxy chain detected");
        p = p->_parent;
    }
}

void SGNodeProxy::checkForProxyInProxy(const SGNode& target)
{
    const SGNodeProxy* sgn = dynamic_cast<const SGNodeProxy*>(&target);
    if(sgn) throw std::logic_error("making a proxy that contains another proxy is not supported");

    for(Children::const_iterator it = target.begin(); it != target.end(); ++it) {
        checkForProxyInProxy(**it);
    }
}


} // namespace
} // namespace
