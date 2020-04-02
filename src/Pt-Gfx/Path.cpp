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

#include <Pt/Gfx/Path.h>
#include <Pt/Gfx/Painter.h>


namespace {


void quadraticBezierToPoints(std::vector<Pt::Gfx::PointF>& dst,
                             double x1, double y1,
                             double x2, double y2,
                             double x3, double y3,
                             double smoothness)
{
    // checkfor a straight line
    const double dx32 = x3 - x2;
    const double dy32 = y3 - y2;
    const double dx12 = x1 - x2;
    const double dy12 = y1 - y2;

    if( ! (dx12 * dy32 - dy12 * dx32) )
    {
        // curvature
        if( dst.empty() )
            dst.push_back( Pt::Gfx::PointF(x1, y1) );

        dst.push_back( Pt::Gfx::PointF(x3, y3) );
        return;
    }

    // calculate the approximate length of the curve
    const double l32 = ::sqrt(dx32 * dx32 + dy32 * dy32);
    const double l12 = ::sqrt(dx12 * dx12 + dy12 * dy12);
    const double lb  = l32 + l12;

    // determine the number of segments
    const Pt::int32_t nSegs = Pt::lround(lb * abs(smoothness) / 20) + 3 + 1;

    // calculate the inverse multiplication factor
    const double nSegs1i = 1.0 / (nSegs - 1);

    // generate the points
    // PB = (1 - t) * (1 - t) * P1 + 2 * t * (1 - t) * P2 + t * t * P3
    //      -----------------        ---------------        -----
    //      a                        b                      c
    for(Pt::int32_t i = 0; i < nSegs; ++i)
    {
        const double t  = i * nSegs1i;
        const double it = 1.0 - t;
        const double a  = it * it;
        const double b  = 2.0 * t * it;
        const double c  = t * t;
        const double x  = a * x1 + b * x2 + c * x3;
        const double y  = a * y1 + b * y2 + c * y3;

        if( i || dst.empty() )
            dst.push_back( Pt::Gfx::PointF(x, y) );
    }
}


void cubicBezierToPoints(std::vector<Pt::Gfx::PointF>& dst,
                         double x1, double y1,
                         double x2, double y2,
                         double x3, double y3,
                         double x4, double y4,
                         double smoothness)
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
    const Pt::int32_t nSegs = Pt::lround(lb * abs(smoothness) / 20) + 4 + 1;

    // Calculate the inverse multiplication factor
    const double nSegs1i = 1.0 / (nSegs - 1);

    // Generate the points
    // PB = (1 - t) * (1 - t) * (1 - t) * P1 + 3 * t * (1 - t) * (1 - t) * P2 + 3 * t * t * (1 - t) * P3 + t * t * t * P4
    //      ---------------------------        -------------------------        -------------------        ---------
    //      a                                  b                                c                          d
    for(Pt::int32_t i = 0; i < nSegs; ++i)
    {
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
        if( i || dst.empty() )
            dst.push_back( Pt::Gfx::PointF(x, y) );
    }
}


