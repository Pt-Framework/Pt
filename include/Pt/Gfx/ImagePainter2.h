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

#ifndef PT_GFX_IMAGEPAINTER_2_H
#define PT_GFX_IMAGEPAINTER_2_H

#include <Pt/Gfx/Transform.h>
#include <Pt/Gfx/ArcMode.h>
#include <Pt/Gfx/Path.h>
#include <Pt/Gfx/Painter.h>
#include <Pt/System/Path.h>

namespace Pt {

namespace Gfx {

class Rasterizer2;
struct SAGOpState;

class PT_GFX_API ImagePainter2 : public Painter
{
    public:
        ImagePainter2(Image& image);

        virtual ~ImagePainter2();

        void setImage(Image& image);

        virtual const ImageFormat& format() const;

        virtual bool isAntialiasing() const;

        virtual void setAntialiasing(bool on);

        virtual const CompositionMode& compositionMode() const;

        virtual void setCompositionMode(const CompositionMode& mode);

        virtual const Gfx::RectF& clip() const;

        virtual void setClip(const RectF& clip);

        virtual const Pen& pen() const;
        
        virtual void setPen(const Pen& pen);
        
        virtual const Brush& brush() const;
        
        virtual void setBrush(const Brush& brush);

        virtual const Font& font() const;
        
        virtual void setFont(const Font& font);

        virtual FontMetrics fontMetrics(const Pt::String& text) const;

        virtual void drawImage(const PointF& to, const Image& image);

        virtual void drawImage(const PointF& to, const Image& image, const RectF& imageRect);
        
        virtual void drawText(const PointF& to, const Pt::String& text);

        //virtual void drawText(const PointF& to, const Pt::String& text, const Transform& t);

        virtual void drawLine(const PointF& from, const PointF& to);

        virtual void drawRect(const RectF& rect);

        virtual void drawRoundedRect(const RectF& rect, float radius);

        // NOTE: The points must move in counter-clockwise (CCW) direction or something wrong may be drawn!
        virtual void drawPolyline(const PointF* points, const size_t pointCount);


        // NOTE: The begin and end angle must move in counter-clockwise (CCW) direction or something wrong may be drawn!
        virtual void drawArc(const PointF& topLeft, const SizeF& size, 
                             float degBegin, float degEnd);

        // NOTE: The begin and end angle must move in counter-clockwise (CCW) direction or something wrong may be drawn!
        virtual void drawChord(const PointF& topLeft, const SizeF& size, 
                               float degBegin, float degEnd);

        // NOTE: The begin and end angle must move in counter-clockwise (CCW) direction or something wrong may be drawn!
        virtual void drawPie(const PointF& topLeft, const SizeF& size, 
                             float degBegin, float degEnd);

        virtual void drawEllipse(const PointF& topLeft, const SizeF& size);


        void drawQuadraticBezier(const PointF& from, const PointF& to, 
                                 const PointF& c);

        void drawCubicBezier(const PointF& from, const PointF& to, 
                            const PointF &c1, const PointF &c2);

        virtual void drawQuadraticPolybezier(const PointF& from, const PointF& to, 
                                             const PointF* controls, const size_t n);

        
        // maybe better have ImagePainter::setSmoothness
        virtual void drawPath(const Path& path, float smoothness = 1.0f);


        virtual void fillRect(const RectF& rect);

        virtual void fillRoundedRect(const RectF& rect, float radius);

        virtual void fillPolygon(const PointF* points, const size_t pointCount);

        virtual void fillEllipse(const PointF& topLeft, const SizeF& size);

        virtual void fillPie(const PointF& topLeft, const SizeF& size, 
                             float degBegin, float degEnd);

        virtual void fillChord(const PointF& topLeft, const SizeF& size, 
                              float degBegin, float degEnd);

        // maybe better have a separate ImagePainter::setSmoothness
        virtual void fillPath(const Path& path, float smoothness = 1.0f);

    public:
        static void setFontDir(const System::Path& path);
        static void setDefaultFont(const std::string& name);
        static std::string defaultFont();
        static std::vector<std::string> fontNames();
        static FontMetrics fontMetrics(const Font& font, const Pt::String& text);

    private:
        void clipPolygon(std::vector<PointF>& result, const std::vector<PointF>& subject, const std::vector<PointF>& clipRegion);

