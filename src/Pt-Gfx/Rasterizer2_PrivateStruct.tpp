/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/


//
// Scanline element
//

template <typename T>
struct Rasterizer2::ScanlineElement {
    T from;
    T to;

    inline ScanlineElement(T from_ = -1, T to_ = -1)
    : from(from_), to(to_)
    {}

    inline bool isNull() const
    { return from == -1 && to == -1; }
};


//
// Filled-arc information structure (used for drawing filled arcs)
//

struct Rasterizer2::FilledArcInfo {
    bool        antiAlias;    // A flag that indicate if the arc will be anti-aliased

    float       degBegin;     // Begin angle
    float       degEnd;       // End angle

    Pt::int32_t minX, minY;   // Top-left coordinate of the arc
    Pt::int32_t ctrX, ctrY;   // Center coordinate of the arc
    Pt::int32_t radX, radY;   // Radius of the arc
    Pt::int32_t radX2, radY2; // Squared radius of the arc
    float       xyRat;        // Ratio of the X and Y radius

    Pt::int32_t x1, y1;       // Coordinate of the begin point
    Pt::int32_t x2, y2;       // Coordinate of the end point

    Pt::int32_t quartersX;    // The number of quarter points in the X direction
    Pt::int32_t quartersY;    // The number of quarter points in the Y direction
};


//
// Xiaolin Wu's anti-aliased line data structure (used for drawing filled arcs)
//

struct Rasterizer2::ArcXWLineData {
    // --- Point data sub-structure ---
    struct XWPoint {
        Pt::int32_t x;
        Pt::uint8_t a1, a2;

        inline XWPoint(Pt::int32_t x_ = -1, Pt::uint8_t a1_ = 0, Pt::uint8_t a2_ = 0)
        : x(x_), a1(a1_), a2(a2_)
        {}

        inline bool isNull() const
        { return x == -1 && a1 == 0 && a2 == 0; }
    };

    typedef std::vector< std::vector<XWPoint> > XWPoints; // The vector index is the Y coordinate

    // --- Data ---
    XWPoints points;  // The line's points
    bool     steep;   // If "true"  then the a2 belongs to (x + 1, y)
                      // If "false" then the a2 belongs to (x, y + 1)
    bool     swapDir; // A flag that indicates if the line direction is swapped

    bool faceL; // The direction that the line is facing to
    bool faceR; // ---
    bool faceT; // ---
    bool faceB; // ---

    // The line's coordinates
    Pt::int32_t x1, y1, x2, y2;
    Pt::int32_t minY, maxY;

    inline bool insideYRange(Pt::int32_t y) const
    { return (y >= minY) && (y <= maxY); }
};


//
// ValueT converter
//

template <>
struct Rasterizer2::CnvValueT<ssize_t> {
    typedef Pt::int32_t T;
};

template <>
struct Rasterizer2::CnvValueT<double > {
    typedef float T;
};
