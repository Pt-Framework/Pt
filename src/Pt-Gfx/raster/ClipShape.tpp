/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


// ======================================================================================
// ===== Clip Line ======================================================================
// ======================================================================================

// Cohen–Sutherland clipping algorithm clips a line from (x0, y0) to (x1, y1)
// against a clip rectangle (https://en.wikipedia.org/wiki/Cohen–Sutherland_algorithm)
template <typename T>
inline bool BasicClipShape<T>::clipLine(T& x0, T& y0, T& x1, T& y1, const Rect& clip)
{
    // Compute the initial outcodes for the endpoints
    Pt::int32_t outcode0 = csComputeOutcode(x0, y0, clip);
    Pt::int32_t outcode1 = csComputeOutcode(x1, y1, clip);

    bool accept = false;
    T    x      = 0;
    T    y      = 0;

    while(true) {
        // Both endpoints are inside the clip region
        if(!(outcode0 | outcode1)) {
            accept = true;
            break;
        }
        // Both endpoints are outside the clip region
        else if (outcode0 & outcode1) {
            break;
        }
        // At least one endpoint is outside the clip rectangle
        else {
            // Pick the one that is outside the clip rectangle
            // and find the intersection point using:
            //     y = y0 + (x - x0) * slope
            //     x = x0 + (y - y0) * slope
            Pt::int32_t outcodeOut = outcode0 ? outcode0 : outcode1;
            // Endpoint is above the clip rectangle
            if(outcodeOut & CS_Top) {
                x = x0 + (x1 - x0) * (clip.top   () - y0) / (y1 - y0);
                y = clip.top();
            }
            // Endpoint is below the clip rectangle
            else if(outcodeOut & CS_Bottom) {
                x = x0 + (x1 - x0) * (clip.bottom() - y0) / (y1 - y0);
                y = clip.bottom();
            }
            // Endpoint is to the right of clip rectangle
            else if(outcodeOut & CS_Right) {
                y = y0 + (y1 - y0) * (clip.right () - x0) / (x1 - x0);
                x = clip.right();
            }
            // Endpoint is to the left of clip rectangle
            else if(outcodeOut & CS_Left) {
                y = y0 + (y1 - y0) * (clip.left  () - x0) / (x1 - x0);
                x = clip.left();
            }
            // Replace the endpoint which is outside the clip rectangle
            // with the intersection point and run the next pass
            if(outcodeOut == outcode0) {
                x0 = x;
                y0 = y;
                outcode0 = csComputeOutcode(x0, y0, clip);
            }
            else {
                x1 = x;
                y1 = y;
                outcode1 = csComputeOutcode(x1, y1, clip);
            }
        }
    }

    return accept;
}

// Compute the bit code for a point (x, y) using the clip rectangle
template <typename T>
inline Pt::int32_t BasicClipShape<T>::csComputeOutcode(T x, T y, const Rect& clip)
{
    Pt::int32_t code = CS_Inside; // Initialised as being inside of the clip region

         if(x < clip.left  ()) code |= CS_Left;   // to the left of clip region
    else if(x > clip.right ()) code |= CS_Right;  // to the right of clip region
         if(y < clip.top   ()) code |= CS_Top;    // above the clip region
    else if(y > clip.bottom()) code |= CS_Bottom; // below the clip region

    return code;
}


/*
// ======================================================================================
// ===== Clip Polyline ==================================================================
// ======================================================================================

template <typename T>
inline void BasicClipShape<T>::clipPolyline(std::vector<PointT>& pio, const Rect& clippingArea)
{
    return;
    // If the clipping area is null or there is too few elements, simply clear the vector
    if(clippingArea.isNull() || pio.size() < 2) {
        pio.clear();
        return;
    }

    // Perform clipping
    std::vector<PointT> tmp;

    clipPolylineToEdge(tmp, pio, clippingArea.topLeft   (), clippingArea.bottomLeft  (), CM_Left  );
    clipPolylineToEdge(pio, tmp, clippingArea.topRight  (), clippingArea.bottomRight (), CM_Right );
    clipPolylineToEdge(tmp, pio, clippingArea.topLeft   (), clippingArea.topRight    (), CM_Top   );
    clipPolylineToEdge(pio, tmp, clippingArea.bottomLeft(), clippingArea.bottomRight (), CM_Bottom);
}

template <typename T>
inline void BasicClipShape<T>::clipPolylineToEdge(std::vector<PointT>& out, const std::vector<PointT>& in, const Point& edge0, const Point& edge1, ClipMode cm)
{
    out.clear();
    if(in.empty()) return;

    const size_t size1 = in.size() - 1;

    for(size_t i = 0; i < size1; ++i) {
        const PointT& s       = in[i    ];
        const PointT& p       = in[i + 1];
        const bool    sInside = inside(s, edge0, cm);
        const bool    pInside = inside(p, edge0, cm);
        if(sInside && pInside) {
            out.push_back(s);
        }
        else if(!sInside && pInside) {
           out.push_back(intersect(s, p, edge0, edge1));
        }
        else if(sInside && !pInside) {
            out.push_back(s);
        }
        else if(!sInside && !pInside && i) {
            const PointT& a = in[i - 1];
            const PointT& b = in[i    ];
            if(inside(a, edge0, cm)) out.push_back(intersect(a, b, edge0, edge1));
        }
    }

    const PointT& s       = in[size1    ];
    const PointT& p       = in[size1 - 1];
    const bool    sInside = inside(s, edge0, cm);
    const bool    pInside = inside(p, edge0, cm);
         if(sInside) out.push_back(s);
    else if(pInside) out.push_back(intersect(s, p, edge0, edge1));
}
*/