        void generateSolidLineSegment(std::vector<PointF>& dst, float x1, float y1, float x2, float y2, bool openingCap, bool closingCap);        
        void generatePatternedSingleLineSegment(std::vector<PointF>& dst, float x1, float y1, float x2, float y2, Pt::int32_t& piCtrInOut);
        void deduplicatePointsF(std::vector<PointF>& dst, const PointF* src, const size_t pointCount);
        void cnvPointsFToPointsDeduplicate(std::vector<Point>& dst, const PointF* src, const size_t pointCount);
        void drawThickPolyline_impl(const PointF* ps, const size_t pointCount, bool autoClose, const int32_t* segmentIndexMarker);
        bool thickenSolidClosedPolygon(std::vector<PointF>& pointsF, const PointF* basePtr, size_t curPCnt, const int32_t* segmentIndexMarker);
        bool thickenSolidOpenPolygon(std::vector<PointF>& pointsF, const PointF* basePtr, size_t curPCnt, const int32_t* segmentIndexMarker);
        void thickenPatternedPolygon(std::vector<PointF>& pointsF, const PointF* src, size_t pointCount);
        void drawArc( const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, const ArcMode& arcMode);
        bool combineLineSegmentForSolidOpenPolygon(std::vector<PointF>& polygon, std::vector<PointF>& inner, const std::vector<PointF>& segment, const PointF& origMeetingPoint, bool inSameSegment);
        bool combineLineSegmentForSolidClosedPolygon(std::vector<PointF>& outer, std::vector<PointF>& inner, const std::vector<PointF>& segment, const PointF& origMeetingPoint, bool isFirst, bool isLast, bool inSameSegment);
        bool sagPolygonPoints(SAGOpState& state, bool draw);
        void sagGeneratePolyLineSegment(SAGOpState& state);
        void sagGenerateSimpleLineSegment(SAGOpState& state, float x1, float y1, float x2, float y2);

     private:

        static inline void generateLineButtCap(std::vector<PointF>& dst, float x, float y, float nx, float ny)
        {
            dst.push_back( PointF(x + nx, y + ny) );
            dst.push_back( PointF(x - nx, y - ny) );
        }

        static inline void generateLineSquareCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
        {
            dst.push_back( PointF(x - dx + nx, y - dy + ny) );
            dst.push_back( PointF(x - dx - nx, y - dy - ny) );
        }

        static inline void generateLineRoundCap(std::vector<PointF>& dst, float x, float y, float wh, float dx, float dy, float nx, float ny)
        {
        #if 0
            generateQuadraticBezierPoints(
                dst,
                lround(x + nx     ), lround(y + ny     ),
                lround(x + nx - dx), lround(y + ny - dy),
                lround(x      - dx), lround(y      - dy),
                Gfx::Math::zcint(wh * 0.5f)
            );
            generateQuadraticBezierPoints(
                dst,
                lround(x      - dx), lround(y      - dy),
                lround(x - nx - dx), lround(y - ny - dy),
                lround(x - nx     ), lround(y - ny     ),
                Gfx::Math::zcint(wh * 0.5f)
            );
        #else
            generateQuadraticBezierPoints(
                dst,
                lround(x + nx       ), lround(y + ny       ),
                lround(x - dx * 2.0f), lround(y - dy * 2.0f),
                lround(x - nx       ), lround(y - ny       ),
                Pt::lround(ceil(wh)) - 1
            );
        #endif
        }

        static inline void generateLineTriangularOutCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
        {
            dst.push_back( PointF(x + nx, y + ny) );
            dst.push_back( PointF(x - dx, y - dy) );
            dst.push_back( PointF(x - nx, y - ny) );
        }

        static inline void generateLineTriangularInCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
        {
            dst.push_back( PointF(x + nx - dx, y + ny - dy) );
            dst.push_back( PointF(x,           y          ) );
            dst.push_back( PointF(x - nx - dx, y - ny - dy) );
        }

        static inline void generateLineRoundHoleCap(std::vector<PointF>& dst, float x, float y, float wh, float dx, float dy, float nx, float ny)
        {
        #if 0
            generateQuadraticBezierPoints(
                dst,
                lround(x + nx - dx), lround(y + ny - dy),
                lround(x + nx     ), lround(y + ny     ),
                lround(x          ), lround(y          ),
                Gfx::Math::zcint(wh * 0.5f)
            );
            generateQuadraticBezierPoints(
                dst,
                lround(x          ), lround(y          ),
                lround(x - nx     ), lround(y - ny     ),
                lround(x - nx - dx), lround(y - ny - dy),
                Gfx::Math::zcint(wh * 0.5f)
            );
        #else
            generateQuadraticBezierPoints(
                dst,
                lround(x + nx - dx), lround(y + ny - dy),
                lround(x      + dx), lround(y      + dy),
                lround(x - nx - dx), lround(y - ny - dy),
                Pt::lround(ceil(wh)) - 1
            );
        #endif
        }