void getBezierPoint(double& x, double& y,
                            const std::vector<double>& points, double t)
{
    // Based on: How do I implement a Bezier curve in C++?
    // http://stackoverflow.com/questions/785097/how-do-i-implement-a-bézier-curve-in-c
    // Answer by iforce2d, 2014 (permalink: http://stackoverflow.com/a/21642962)

    std::vector<double> tmp = points;

    size_t i = points.size() / 2 - 1;

    while(i > 0)
    {
        for(size_t k = 0; k < i; ++k)
        {
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


void bezierToPoints(std::vector<Pt::Gfx::PointF>& dst,
                    double x1, double y1,
                    const std::vector<double>& points,
                    double smoothness)
{
    // Add the start coordinate to the point
    std::vector<double> pts;
    pts.reserve(points.size() + 2);

    pts.push_back(x1);
    pts.push_back(y1);

    pts.insert(pts.end(), points.begin(), points.end());

    // Calculate the approximate length of the curve
    double clen = 0.0;
    for(size_t i = 0; i < (points.size() / 2 - 1); ++i)
    {
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
    const Pt::int32_t nSegs = Pt::lround(clen * abs(smoothness) / 20) + (pts.size() / 2 + 1 + 1);

    // Calculate the inverse multiplication factor
    const double nSegs1i = 1.0 / (nSegs - 1);

    // Generate the points
    for(Pt::int32_t i = 0; i < nSegs; ++i)
    {
        // Calculate the coordinates
        const double t  = i * nSegs1i;

        double x;
        double y;
        getBezierPoint(x, y, pts, t);

        // Store the coordinate as needed
        if( i || dst.empty() )
            dst.push_back( Pt::Gfx::PointF(x, y) );
    }
}

} // namespace

namespace Pt {

namespace Gfx {

Path::Path()
{
}


Path::~Path()
{
}


std::size_t Path::size() const
{
    return _elements.size();
}


bool Path::isEmpty() const
{
   return _elements.empty();
}


const Element& Path::at(std::size_t n) const
{
    return _elements.at(n);
}


void Path::clear()
{
    return _elements.clear();
}


RectF Path::boundingRect() const
{
    // TODO
    RectF result;
    assert(false);
    return result;
}


const PointF& Path::currentPosition() const
{
    return _position;
}


void Path::moveTo(const PointF& p)
{
    Element elem(Element::IT_MoveTo, p.x() , p.y());
    _elements.push_back(elem);

    _position = p;
}


void Path::lineTo(const PointF& p)
{
    Element elem(Element::IT_LineTo, p.x() , p.y());
    _elements.push_back(elem);

    _position = p;
}


void Path::arcTo(const PointF& p, double r)
{
    double x1 = _position.x();
    double y1 = _position.y();
    double x2 = p.x();
    double y2 = p.y();

    // Based on How to create circle with Bézier curves?
    // http://stackoverflow.com/questions/1734745/how-to-create-circle-with-bézier-curves
    // Answer by Kpym, 2015 (permalink: http://stackoverflow.com/a/27863181)

    // Line equation : 0 = aX + By + c
    // Normal        : n = ai + bj
    const double a = y2 - y1;
    const double b = x1 - x2;
   //const double c = -(x1 * y2 - x2 * y1);

    // Middle point
    const double xm = (x1 + x2) * 0.5;
    const double ym = (y1 + y2) * 0.5;

    // Radius
    const double ab = sqrt(a * a + b * b);
    const double rx = ab * 0.5f;
    const double ry = r;

    // Normal vector
    const double iz = -1.0 / ab;
    const double nx = a * iz;
    const double ny = b * iz;

    // Circumference vectors
    const double nxrx = nx * rx;
    const double nxry = nx * ry;
    const double nyrx = ny * rx;
    const double nyry = ny * ry;

    // Optimal distance to the control points for circle approximation
    // using N segments of cubic bezier:
    //    dist = (4 / 3) * tan(pi / 2 / N)
    // If N = 4, then:
    //    dist = (4 / 3) * tan(pi / 2 / 4) = 0.0822479912358
    const double od = 0.552284749831;

    // Curve #1
    const double c1x1 = x1;
    const double c1y1 = y1;
    const double c1x4 = xm   + nxrx;
    const double c1y4 = ym   + nyry;
    const double c1x2 = c1x1 + nxrx * od;
    const double c1y2 = c1y1 + nyry * od;
    const double c1x3 = c1x4 - nyrx * od;
    const double c1y3 = c1y4 - nxry * od;

    Element elem1(Element::IT_CubicBezierTo, c1x2, c1y2, c1x3, c1y3, c1x4, c1y4);
    _elements.push_back(elem1);

    // Curve #2
    const double c2x1 = xm   + nxrx;
    const double c2y1 = ym   + nyry;
    const double c2x4 = x2;
    const double c2y4 = y2;
    const double c2x2 = c2x1 + nyrx * od;
    const double c2y2 = c2y1 - nxry * od;
    const double c2x3 = c2x4 - nxrx * od;
    const double c2y3 = c2y4 + nyry * od;

    Element elem2(Element::IT_CubicBezierTo, c2x2, c2y2, c2x3, c2y3, c2x4, c2y4);
    _elements.push_back(elem2);

    _position = p;
}


void Path::quadraticBezierTo(const PointF &c, const PointF& to)
{
    Element elem(Element::IT_QuadBezierTo, c.x() , c.y(), to.x(), to.y());
    _elements.push_back(elem);

    _position = to;

}


void Path::cubicBezierTo(const PointF &c1, const PointF &c2, const PointF& to)
{
    Element elem(Element::IT_CubicBezierTo,
                 c1.x() , c1.y(),
                 c2.x(), c2.y(),
                 to.x(), to.y());
    _elements.push_back(elem);

    _position = to;
}


void Path::bezierTo(const PointF* cxy, size_t controlPointCount, const PointF& to)
{
    std::vector<double> points;

    for(size_t i = 0; i < controlPointCount; ++i)
    {
        points.push_back(cxy[i].x());
        points.push_back(cxy[i].y());
    }

    points.push_back(to.x());
    points.push_back(to.y());

    Element elem(Element::IT_GenNBezierTo, points);
    _elements.push_back(elem);

    _position = to;
}


void Path::close()
{
    Element elem(Element::IT_Close);
    _elements.push_back(elem);
}


void Path::addPath(const Path& p)
{
    close();
    insertPath(p);
}


void Path::insertPath(const Path& p)
{
    ElementVector::const_iterator it;

    for(it = p._elements.begin(); it != p._elements.end(); ++it)
        _elements.push_back( *it);
}


void Path::addRect(const SizeF& size)
{
    const double x = _position.x();
    const double y = _position.y();

    lineTo(Pt::Gfx::PointF(x, y + size.height()));
    lineTo(Pt::Gfx::PointF(x + size.width(), y+ size.height()));
    lineTo(Pt::Gfx::PointF(x + size.width(), y));
    lineTo(Pt::Gfx::PointF(x, y));
    close();
}


void Path::addRoundedRect(const SizeF& size, float radius)
{
    const double x = _position.x();
    const double y = _position.y();

    moveTo(Pt::Gfx::PointF(x, y +  radius));
    quadraticBezierTo(Pt::Gfx::PointF(x, y), Pt::Gfx::PointF(x + radius, y));

    lineTo(Pt::Gfx::PointF(x +  size.width() - radius, y));
    quadraticBezierTo(Pt::Gfx::PointF(x + size.width(), y),
                      Pt::Gfx::PointF(x + size.width(), y + radius));

    lineTo(Pt::Gfx::PointF(x +  size.width(), y + size.height() - radius));
    quadraticBezierTo(Pt::Gfx::PointF(x + size.width(), y+ size.height() ),
                      Pt::Gfx::PointF(x + size.width() - radius, y + size.height()));

    lineTo(Pt::Gfx::PointF(x +  radius, y + size.height()));
    quadraticBezierTo(Pt::Gfx::PointF(x, y + size.height()),
                      Pt::Gfx::PointF(x, y + size.height() - radius));

    lineTo(Pt::Gfx::PointF(x, y + radius));

    close();
}


void Path::addEllipse(const SizeF& size)
{
  const Pt::Gfx::PointF p1(_position.x(), _position.y() +  size.height() / 2);
  const Pt::Gfx::PointF p2(_position.x() + size.width(), _position.y() + size.height() / 2);

  moveTo(p1);
  arcTo( p2, size.height()/2 );

  moveTo(p2);
  arcTo( p1, size.height()/2 );

  close();
}


void Path::addPie(const SizeF& size, float degBegin, float degEnd)
{
    // TODO: decompose into 4 quadrants and use Trigonometry sin/cos to calculate the point coordinates
}


void Path::addChord(const SizeF& size,  float degBegin, float degEnd)
{
    // TODO: decompose into 4 quadrants and use Trigonometry sin/cos to calculate the point coordinates
}


void Path::transform(const Transform& transform)
{
   // For convenience
    typedef ElementVector::iterator PDIIterator;

    // Walk through the instructions
    for(PDIIterator it = _elements.begin(); it != _elements.end(); ++it)
    {
        // Get the instruction
        Element& elem = *it;

        // Act based on the type of the instruction
        switch(elem.type)
        {
            case Element::IT_Close:
                break;

            case Element::IT_MoveTo:
            case Element::IT_LineTo:
            {
                Pt::Gfx::PointF p( elem.pxy[0], elem.pxy[1] );
                p = transform * p;

                elem.pxy[0] = p.x();
                elem.pxy[1] = p.y();
                break;
            }

            case Element::IT_QuadBezierTo:
            {
                Pt::Gfx::PointF p1( elem.pxy[0], elem.pxy[1] );
                p1 = transform * p1;

                elem.pxy[0] = p1.x();
                elem.pxy[1] = p1.y();

                Pt::Gfx::PointF p2( elem.pxy[2], elem.pxy[3] );
                p2 = transform * p2;

                elem.pxy[2] = p2.x();
                elem.pxy[3] = p2.y();
                break;
            }

            case Element::IT_CubicBezierTo:
            {
                Pt::Gfx::PointF p1( elem.pxy[0], elem.pxy[1] );
                p1 = transform * p1;

                elem.pxy[0] = p1.x();
                elem.pxy[1] = p1.y();

                Pt::Gfx::PointF p2( elem.pxy[2], elem.pxy[3] );
                p2 = transform * p2;

                elem.pxy[2] = p2.x();
                elem.pxy[3] = p2.y();

                Pt::Gfx::PointF p3( elem.pxy[4], elem.pxy[5] );
                p3 = transform * p3;

                elem.pxy[4] = p3.x();
                elem.pxy[5] = p3.y();
                break;
            }

            case Element::IT_GenNBezierTo:
                break;

            default:
                break;
        }
    }
}


void Path::toPolygons(std::vector<Polygon>& polygons, float smoothness) const
{
    // State variables
    double curX = 0.0;
    double curY = 0.0;

    Polygon polygon;

    ElementVector::const_iterator it;
    for(it = _elements.begin(); it != _elements.end(); ++it)
    {
        // Get the instruction
        const Element& ins = *it;

        // Act based on the type of the instruction
        switch(ins.type)
        {
            case Element::IT_Close:
                polygons.push_back(polygon);
                polygon.clear();
                break;

            case Element::IT_MoveTo:
                curX = ins.pxy[0];
                curY = ins.pxy[1];
                break;

            case Element::IT_LineTo:
                if( polygon.empty() )
                    polygon.push_back( PointF(curX, curY) );

                curX = ins.pxy[0];
                curY = ins.pxy[1];
                polygon.push_back( PointF(curX, curY) );
                break;

            case Element::IT_QuadBezierTo:
                quadraticBezierToPoints(polygon.points(), curX, curY, ins.pxy[0], ins.pxy[1], ins.pxy[2], ins.pxy[3], smoothness);
                curX = ins.pxy[2];
                curY = ins.pxy[3];
                break;

            case Element::IT_CubicBezierTo:
                cubicBezierToPoints(polygon.points(), curX, curY, ins.pxy[0], ins.pxy[1], ins.pxy[2], ins.pxy[3], ins.pxy[4], ins.pxy[5], smoothness);
                curX = ins.pxy[4];
                curY = ins.pxy[5];
                break;

            case Element::IT_GenNBezierTo:
                bezierToPoints(polygon.points(), curX, curY, ins.pxy, smoothness);
                curX = ins.pxy[ins.pxy.size() - 2];
                curY = ins.pxy[ins.pxy.size() - 1];
                break;

            default:
                break;
        }
    }

    if( ! polygon.empty() )
        polygon.push_back( polygon.at(0) );
}

} // namespace

} // namespace
