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

#include <Pt/Gfx/SGNodeEllipse.h>
#include <Pt/Gfx/ImagePainter2.h>


namespace Pt {
namespace Gfx {


SGNodeEllipse::~SGNodeEllipse()
{}

void SGNodeEllipse::clear()
{
    // Clear the ellipse
    _topLeft.set(0, 0);
    _size   .set(0, 0);

    // Clear the base class' data
    SGNodePath::clear();
}

void SGNodeEllipse::set(const PointT& topLeft, const SizeT& size)
{
    // Save the parameters
    _topLeft = topLeft;
    _size    = size;

    // Clear the base class' data
    SGNodePath::clear();

    // Determine the coordinates and radius
    const ValueT xl = _topLeft.x();
    const ValueT xr = xl + _size.width ();

    const ValueT yr = _size.height() * 0.5f;
    const ValueT ym = _topLeft.y() + yr;

    // Create an ellipse
    path().beginPath();
    path().moveTo   (xl, ym    ); // CCW
    path().arcTo    (xr, ym, yr);
    path().arcTo    (xl, ym, yr);
    path().endPath  ();
}


} // namespace
} // namespace
