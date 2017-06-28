/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2017-2017 Marc Boris Duerner

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

#ifndef PT_GFX_LINERENDERER_H
#define PT_GFX_LINERENDERER_H
 
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Path.h> // Polygon

namespace Pt {

namespace Gfx { 

struct PatternState 
{
    std::vector<Polygon>& dstPolygons;  // Destination vector
    //size_t                dstPStart;  // Start index of the previous polygon in the above vector
    //size_t                dstPCount;  // The number of points of the previous polygon in the above vector
    //size_t                dstPCount0; // The number of points of the first polygon in the above vector
                          
    const PointF*         srcPoints;  // Source points
    size_t                srcCount;   // The number of source points
                          
    float                 cellSize;   // Cell size
    float                 patSegLen;  // Length of the currently processed "pattern" segment
                          
    size_t                idx1;       // Index to the first point which is currently being processed;
                                      // the index to the second point is always (idx1 + 1)
                          
    float                 px, py;     // Current interpolation coordinate (in-between the two points)
    float                 ex, ey;     // Current end coordinate (coordinate of the the second point)
    float                 uvx, uvy;   // Unit vector from the first point to the second point
    float                 cvx, cvy;   // Cell vector from the first point to the second point
    float                 remLen;     // Remaining length between the two points that has not been "consumed" by the "pattern" segment(s)
                          
    std::vector<PointF>   gather;     // Gathered polygon points
    float                 gatherLen;  // Length of the gathered points

    PatternState(std::vector<Polygon>& polygons, 
                 const PointF* src, size_t pointCount, size_t penSize)
    : dstPolygons(polygons)
    //, dstPStart(0)
    //, dstPCount(0)
    //, dstPCount0(0)
    , srcPoints(src)
    , srcCount(pointCount), cellSize(penSize * 0.25f)
    , idx1(0)
    , remLen(-1.0f)
    , gatherLen(0.0f)
    {}
};

class LineRenderer
{
    public:
        LineRenderer();

        void renderWidePolyline(std::vector<Polygon>& polygons,
                                const PointF* points, const std::size_t n,
                                const Pen& pen);

        void setPattern(const Pen::Style& style);

    private:
        void renderSolidClosedWidePolyline(std::vector<Polygon>& polygons, 
                                           const PointF* basePtr, size_t curPCnt,
                                           const Pen& pen);

        void renderSolidOpenWidePolyline(std::vector<Polygon>& polygons, 
                                         const PointF* basePtr, size_t curPCnt,
                                         const Pen& pen);

        void renderDashedWidePolyLine(std::vector<Polygon>& polygons, 
                                      const PointF* src, size_t pointCount,
                                      const Pen& pen);

        void renderDashedWidePolyLine();


        bool sagPolygonPoints(PatternState& state, bool draw, const Pen& pen);

        void sagGenerateSimpleLineSegment(PatternState& state, 
                                          float x1, float y1, 
                                          float x2, float y2,
                                          const Pen& pen);

        bool satDetectPolygonCollision(const PointF* poly1, size_t poly1Count, 
                                       const PointF* poly2, size_t poly2Count);

        void satDPIProjMinMax(float& min, float& max, 
                              const PointF* points, size_t pointCount, 
                              float px, float py);

        void sagGeneratePolyLineSegment(PatternState& state, const Pen& pen);

        void renderSolidLineSegment(std::vector<PointF>& dst, 
                                    float x1, float y1, float x2, float y2, 
                                    const Pen& pen, bool openingCap, bool closingCap);

        bool joinClosedWidePolyline(std::vector<PointF>& outer, 
                                    std::vector<PointF>& inner, 
                                    const std::vector<PointF>& segment, 
                                    const PointF& origMeetingPoint, const Pen& pen,
                                    bool isFirst, bool isLast, bool inSameSegment);

        bool joinOpenWidePolyline(std::vector<PointF>& polygon, 
                                  std::vector<PointF>& inner, 
                                  const std::vector<PointF>& segment, 
                                  const PointF& origMeetingPoint, const Pen& pen,
                                  bool inSameSegment);

        void renderLineButtCap(std::vector<PointF>& dst, 
                              float x, float y, float nx, float ny);

        void renderLineSquareCap(std::vector<PointF>& dst, 
                                float x, float y, float dx, float dy, 
                                float nx, float ny);

        void renderLineRoundCap(std::vector<PointF>& dst, 
                                float x, float y, float wh, 
                                float dx, float dy, float nx, float ny);

        void renderLineTriangularOutCap(std::vector<PointF>& dst, 
                                        float x, float y, float dx, float dy, 
                                        float nx, float ny);

        void renderLineTriangularInCap(std::vector<PointF>& dst, 
                                       float x, float y, float dx, float dy, 
                                       float nx, float ny);

        void renderLineRoundHoleCap(std::vector<PointF>& dst, 
                                    float x, float y, float wh, 
                                    float dx, float dy, float nx, float ny);

        void renderLineArrow1Cap(std::vector<PointF>& dst, 
                                 float x, float y, float dx, float dy, 
                                 float nx, float ny);

        void renderLineArrow2Cap(std::vector<PointF>& dst, 
                                 float x, float y, float dx, float dy, 
                                 float nx, float ny);

        void renderQuadraticBezierPoints(std::vector<PointF>& dst, 
                                           float x1, float y1, 
                                           float x2, float y2, 
                                           float x3, float y3, 
                                           Pt::int32_t nSegs);

        void calculateLineParams(float& wh, float& dx, float& dy, 
                                 float& nx, float& ny, float x1, float y1, 
                                 float x2, float y2, size_t w);

        bool intersectLine(bool& inLine, PointF& intersect, 
                           const PointF& line1a, const PointF& line1b, 
                           const PointF& line2a, const PointF& line2b, size_t penSize);

    private:
        static const int PatternCells = 64;
        Pt::uint8_t _patternBufferMP[PatternCells]; 
};

} //namespace

} //namespace

#endif