        static inline void generateLineArrow1Cap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
        {
            dst.push_back( PointF(x + nx,        y + ny       ) );
            dst.push_back( PointF(x + nx * 2.0f, y + ny * 2.0f) );
            dst.push_back( PointF(x - dx,        y - dy       ) );
            dst.push_back( PointF(x - nx * 2.0f, y - ny * 2.0f) );
            dst.push_back( PointF(x - nx,        y - ny       ) );
        }

        static inline void generateLineArrow2Cap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
        {
            dst.push_back( PointF(x + dx * 0.5f + nx,        y + dy * 0.5f + ny       ) );
            dst.push_back( PointF(x + dx        + nx * 2.0f, y + dy        + ny * 2.0f) );
            dst.push_back( PointF(x - dx,                    y - dy                   ) );
            dst.push_back( PointF(x + dx        - nx * 2.0f, y + dy        - ny * 2.0f) );
            dst.push_back( PointF(x + dx * 0.5f - nx,        y + dy * 0.5f - ny       ) );
        }

        // Based on: Bitmap/Bézier curves/Quadratic
        //           https://rosettacode.org/wiki/Bitmap/B%C3%A9zier_curves/Quadratic#C
        //           Last modified on February 17, 2017
        static inline void generateQuadraticBezierPoints(std::vector<PointF>& dst, float x1, float y1, float x2, float y2, float x3, float y3, Pt::int32_t nSegs)
        {
            // Check if the points actually specify a straight line
            const float sx = x3 - x2;
            const float sy = y3 - y2;
            const float xx = x1 - x2;
            const float yy = y1 - y2;

            if( !(xx * sy - yy * sx) ) { // Curvature
                if( dst.empty() || dst.back().x() != x1 || dst.back().y() != y1 ) dst.push_back( PointF(x1, y1) );
                if( dst.empty() || dst.back().x() != x3 || dst.back().y() != y3 ) dst.push_back( PointF(x3, y3) );
                return;
            }

            // Ensure that the number of segments are not too few
            if(nSegs < 4) nSegs = 4;

            // Calculate the inverse multiplication factor
            const float nSegs1i = 1.0f / (nSegs - 1);

            for(Pt::int32_t i = 0; i < nSegs; ++i) {
                // Calculate the coordinates
                const float t  = i * nSegs1i;
                const float it = 1.0f - t;
                const float a  = it * it;
                const float b  = 2.0f * t  * it;
                const float c  = t * t;
                const float x  = a * x1 + b * x2 + c * x3;
                const float y  = a * y1 + b * y2 + c * y3;
                // Check if the coordinate is the same with the previous one
                if( !dst.empty() && ( dst.back().x() == x && dst.back().y() == y ) ) continue;
                // Store the coordinate
                dst.push_back( PointF(x, y) );
            }
        }


        static inline void generateEllipsePoints(std::vector<PointF>& dst, Pt::int32_t radiusX, Pt::int32_t radiusY, Pt::int32_t centerX, Pt::int32_t centerY, size_t penSize)
        {
            // Calculate the ellipse's parameters
            const Pt::int32_t circFac = lround(
                                            sqrt( 0.5f * (radiusX * radiusX + radiusY * radiusY) ) /
                                            ( (penSize > 4) ? (penSize * 0.25f) : 1.0f )
                                        );
            const Pt::int32_t circSeg = (circFac / 16) * 20 + 1;
            const Pt::int32_t nSegs   = (circSeg <  9) ?  9 : circSeg;
            const float       nSegs1i = 1.0f / (nSegs - 1);

            // Generate a polygon that approximates the ellipse
            for(Pt::int32_t i = 0; i < nSegs; ++i) {
                const float angle = piDouble<float>() * i * nSegs1i;
                // Calculate the coordinate
                const float x = centerX + radiusX * fastCos(angle);
                const float y = centerY - radiusY * fastSin(angle); // Sign inversion due to differences between cartesian and computer coordinate systems
                // Store the coordinate only if it is different with the previous one
                if( !dst.empty() && dst.back().x() == x && dst.back().y() == y ) continue;
                dst.push_back( PointF(x, y) );
            }

            // Discard the last point if it has the same coordinate with the first one
            if(dst.back() == dst[0]) dst.pop_back();
        }


