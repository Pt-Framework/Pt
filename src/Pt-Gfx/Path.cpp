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
#include <algorithm>

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

// Returns true if two doubles are close enough to be treated as equal.
inline bool pathIsNear(double a, double b)
{
    return std::fabs(a - b) < 1e-10;
}

// Tests a single line segment [x0,y0]->[x1,y1] against a horizontal ray
// cast from pt=(px,py) to the right.  Returns the signed winding delta
// (+1 upward, -1 downward, 0 no crossing). Uses a half-open y-interval
// so shared endpoints between adjacent segments are counted exactly once.
inline int pathLineRayCast(double x0, double y0, double x1, double y1,
                           double px, double py)
{
    const double dx = x1 - x0;
    const double dy = y1 - y0;

    if (dy > 0.0)
    {
        if (py >= y0 && py < y1)
        {
            const double ix = x0 + (py - y0) * dx / dy;
            return (px >= ix) ? 1 : 0;
        }
    }
    else if (dy < 0.0)
    {
        if (py >= y1 && py < y0)
        {
            const double ix = x0 + (py - y0) * dx / dy;
            return (px >= ix) ? -1 : 0;
        }
    }

    return 0;
}

// Numerically stable quadratic root solver.
// Returns roots in [tMin, tMax], sorted ascending, count in return value.
// Based on the stable form: q = -0.5*(b + sign(b)*sqrt(disc)); roots q/a, c/q.
inline std::size_t pathQuadRoots(double dst[2],
                                  double a, double b, double c,
                                  double tMin, double tMax)
{
    const double disc = b * b - 4.0 * a * c;
    if (disc < 0.0)
        return 0;

    const double s = std::sqrt(disc);
    const double q = -0.5 * (b + (b >= 0.0 ? s : -s));

    double t0 = (a != 0.0) ? q / a : tMax + 1.0;
    double t1 = (q != 0.0) ? c / q : tMax + 1.0;

    if (t0 > t1)
        std::swap(t0, t1);

    std::size_t n = 0;
    if (t0 >= tMin && t0 <= tMax)
        dst[n++] = t0;
    if (t1 > t0 && t1 >= tMin && t1 <= tMax)
        dst[n++] = t1;

    return n;
}

// Cubic root solver using Cardano's formula.
// Returns roots in [tMin, tMax], count in return value (0-3).
std::size_t pathCubicRoots(double dst[3],
                            double a, double b, double c, double d,
                            double tMin, double tMax)
{
    // Degenerate to quadratic when leading coefficient is zero.
    if (pathIsNear(a, 0.0))
        return pathQuadRoots(dst, b, c, d, tMin, tMax);

    const double k1Div3  = 1.0 / 3.0;
    const double k1Div6  = 1.0 / 6.0;
    const double k1Div9  = 1.0 / 9.0;
    const double k1Div27 = 1.0 / 27.0;

    // Normalize to x^3 + Ax^2 + Bx + C = 0.
    const double na = b / a;
    const double nb = c / a;
    const double nc = d / a;

    // Eliminate quadratic term via x = y - A/3.
    const double sa = na * na;
    const double p  = -k1Div9  * sa + k1Div3 * nb;
    const double q  = (k1Div27 * sa - k1Div6 * nb) * na + 0.5 * nc;

    const double p3 = p * p * p;
    const double disc = q * q + p3;
    const double sub  = -k1Div3 * na;

    double roots[3];
    std::size_t nRoots = 0;

    if (pathIsNear(disc, 0.0))
    {
        if (pathIsNear(q, 0.0))
        {
            roots[0] = sub;
            nRoots = 1;
        }
        else
        {
            const double u = std::cbrt(-q);
            roots[0] = sub + 2.0 * u;
            roots[1] = sub - u;
            nRoots = 2;
            if (roots[0] > roots[1])
                std::swap(roots[0], roots[1]);
        }
    }
    else if (disc < 0.0)
    {
        const double phi = k1Div3 * std::acos(-q / std::sqrt(-p3));
        const double t   = 2.0 * std::sqrt(-p);
        const double pi  = 3.14159265358979323846;

        roots[0] = sub + t * std::cos(phi);
        roots[1] = sub - t * std::cos(phi + pi / 3.0);
        roots[2] = sub - t * std::cos(phi - pi / 3.0);
        nRoots = 3;

        if (roots[0] > roots[1]) std::swap(roots[0], roots[1]);
        if (roots[1] > roots[2]) std::swap(roots[1], roots[2]);
        if (roots[0] > roots[1]) std::swap(roots[0], roots[1]);
    }
    else
    {
        const double sqrtDisc = std::sqrt(disc);
        const double u =  std::cbrt(sqrtDisc - q);
        const double v = -std::cbrt(sqrtDisc + q);
        roots[0] = sub + u + v;
        nRoots = 1;
    }

    std::size_t n = 0;
    for (std::size_t i = 0; i < nRoots; ++i)
    {
        if (roots[i] >= tMin && roots[i] <= tMax)
            dst[n++] = roots[i];
    }
    return n;
}

