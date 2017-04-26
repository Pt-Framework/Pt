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

#include <Pt/Gfx/SGNodeLine.h>
#include <Pt/Gfx/ImagePainter2.h>


namespace Pt {
namespace Gfx {


SGNodeLine::~SGNodeLine()
{}

void SGNodeLine::clear()
{
    // Clear the line
    _from.set(0, 0);
    _to  .set(0, 0);

    // Clear the base class' data
    SGNode::clear();
}

// ======================================================================================

SGNode* SGNodeLine::cloneImpl(SGNode* newInst) const
{
    // The new instance
    SGNodeLine* sgn;

    // Convert and check the object type
    if(newInst) {
        sgn = dynamic_cast<SGNodeLine*>(newInst);
        if(!sgn) throw std::runtime_error("SGNodeLine: invalid clone operation");
    }
    // Ceate a new object
    else {
        sgn = new SGNodeLine();
    }

    // Call the base class implementation so that it can copy its data
    SGNode::cloneImpl(sgn);

    // Copy this class' data
    sgn->_from = _from;
    sgn->_to   = _to;

    // Return the new instance
    return sgn;
}

void SGNodeLine::drawImpl(ImagePainter2& painter, const TransformT& transform, RenderMode overrideRM) const
{
    // TransformT the coordinates
    PointT from, to;

    transform.transformPoint(from, _from);
    transform.transformPoint(to  , _to  );

    // Draw based on the effective mode
    switch(effectiveRenderMode(overrideRM)) {
        case RenderFill            : /* Fallthrough */
        case RenderStroke          : /* Fallthrough */
        case RenderStrokeAutoClose : painter.drawLine(from, to); break;
        default                    :                             break;
    }
}


} // namespace
} // namespace