        static inline void generateArcPoints(std::vector<PointF>& dst, Pt::int32_t radiusX, Pt::int32_t radiusY, Pt::int32_t centerX, Pt::int32_t centerY, float degBegin, float degEnd, size_t penSize)
        {
            // Calculate the arc's parameters
            const float       degDlt  = degEnd - degBegin;
            const float       degFac  = degDlt / 360.0f;
            const Pt::int32_t circFac = lround(
                                            degFac *
                                            sqrt( 0.5f * (radiusX * radiusX + radiusY * radiusY) ) /
                                            ( (penSize > 4) ? (penSize * 0.25f) : 1.0f )
                                        );
            const Pt::int32_t circSeg = (circFac / 16) * 20 + 1;
            const Pt::int32_t nSegs   = (circSeg <  9) ?  9 : circSeg;
            const float       nSegs1i = 1.0f / (nSegs - 1);

            // Generate a polygon that approximates the arc
            const float fdegInc = (degDlt   * DegToRadF) * nSegs1i;
                  float angle   =  degBegin * DegToRadF;

            for(Pt::int32_t i = 0; i < nSegs; ++i) {
                // Calculate the coordinate
                const float x = centerX + radiusX * fastCos(angle);
                const float y = centerY - radiusY * fastSin(angle); // Sign inversion due to differences between cartesian and computer coordinate systems
                // Update the angle
                angle += fdegInc;
                // Store the coordinate only if it is different with the previous one
                if( !dst.empty() && dst.back().x() == x && dst.back().y() == y ) continue;
                dst.push_back( PointF(x, y) );
            }

            // Discard the last point if it has the same coordinate with the first one
            if(dst.back() == dst[0]) dst.pop_back();
        }