// De Casteljau split of a quadratic at parameter t.
inline void splitQuadAt(const Pt::Gfx::PointF p[3], double t,
                         Pt::Gfx::PointF left[3], Pt::Gfx::PointF right[3])
{
    const Pt::Gfx::PointF p01(p[0].x() + t * (p[1].x() - p[0].x()),
                               p[0].y() + t * (p[1].y() - p[0].y()));
    const Pt::Gfx::PointF p12(p[1].x() + t * (p[2].x() - p[1].x()),
                               p[1].y() + t * (p[2].y() - p[1].y()));
    const Pt::Gfx::PointF pm( p01.x() + t * (p12.x() - p01.x()),
                               p01.y() + t * (p12.y() - p01.y()));

    left[0]  = p[0];  left[1]  = p01;  left[2]  = pm;
    right[0] = pm;    right[1] = p12;  right[2] = p[2];
}

// De Casteljau split of a cubic at parameter t.
inline void splitCubicAt(const Pt::Gfx::PointF p[4], double t,
                          Pt::Gfx::PointF left[4], Pt::Gfx::PointF right[4])
{
    const Pt::Gfx::PointF p01(p[0].x() + t * (p[1].x() - p[0].x()),
                               p[0].y() + t * (p[1].y() - p[0].y()));
    const Pt::Gfx::PointF p12(p[1].x() + t * (p[2].x() - p[1].x()),
                               p[1].y() + t * (p[2].y() - p[1].y()));
    const Pt::Gfx::PointF p23(p[2].x() + t * (p[3].x() - p[2].x()),
                               p[2].y() + t * (p[3].y() - p[2].y()));

    const Pt::Gfx::PointF p012(p01.x() + t * (p12.x() - p01.x()),
                                p01.y() + t * (p12.y() - p01.y()));
    const Pt::Gfx::PointF p123(p12.x() + t * (p23.x() - p12.x()),
                                p12.y() + t * (p23.y() - p12.y()));
    const Pt::Gfx::PointF pm(  p012.x() + t * (p123.x() - p012.x()),
                                p012.y() + t * (p123.y() - p012.y()));

    left[0]  = p[0];   left[1]  = p01;   left[2]  = p012;  left[3]  = pm;
    right[0] = pm;     right[1] = p123;  right[2] = p23;   right[3] = p[3];
}

// Ray-cast a single monotone (in Y) quadratic segment.
// q[0] and q[2] must bracket px.y with q[0].y <= q[2].y (or vice versa).
inline int pathMonotoneQuadRayCast(const Pt::Gfx::PointF q[3],
                                    double px, double py)
{
    const double minY = std::min(q[0].y(), q[2].y());
    const double maxY = std::max(q[0].y(), q[2].y());

    if (py < minY || py >= maxY)
        return 0;

    const int dir = (q[0].y() < q[2].y()) ? 1 : -1;

    // Quadratic coefficients for Y: A*t^2 + B*t + C - py = 0.
    const double ay = q[2].y() - 2.0 * q[1].y() + q[0].y();
    const double by = 2.0 * (q[1].y() - q[0].y());
    const double cy = q[0].y() - py;

    double ti[2];
    double ix;
    if (pathQuadRoots(ti, ay, by, cy, 0.0, 1.0) >= 1)
    {
        const double ax = q[2].x() - 2.0 * q[1].x() + q[0].x();
        const double bx = 2.0 * (q[1].x() - q[0].x());
        ix = (ax * ti[0] + bx) * ti[0] + q[0].x();
    }
    else
    {
        // Fallback: use endpoint whose y is nearest to py.
        ix = (py - minY < maxY - py) ? q[0].x() : q[2].x();
    }

    return (px >= ix) ? dir : 0;
}

