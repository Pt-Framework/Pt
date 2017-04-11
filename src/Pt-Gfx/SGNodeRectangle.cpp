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

#include <Pt/Gfx/SGNodeRectangle.h>
#include <Pt/Gfx/ImagePainter2.h>


namespace Pt {
namespace Gfx {


SGNodeRectangle::~SGNodeRectangle()
{}

void SGNodeRectangle::clear()
{
    // Clear the rectangle
    _rect.set( PointT(0, 0), SizeT(0, 0) );
    _radius = 0.0f;

    // Clear the base class' data
    SGNodePath::clear();
}

void SGNodeRectangle::set(const RectT& rect, float radius)
{
    // Save the parameters
    _rect   = rect;
    _radius = ::abs(radius);

    // Clear the base class' data
    SGNodePath::clear();

    // Determine the coordinates
    const ValueT x1 = _rect.topLeft    ().x();
    const ValueT y1 = _rect.topLeft    ().y();
    const ValueT x2 = _rect.bottomRight().x();
    const ValueT y2 = _rect.bottomRight().y();

    // Create a rounded rectangle
    if(_radius > 0.0f) {
        const ValueT r = _radius;
        path().beginPath();
        path().moveTo           (        x1,     y2 - r); // CCW
        path().quadraticBezierTo(x1, y2, x1 + r, y2    );
        path().lineTo           (        x2 - r, y2    );
        path().quadraticBezierTo(x2, y2, x2    , y2 - r);
        path().lineTo           (        x2    , y1 + r);
        path().quadraticBezierTo(x2, y1, x2 - r, y1    );
        path().lineTo           (        x1 + r, y1    );
        path().quadraticBezierTo(x1, y1, x1    , y1 + r);
        path().lineTo           (        x1    , y2 - r);
        path().endPath  ();
    }

    // Create a normal rectangle
    else {
        path().beginPath();
        path().moveTo   (x1, y2); // CCW
        path().lineTo   (x2, y2);
        path().lineTo   (x2, y1);
        path().lineTo   (x1, y1);
        path().lineTo   (x1, y2);
        path().endPath  ();
    }
}


} // namespace
} // namespace