        static inline void generateRoundRectPoints(std::vector<PointF>& dst, float x1, float y1, float x2, float y2, float radius, Pt::int32_t nSegs)
    {
        // CCW

        // --- Bottom left ---
        generateQuadraticBezierPoints(
            dst,
            x1         , y2 - radius,
            x1         , y2         ,
            x1 + radius, y2         ,
            nSegs
        );

        // --- Bottom middle ---
        dst.push_back( PointF((x1 + x2) * 0.5f, y2) );

        // --- Bottom left ---
        generateQuadraticBezierPoints(
            dst,
            x2 - radius, y2         ,
            x2,          y2         ,
            x2,          y2 - radius,
            nSegs
        );

        // --- Center right ---
        dst.push_back( PointF(x2, (y1 + y2) * 0.5f) );

        // --- Top right ---
        generateQuadraticBezierPoints(
            dst,
            x2,          y1 + radius,
            x2,          y1         ,
            x2 - radius, y1         ,
            nSegs
        );

        // --- Top middle ---
        dst.push_back( PointF((x1 + x2) * 0.5f, y1) );

        // --- Top left ---
        generateQuadraticBezierPoints(
            dst,
            x1 + radius, y1         ,
            x1,          y1         ,
            x1,          y1 + radius,
            nSegs
        );

        // --- Center left ---
        dst.push_back( PointF(x1, (y1 + y2) * 0.5f) );
    }
    
        
        static inline void combineLinePointsAndAddCaps(std::vector<PointF>& dst, const std::vector<PointF>& inner, const std::vector<PointF>& outer, Pen::CapStyle begCap, Pen::CapStyle endCap, size_t penSize)
      {
          // Calculate the end lines' parameters
          const Pt::int32_t ox2a = outer[outer.size() - 1].x();
          const Pt::int32_t oy2a = outer[outer.size() - 1].y();
          const Pt::int32_t ox2b = outer[outer.size() - 2].x();
          const Pt::int32_t oy2b = outer[outer.size() - 2].y();
          const Pt::int32_t ix2a = inner[inner.size() - 1].x();
          const Pt::int32_t iy2a = inner[inner.size() - 1].y();
          const Pt::int32_t ix2b = inner[inner.size() - 2].x();
          const Pt::int32_t iy2b = inner[inner.size() - 2].y();
          const float       x2a  = (float) (ox2a + ix2a) * 0.5f;
          const float       y2a  = (float) (oy2a + iy2a) * 0.5f;
          const float       x2b  = (float) (ox2b + ix2b) * 0.5f;
          const float       y2b  = (float) (oy2b + iy2b) * 0.5f;

          // Calculate the line parameters
          float wh2, dx2, dy2, nx2, ny2;
          calculateLineParams(wh2, dx2, dy2, nx2, ny2, x2a, y2a, x2b, y2b, penSize);

          // Generate the end cap
          switch(endCap) {
              case Pen::SquareCap:
                  dst.push_back( PointF( ix2a - dx2, iy2a - dy2 ) );
                  dst.push_back( PointF( ox2a - dx2, oy2a - dy2 ) );
                  break;

              case Pen::RoundCap: {
                  std::vector<PointF> tmp;
                  generateQuadraticBezierPoints(tmp, ix2a, iy2a, x2a - dx2 * 2.0f, y2a - dy2 * 2.0f, ox2a, oy2a, Pt::lround(ceil(penSize * 0.5f)) - 1);
                  if(tmp.size() <= 2) break;
                  for(size_t i = 1; i < tmp.size() - 1; ++i) {
                      dst.push_back( PointF( tmp[i].x(), tmp[i].y() ) );
                  }
                  break;
              }

              case Pen::TriangularOutCap:
                  dst.push_back( PointF( x2a - dx2, y2a - dy2 ) );
                  break;

              case Pen::TriangularInCap:
                  dst.push_back( PointF( x2a + dx2, y2a + dy2 ) );
                  break;

              case Pen::RoundHoleCap: {
                  // Calculate additional line parameters
                  float wh2i, dx2i, dy2i, nx2i, ny2i;
                  float wh2o, dx2o, dy2o, nx2o, ny2o;
                  calculateLineParams(wh2i, dx2i, dy2i, nx2i, ny2i, ix2a, iy2a, ix2b, iy2b, penSize);
                  calculateLineParams(wh2o, dx2o, dy2o, nx2o, ny2o, ox2a, oy2a, ox2b, oy2b, penSize);
                  // Generate the points
                  std::vector<PointF> tmp;
                  generateQuadraticBezierPoints(tmp, ix2a - dx2i, iy2a - dy2i, x2a + dx2, y2a + dy2, ox2a - dx2o, oy2a - dy2o, penSize);
                  if(tmp.size() <= 2) break;
                  for(size_t i = 1; i < tmp.size() - 1; ++i) {
                      dst.push_back( PointF( tmp[i].x(), tmp[i].y() ) );
                  }
                  break;
              }

              case Pen::Arrow1Cap:
                  dst.push_back( PointF( x2a - nx2 * 2.0f, y2a - ny2 * 2.0f ) );
                  dst.push_back( PointF( x2a - dx2       , y2a - dy2        ) );
                  dst.push_back( PointF( x2a + nx2 * 2.0f, y2a + ny2 * 2.0f ) );
                  break;

              case Pen::Arrow2Cap:
                  dst.push_back( PointF( x2a - dx2 * 0.5f - nx2       , y2a - dy2 * 0.5f - ny2        ) );
                  dst.push_back( PointF( x2a              - nx2 * 2.0f, y2a              - ny2 * 2.0f ) );
                  dst.push_back( PointF( x2a - dx2 * 2.0f             , y2a - dy2 * 2.0f              ) );
                  dst.push_back( PointF( x2a              + nx2 * 2.0f, y2a              + ny2 * 2.0f ) );
                  dst.push_back( PointF( x2a - dx2 * 0.5f + nx2       , y2a - dy2 * 0.5f + ny2        ) );
                  break;

              default:
                  break;
          }

          // Store the "outside" points
          dst.insert(dst.end(), outer.rbegin(), outer.rend());

          // Calculate the begin lines' parameters
          const Pt::int32_t ox1a = outer[0].x();
          const Pt::int32_t oy1a = outer[0].y();
          const Pt::int32_t ox1b = outer[1].x();
          const Pt::int32_t oy1b = outer[1].y();
          const Pt::int32_t ix1a = inner[0].x();
          const Pt::int32_t iy1a = inner[0].y();
          const Pt::int32_t ix1b = inner[1].x();
          const Pt::int32_t iy1b = inner[1].y();
          const float       x1a  = (float) (ox1a + ix1a) * 0.5f;
          const float       y1a  = (float) (oy1a + iy1a) * 0.5f;
          const float       x1b  = (float) (ox1b + ix1b) * 0.5f;
          const float       y1b  = (float) (oy1b + iy1b) * 0.5f;

          // Intersect the begin lines
          float wh1, dx1, dy1, nx1, ny1;
          calculateLineParams(wh1, dx1, dy1, nx1, ny1, x1b, y1b, x1a, y1a, penSize);

          // Generate the begin cap
          switch(begCap) {
              case Pen::SquareCap:
                  dst.push_back( PointF( ox1a + dx1, oy1a + dy1 ) );
                  dst.push_back( PointF( ix1a + dx1, iy1a + dy1 ) );
                  break;

              case Pen::RoundCap: {
                  std::vector<PointF> tmp;
                  generateQuadraticBezierPoints(tmp, ox1a, oy1a, x1a + dx1 * 2.0f, y1a + dy1 * 2.0f, ix1a, iy1a, Pt::lround(ceil(penSize * 0.5f)) - 1);
                  if(tmp.size() <= 2) break;
                  for(size_t i = 1; i < tmp.size() - 1; ++i) {
                      dst.push_back( PointF( tmp[i].x(), tmp[i].y() ) );
                  }
                  break;
              }

              case Pen::TriangularOutCap:
                  dst.push_back( PointF( x1a + dx1, y1a + dy1 ) );
                  break;

              case Pen::TriangularInCap:
                  dst.push_back( PointF( x1a - dx1, y1a - dy1 ) );
                  break;

              case Pen::RoundHoleCap: {
                  /*
                  // Calculate additional line parameters
                  float wh1i, dx1i, dy1i, nx1i, ny1i;
                  float wh1o, dx1o, dy1o, nx1o, ny1o;
                  calculateLineParams(wh1i, dx1i, dy1i, nx1i, ny1i, ix1a, iy1a, ix1b, iy1b, penSize);
                  calculateLineParams(wh1o, dx1o, dy1o, nx1o, ny1o, ox1a, oy1a, ox1b, oy1b, penSize);
                  // Generate the points
                  */
                  std::vector<PointF> tmp;
                  generateQuadraticBezierPoints(tmp, ox1a + dx1, oy1a + dy1, x1a - dx1, y1a - dy1, ix1a + dx1, iy1a + dy1, penSize);
                  if(tmp.size() <= 2) break;
                  for(size_t i = 1; i < tmp.size() - 1; ++i) {
                      dst.push_back( PointF( tmp[i].x(), tmp[i].y() ) );
                  }
                  break;
              }

              case Pen::Arrow1Cap:
                  dst.push_back( PointF( x1a + nx1 * 2.0f, y1a + ny1 * 2.0f ) );
                  dst.push_back( PointF( x1a + dx1       , y1a + dy1        ) );
                  dst.push_back( PointF( x1a - nx1 * 2.0f, y1a - ny1 * 2.0f ) );
                  break;

              case Pen::Arrow2Cap:
                  dst.push_back( PointF( x1a + dx1 * 0.5f + nx1       , y1a + dy1 * 0.5f + ny1        ) );
                  dst.push_back( PointF( x1a              + nx1 * 2.0f, y1a              + ny1 * 2.0f ) );
                  dst.push_back( PointF( x1a + dx1 * 2.0f             , y1a + dy1 * 2.0f              ) );
                  dst.push_back( PointF( x1a              - nx1 * 2.0f, y1a              - ny1 * 2.0f ) );
                  dst.push_back( PointF( x1a + dx1 * 0.5f - nx1       , y1a + dy1 * 0.5f - ny1        ) );
                  break;

              default:
                  break;
          }

          // Store the "inside" points
          dst.insert(dst.end(), inner. begin(), inner. end());
      }