// Ray-cast a single monotone (in Y) cubic segment.
inline int pathMonotoneCubicRayCast(const Pt::Gfx::PointF c[4],
                                     double px, double py)
{
    const double minY = std::min(c[0].y(), c[3].y());
    const double maxY = std::max(c[0].y(), c[3].y());

    if (py < minY || py >= maxY)
        return 0;

    const int dir = (c[0].y() < c[3].y()) ? 1 : -1;

    // Cubic coefficients: A*t^3 + B*t^2 + C*t + D - py = 0.
    const double v1y = c[1].y() - c[0].y();
    const double v2y = c[2].y() - c[1].y();
    const double v3y = c[3].y() - c[2].y();
    const double ay  = v3y - v2y - v2y + v1y;
    const double by  = 3.0 * (v2y - v1y);
    const double cy  = 3.0 * v1y;
    const double dy  = c[0].y() - py;

    double ti[3];
    double ix;
    if (pathCubicRoots(ti, ay, by, cy, dy, 0.0, 1.0) >= 1)
    {
        const double v1x = c[1].x() - c[0].x();
        const double v2x = c[2].x() - c[1].x();
        const double v3x = c[3].x() - c[2].x();
        const double ax  = v3x - v2x - v2x + v1x;
        const double bx  = 3.0 * (v2x - v1x);
        const double cx  = 3.0 * v1x;
        ix = ((ax * ti[0] + bx) * ti[0] + cx) * ti[0] + c[0].x();
    }
    else
    {
        ix = (py - minY < maxY - py) ? c[0].x() : c[3].x();
    }

    return (px >= ix) ? dir : 0;
}

// Ray-cast a full (possibly non-monotone) quadratic.
// Splits at Y-extremum if present, then calls pathMonotoneQuadRayCast.
int pathQuadHit(const Pt::Gfx::PointF p[3], double px, double py)
{
    const double minY = std::min({p[0].y(), p[1].y(), p[2].y()});
    const double maxY = std::max({p[0].y(), p[1].y(), p[2].y()});

    if (py < minY || py > maxY)
        return 0;

    // Degenerate: all y-values nearly equal -> treat as line.
    if (pathIsNear(p[0].y(), p[1].y()) && pathIsNear(p[1].y(), p[2].y()))
        return pathLineRayCast(p[0].x(), p[0].y(), p[2].x(), p[2].y(), px, py);

    // Find Y-extremum parameter: t = (p0.y - p1.y) / (p0.y - 2*p1.y + p2.y).
    const double denom = p[0].y() - 2.0 * p[1].y() + p[2].y();
    int winding = 0;

    if (!pathIsNear(denom, 0.0))
    {
        const double tExt = (p[0].y() - p[1].y()) / denom;
        if (tExt > 0.0 && tExt < 1.0)
        {
            // Split into two monotone sub-quads on the stack.
            Pt::Gfx::PointF left[3];
            Pt::Gfx::PointF right[3];
            splitQuadAt(p, tExt, left, right);
            winding += pathMonotoneQuadRayCast(left,  px, py);
            winding += pathMonotoneQuadRayCast(right, px, py);
            return winding;
        }
    }

    // Already monotone.
    winding += pathMonotoneQuadRayCast(p, px, py);
    return winding;
}

