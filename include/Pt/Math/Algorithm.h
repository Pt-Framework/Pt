/*
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef PT_MATH_ALGORITH_H
#define PT_MATH_ALGORITH_H

#include <iostream>

namespace Pt {
namespace Math {


namespace {

/** @brief Relative position

    The quadrant code is a bitmask type indicating if the
    object is above, below, left or right of the rectangle.
*/
enum QuadrantCode
{
    Top    = 1 << 0, 
    Bottom = 1 << 1, 
    Right  = 1 << 2, 
    Left   = 1 << 3 
};

/** @brief Determines quadrant code

    Determines the quadrant code of x,y within the rectangle
    xmin, xmax, ymin, ymax. The quadrant code is a bit-mask type
    indicating if the point is above, below, left or right of
    the rectangle.
    
    Please note that the underlying coordinate system has its
    origin in the left-lower corner. This means that bigger y
    values are above smaller y values.
*/
template <typename T, typename U>
int quadrant( T x, T y, U xmin, U xmax, U ymin, U ymax )
{
    int code = 0;

    if( y > ymax)
    {
      code |= Top;
    }
    else if( y < ymin )
    {
      code |= Bottom;
    }

    if( x > xmax )
    {
      code |= Right;
    }
    else if( x < xmin )
    {
      code |= Left;
    }
    return code;
}

}//namespace




/** @brief Returns true if intersect

    Returns true if the line x1,y1 and x2,y2 intersects with the
    rectangle xmin, xmax, ymin, ymax.
*/
template<typename T, typename U>
bool intersect(T x1, T y1, T x2, T y2, U xmin, U xmax, U ymin, U ymax)
{
    int q1 = quadrant( x1, y1, xmin, xmax, ymin, ymax);
    int q2 = quadrant( x2, y2, xmin, xmax, ymin, ymax);
    
    // if both of the end points are above, below, 
    // left or right of the rectangle
    
    if(q1 & q2)
    {   
        return false;
    }

    const int LR = Left | Right;
    const int TB = Top | Bottom;

    // check strict horizontal and vertical crossing
    if((!(q1 & LR) && !(q2 & LR)) ||
       (!(q1 & TB) && !(q2 & TB)))
    {
        return true;
    }
    
    // line intersects rect if it intersects with the top
    // or bottom line. Previous checks exclude horizontal
    // and vertical crossing
    double m = double(x2-x1) / double(y2-y1); // slope
    double b = y1 - (m * x1);  // intersection point from line2 at Y-axis
    double xi1 = (ymin - b) / m;
    double xi2 = (ymax - b) / m;
    return( (xi1 <= xmax || xi1 >= xmin) || (xi2 <= xmax || xi2 >= xmin));
}

} //namespace Math
} //namespace Pt


#endif //PT_MATH_ALGORITH_H