// ======================================================================================
// ===== Clip Polygon ===================================================================
// ======================================================================================

template <typename T>
inline void BasicClipShape<T>::clipPolygon(std::vector<PointT>& pio, const Rect& clippingArea)
{
    return;
    // If the clipping area is null or there is too few elements, simply clear the vector
    if(clippingArea.isNull() || pio.size() < 3) {
        pio.clear();
        return;
    }

    // Perform clipping
    std::vector<PointT> tmp;

    clipPolygonToEdge(tmp, pio, clippingArea.topLeft   (), clippingArea.bottomLeft  (), CM_Left  );
    clipPolygonToEdge(pio, tmp, clippingArea.topRight  (), clippingArea.bottomRight (), CM_Right );
    clipPolygonToEdge(tmp, pio, clippingArea.topLeft   (), clippingArea.topRight    (), CM_Top   );
    clipPolygonToEdge(pio, tmp, clippingArea.bottomLeft(), clippingArea.bottomRight (), CM_Bottom);

    // Shift around the elements so that their original order are restored
    if(pio.size() == 3) {
        tmp.clear();
        tmp.push_back(pio.back());
        for(size_t i = 0; i < pio.size() - 1; ++i) tmp.push_back(pio[i]);
        pio = tmp;
    }
    else if(pio.size() > 4) {
        tmp.clear();
        for(size_t i = pio.size() - 4; i < pio.size(); ++i) tmp.push_back(pio[i]);
        for(size_t i = 0; i < pio.size() - 4; ++i) tmp.push_back(pio[i]);
        pio = tmp;
    }
}

template <typename T>
inline void BasicClipShape<T>::clipPolygonToEdge(std::vector<PointT>& out, const std::vector<PointT>& in, const Point& edge0, const Point& edge1, ClipMode cm)
{
    out.clear();
    if(in.empty()) return;

    const size_t size1 = in.size() - 1;

    for(size_t i = 0; i <= size1; ++i) {
        const PointT& s       = in[i                         ];
        const PointT& p       = in[(i == size1) ? 0 : (i + 1)];
        const bool    sInside = inside(s, edge0, cm);
        const bool    pInside = inside(p, edge0, cm);
        if(sInside && pInside) {
            out.push_back(p);
        }
        else if(sInside && !pInside) {
            out.push_back(intersect(s, p, edge0, edge1));
        }
        else if(!sInside && pInside) {
            out.push_back(intersect(s, p, edge0, edge1));
            out.push_back(p);
        }
    }
}


// ======================================================================================
// ===== Clip Polyline and Polygon ======================================================
// ======================================================================================

template <typename T>
inline bool BasicClipShape<T>::inside(const PointT& p, const Point& corner, ClipMode cm)
{
    switch(cm) {
        case CM_Left   : return p.x() >= (T) corner.x();
        case CM_Right  : return p.x() <= (T) corner.x();
        case CM_Top    : return p.y() >= (T) corner.y();
        case CM_Bottom : return p.y() <= (T) corner.y();
    }

    return false;
}

template <typename T>
inline const typename BasicClipShape<T>::PointT BasicClipShape<T>::intersect(const PointT& from, const PointT& to, const Point& edge0, const Point& edge1)
{
    PointT p;

    // Horizontal clip edge
    if(edge0.y() == edge1.y()) {
        // Abnormal case - the polygon edge is parallel with the clip edge
        if(to.y() == from.y()) {
                 if(edge0.y() == to  .y()) p.set( to  .x(),   to.y() );
            else if(edge0.y() == from.y()) p.set( from.x(), from.y() );
        }
        // Normal case
        else {
            const T dx =     to   .x() - from.x();
            const T dy =     to   .y() - from.y();
            const T de = (T) edge0.y() - from.y();
            p.set( from.x() + dx * de / dy, edge0.y() );
        }
    }

    // Vertical clip edge
    if(edge0.x() == edge1.x()) {
        // Abnormal case - the polygon edge is parallel with the clip edge
        if(to.x() == from.x()) {
                 if(to  .x() == edge0.x())  p.set( to  .x(),  to  .y() );
            else if(from.x() == edge0.x())  p.set( from.x(),  from.y() );
        }
        // Normal case
        else {
            const T dx =     to   .x() - from.x();
            const T dy =     to   .y() - from.y();
            const T de = (T) edge0.x() - from.x();
            p.set( edge0.x(), from.y() + dy * de / dx );
        }
    }

    return p;
}