// Ray-cast a full (possibly non-monotone) cubic.
// Splits at Y-extrema (up to 2), then calls pathMonotoneCubicRayCast.
int pathCubicHit(const Pt::Gfx::PointF p[4], double px, double py)
{
    const double minY = std::min({p[0].y(), p[1].y(), p[2].y(), p[3].y()});
    const double maxY = std::max({p[0].y(), p[1].y(), p[2].y(), p[3].y()});

    if (py < minY || py > maxY)
        return 0;

    // Degenerate: all y-values nearly equal -> treat as line.
    if (pathIsNear(p[0].y(), p[1].y()) &&
        pathIsNear(p[1].y(), p[2].y()) &&
        pathIsNear(p[2].y(), p[3].y()))
    {
        return pathLineRayCast(p[0].x(), p[0].y(), p[3].x(), p[3].y(), px, py);
    }

    // Derivative coefficients (quadratic): 3A*t^2 + 2B*t + C.
    const double v1y = p[1].y() - p[0].y();
    const double v2y = p[2].y() - p[1].y();
    const double v3y = p[3].y() - p[2].y();
    const double da  = 3.0 * (v3y - v2y - v2y + v1y);
    const double db  = 6.0 * (v2y - v1y);
    const double dc  = 3.0 * v1y;

    double ts[2];
    const std::size_t nExt = pathQuadRoots(ts, da, db, dc, 0.0, 1.0);

    if (nExt == 0)
        return pathMonotoneCubicRayCast(p, px, py);

    int winding = 0;

    if (nExt == 1)
    {
        Pt::Gfx::PointF left[4];
        Pt::Gfx::PointF right[4];
        splitCubicAt(p, ts[0], left, right);
        winding += pathMonotoneCubicRayCast(left,  px, py);
        winding += pathMonotoneCubicRayCast(right, px, py);
    }
    else
    {
        // Two extrema: split into three monotone segments.
        Pt::Gfx::PointF seg0[4];
        Pt::Gfx::PointF seg1[4];
        Pt::Gfx::PointF seg2[4];
        Pt::Gfx::PointF tmp[4];

        splitCubicAt(p, ts[0], seg0, tmp);
        // Reparametrise ts[1] relative to the right part.
        const double t1rel = (ts[1] - ts[0]) / (1.0 - ts[0]);
        splitCubicAt(tmp, t1rel, seg1, seg2);

        winding += pathMonotoneCubicRayCast(seg0, px, py);
        winding += pathMonotoneCubicRayCast(seg1, px, py);
        winding += pathMonotoneCubicRayCast(seg2, px, py);
    }

    return winding;
}

// ---------------------------------------------------------------------------
// Rect-edge intersection helpers for contains(RectF).
// ---------------------------------------------------------------------------

// Returns true if line segment [p0→p1] crosses the horizontal edge y=hy
// within x in [xMin, xMax].
inline bool pathLineIntersectsH(double x0, double y0, double x1, double y1,
                                 double hy, double xMin, double xMax)
{
    const double dy = y1 - y0;
    if (pathIsNear(dy, 0.0))
        return false;
    const double t = (hy - y0) / dy;
    if (t < 0.0 || t > 1.0)
        return false;
    const double ix = x0 + t * (x1 - x0);
    return ix >= xMin && ix <= xMax;
}

// Returns true if line segment [p0→p1] crosses the vertical edge x=vx
// within y in [yMin, yMax].
inline bool pathLineIntersectsV(double x0, double y0, double x1, double y1,
                                 double vx, double yMin, double yMax)
{
    const double dx = x1 - x0;
    if (pathIsNear(dx, 0.0))
        return false;
    const double t = (vx - x0) / dx;
    if (t < 0.0 || t > 1.0)
        return false;
    const double iy = y0 + t * (y1 - y0);
    return iy >= yMin && iy <= yMax;
}

