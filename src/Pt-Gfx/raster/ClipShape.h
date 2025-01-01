/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
 * Copyright (C) 2017-2017 Aloysius Indrayanto
 *
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
#ifndef PT_GFX_CLIPSHAPE_H
#define PT_GFX_CLIPSHAPE_H

#include <vector>

#include <Pt/Gfx/Rect.h>


namespace Pt{
namespace Gfx{


template <typename T>
class BasicClipShape {
    public:
        typedef T             ValueT;
        typedef BasicPoint<T> PointT;

    public:
        inline BasicClipShape()
        {}

        static inline bool insideXRange(T v, const Rect& clippingArea)
        { return ( v >= clippingArea.left() && v <= clippingArea.right() ); }
        //{ return (size_t) ( v - clippingArea.left() ) <= (size_t) ( clippingArea.right() - clippingArea.left() ); }

        static inline bool insideYRange(T v, const Rect& clippingArea)
        { return ( v >= clippingArea.top() && v <= clippingArea.bottom() ); }
        //{ return (size_t) ( v - clippingArea.top() ) <= (size_t) ( clippingArea.bottom() - clippingArea.top() ); }

        static inline bool insideXYRange(T x, T y, const Rect& clippingArea)
        { return insideXRange(x, clippingArea) && insideYRange(y, clippingArea); }

        static inline T clipLeft(T x, const Rect& clippingArea)
        { return (clippingArea.left() > x) ? clippingArea.left() : x; }

        static inline T clipRight(T x, const Rect& clippingArea)
        { return (clippingArea.right() < x) ? clippingArea.right() : x; }

        static inline T clipTop(T y, const Rect& clippingArea)
        { return (clippingArea.top() > y) ? clippingArea.top() : y; }

        static inline T clipBottom(T y, const Rect& clippingArea)
        { return (clippingArea.bottom() < y) ? clippingArea.bottom() : y; }

        static inline bool clipLine(T& x0, T& y0, T& x1, T& y1, const Rect& clip);
      //static inline void clipPolyline(std::vector<PointT>& pio, const Rect& clippingArea);
        static inline void clipPolygon(std::vector<PointT>& pio, const Rect& clippingArea);

    private:
        // Used by clip line
        enum Outcode {
            CS_Inside = 0, // 0000
            CS_Left   = 1, // 0001
            CS_Right  = 2, // 0010
            CS_Bottom = 4, // 0100
            CS_Top    = 8  // 1000
        };

        // Used by clip polyline and polygon
        enum ClipMode {
            CM_Left, CM_Right, CM_Top, CM_Bottom
        };

    private:
        // Clip line
        static inline Pt::int32_t csComputeOutcode(T x, T y, const Rect& clip);

        // Clip polyline and polygon
        static inline void clipPolylineToEdge(std::vector<PointT>& out, const std::vector<PointT>& in, const Point& edge0, const Point& edge1, ClipMode cm);
        static inline void clipPolygonToEdge(std::vector<PointT>& out, const std::vector<PointT>& in, const Point& edge0, const Point& edge1, ClipMode cm);

        static inline bool inside(const PointT& p, const Point& corner, ClipMode cm);
        static inline const PointT intersect(const PointT& from, const PointT& to, const Point& edge0, const Point& edge1);
};


//
// Include the template implementation
//
#include "ClipShape.tpp"


//
// For convenience
//
typedef BasicClipShape<Pt::int32_t> ClipShapeI;
typedef BasicClipShape<Pt::ssize_t> ClipShapeZ;
typedef BasicClipShape<double>      ClipShapeF;



} // namespace
} // namespace





#endif
