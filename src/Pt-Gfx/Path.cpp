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
#include <cmath>
#include <limits>

namespace {

const int MaxBezierSplits = 16;


void quadraticBezierSplit(Pt::Gfx::Polygon& dst,
                          double x1, double y1,
                          double x2, double y2,
                          double x3, double y3,
                          double tolerance,
                          int depth)
{
    const double dx = x3 - x1;
    const double dy = y3 - y1;
    const double d = std::fabs((x2 - x3) * dy - (y2 - y3) * dx);
    const double len = std::sqrt(dx * dx + dy * dy);

    if (depth >= MaxBezierSplits || d <= tolerance * len)
    {
        dst.push_back(Pt::Gfx::PointF(x3, y3));
        return;
    }

    const double x12 = (x1 + x2) * 0.5;
    const double y12 = (y1 + y2) * 0.5;
    const double x23 = (x2 + x3) * 0.5;
    const double y23 = (y2 + y3) * 0.5;
    const double xm = (x12 + x23) * 0.5;
    const double ym = (y12 + y23) * 0.5;

    quadraticBezierSplit(dst, x1, y1, x12, y12, xm, ym, tolerance, depth + 1);
    quadraticBezierSplit(dst, xm, ym, x23, y23, x3, y3, tolerance, depth + 1);
}


void quadraticBezierToPoints(Pt::Gfx::Polygon& dst,
                             double x1, double y1,
                             double x2, double y2,
                             double x3, double y3,
                             double tolerance = 0.25)
{
      if (dst.empty())
        dst.push_back(Pt::Gfx::PointF(x1, y1));

      quadraticBezierSplit(dst, x1, y1, x2, y2, x3, y3, tolerance, 0);
}


void cubicBezierSplit(Pt::Gfx::Polygon& dst,
                      double x1, double y1,
                      double x2, double y2,
                      double x3, double y3,
                      double x4, double y4,
                      double tolerance,
                      int depth)
{
    const double dx = x4 - x1;
    const double dy = y4 - y1;
    const double len = std::sqrt(dx * dx + dy * dy);
    const double d2 = std::fabs((x2 - x4) * dy - (y2 - y4) * dx);
    const double d3 = std::fabs((x3 - x4) * dy - (y3 - y4) * dx);
    
    if (depth >= MaxBezierSplits || (d2 + d3) <= tolerance * len)
    {
        dst.push_back(Pt::Gfx::PointF(x4, y4));
        return;
    }
    
    const double x12 = (x1 + x2) * 0.5;
    const double y12 = (y1 + y2) * 0.5;
    const double x23 = (x2 + x3) * 0.5;
    const double y23 = (y2 + y3) * 0.5;
    const double x34 = (x3 + x4) * 0.5;
    const double y34 = (y3 + y4) * 0.5;
    const double x123 = (x12 + x23) * 0.5;
    const double y123 = (y12 + y23) * 0.5;
    const double x234 = (x23 + x34) * 0.5;
    const double y234 = (y23 + y34) * 0.5;
    const double xm = (x123 + x234) * 0.5;
    const double ym = (y123 + y234) * 0.5;
    
    cubicBezierSplit(dst, x1, y1, x12, y12, x123, y123, xm, ym, tolerance, depth + 1);
    cubicBezierSplit(dst, xm, ym, x234, y234, x34, y34, x4, y4, tolerance, depth + 1);
}


void cubicBezierToPoints(Pt::Gfx::Polygon& dst,
                         double x1, double y1,
                         double x2, double y2,
                         double x3, double y3,
                         double x4, double y4,
                         double tolerance = 0.25)
{
    if( dst.empty() )
        dst.push_back(Pt::Gfx::PointF(x1, y1));

    cubicBezierSplit(dst, x1, y1, x2, y2, x3, y3, x4, y4, tolerance, 0);
}

} // namespace