// Returns true if the quadratic segment p[3] crosses any of the four
// axis-aligned edges of the rect [xMin,xMax] x [yMin,yMax].
bool pathQuadIntersectsRect(const Pt::Gfx::PointF p[3],
                             double xMin, double yMin,
                             double xMax, double yMax)
{
    // Control-polygon BBox fast reject.
    const double qMinX = std::min({p[0].x(), p[1].x(), p[2].x()});
    const double qMaxX = std::max({p[0].x(), p[1].x(), p[2].x()});
    const double qMinY = std::min({p[0].y(), p[1].y(), p[2].y()});
    const double qMaxY = std::max({p[0].y(), p[1].y(), p[2].y()});

    if (qMaxX < xMin || qMinX > xMax || qMaxY < yMin || qMinY > yMax)
        return false;

    // Quadratic polynomial coefficients: curve(t) = A*t^2 + B*t + C.
    const double ax = p[2].x() - 2.0 * p[1].x() + p[0].x();
    const double bx = 2.0 * (p[1].x() - p[0].x());
    const double cx = p[0].x();

    const double ay = p[2].y() - 2.0 * p[1].y() + p[0].y();
    const double by = 2.0 * (p[1].y() - p[0].y());
    const double cy = p[0].y();

    double ti[2];
    std::size_t n;

    // Top edge: curve.y(t) = yMin  →  check curve.x(t) in [xMin, xMax].
    n = pathQuadRoots(ti, ay, by, cy - yMin, 0.0, 1.0);
    for (std::size_t i = 0; i < n; ++i)
    {
        const double ix = (ax * ti[i] + bx) * ti[i] + cx;
        if (ix >= xMin && ix <= xMax)
            return true;
    }

    // Bottom edge: curve.y(t) = yMax.
    n = pathQuadRoots(ti, ay, by, cy - yMax, 0.0, 1.0);
    for (std::size_t i = 0; i < n; ++i)
    {
        const double ix = (ax * ti[i] + bx) * ti[i] + cx;
        if (ix >= xMin && ix <= xMax)
            return true;
    }

    // Left edge: curve.x(t) = xMin  →  check curve.y(t) in [yMin, yMax].
    n = pathQuadRoots(ti, ax, bx, cx - xMin, 0.0, 1.0);
    for (std::size_t i = 0; i < n; ++i)
    {
        const double iy = (ay * ti[i] + by) * ti[i] + cy;
        if (iy >= yMin && iy <= yMax)
            return true;
    }

    // Right edge: curve.x(t) = xMax.
    n = pathQuadRoots(ti, ax, bx, cx - xMax, 0.0, 1.0);
    for (std::size_t i = 0; i < n; ++i)
    {
        const double iy = (ay * ti[i] + by) * ti[i] + cy;
        if (iy >= yMin && iy <= yMax)
            return true;
    }

    return false;
}