        static inline void calculateLineParams(float& wh, float& dx, float& dy, float& nx, float& ny, float x1, float y1, float x2, float y2, size_t w)
      {
          // Line equation : 0 = aX + By + c
          // Normal        : n = ai + bj
          const float a = y2 - y1;
          const float b = x1 - x2;
        //const float c = -(x1 * y2 - x2 * y1);

          // Inverse line length
          // NOTE: Gfx::Math::fastInvSqrt() will produce artifacts!
          const float il = 1.0f / ::sqrtf(a * a + b * b);

          // Half line width
          wh = (float) w * 0.5f;

          // Direction vector
          dx = -b * il * wh;
          dy =  a * il * wh;

          // Normal vector
          nx =  a * il * wh;
          ny =  b * il * wh;
      }

      // Based on: Collision Detection Using the Separating Axis Theorem
      //           https://gamedevelopment.tutsplus.com/tutorials/collision-detection-using-the-separating-axis-theorem--gamedev-169
      //           http://cdn.tutsplus.com/gamedev/uploads/legacy/008_separatingAxisTheorem/SeparatingAxisTheorem.zip
      //           Article and original code by Kah Shiu Chong, 2012

        static inline bool satDetectPolygonCollision(const PointF* poly1, size_t poly1Count, const PointF* poly2, size_t poly2Count)
      {
          // Evaluate using the first polygon's normals
          for(size_t i = poly1Count; i >= 1; --i) {
              // Calculate the indexes
              const size_t idx1 =                               (i - 1);
              const size_t idx2 = (i == 1) ? (poly1Count - 1) : (i - 2);
              // Calculate the normals
              const float dx = poly1[idx2].x() - poly1[idx1].x();
              const float dy = poly1[idx2].y() - poly1[idx1].y();
              const float nx =  dy;
              const float ny = -dx;
              // Get the minimum and maximum projection values
              float min1, max1, min2, max2;
              satDPIProjMinMax(min1, max1, poly1, poly1Count, nx, ny);
              satDPIProjMinMax(min2, max2, poly2, poly2Count, nx, ny);
              // Check if the polygon is separated
              //lprintf("A: %+7.1f , %+7.1f --- %+7.1f , %+7.1f ### %d ### %+7.1f , %+7.1f\n", max1, min2, max2, min1, (max1 < min2 || max2 < min1), nx, ny);
              if(max1 < min2 || max2 < min1) return false;
              if(fabs( (min2 - max1) / max1 ) <= 0.003f || fabs( (min1 - max2) / max2 ) <= 0.003f) return false;
          }

          // Calculate the second polygon's normals
          for(size_t i = poly2Count; i >= 1; --i) {
              // Calculate the indexes
              const size_t idx1 =                               (i - 1);
              const size_t idx2 = (i == 1) ? (poly2Count - 1) : (i - 2);
              // Calculate the normals
              const float dx = poly2[idx2].x() - poly2[idx1].x();
              const float dy = poly2[idx2].y() - poly2[idx1].y();
              const float nx =  dy;
              const float ny = -dx;
              // Get the minimum and maximum projection values
              float min1, max1, min2, max2;
              satDPIProjMinMax(min1, max1, poly1, poly1Count, nx, ny);
              satDPIProjMinMax(min2, max2, poly2, poly2Count, nx, ny);
              // Check if the polygon is separated
              //lprintf("B: %+7.1f , %+7.1f --- %+7.1f , %+7.1f ### %d ### %+7.1f , %+7.1f\n", max1, min2, max2, min1, (max1 < min2 || max2 < min1), nx, ny);
              if(max1 < min2 || max2 < min1) return false;
              if(fabs( (min2 - max1) / max1 ) <= 0.003f || fabs( (min1 - max2) / max2 ) <= 0.003f) return false;
          }

          // There is a collision
          return true;
      }

      

      // Based on: Collision Detection Using the Separating Axis Theorem
      //           https://gamedevelopment.tutsplus.com/tutorials/collision-detection-using-the-separating-axis-theorem--gamedev-169
      //           http://cdn.tutsplus.com/gamedev/uploads/legacy/008_separatingAxisTheorem/SeparatingAxisTheorem.zip
      //           Article and original code by Kah Shiu Chong, 2012
      static inline void satDPIProjMinMax(float& min, float& max, const PointF* points, size_t pointCount, float px, float py)
      {
          min =  Painter::MaximumCoordinate;
          max = -Painter::MaximumCoordinate;

          for(size_t i = 0; i < pointCount; ++i) {
              const float val = points[i].x() * px + points[i].y() * py;
              if(val > max) max = val;
              if(val < min) min = val;
          }
      }


         static bool intersectLine(bool& inLine, PointF& intersect, const PointF& line1a, const PointF& line1b, const PointF& line2a, const PointF& line2b, size_t penSize);

    private:
      RectF        _clip;
      Rasterizer2* _rasterizer;
};


} // namespace
} // namespace

#endif
