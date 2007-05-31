#ifndef PT_MATH_ALGORITH_H
#define PT_MATH_ALGORITH_H


namespace Pt {
namespace Math {


namespace {

/** @brief Relative position

    The quadrant code is a bitmask type indicating if the
    object is above, below, left or right of the rectangle.
*/
enum QuadrantCode
{
    Top=0x1,
    Bottom=0x2,
    Right=0x4,
    Left=0x8
};

/** @brief Determines quadrant code

    Determines the quadrant code of x,y within the rectangle
    xmin, xmax, ymin, ymax. The quadrant code is a bitmask type
    indicating if the point is above, below, left or right of
    the rectangle.
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


template <typename T, typename U>
bool horizontalIntersect( T x1, T y1, T x2, T y2, U xmin, U xmax, U ymin)
{
    U m = (x2-x1) / (y2-y1); // slope
    U b = y1 - (m * x1);  // intersection point from line2 at Y-axis
    T xi = (ymin - b) / m;

    if( xi > xmax || xi < xmin)
    {
        return false;
    }
    return true;
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

    // if both of the end points are in the same side, outside of the rect
    if(q1 & q2)
    {
        return false;
    }

    const int LR = Left | Right;
    const int TB = Top | Bottom;

    // if neither of the points is left or right of the rect it intersects
    if((!(q1 & LR) && !(q2 & LR)) ||
       (!(q1 & TB) && !(q2 & TB)))
    {
        return true;
    }

    bool retValue = false;
    retValue = horizontalIntersect(x1, y1, x2, y2, xmin, xmax, ymin);
    if(retValue)
    {
        return true;
    }
    retValue = horizontalIntersect(x1, y1, x2, y2, xmin, xmax, ymax);
    if(retValue)
    {
        return true;
    }
    return false;
}


}//namespace Pt{

}//namespace Math{


#endif //PT_MATH_ALGORITH_H
