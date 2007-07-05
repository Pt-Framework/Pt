/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_MATH_H
#define PT_MATH_H

#include <Pt/Types.h>
#include <Pt/Math/Api.h>


namespace Pt {

    namespace Math {

        template <typename T>
        class BasicSize;

        template <typename T>
        class BasicPoint;

        template <typename T>
        class BasicPoint3D;

        template <typename T>
        class BasicRange;

        template <typename PointT>
        class BasicLine;

        template <typename PointT>
        class BasicLineSegment;

        template <typename PointT>
        class BasicPolygon;

        template <typename T, size_t rowDim, size_t colDim>
        class BasicMatrix;

        template <typename T>
        class BasicVector2d;

        template <typename T>
        class BasicVector3d;

        typedef BasicSize<Pt::size_t>      Size;
        typedef BasicSize<double>          SizeF;

        typedef BasicPoint<Pt::ssize_t>    Point;
        typedef BasicPoint<double>         PointF;

        typedef BasicPoint3D<Pt::ssize_t>    Point3D;
        typedef BasicPoint3D<double>         Point3DF;

        typedef BasicRange<Pt::ssize_t>          Range;
        typedef BasicRange<double>               RangeF;

        typedef BasicLine<Pt::ssize_t>           Line;
        typedef BasicLine<double>                LineF;

        typedef BasicLineSegment<Pt::ssize_t>    LineSegment;
        typedef BasicLineSegment<double>         LineSegmentF;

        typedef BasicPolygon<Pt::ssize_t>        Polygon;
        typedef BasicPolygon<double>             PolygonF;

        typedef BasicVector2d<Pt::ssize_t>       Vector2d;
        typedef BasicVector2d<double>            Vector2dF;

        typedef BasicVector3d<Pt::ssize_t>       Vector3d;
        typedef BasicVector3d<double>            Vector3dF;

        typedef BasicMatrix<Pt::ssize_t, 3, 3>   Matrix3x3;
        typedef BasicMatrix<double, 3, 3>        Matrix3x3F;
        typedef BasicMatrix<Pt::ssize_t, 4, 4>   Matrix4x4;
        typedef BasicMatrix<double, 4, 4>        Matrix4x4F;


        template <typename PointT, typename SizeT>
        class BasicRect;

        typedef BasicRect<Pt::ssize_t, Pt::size_t>  Rect;
        typedef BasicRect<double, double>           RectF;

    } // namespace Math

} // namespace Pt

#endif

