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


void appendArc(Pt::Gfx::Path& path,
               double cx, double cy,
               double rx, double ry,
               double radBegin, double radEnd)
{
    const double pi = 3.14159265358979323846;
    const double sweep = radEnd - radBegin;

    if (sweep == 0.0)
        return;

    const int segments = static_cast<int>(std::ceil(std::fabs(sweep) / (pi * 0.5)));
    const double segAngle = sweep / segments;
    const double k = (4.0 / 3.0) * std::tan(segAngle * 0.25);

    double angle = radBegin;
    for (int i = 0; i < segments; ++i)
    {
        const double cosA = std::cos(angle);
        const double sinA = std::sin(angle);
        const double cosB = std::cos(angle + segAngle);
        const double sinB = std::sin(angle + segAngle);

        const Pt::Gfx::PointF cp1(cx + rx * (cosA - k * sinA),
                                   cy + ry * (sinA + k * cosA));
        const Pt::Gfx::PointF cp2(cx + rx * (cosB + k * sinB),
                                   cy + ry * (sinB - k * cosB));
        const Pt::Gfx::PointF end(cx + rx * cosB,
                                   cy + ry * sinB);

        path.cubicTo(cp1, cp2, end);
        angle += segAngle;
    }
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


void Path::arcTo(const PointF& topLeft, const SizeF& size,
                 double degBegin, double degEnd)
{
    detach();

    const double pi = 3.14159265358979323846;
    const double toRad = pi / 180.0;

    const double cx = topLeft.x() + size.width()  * 0.5;
    const double cy = topLeft.y() + size.height() * 0.5;
    const double rx = size.width()  * 0.5;
    const double ry = size.height() * 0.5;

    const double radBegin = degBegin * toRad;
    const double radEnd   = degEnd   * toRad;

    const PointF startPt(cx + rx * std::cos(radBegin),
                         cy + ry * std::sin(radBegin));

    if( isEmpty() )
        moveTo(startPt);
    else
        lineTo(startPt);

    appendArc(*this, cx, cy, rx, ry, radBegin, radEnd);
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


void Path::addRect(const RectF& rect)
{
    detach();

    const double x = rect.x();
    const double y = rect.y();
    const double w = rect.width();
    const double h = rect.height();

    moveTo(PointF(x, y));
    lineTo(PointF(x, y + h));
    lineTo(PointF(x + w, y + h));
    lineTo(PointF(x + w, y));
    close();
}


void Path::addRoundedRect(const RectF& rect, float radius)
{
    double x = rect.x();
    double y = rect.y();
    double w = rect.width();
    double h = rect.height();
    double d = 2.0 * radius;

    moveTo(PointF(x + radius, y));
    arcTo(PointF(x + w - d, y), SizeF(d, d), 270.0, 360.0);
    arcTo(PointF(x + w - d, y + h - d), SizeF(d, d), 0.0, 90.0);
    arcTo(PointF(x, y + h - d), SizeF(d, d), 90.0, 180.0);
    arcTo(PointF(x, y), SizeF(d, d), 180.0, 270.0);
    close();
}


void Path::addEllipse(const PointF& topLeft, const SizeF& size)
{
    addChord(topLeft, size, 0.0, 360.0);
}


void Path::addArc(const PointF& topLeft, const SizeF& size,
                  double degBegin, double degEnd)
{
    detach();

    const double pi = 3.14159265358979323846;
    const double toRad = pi / 180.0;

    const double cx = topLeft.x() + size.width()  * 0.5;
    const double cy = topLeft.y() + size.height() * 0.5;
    const double rx = size.width()  * 0.5;
    const double ry = size.height() * 0.5;

    const double radBegin = degBegin * toRad;
    const double radEnd   = degEnd   * toRad;

    moveTo(PointF(cx + rx * std::cos(radBegin),
                  cy + ry * std::sin(radBegin)));

    appendArc(*this, cx, cy, rx, ry, radBegin, radEnd);
}


void Path::addPie(const PointF& topLeft, const SizeF& size,
                  double degBegin, double degEnd)
{
    const double cx = topLeft.x() + size.width()  * 0.5;
    const double cy = topLeft.y() + size.height() * 0.5;

    addArc(topLeft, size, degBegin, degEnd);
    lineTo(PointF(cx, cy));
    close();
}


void Path::addChord(const PointF& topLeft, const SizeF& size,
                    double degBegin, double degEnd)
{
    addArc(topLeft, size, degBegin, degEnd);
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