// Returns true if the cubic segment p[4] crosses any of the four
// axis-aligned edges of the rect [xMin,xMax] x [yMin,yMax].
bool pathCubicIntersectsRect(const Pt::Gfx::PointF p[4],
                              double xMin, double yMin,
                              double xMax, double yMax)
{
    // Control-polygon BBox fast reject.
    const double qMinX = std::min({p[0].x(), p[1].x(), p[2].x(), p[3].x()});
    const double qMaxX = std::max({p[0].x(), p[1].x(), p[2].x(), p[3].x()});
    const double qMinY = std::min({p[0].y(), p[1].y(), p[2].y(), p[3].y()});
    const double qMaxY = std::max({p[0].y(), p[1].y(), p[2].y(), p[3].y()});

    if (qMaxX < xMin || qMinX > xMax || qMaxY < yMin || qMinY > yMax)
        return false;

    // Cubic polynomial coefficients: curve(t) = A*t^3 + B*t^2 + C*t + D.
    const double v1x = p[1].x() - p[0].x();
    const double v2x = p[2].x() - p[1].x();
    const double v3x = p[3].x() - p[2].x();
    const double ax  = v3x - v2x - v2x + v1x;
    const double bx  = 3.0 * (v2x - v1x);
    const double cx  = 3.0 * v1x;
    const double dx  = p[0].x();

    const double v1y = p[1].y() - p[0].y();
    const double v2y = p[2].y() - p[1].y();
    const double v3y = p[3].y() - p[2].y();
    const double ay  = v3y - v2y - v2y + v1y;
    const double by  = 3.0 * (v2y - v1y);
    const double cy  = 3.0 * v1y;
    const double dy  = p[0].y();

    double ti[3];
    std::size_t n;

    // Top edge: curve.y(t) = yMin.
    n = pathCubicRoots(ti, ay, by, cy, dy - yMin, 0.0, 1.0);
    for (std::size_t i = 0; i < n; ++i)
    {
        const double ix = ((ax * ti[i] + bx) * ti[i] + cx) * ti[i] + dx;
        if (ix >= xMin && ix <= xMax)
            return true;
    }

    // Bottom edge: curve.y(t) = yMax.
    n = pathCubicRoots(ti, ay, by, cy, dy - yMax, 0.0, 1.0);
    for (std::size_t i = 0; i < n; ++i)
    {
        const double ix = ((ax * ti[i] + bx) * ti[i] + cx) * ti[i] + dx;
        if (ix >= xMin && ix <= xMax)
            return true;
    }

    // Left edge: curve.x(t) = xMin.
    n = pathCubicRoots(ti, ax, bx, cx, dx - xMin, 0.0, 1.0);
    for (std::size_t i = 0; i < n; ++i)
    {
        const double iy = ((ay * ti[i] + by) * ti[i] + cy) * ti[i] + dy;
        if (iy >= yMin && iy <= yMax)
            return true;
    }

    // Right edge: curve.x(t) = xMax.
    n = pathCubicRoots(ti, ax, bx, cx, dx - xMax, 0.0, 1.0);
    for (std::size_t i = 0; i < n; ++i)
    {
        const double iy = ((ay * ti[i] + by) * ti[i] + cy) * ti[i] + dy;
        if (iy >= yMin && iy <= yMax)
            return true;
    }

    return false;
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


bool Path::contains(const PointF& point, FillRule rule) const
{
    if( _pathData->isEmpty() )
        return false;

    const double px = point.x();
    const double py = point.y();

    int winding = 0;

    bool hasMoveTo = false;
    PointF start;
    PointF lastPos;

    for( PathIterator it = _pathData->begin(); it != _pathData->end(); ++it )
    {
        const PathElement& elem = *it;

        switch( elem.type() )
        {
            case Path::MoveTo:
            {
                if( hasMoveTo )
                {
                    // Implicitly close the previous subpath.
                    winding += pathLineRayCast(lastPos.x(), lastPos.y(),
                                               start.x(),   start.y(),
                                               px, py);
                }

                start   = elem.point(0);
                lastPos = elem.point(0);
                hasMoveTo = true;
                break;
            }

            case Path::LineTo:
            {
                const PointF& to = elem.point(0);
                winding += pathLineRayCast(elem.position().x(), elem.position().y(),
                                           to.x(), to.y(),
                                           px, py);
                lastPos = to;
                break;
            }

            case Path::QuadTo:
            {
                const Pt::Gfx::PointF q[3] = {
                    elem.position(), elem.point(0), elem.point(1)
                };
                winding += pathQuadHit(q, px, py);
                lastPos = elem.point(1);
                break;
            }

            case Path::CubicTo:
            {
                const Pt::Gfx::PointF c[4] = {
                    elem.position(), elem.point(0), elem.point(1), elem.point(2)
                };
                winding += pathCubicHit(c, px, py);
                lastPos = elem.point(2);
                break;
            }

            case Path::Close:
            {
                // PathData::close() already inserts a LineTo(_start) before the
                // Close entry, so the closing segment has been counted above.
                // Reset hasMoveTo so the next MoveTo does not add a duplicate
                // implicit-close line.
                hasMoveTo = false;
                lastPos   = start;
                break;
            }
        }
    }

    // Implicitly close any open trailing subpath.
    if( hasMoveTo )
    {
        winding += pathLineRayCast(lastPos.x(), lastPos.y(),
                                   start.x(),   start.y(),
                                   px, py);
    }

    if( rule == FillRule::EvenOdd )
        return (winding & 1) != 0;

    return winding != 0;
}


bool Path::contains(const RectF& rect, FillRule rule) const
{
    if( _pathData->isEmpty() || rect.isNull() )
        return false;

    // BBox fast reject: path and rect must overlap.
    const RectF bbox = boundingRect();
    if( bbox.right()  < rect.left()  || rect.right()  < bbox.left() ||
        bbox.bottom() < rect.top()   || rect.bottom() < bbox.top() )
        return false;

    // All 4 corners of the rect must lie inside the filled area.
    if( !contains(rect.topLeft(),     rule) ) return false;
    if( !contains(rect.topRight(),    rule) ) return false;
    if( !contains(rect.bottomLeft(),  rule) ) return false;
    if( !contains(rect.bottomRight(), rule) ) return false;

    const double xMin = rect.left();
    const double xMax = rect.right();
    const double yMin = rect.top();
    const double yMax = rect.bottom();

    // No path segment may cross a rect edge.
    bool hasMoveTo = false;
    PointF start;
    PointF lastPos;

    for( PathIterator it = _pathData->begin(); it != _pathData->end(); ++it )
    {
        const PathElement& elem = *it;

        switch( elem.type() )
        {
            case Path::MoveTo:
            {
                if( hasMoveTo )
                {
                    if( pathLineIntersectsH(lastPos.x(), lastPos.y(), start.x(), start.y(), yMin, xMin, xMax) ||
                        pathLineIntersectsH(lastPos.x(), lastPos.y(), start.x(), start.y(), yMax, xMin, xMax) ||
                        pathLineIntersectsV(lastPos.x(), lastPos.y(), start.x(), start.y(), xMin, yMin, yMax) ||
                        pathLineIntersectsV(lastPos.x(), lastPos.y(), start.x(), start.y(), xMax, yMin, yMax) )
                        return false;
                }

                start     = elem.point(0);
                lastPos   = elem.point(0);
                hasMoveTo = true;
                break;
            }

            case Path::LineTo:
            {
                const PointF& from = elem.position();
                const PointF& to   = elem.point(0);

                if( pathLineIntersectsH(from.x(), from.y(), to.x(), to.y(), yMin, xMin, xMax) ||
                    pathLineIntersectsH(from.x(), from.y(), to.x(), to.y(), yMax, xMin, xMax) ||
                    pathLineIntersectsV(from.x(), from.y(), to.x(), to.y(), xMin, yMin, yMax) ||
                    pathLineIntersectsV(from.x(), from.y(), to.x(), to.y(), xMax, yMin, yMax) )
                    return false;

                lastPos = to;
                break;
            }

            case Path::QuadTo:
            {
                const PointF q[3] = { elem.position(), elem.point(0), elem.point(1) };

                if( pathQuadIntersectsRect(q, xMin, yMin, xMax, yMax) )
                    return false;

                lastPos = elem.point(1);
                break;
            }

            case Path::CubicTo:
            {
                const PointF c[4] = { elem.position(), elem.point(0), elem.point(1), elem.point(2) };

                if( pathCubicIntersectsRect(c, xMin, yMin, xMax, yMax) )
                    return false;

                lastPos = elem.point(2);
                break;
            }

            case Path::Close:
            {
                hasMoveTo = false;
                lastPos   = start;
                break;
            }
        }
    }

    // Implicitly close any open trailing subpath.
    if( hasMoveTo )
    {
        if( pathLineIntersectsH(lastPos.x(), lastPos.y(), start.x(), start.y(), yMin, xMin, xMax) ||
            pathLineIntersectsH(lastPos.x(), lastPos.y(), start.x(), start.y(), yMax, xMin, xMax) ||
            pathLineIntersectsV(lastPos.x(), lastPos.y(), start.x(), start.y(), xMin, yMin, yMax) ||
            pathLineIntersectsV(lastPos.x(), lastPos.y(), start.x(), start.y(), xMax, yMin, yMax) )
            return false;
    }

    return true;
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


void Path::addPolyline(const PointF* points, std::size_t count)
{
    if (count == 0)
        return;

    moveTo(points[0]);
    for (std::size_t i = 1; i < count; ++i)
    {
        lineTo(points[i]);
    }
}


void Path::addPolygon(const PointF* points, std::size_t count)
{
    if (count == 0)
        return;

    moveTo(points[0]);
    for (std::size_t i = 1; i < count; ++i)
    {
        lineTo(points[i]);
    }
    close();
}


void Path::transform(const Transform& tform)
{
    detach();

    _pathData->transform(tform);
}


Path Path::toTransformed(const Transform& tform) const
{
    Path result = *this;
    result.transform(tform);
    return result;
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
