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

#include <Pt/Gfx/SGNodeArc.h>
#include <Pt/Gfx/ImagePainter2.h>


namespace Pt {
namespace Gfx {


SGNodeArc::~SGNodeArc()
{}

void SGNodeArc::clear()
{
    // Clear the arc
    _center.set(0, 0);
    _radius.set(0, 0);
    _degBegin = 0;
    _degEnd   = 0;
    _arcMode  = ArcMode::Open;

    // Clear the base class' data
    SGNodePath::clear();
}

void SGNodeArc::set(const PointT& center, const SizeT& radius, ValueT degBegin, ValueT degEnd, const ArcMode& arcMode)
{
    // Save the parameters
    _center   = center;
    _radius   = radius;
    _degBegin = degBegin;
    _degEnd   = degEnd;
    _arcMode  = arcMode;

    // Clear the base class' data
    SGNodePath::clear();

    // Create an arc
    path().beginPath();
    path().moveTo   (_center.x    (), _center.y     ()                              );
    path().putArc   (_radius.width(), _radius.height(), _degBegin, _degEnd, _arcMode);
    path().endPath  ();
}

// ======================================================================================

SGNode* SGNodeArc::cloneImpl(SGNode* newInst) const
{
    // The new instance
    SGNodeArc* sgn;

    // Convert and check the object type
    if(newInst) {
        sgn = dynamic_cast<SGNodeArc*>(newInst);
        if(!sgn) throw std::runtime_error("SGNodeArc: invalid clone operation");
    }
    // Ceate a new object
    else {
        sgn = new SGNodeArc();
    }

    // Call the base class implementation so that it can copy its data
    SGNodePath::cloneImpl(sgn);

    // Copy this class' data
    sgn->_center   = _center;
    sgn->_radius   = _radius;
    sgn->_degBegin = _degBegin;
    sgn->_degEnd   = _degEnd;
    sgn->_arcMode  = _arcMode;

    // Return the new instance
    return sgn;
}


} // namespace
} // namespace
