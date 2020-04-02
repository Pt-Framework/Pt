/* Copyright (C) 2017-2020 Aloysius Indrayanto
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

#ifndef PT_GFX_POLYGONIZER_H
#define PT_GFX_POLYGONIZER_H

#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Path.h> // Polygon


namespace Pt {

namespace Gfx {


class ArcMode;
struct PatternState;


class Polygonizer
{
    public:
        static std::vector<Pt::uint8_t> dashPatternDot;
        static std::vector<Pt::uint8_t> dashPatternDash;

    public:
        Polygonizer();

        void setPattern(const Pen::Style& style, const Pen::CapStyle& cap,
                        const std::vector<Pt::uint8_t>& userDashPattern, std::size_t penSize);

        void renderRoundedRect(std::vector<Polygon>& polygons,
                               const RectF& rect, float radius,
                               const Pen& pen);

        void fillRoundedRect(std::vector<PointF>& points,
                             const RectF& rect, float radius);


        void renderEllipse(std::vector<Polygon>& polygons,
                           const PointF& topLeft, const SizeF& size,
                           const Pen& pen);

        void renderArc(std::vector<Polygon>& dst, const ArcMode& mode,
                       const PointF& topLeft, const SizeF& size,
                       float degBegin, float degEnd, const Pen& pen);

        void renderWidePolyline(std::vector<Polygon>& polygons,
                                const PointF* points, const std::size_t n,
                                const Pen& pen, bool nonZeroFillingRule);

    private:
        void renderRoundedRectPoints(std::vector<PointF>& dst,
                                     const RectF& rect, float radius,
                                     const Pen& pen);

        void renderEllipsePoints(std::vector<PointF>& dst,
                                 Pt::int32_t radiusX, Pt::int32_t radiusY,
                                 Pt::int32_t centerX, Pt::int32_t centerY,
                                 const Pen& pen);

        void renderArcPoints(std::vector<PointF>& dst,
                             Pt::int32_t radiusX, Pt::int32_t radiusY,
                             Pt::int32_t centerX, Pt::int32_t centerY,
                             float degBegin, float degEnd, const Pen& pen);

        void renderSolidClosedWidePolyline(std::vector<Polygon>& polygons,
                                           const PointF* basePtr, size_t curPCnt,
                                           const Pen& pen);

        void renderSolidOpenWidePolyline(std::vector<Polygon>& polygons,
                                         const PointF* basePtr, size_t curPCnt,
                                         const Pen& pen, bool cleanUpSelfIntersection);

        void renderDashedWidePolyLine(std::vector<Polygon>& polygons,
                                      const PointF* src, size_t pointCount,
                                      const Pen& pen,
                                      bool collisionDetection);

        void renderDashedWidePolyLine();

        void satDPIProjMinMax(double& min, double& max,
                              const PointF* points, size_t pointCount,
                              double px, double py);

        bool satDetectPolygonCollision(const PointF* poly1, size_t poly1Count,
                                       const PointF* poly2, size_t poly2Count);

        bool sagPolygonPoints(PatternState& state, bool draw, const Pen& pen, bool collisionDetection);

        void sagGenerateSimpleLineSegment(PatternState& state,
                                          float x1, float y1,
                                          float x2, float y2,
                                          const Pen& pen,
                                          bool collisionDetection);

        void sagGeneratePolyLineSegment(PatternState& state, const Pen& pen, bool collisionDetection);

        void renderSolidLineSegment(std::vector<PointF>& dst,
                                    float x1, float y1, float x2, float y2,
                                    const Pen& pen, bool openingCap, bool closingCap);

        // TODO: REMOVE "inSameSegment" ???

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

        void combineLinePointsAndAddCaps(std::vector<PointF>& dst,
                                         const std::vector<PointF>& inner,
                                         const std::vector<PointF>& outer,
                                         Pen::CapStyle begCap,
                                         Pen::CapStyle endCap,
                                         size_t penSize);

        void renderLineButtCap(std::vector<PointF>& dst,
                              float x, float y, float wh,
                              float dx, float dy, float nx, float ny);

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

        void calculateLineParams(float& dx, float& dy,
                                 float x1, float y1, float x2, float y2, size_t w);

        void calculateLineParams(float& wh, float& dx, float& dy, float& nx, float& ny,
                                 float x1, float y1, float x2, float y2, size_t w);

        bool intersectLine(bool& inLine, PointF& intersect,
                           const PointF& line1a, const PointF& line1b,
                           const PointF& line2a, const PointF& line2b, size_t penSize);

        static void sagCombinePolygons(std::vector<Polygon>& allPolys, Polygon& prevPoly, const std::vector<PointF>& poly1, const std::vector<PointF>& poly2);

        static void cleanupOnePolygon(std::vector<PointF>& polygon, bool nonZeroFillingRule);
        static void cleanupAllPolygons(std::vector<Polygon>& polygons, bool nonZeroFillingRule);

    private:
        static const double VecResScaleUp;
        static const double VecResScaleDn;

        std::vector<float> dashPatternBuffer;
};


} //namespace

} //namespace

#endif
