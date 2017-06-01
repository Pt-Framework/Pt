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

#ifndef PT_GFX_PATH_H
#define PT_GFX_PATH_H

#include <stdexcept>
#include <string>
#include <vector>
#include <Pt/SmartPtr.h>
#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ArcMode.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Math.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Transform.h>


namespace Pt{
namespace Gfx{


struct Element
{
    enum ElementType 
    {
      IT_Close,
      IT_MoveTo, 
      IT_LineTo, 
      IT_QuadBezierTo, 
      IT_CubicBezierTo, 
      IT_GenNBezierTo,
    };



    inline Element(ElementType type_)
    : type(type_)
    {}

    inline Element(ElementType type_, double x0, double y0)
    : type(type_), pxy(2)
    { pxy[0] = x0; pxy[1] = y0; }

    inline Element(ElementType type_, double x0, double y0, double x1, double y1)
    : type(type_), pxy(4)
    { pxy[0] = x0; pxy[1] = y0; pxy[2] = x1; pxy[3] = y1; }

    inline Element(ElementType type_, double x0, double y0, double x1, double y1, double x2, double y2)
    : type(type_), pxy(6)
    { pxy[0] = x0; pxy[1] = y0; pxy[2] = x1; pxy[3] = y1; pxy[4] = x2; pxy[5] = y2; }

    inline Element(ElementType type_, const std::vector<double>& pxy_)
    : type(type_), pxy(pxy_)
    {}

    ElementType      type;
    std::vector<double> pxy;
};

class PT_GFX_API Path 
{
    public:
        Path();

        Path(const Path& p);

        ~Path();

        const Path& operator=(const Path& p);

        std::size_t size();

        const Element& at(std::size_t n) const;

        bool isEmpty() const;

        void clear();

        // adds a closed subpath
        void addPath(const Path& p);

        // adds the elements to this path, connecting the last point with the 
        // first point of the inserted path
        void insertPath(const Path& p);

        // ads a LineTo to the begin of the subpath and adds a moveTo(0,0)
        void closeSubpath();

        RectF boundingRect() const;

        const PointF& currentPosition() const;

        void moveTo(const PointF& p);

        void lineTo(const PointF& p);

        void arcTo(const PointF& p, double r);

        void quadraticBezierTo(const PointF &c, const PointF& to);

        void cubicBezierTo(const PointF &c1, const PointF &c2, const PointF& to);

        void bezierTo(const PointF* controlPoints, size_t n, const PointF& to);

        // Apply the tranformation to the points in the path
        void transform(const Transform& transform);

        void generatePoints(std::vector<PointF>& dst, float smoothness = 1) const;  

    private:
       void decomposeArcTo(double x1, double y1, double x2, double y2, double r);


    private:
      static inline void generateQuadraticBezierPoints(std::vector<PointF>& dst, double x1, double y1, double x2, double y2, double x3, double y3, double smoothness)
      {
          //lprintf("(%5.1f, %5.1f) (%5.1f, %5.1f) (%5.1f, %5.1f)\n", x1, y1, x2, y2, x3, y3);
          //lprintf("(%5.1f, %5.1f) (%5.1f, %5.1f)\n", curX, curY, ins.p[0], ins.p[1]);

          // Check if the points actually specify a straight line
          const double dx32 = x3 - x2;
          const double dy32 = y3 - y2;
          const double dx12 = x1 - x2;
          const double dy12 = y1 - y2;

          if( !(dx12 * dy32 - dy12 * dx32) ) { // Curvature
              if(dst.empty()) dst.push_back( PointF(x1, y1) );
              dst.push_back( PointF(x3, y3) );
              return;
          }

          // Calculate the approximate length of the curve
          const double l32 = ::sqrt(dx32 * dx32 + dy32 * dy32);
          const double l12 = ::sqrt(dx12 * dx12 + dy12 * dy12);
          const double lb  = l32 + l12;

          // Determine the number of segments
          const Pt::int32_t nSegs = Gfx::Math::zrint(lb * abs(smoothness) / 20) + 3 + 1;

          // Calculate the inverse multiplication factor
          const double nSegs1i = 1.0 / (nSegs - 1);

          // Generate the points
          // PB = (1 - t) * (1 - t) * P1 + 2 * t * (1 - t) * P2 + t * t * P3
          //      -----------------        ---------------        -----
          //      a                        b                      c
          for(Pt::int32_t i = 0; i < nSegs; ++i) {
              // Calculate the coordinates
              const double t  = i * nSegs1i;
              const double it = 1.0 - t;
              const double a  = it * it;
              const double b  = 2.0 * t * it;
              const double c  = t * t;
              const double x  = a * x1 + b * x2 + c * x3;
              const double y  = a * y1 + b * y2 + c * y3;
              // Store the coordinate as needed
              if(i || dst.empty()) dst.push_back( PointF(x, y) );
          }
      }