namespace Pt {

namespace Gfx {

Path::Path()
: _pathData( new PathData() )
{
}


Path::Path(const Path& other)
: _pathData(other._pathData)
{
}


Path& Path::operator=(const Path& other)
{
  if(this != &other)
        _pathData = other._pathData;

    return *this;
}


Path::~Path()
{
}


void Path::detach()
{
    if(_pathData.refs() > 1)
    {
        SmartPtr<PathData> pathData( new PathData() );
        *pathData = *_pathData;
        _pathData = pathData;
    }
}


std::size_t Path::size() const
{
    return _pathData->size();
}


bool Path::isEmpty() const
{
   return _pathData->isEmpty();
}


Path::Iterator Path::begin() const 
{
    return _pathData->begin();
}


Path::Iterator Path::end() const 
{
    return _pathData->end();
}


void Path::clear()
{
    detach();

    return _pathData->clear();
}


RectF Path::boundingRect() const
{
    if( _pathData->isEmpty() )
        return RectF();

    double minX =  std::numeric_limits<double>::max();
    double minY =  std::numeric_limits<double>::max();
    double maxX = -std::numeric_limits<double>::max();
    double maxY = -std::numeric_limits<double>::max();

    for( PathIterator it = _pathData->begin(); it != _pathData->end(); ++it )
    {
        const PathElement& elem = *it;

        for( std::size_t i = 0; i < elem.size(); ++i )
        {
            const PointF& p = elem.point(i);
            minX = std::min(minX, p.x());
            minY = std::min(minY, p.y());
            maxX = std::max(maxX, p.x());
            maxY = std::max(maxY, p.y());
        }
    }

    return RectF( PointF(minX, minY), PointF(maxX, maxY) );
}


const PointF& Path::currentPosition() const
{
    return _pathData->currentPosition();
}


void Path::moveTo(const PointF& to)
{
    detach();

    _pathData->moveTo(to);
}


void Path::lineTo(const PointF& to)
{
    detach();

   _pathData->lineTo(to);
}


void Path::quadTo(const PointF &c, const PointF& to)
{
    detach();

    _pathData->quadTo(c, to);

}


void Path::cubicTo(const PointF& c1, const PointF& c2, const PointF& to)
{   
    detach();

    _pathData->cubicTo(c1, c2, to);
}


void Path::arcTo(const PointF& p, double r)
{
    detach();

    double x1 = _pathData->currentPosition().x();
    double y1 = _pathData->currentPosition().y();
    double x2 = p.x();
    double y2 = p.y();

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

    cubicTo( PointF(c1x2, c1y2), PointF(c1x3, c1y3), PointF(c1x4, c1y4) );

    // Curve #2
    const double c2x1 = xm   + nxrx;
    const double c2y1 = ym   + nyry;
    const double c2x4 = x2;
    const double c2y4 = y2;
    const double c2x2 = c2x1 + nyrx * od;
    const double c2y2 = c2y1 - nxry * od;
    const double c2x3 = c2x4 - nxrx * od;
    const double c2y3 = c2y4 + nyry * od;

    cubicTo( PointF(c2x2, c2y2), PointF(c2x3, c2y3), PointF(c2x4, c2y4) );
}


void Path::close()
{
    detach();

    _pathData->close();
}


void Path::addPath(const Path& p)
{
    close();
    appendPath(p);
}


void Path::appendPath(const Path& p)
{
    detach();

    _pathData->append(*p._pathData);
}


void Path::addRect(const SizeF& size)
{
    detach();

    const PointF& pos = _pathData->currentPosition();
    double x = pos.x();
    double y = pos.y();

    lineTo(Pt::Gfx::PointF(x, y + size.height()));
    lineTo(Pt::Gfx::PointF(x + size.width(), y+ size.height()));
    lineTo(Pt::Gfx::PointF(x + size.width(), y));
    close();
}


void Path::addRoundedRect(const SizeF& size, float radius)
{
    detach();

    const PointF& pos = _pathData->currentPosition();
    double x = pos.x();
    double y = pos.y();

    moveTo(Pt::Gfx::PointF(x, y +  radius));
    quadTo(Pt::Gfx::PointF(x, y), Pt::Gfx::PointF(x + radius, y));

    lineTo(Pt::Gfx::PointF(x +  size.width() - radius, y));
    quadTo(Pt::Gfx::PointF(x + size.width(), y),
                      Pt::Gfx::PointF(x + size.width(), y + radius));

    lineTo(Pt::Gfx::PointF(x +  size.width(), y + size.height() - radius));
    quadTo(Pt::Gfx::PointF(x + size.width(), y+ size.height() ),
                      Pt::Gfx::PointF(x + size.width() - radius, y + size.height()));

    lineTo(Pt::Gfx::PointF(x +  radius, y + size.height()));
    quadTo(Pt::Gfx::PointF(x, y + size.height()),
                      Pt::Gfx::PointF(x, y + size.height() - radius));

    lineTo(Pt::Gfx::PointF(x, y + radius));

    close();
}


void Path::addEllipse(const SizeF& size)
{
    detach();

    const PointF& pos = _pathData->currentPosition();

    const Pt::Gfx::PointF p1(pos.x(), pos.y() +  size.height() / 2);
    const Pt::Gfx::PointF p2(pos.x() + size.width(), pos.y() + size.height() / 2);

    moveTo(p1);
    arcTo( p2, size.height()/2 );

    moveTo(p2);
    arcTo( p1, size.height()/2 );

    close();
}


void Path::addPie(const SizeF& size, float degBegin, float degEnd)
{
    detach();

    const double pi = 3.14159265358979323846;
    const double toRad = pi / 180.0;

    const double ox = _pathData->currentPosition().x();
    const double oy = _pathData->currentPosition().y();
    const double cx = ox + size.width()  * 0.5;
    const double cy = oy + size.height() * 0.5;
    const double rx = size.width()  * 0.5;
    const double ry = size.height() * 0.5;

    const double aBegin = degBegin * toRad;
    const double aEnd   = degEnd   * toRad;

    const double startX = cx + rx * std::cos(aBegin);
    const double startY = cy + ry * std::sin(aBegin);
    const double endX   = cx + rx * std::cos(aEnd);
    const double endY   = cy + ry * std::sin(aEnd);

    // start at center, line to arc start, arc to arc end, close back to center
    moveTo(PointF(cx, cy));
    lineTo(PointF(startX, startY));
    arcTo(PointF(endX, endY), ry);
    close();
}


void Path::addChord(const SizeF& size, float degBegin, float degEnd)
{
    detach();

    const double pi = 3.14159265358979323846;
    const double toRad = pi / 180.0;

    const double ox = _pathData->currentPosition().x();
    const double oy = _pathData->currentPosition().y();
    const double cx = ox + size.width()  * 0.5;
    const double cy = oy + size.height() * 0.5;
    const double rx = size.width()  * 0.5;
    const double ry = size.height() * 0.5;

    const double aBegin = degBegin * toRad;
    const double aEnd   = degEnd   * toRad;

    const double startX = cx + rx * std::cos(aBegin);
    const double startY = cy + ry * std::sin(aBegin);
    const double endX   = cx + rx * std::cos(aEnd);
    const double endY   = cy + ry * std::sin(aEnd);

    // arc from start to end, then close (straight line back to arc start)
    moveTo(PointF(startX, startY));
    arcTo(PointF(endX, endY), ry);
    close();
}


void Path::transform(const Transform& tform)
{
    detach();

    _pathData->transform(tform);
}


void Path::toPolygons(std::vector<Polygon>& polygons, float tolerance) const
{
    Polygon polygon;

    for(PathIterator it = _pathData->begin(); it != _pathData->end(); ++it)
    {
        const PathElement& elem = *it;

        if( elem.type() == Path::Close )
        {
            polygons.push_back(polygon);
            polygon.clear();
        }
        else if( elem.type() != Path::MoveTo )
        {
            elem.flatten(polygon, tolerance);
        }
    }

    if( ! polygon.empty() )
        polygon.push_back( polygon.at(0) );
}


void PathElement::flatten(Polygon& points, double tolerance) const
{
    switch( _entry->type() )
    {
        case Path::LineTo:
        {
            if( points.empty() )
                points.push_back( position() );

            points.push_back( point(0) );
            break;
        }

        case Path::QuadTo:
        {
            const PointF& c1 = point(0);
            const PointF& to = point(1);
            quadraticBezierToPoints(points, position().x(), position().y(),
                                    c1.x(), c1.y(),
                                    to.x(), to.y(), tolerance);
            break;
        }

        case Path::CubicTo:
        {
            const PointF& c1 = point(0);
            const PointF& c2 = point(1);
            const PointF& to = point(2);

            cubicBezierToPoints(points, position().x(), position().y(),
                                c1.x(), c1.y(), c2.x(), c2.y(),
                                to.x(), to.y(), tolerance);
            break;
        }

        default:
            break;
    }
}


void PathData::append(const PathData& path)
{
    _entries.insert(_entries.end(), 
                    path._entries.cbegin(), path._entries.end());

    _points.insert(_points.end(), 
                    path._points.cbegin(), path._points.end());
}


void PathData::moveTo(const PointF& to)
{
    _entries.push_back( PathEntry(Path::MoveTo, 1) );
    _points.push_back(to);
    _start = to;
    setCurrentPosition(to);
}


void PathData::lineTo(const PointF& to)
{
    _entries.push_back( PathEntry(Path::LineTo, 1) );
    _points.push_back(to);

    setCurrentPosition(to);
}


void PathData::quadTo(const PointF& cp, const PointF& to)
{
    _entries.push_back( PathEntry(Path::QuadTo, 2) );
    _points.push_back(cp);
    _points.push_back(to);

    setCurrentPosition(to);
}


void PathData::cubicTo(const PointF& cp1, const PointF& cp2, const PointF& to)
{
    _entries.push_back( PathEntry(Path::CubicTo, 3) );
    _points.push_back(cp1);
    _points.push_back(cp2);
    _points.push_back(to);

    setCurrentPosition(to);
}


void PathData::close()
{
    double epsilon = 0.001;
    bool closeX = std::abs( _start.x() - _position.x() ) > epsilon;
    bool closeY = std::abs( _start.y() - _position.y() ) > epsilon;

    if( closeX || closeY )
    {
        lineTo(_start);
    }

    _entries.push_back( PathEntry(Path::Close, 0) );
}


void PathData::transform(const Transform& tform)
{
    for(std::vector<PointF>::iterator it = _points.begin(); it != _points.end(); ++it)
    {
        *it = tform * *it;
    }
}

} // namespace

} // namespace