        static inline void generateCubicBezierPoints(std::vector<PointF>& dst, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double smoothness)
        {
            // Calculate the approximate length of the curve
            const double dx43 = x4 - x3;
            const double dy43 = y4 - y3;
            const double dx32 = x3 - x2;
            const double dy32 = y3 - y2;
            const double dx12 = x1 - x2;
            const double dy12 = y1 - y2;
            const double l43  = ::sqrt(dx43 * dx43 + dy43 * dy43);
            const double l32  = ::sqrt(dx32 * dx32 + dy32 * dy32);
            const double l12  = ::sqrt(dx12 * dx12 + dy12 * dy12);
            const double lb   = l43 + l32 + l12;

            // Determine the number of segments
            const Pt::int32_t nSegs = Gfx::Math::zrint(lb * abs(smoothness) / 20) + 4 + 1;

            // Calculate the inverse multiplication factor
            const double nSegs1i = 1.0 / (nSegs - 1);

            // Generate the points
            // PB = (1 - t) * (1 - t) * (1 - t) * P1 + 3 * t * (1 - t) * (1 - t) * P2 + 3 * t * t * (1 - t) * P3 + t * t * t * P4
            //      ---------------------------        -------------------------        -------------------        ---------
            //      a                                  b                                c                          d
            for(Pt::int32_t i = 0; i < nSegs; ++i) {
                // Calculate the coordinates
                const double t  = i * nSegs1i;
                const double it = 1.0 - t;
                const double a  = it * it * it;
                const double b  = 3.0 * t * it * it;
                const double c  = 3.0 * t * t * it;
                const double d  = t * t * t;
                const double x  = a * x1 + b * x2 + c * x3 + d * x4;
                const double y  = a * y1 + b * y2 + c * y3 + d * y4;
                // Store the coordinate as needed
                if(i || dst.empty()) dst.push_back( PointF(x, y) );
            }
        }


        static inline void generateGenericNBezierPoints(std::vector<PointF>& dst, double x1, double y1, const std::vector<double>& points, double smoothness)
        {
            // Add the start coordinate to the point
            std::vector<double> pts;
            pts.reserve(points.size() + 2);

            pts.push_back(x1);
            pts.push_back(y1);

            pts.insert(pts.end(), points.begin(), points.end());

            // Calculate the approximate length of the curve
            double clen = 0.0;
            for(size_t i = 0; i < (points.size() / 2 - 1); ++i) {
                const size_t cidx =  i      * 2;
                const size_t nidx = (i + 1) * 2;
                const double x1   = pts[cidx + 0];
                const double y1   = pts[cidx + 1];
                const double x2   = pts[nidx + 0];
                const double y2   = pts[nidx + 1];
                const double dx   = x2 - x1;
                const double dy   = y2 - y1;
                clen += ::sqrt(dx * dx + dy * dy);
            }

            // Determine the number of segments
            const Pt::int32_t nSegs = Gfx::Math::zrint(clen * abs(smoothness) / 20) + (pts.size() / 2 + 1 + 1);

            // Calculate the inverse multiplication factor
            const double nSegs1i = 1.0 / (nSegs - 1);

            // Generate the points
            for(Pt::int32_t i = 0; i < nSegs; ++i) {
                // Calculate the coordinates
                const double t  = i * nSegs1i;
                      double x;
                      double y;
                getGenericNBezierPoint(x, y, pts, t);
                // Store the coordinate as needed
                if(i || dst.empty()) dst.push_back( PointF(x, y) );
            }
        }

      

      // Based on: How do I implement a Bézier curve in C++?
      //           http://stackoverflow.com/questions/785097/how-do-i-implement-a-bézier-curve-in-c
      //           Answer by iforce2d, 2014 (permalink: http://stackoverflow.com/a/21642962)
      static inline void getGenericNBezierPoint(double& x, double& y, const std::vector<double>& points, double t)
      {
          std::vector<double> tmp = points;

          size_t i = points.size() / 2 - 1;

          while(i > 0) {
              for(size_t k = 0; k < i; ++k) {
                  const size_t cidx =  k      * 2;
                  const size_t nidx = (k + 1) * 2;
                  tmp[cidx + 0] = tmp[cidx + 0] + t * ( tmp[nidx + 0] - tmp[cidx + 0] ); // X
                  tmp[cidx + 1] = tmp[cidx + 1] + t * ( tmp[nidx + 1] - tmp[cidx + 1] ); // Y
              }
              --i;
          }

          x = tmp[0];
          y = tmp[1];
      }

    private:
      typedef std::vector<Element> ElementVector;

    private:
      ElementVector _elements;
      double       _curX, _curY;
};


} // namespace
} // namespace

#endif
