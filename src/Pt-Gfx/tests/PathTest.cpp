/* Copyright (C) 2024 Marc Boris Duerner

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

#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>

namespace Pt {

namespace Gfx {

class PathTest : public Pt::Unit::TestSuite
{
    public:
        PathTest()
          : Pt::Unit::TestSuite("Pt::Gfx::PathTest")
        {
            registerMethod("MoveTo",     *this, &PathTest::MoveTo);
            registerMethod("LineTo",     *this, &PathTest::LineTo);
            registerMethod("QuadTo",     *this, &PathTest::QuadTo);
            registerMethod("CubicTo",    *this, &PathTest::CubicTo);
            registerMethod("Close",      *this, &PathTest::Close);
            registerMethod("Clear",      *this, &PathTest::Clear);
            registerMethod("Size",       *this, &PathTest::Size);
            registerMethod("AddPath",       *this, &PathTest::AddPath);
            registerMethod("ToPolygon",     *this, &PathTest::ToPolygon);
            registerMethod("Transform",     *this, &PathTest::Transform);
            registerMethod("ToTransformed", *this, &PathTest::ToTransformed);
            registerMethod("BoundingRect",  *this, &PathTest::BoundingRect);
            registerMethod("ArcTo",         *this, &PathTest::ArcTo);
            registerMethod("AddRect",       *this, &PathTest::AddRect);
            registerMethod("AddPie",     *this, &PathTest::AddPie);
            registerMethod("AddChord",      *this, &PathTest::AddChord);
            registerMethod("AddArc",        *this, &PathTest::AddArc);
            registerMethod("AddRoundedRect", *this, &PathTest::AddRoundedRect);
            registerMethod("AddEllipse",    *this, &PathTest::AddEllipse);
            registerMethod("AddPolyline",   *this, &PathTest::AddPolyline);
            registerMethod("AddPolygon",    *this, &PathTest::AddPolygon);
            registerMethod("ContainsEmpty",             *this, &PathTest::ContainsEmpty);
            registerMethod("ContainsRect",              *this, &PathTest::ContainsRect);
            registerMethod("ContainsEllipse",           *this, &PathTest::ContainsEllipse);
            registerMethod("ContainsOnEdge",            *this, &PathTest::ContainsOnEdge);
            registerMethod("ContainsConcentricCircles", *this, &PathTest::ContainsConcentricCircles);
            registerMethod("ContainsRectInRect",        *this, &PathTest::ContainsRectInRect);
            registerMethod("ContainsRectOutside",       *this, &PathTest::ContainsRectOutside);
            registerMethod("ContainsRectLarger",        *this, &PathTest::ContainsRectLarger);
            registerMethod("ContainsRectInEllipse",     *this, &PathTest::ContainsRectInEllipse);
            registerMethod("ContainsRectCrossing",      *this, &PathTest::ContainsRectCrossing);
            registerMethod("IntersectsRectPartial",      *this, &PathTest::IntersectsRectPartial);
            registerMethod("IntersectsRectOutside",      *this, &PathTest::IntersectsRectOutside);
            registerMethod("IntersectsRectContains",     *this, &PathTest::IntersectsRectContains);
            registerMethod("IntersectsPathInsideRect",   *this, &PathTest::IntersectsPathInsideRect);
            registerMethod("IntersectsRectEllipse",      *this, &PathTest::IntersectsRectEllipse);
        }

        void MoveTo()
        {
            Path path;

            PT_UNIT_ASSERT(path.isEmpty());

            path.moveTo(PointF(3.0, 7.0));

            PT_UNIT_ASSERT(!path.isEmpty());
            PT_UNIT_ASSERT_NEAR(path.currentPosition().x(), 3.0);
            PT_UNIT_ASSERT_NEAR(path.currentPosition().y(), 7.0);

            path.moveTo(PointF(5.0, 9.0));

            PT_UNIT_ASSERT_NEAR(path.currentPosition().x(), 5.0);
            PT_UNIT_ASSERT_NEAR(path.currentPosition().y(), 9.0);

            Path::Iterator it = path.begin();
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 3.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 7.0);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 5.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 9.0);

            ++it;
            PT_UNIT_ASSERT(it == path.end());
        }

        void LineTo()
        {
            Path path;

            PT_UNIT_ASSERT(path.isEmpty());

            path.moveTo(PointF(1.0, 2.0));
            path.lineTo(PointF(4.0, 6.0));

            PT_UNIT_ASSERT(!path.isEmpty());
            PT_UNIT_ASSERT_NEAR(path.currentPosition().x(), 4.0);
            PT_UNIT_ASSERT_NEAR(path.currentPosition().y(), 6.0);

            path.lineTo(PointF(7.0, 8.0));

            PT_UNIT_ASSERT_NEAR(path.currentPosition().x(), 7.0);
            PT_UNIT_ASSERT_NEAR(path.currentPosition().y(), 8.0);

            Path::Iterator it = path.begin();
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 1.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 2.0);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 4.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 6.0);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 7.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 8.0);

            ++it;
            PT_UNIT_ASSERT(it == path.end());
        }

        void QuadTo()
        {
            Path path;

            PT_UNIT_ASSERT(path.isEmpty());

            path.moveTo(PointF(0.0, 0.0));
            path.quadTo(PointF(5.0, 10.0), PointF(10.0, 0.0));

            PT_UNIT_ASSERT(!path.isEmpty());
            PT_UNIT_ASSERT_NEAR(path.currentPosition().x(), 10.0);
            PT_UNIT_ASSERT_NEAR(path.currentPosition().y(), 0.0);

            path.quadTo(PointF(15.0, 10.0), PointF(20.0, 0.0));

            PT_UNIT_ASSERT_NEAR(path.currentPosition().x(), 20.0);
            PT_UNIT_ASSERT_NEAR(path.currentPosition().y(), 0.0);

            Path::Iterator it = path.begin();
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::QuadTo);
            PT_UNIT_ASSERT_EQUAL(it->size(), std::size_t(2));
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 5.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 10.0);
            PT_UNIT_ASSERT_NEAR(it->point(1).x(), 10.0);
            PT_UNIT_ASSERT_NEAR(it->point(1).y(), 0.0);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::QuadTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 15.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 10.0);
            PT_UNIT_ASSERT_NEAR(it->point(1).x(), 20.0);
            PT_UNIT_ASSERT_NEAR(it->point(1).y(), 0.0);

            ++it;
            PT_UNIT_ASSERT(it == path.end());
        }

        void CubicTo()
        {
            Path path;

            PT_UNIT_ASSERT(path.isEmpty());

            path.moveTo(PointF(0.0, 0.0));
            path.cubicTo(PointF(2.0, 8.0), PointF(8.0, 8.0), PointF(10.0, 0.0));

            PT_UNIT_ASSERT(!path.isEmpty());
            PT_UNIT_ASSERT_NEAR(path.currentPosition().x(), 10.0);
            PT_UNIT_ASSERT_NEAR(path.currentPosition().y(), 0.0);

            path.cubicTo(PointF(12.0, 8.0), PointF(18.0, 8.0), PointF(20.0, 0.0));

            PT_UNIT_ASSERT_NEAR(path.currentPosition().x(), 20.0);
            PT_UNIT_ASSERT_NEAR(path.currentPosition().y(), 0.0);

            Path::Iterator it = path.begin();
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::CubicTo);
            PT_UNIT_ASSERT_EQUAL(it->size(), std::size_t(3));
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 2.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 8.0);
            PT_UNIT_ASSERT_NEAR(it->point(1).x(), 8.0);
            PT_UNIT_ASSERT_NEAR(it->point(1).y(), 8.0);
            PT_UNIT_ASSERT_NEAR(it->point(2).x(), 10.0);
            PT_UNIT_ASSERT_NEAR(it->point(2).y(), 0.0);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::CubicTo);
            PT_UNIT_ASSERT_NEAR(it->point(2).x(), 20.0);
            PT_UNIT_ASSERT_NEAR(it->point(2).y(), 0.0);

            ++it;
            PT_UNIT_ASSERT(it == path.end());
        }

        void Close()
        {
            Path path;

            path.moveTo(PointF(0.0, 0.0));
            path.lineTo(PointF(5.0, 0.0));
            path.lineTo(PointF(5.0, 5.0));
            path.close();

            Path::Iterator it = path.begin();
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 0.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 0.0);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::Close);

            ++it;
            PT_UNIT_ASSERT(it == path.end());

            Path path2;
            path2.moveTo(PointF(1.0, 1.0));
            path2.lineTo(PointF(4.0, 1.0));
            path2.lineTo(PointF(1.0, 1.0));
            path2.close();

            Path::Iterator it2 = path2.begin();
            PT_UNIT_ASSERT(it2->type() == Path::MoveTo);
            ++it2;
            PT_UNIT_ASSERT(it2->type() == Path::LineTo);
            ++it2;
            PT_UNIT_ASSERT(it2->type() == Path::LineTo);
            ++it2;
            PT_UNIT_ASSERT(it2->type() == Path::Close);
            ++it2;
            PT_UNIT_ASSERT(it2 == path2.end());
        }

        void Clear()
        {
            Path path;

            path.moveTo(PointF(1.0, 2.0));
            path.lineTo(PointF(3.0, 4.0));

            PT_UNIT_ASSERT(!path.isEmpty());
            PT_UNIT_ASSERT(path.size() == 2);

            path.clear();

            PT_UNIT_ASSERT(path.isEmpty());
            PT_UNIT_ASSERT(path.size() == 0);
            PT_UNIT_ASSERT(path.begin() == path.end());
            PT_UNIT_ASSERT_NEAR(path.currentPosition().x(), 0.0);
            PT_UNIT_ASSERT_NEAR(path.currentPosition().y(), 0.0);
        }

        void Size()
        {
            Path path;

            PT_UNIT_ASSERT_EQUAL(path.size(), std::size_t(0));

            path.moveTo(PointF(0.0, 0.0));
            PT_UNIT_ASSERT_EQUAL(path.size(), std::size_t(1));

            path.lineTo(PointF(1.0, 0.0));
            PT_UNIT_ASSERT_EQUAL(path.size(), std::size_t(2));

            path.quadTo(PointF(2.0, 1.0), PointF(3.0, 0.0));
            PT_UNIT_ASSERT_EQUAL(path.size(), std::size_t(3));

            path.cubicTo(PointF(4.0, 1.0), PointF(5.0, 1.0), PointF(6.0, 0.0));
            PT_UNIT_ASSERT_EQUAL(path.size(), std::size_t(4));
        }

        void AddPath()
        {
            Path path;
            path.moveTo(PointF(0.0, 0.0));
            path.lineTo(PointF(1.0, 0.0));

            Path other;
            other.moveTo(PointF(2.0, 0.0));
            other.lineTo(PointF(3.0, 0.0));

            path.addPath(other);

            PT_UNIT_ASSERT_EQUAL(path.size(), std::size_t(4));

            Path::Iterator it = path.begin();
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 0.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 1.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 2.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 3.0);

            ++it;
            PT_UNIT_ASSERT(it == path.end());
        }

        void ToPolygon()
        {
            {
                // Closed triangle: getPolygon returns the 4 flattened points
                // (start + 2 explicit corners + closing lineTo back to start)
                Path path;
                path.moveTo(PointF(0.0, 0.0));
                path.lineTo(PointF(4.0, 0.0));
                path.lineTo(PointF(4.0, 3.0));
                path.close();

                Polygon poly;
                PathIterator it = path.getPolygon(path.begin(), poly);

                PT_UNIT_ASSERT(it == path.end());
                PT_UNIT_ASSERT_EQUAL(poly.size(), std::size_t(4));
                PT_UNIT_ASSERT_NEAR(poly[0].x(), 0.0);
                PT_UNIT_ASSERT_NEAR(poly[0].y(), 0.0);
                PT_UNIT_ASSERT_NEAR(poly[1].x(), 4.0);
                PT_UNIT_ASSERT_NEAR(poly[1].y(), 0.0);
                PT_UNIT_ASSERT_NEAR(poly[2].x(), 4.0);
                PT_UNIT_ASSERT_NEAR(poly[2].y(), 3.0);
                PT_UNIT_ASSERT_NEAR(poly[3].x(), 0.0);
                PT_UNIT_ASSERT_NEAR(poly[3].y(), 0.0);
            }

            {
                // Unclosed path: getPolygon returns the open polyline
                Path path;
                path.moveTo(PointF(0.0, 0.0));
                path.lineTo(PointF(2.0, 0.0));
                path.lineTo(PointF(2.0, 2.0));

                Polygon poly;
                PathIterator it = path.getPolygon(path.begin(), poly);

                PT_UNIT_ASSERT(it == path.end());
                PT_UNIT_ASSERT_EQUAL(poly.size(), std::size_t(3));
                PT_UNIT_ASSERT_NEAR(poly[0].x(), 0.0);
                PT_UNIT_ASSERT_NEAR(poly[1].x(), 2.0);
                PT_UNIT_ASSERT_NEAR(poly[2].x(), 2.0);
                PT_UNIT_ASSERT_NEAR(poly[2].y(), 2.0);
            }

            {
                // Two closed subpaths: iterate with getPolygon
                Path path;
                path.moveTo(PointF(0.0, 0.0));
                path.lineTo(PointF(1.0, 0.0));
                path.lineTo(PointF(0.0, 0.0));
                path.close();

                path.moveTo(PointF(5.0, 5.0));
                path.lineTo(PointF(6.0, 5.0));
                path.lineTo(PointF(5.0, 5.0));
                path.close();

                Polygon poly;

                PathIterator it = path.getPolygon(path.begin(), poly);
                PT_UNIT_ASSERT_EQUAL(poly.size(), std::size_t(3));
                PT_UNIT_ASSERT_NEAR(poly[0].x(), 0.0);
                PT_UNIT_ASSERT_NEAR(poly[1].x(), 1.0);
                PT_UNIT_ASSERT_NEAR(poly[2].x(), 0.0);

                poly.clear();
                it = path.getPolygon(it, poly);
                PT_UNIT_ASSERT_EQUAL(poly.size(), std::size_t(3));
                PT_UNIT_ASSERT_NEAR(poly[0].x(), 5.0);
                PT_UNIT_ASSERT_NEAR(poly[1].x(), 6.0);
                PT_UNIT_ASSERT_NEAR(poly[2].x(), 5.0);

                PT_UNIT_ASSERT(it == path.end());
            }
        }

        void Transform()
        {
            Path path;
            path.moveTo(PointF(1.0, 2.0));
            path.lineTo(PointF(4.0, 6.0));

            Pt::Gfx::Transform t;
            t.translate(3.0, 5.0);
            path.transform(t);

            Path::Iterator it = path.begin();
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 4.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 7.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 7.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 11.0);

            ++it;
            PT_UNIT_ASSERT(it == path.end());
        }

        void ToTransformed()
        {
            Path path;
            path.moveTo(PointF(1.0, 2.0));
            path.lineTo(PointF(4.0, 6.0));

            Pt::Gfx::Transform t;
            t.translate(3.0, 5.0);
            Path transformedPath = path.toTransformed(t);

            Path::Iterator it = transformedPath.begin();
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 4.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 7.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 7.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 11.0);

            ++it;
            PT_UNIT_ASSERT(it == transformedPath.end());

            // Check original path is unmodified
            Path::Iterator origIt = path.begin();
            PT_UNIT_ASSERT(origIt->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(origIt->point(0).x(), 1.0);
            PT_UNIT_ASSERT_NEAR(origIt->point(0).y(), 2.0);
        }

        void BoundingRect()
        {
            // empty path returns null rect
            {
                Path path;
                RectF r = path.boundingRect();
                PT_UNIT_ASSERT(r.isNull());
            }

            // simple line path
            {
                Path path;
                path.moveTo(PointF(1.0, 2.0));
                path.lineTo(PointF(5.0, 8.0));

                RectF r = path.boundingRect();
                PT_UNIT_ASSERT_NEAR(r.left(),   1.0);
                PT_UNIT_ASSERT_NEAR(r.top(),    2.0);
                PT_UNIT_ASSERT_NEAR(r.right(),  5.0);
                PT_UNIT_ASSERT_NEAR(r.bottom(), 8.0);
                PT_UNIT_ASSERT_NEAR(r.width(),  4.0);
                PT_UNIT_ASSERT_NEAR(r.height(), 6.0);
            }

            // triangle with negative coordinates
            {
                Path path;
                path.moveTo(PointF(-3.0, -1.0));
                path.lineTo(PointF(4.0, -1.0));
                path.lineTo(PointF(0.0, 5.0));

                RectF r = path.boundingRect();
                PT_UNIT_ASSERT_NEAR(r.left(),   -3.0);
                PT_UNIT_ASSERT_NEAR(r.top(),    -1.0);
                PT_UNIT_ASSERT_NEAR(r.right(),   4.0);
                PT_UNIT_ASSERT_NEAR(r.bottom(),  5.0);
            }

            // quad bezier: control point extends the bounding box
            {
                Path path;
                path.moveTo(PointF(0.0, 0.0));
                path.quadTo(PointF(5.0, 20.0), PointF(10.0, 0.0));

                RectF r = path.boundingRect();
                PT_UNIT_ASSERT_NEAR(r.left(),   0.0);
                PT_UNIT_ASSERT_NEAR(r.top(),    0.0);
                PT_UNIT_ASSERT_NEAR(r.right(),  10.0);
                PT_UNIT_ASSERT_NEAR(r.bottom(), 20.0);
            }

            // cubic bezier: both control points extend the bounding box
            {
                Path path;
                path.moveTo(PointF(0.0, 0.0));
                path.cubicTo(PointF(-5.0, 10.0), PointF(15.0, 10.0), PointF(10.0, 0.0));

                RectF r = path.boundingRect();
                PT_UNIT_ASSERT_NEAR(r.left(),   -5.0);
                PT_UNIT_ASSERT_NEAR(r.top(),     0.0);
                PT_UNIT_ASSERT_NEAR(r.right(),  15.0);
                PT_UNIT_ASSERT_NEAR(r.bottom(), 10.0);
            }

            // arc: bounding box contains the arc
            {
                Path path;
                path.moveTo(PointF(0.0, 0.0));
                path.arcTo(PointF(-1.0, -1.0), SizeF(2.0, 2.0), 0.0, 90.0);

                RectF r = path.boundingRect();
                PT_UNIT_ASSERT(r.left()   <= 0.0);
                PT_UNIT_ASSERT(r.right()  >= 1.0);
                PT_UNIT_ASSERT(r.top()    <= 0.0);
                PT_UNIT_ASSERT(r.bottom() >= 1.0);
            }
        }

        void ArcTo()
        {
            // arcTo on empty path: moveTo(start) + arc segments
            Path path;
            path.arcTo(PointF(-1.0, -1.0), SizeF(2.0, 2.0), 0.0, 90.0);

            // moveTo(1,0) + cubicTo
            PT_UNIT_ASSERT_EQUAL(path.size(), std::size_t(2));

            Path::Iterator it = path.begin();
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 1.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 0.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::CubicTo);
            PT_UNIT_ASSERT(std::fabs(it->point(2).x()) < 0.001);
            PT_UNIT_ASSERT_NEAR(it->point(2).y(), 1.0);

            ++it;
            PT_UNIT_ASSERT(it == path.end());

            // arcTo on non-empty path: lineTo(start) + arc segments
            Path path2;
            path2.moveTo(PointF(5.0, 5.0));
            path2.arcTo(PointF(-1.0, -1.0), SizeF(2.0, 2.0), 0.0, 90.0);

            // moveTo(5,5) + lineTo(1,0) + cubicTo
            PT_UNIT_ASSERT_EQUAL(path2.size(), std::size_t(3));

            Path::Iterator it2 = path2.begin();
            PT_UNIT_ASSERT(it2->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it2->point(0).x(), 5.0);
            PT_UNIT_ASSERT_NEAR(it2->point(0).y(), 5.0);

            ++it2;
            PT_UNIT_ASSERT(it2->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it2->point(0).x(), 1.0);
            PT_UNIT_ASSERT_NEAR(it2->point(0).y(), 0.0);

            ++it2;
            PT_UNIT_ASSERT(it2->type() == Path::CubicTo);
            PT_UNIT_ASSERT(std::fabs(it2->point(2).x()) < 0.001);
            PT_UNIT_ASSERT_NEAR(it2->point(2).y(), 1.0);

            ++it2;
            PT_UNIT_ASSERT(it2 == path2.end());
        }

        void AddRect()
        {
            Path path;
            path.addRect(RectF(PointF(10.0, 20.0), SizeF(100.0, 50.0)));

            Path::Iterator it = path.begin();

            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_EQUAL(it->point(0).x(), 10.0);
            PT_UNIT_ASSERT_EQUAL(it->point(0).y(), 20.0);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_EQUAL(it->point(0).x(), 10.0);
            PT_UNIT_ASSERT_EQUAL(it->point(0).y(), 70.0);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_EQUAL(it->point(0).x(), 110.0);
            PT_UNIT_ASSERT_EQUAL(it->point(0).y(), 70.0);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_EQUAL(it->point(0).x(), 110.0);
            PT_UNIT_ASSERT_EQUAL(it->point(0).y(), 20.0);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_EQUAL(it->point(0).x(), 10.0);
            PT_UNIT_ASSERT_EQUAL(it->point(0).y(), 20.0);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::Close);

            ++it;
            PT_UNIT_ASSERT(it == path.end());
        }

        void AddPie()
        {
          Path path;
          path.addPie(PointF(0.0, 0.0), SizeF(100.0, 100.0), 0.0, 90.0);

          PT_UNIT_ASSERT(!path.isEmpty());

          Path::Iterator it = path.begin();

          // arc starts at (100, 50) = cos(0)*50+50, sin(0)*50+50
          PT_UNIT_ASSERT(it != path.end());
          PT_UNIT_ASSERT(it->type() == Path::MoveTo);
          PT_UNIT_ASSERT_NEAR(it->point(0).x(), 100.0);
          PT_UNIT_ASSERT_NEAR(it->point(0).y(), 50.0);

          // skip arc segments
          ++it;
          while(it != path.end() && it->type() == Path::CubicTo)
              ++it;

          // lineTo center (50, 50)
          PT_UNIT_ASSERT(it != path.end());
          PT_UNIT_ASSERT(it->type() == Path::LineTo);
          PT_UNIT_ASSERT_NEAR(it->point(0).x(), 50.0);
          PT_UNIT_ASSERT_NEAR(it->point(0).y(), 50.0);

          // lineTo back to arc start + Close
          bool foundClose = false;
          ++it;
          while(it != path.end())
          {
              if (it->type() == Path::Close)
              {
                  foundClose = true;
                  break;
              }
              ++it;
          }
          PT_UNIT_ASSERT(foundClose);

          ++it;
          PT_UNIT_ASSERT(it == path.end());
        }

        void AddChord()
        {
          Path path;
          path.addChord(PointF(0.0, 0.0), SizeF(100.0, 100.0), 0.0, 90.0);

          PT_UNIT_ASSERT(!path.isEmpty());

          Path::Iterator it = path.begin();

          PT_UNIT_ASSERT(it != path.end());
          PT_UNIT_ASSERT(it->type() == Path::MoveTo);
          PT_UNIT_ASSERT_NEAR(it->point(0).x(), 100.0);
          PT_UNIT_ASSERT_NEAR(it->point(0).y(), 50.0);

          bool foundClose = false;
          ++it;
          while( it != path.end() )
          {
              if (it->type() == Path::Close)
              {
                  foundClose = true;
                  break;
              }
              ++it;
          }
          PT_UNIT_ASSERT(foundClose);

          ++it;
          PT_UNIT_ASSERT(it == path.end());
        }

        void AddArc()
        {
            Path path;
            path.addArc(PointF(-1.0, -1.0), SizeF(2.0, 2.0), 0.0, 90.0);

            // moveTo(1,0) + cubicTo
            PT_UNIT_ASSERT_EQUAL( path.size(), std::size_t(2) );

            Path::Iterator it = path.begin();
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 1.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 0.0);

            ++it;

            // end point must be close to (cos(90°), sin(90°)) = (~0, 1)
            PT_UNIT_ASSERT(it->type() == Path::CubicTo);
            PT_UNIT_ASSERT(std::fabs(it->point(2).x()) < 0.001);
            PT_UNIT_ASSERT_NEAR(it->point(2).y(), 1.0);

            ++it;
            PT_UNIT_ASSERT(it == path.end());
        }

        void AddRoundedRect()
        {
            // rx=10, ry=5 produces elliptical corners
            Path path;
            path.addRoundedRect(RectF(PointF(10.0, 20.0), SizeF(100.0, 50.0)), 10.0, 5.0);

            PT_UNIT_ASSERT(!path.isEmpty());

            // moveTo + 4x(lineTo + cubicTo) + close
            Path::Iterator it = path.begin();
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 20.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 20.0);

            // top edge
            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 100.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(),  20.0);

            // top-right corner
            ++it;
            PT_UNIT_ASSERT(it->type() == Path::CubicTo);
            PT_UNIT_ASSERT_NEAR(it->point(2).x(), 110.0);
            PT_UNIT_ASSERT_NEAR(it->point(2).y(),  25.0);

            // right edge
            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 110.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(),  65.0);

            // bottom-right corner
            ++it;
            PT_UNIT_ASSERT(it->type() == Path::CubicTo);
            PT_UNIT_ASSERT_NEAR(it->point(2).x(), 100.0);
            PT_UNIT_ASSERT_NEAR(it->point(2).y(),  70.0);

            // bottom edge
            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 20.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 70.0);

            // bottom-left corner
            ++it;
            PT_UNIT_ASSERT(it->type() == Path::CubicTo);
            PT_UNIT_ASSERT_NEAR(it->point(2).x(), 10.0);
            PT_UNIT_ASSERT_NEAR(it->point(2).y(), 65.0);

            // left edge
            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 10.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 25.0);

            // top-left corner
            ++it;
            PT_UNIT_ASSERT(it->type() == Path::CubicTo);
            PT_UNIT_ASSERT_NEAR(it->point(2).x(), 20.0);
            PT_UNIT_ASSERT_NEAR(it->point(2).y(), 20.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::Close);

            ++it;
            PT_UNIT_ASSERT(it == path.end());

            // bounding rect must match the input rect
            RectF r = path.boundingRect();
            PT_UNIT_ASSERT_NEAR(r.left(),   10.0);
            PT_UNIT_ASSERT_NEAR(r.top(),    20.0);
            PT_UNIT_ASSERT_NEAR(r.right(),  110.0);
            PT_UNIT_ASSERT_NEAR(r.bottom(), 70.0);
        }

        void AddEllipse()
        {
            Path path;
            path.addEllipse(PointF(10.0, 20.0), SizeF(80.0, 40.0));

            PT_UNIT_ASSERT(!path.isEmpty());

            // center = (50, 40), rx = 40, ry = 20
            // moveTo at rightmost point (90, 40) + 4 cubicTo + close
            Path::Iterator it = path.begin();
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 90.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 40.0);

            for (int i = 0; i < 4; ++i)
            {
                ++it;
                PT_UNIT_ASSERT(it != path.end());
                PT_UNIT_ASSERT(it->type() == Path::CubicTo);
            }

            // last segment ends back near start
            PT_UNIT_ASSERT_NEAR(it->point(2).x(), 90.0);
            PT_UNIT_ASSERT_NEAR(it->point(2).y(), 40.0);

            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::Close);

            ++it;
            PT_UNIT_ASSERT(it == path.end());

            // bounding rect must enclose the ellipse area
            RectF r = path.boundingRect();
            PT_UNIT_ASSERT(r.left()   <= 10.0);
            PT_UNIT_ASSERT(r.top()    <= 20.0);
            PT_UNIT_ASSERT(r.right()  >= 90.0);
            PT_UNIT_ASSERT(r.bottom() >= 60.0);
        }

        void AddPolyline()
        {
            PointF pts[] = { PointF(1.0, 2.0), PointF(3.0, 4.0), PointF(5.0, 6.0) };
            
            Path path1;
            path1.addPolyline(pts, 3);
            
            PT_UNIT_ASSERT(!path1.isEmpty());
            PT_UNIT_ASSERT_NEAR(path1.currentPosition().x(), 5.0);
            PT_UNIT_ASSERT_NEAR(path1.currentPosition().y(), 6.0);

            Path::Iterator it = path1.begin();
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 1.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 2.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 3.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 4.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 5.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 6.0);

            ++it;
            PT_UNIT_ASSERT(it == path1.end());
        }

        void AddPolygon()
        {
            PointF pts[] = { PointF(1.0, 2.0), PointF(3.0, 4.0), PointF(5.0, 6.0) };
            
            Path path1;
            path1.addPolygon(pts, 3);
            
            PT_UNIT_ASSERT(!path1.isEmpty());

            Path::Iterator it = path1.begin();
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 1.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 2.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 3.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 4.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 5.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 6.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 1.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 2.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::Close);

            ++it;
            PT_UNIT_ASSERT(it == path1.end());
        }

        void ContainsEmpty()
        {
            Path path;
            PT_UNIT_ASSERT(!path.contains(PointF(0.0, 0.0)));
            PT_UNIT_ASSERT(!path.contains(PointF(0.0, 0.0), FillRule::EvenOdd));
        }

        void ContainsRect()
        {
            // Rectangle from (10,10) to (90,90).
            Path path;
            path.addRect(RectF(PointF(10.0, 10.0), PointF(80.0, 80.0)));

            // Clearly inside.
            PT_UNIT_ASSERT( path.contains(PointF(50.0, 50.0), FillRule::NonZero));
            PT_UNIT_ASSERT( path.contains(PointF(50.0, 50.0), FillRule::EvenOdd));

            // Clearly outside.
            PT_UNIT_ASSERT(!path.contains(PointF(0.0,  0.0),  FillRule::NonZero));
            PT_UNIT_ASSERT(!path.contains(PointF(0.0,  0.0),  FillRule::EvenOdd));
            PT_UNIT_ASSERT(!path.contains(PointF(100.0, 50.0), FillRule::NonZero));
            PT_UNIT_ASSERT(!path.contains(PointF(50.0, 100.0), FillRule::NonZero));
        }

        void ContainsEllipse()
        {
            // Ellipse centred at (50,50), radii 40x30.
            Path path;
            path.addEllipse(PointF(10.0, 20.0), SizeF(80.0, 60.0));

            // Centre: inside.
            PT_UNIT_ASSERT( path.contains(PointF(50.0, 50.0), FillRule::NonZero));
            // Far outside.
            PT_UNIT_ASSERT(!path.contains(PointF(0.0,   0.0),  FillRule::NonZero));
            PT_UNIT_ASSERT(!path.contains(PointF(200.0, 50.0), FillRule::NonZero));
            // Just beyond the right edge (centre.x + rx + 1).
            PT_UNIT_ASSERT(!path.contains(PointF(91.0,  50.0), FillRule::NonZero));
        }

        void ContainsOnEdge()
        {
            // Horizontal ray originates from a point exactly on a bottom edge.
            // With a half-open [y0,y1) interval the point is outside.
            Path path;
            path.addRect(RectF(PointF(0.0, 0.0), PointF(100.0, 100.0)));

            // Bottom edge y=100 — half-open interval means outside.
            PT_UNIT_ASSERT(!path.contains(PointF(50.0, 100.0), FillRule::NonZero));
            // Top edge y=0 — inside by half-open convention.
            PT_UNIT_ASSERT( path.contains(PointF(50.0, 0.0),   FillRule::NonZero));
        }

        void ContainsConcentricCircles()
        {
            // Two concentric ellipses wound in the same direction.
            // NonZero: inner region winding=2 -> inside.
            // EvenOdd: inner region crossings=2 (even) -> outside.
            Path path;
            path.addEllipse(PointF(10.0, 10.0), SizeF(80.0, 80.0));  // outer
            path.addEllipse(PointF(30.0, 30.0), SizeF(40.0, 40.0));  // inner

            // Point in the inner ring.
            PT_UNIT_ASSERT( path.contains(PointF(50.0, 50.0), FillRule::NonZero));
            PT_UNIT_ASSERT(!path.contains(PointF(50.0, 50.0), FillRule::EvenOdd));

            // Point in the outer ring but outside the inner ellipse.
            PT_UNIT_ASSERT( path.contains(PointF(15.0, 48.0), FillRule::NonZero));
            PT_UNIT_ASSERT( path.contains(PointF(15.0, 48.0), FillRule::EvenOdd));
        }

        void ContainsRectInRect()
        {
            // Small rect fully inside a larger rect path.
            Path path;
            path.addRect(RectF(PointF(0.0, 0.0), PointF(100.0, 100.0)));

            PT_UNIT_ASSERT( path.contains(RectF(PointF(20.0, 20.0), PointF(80.0, 80.0))));
        }

        void ContainsRectOutside()
        {
            Path path;
            path.addRect(RectF(PointF(0.0, 0.0), PointF(100.0, 100.0)));

            PT_UNIT_ASSERT(!path.contains(RectF(PointF(200.0, 200.0), PointF(300.0, 300.0))));
            PT_UNIT_ASSERT(!path.contains(RectF(PointF(-50.0, -50.0), PointF(-10.0, -10.0))));
        }

        void ContainsRectLarger()
        {
            // Rect larger than the path: corners lie outside the path.
            Path path;
            path.addRect(RectF(PointF(10.0, 10.0), PointF(90.0, 90.0)));

            PT_UNIT_ASSERT(!path.contains(RectF(PointF(0.0, 0.0), PointF(100.0, 100.0))));
        }

        void ContainsRectInEllipse()
        {
            // Small rect near centre of an ellipse.
            Path path;
            path.addEllipse(PointF(0.0, 0.0), SizeF(100.0, 100.0));

            // Small rect well inside the circle.
            PT_UNIT_ASSERT( path.contains(RectF(PointF(40.0, 40.0), PointF(60.0, 60.0))));
            // Rect extending below the ellipse (bottom at y=110, ellipse bottom at y=100).
            PT_UNIT_ASSERT(!path.contains(RectF(PointF(40.0, 40.0), PointF(60.0, 110.0))));
        }

        void ContainsRectCrossing()
        {
            // Path: horizontal band (0,40)→(100,60).
            Path path;
            path.addRect(RectF(PointF(0.0, 40.0), PointF(100.0, 60.0)));

            // Rect fully inside the band.
            PT_UNIT_ASSERT( path.contains(RectF(PointF(10.0, 42.0), PointF(90.0, 58.0))));

            // Rect straddling the top edge (y=40): top corners outside.
            PT_UNIT_ASSERT(!path.contains(RectF(PointF(10.0, 35.0), PointF(90.0, 55.0))));

            // Rect straddling the bottom edge (y=60).
            PT_UNIT_ASSERT(!path.contains(RectF(PointF(10.0, 45.0), PointF(90.0, 65.0))));
        }

        void IntersectsRectPartial()
        {
            // Two overlapping rects.
            Path path;
            path.addRect(RectF(PointF(0.0, 0.0), PointF(100.0, 100.0)));

            // Partially overlapping — corner (50,50) is inside path.
            PT_UNIT_ASSERT( path.intersects(RectF(PointF(50.0, 50.0), PointF(150.0, 150.0))));
            // Same with EvenOdd.
            PT_UNIT_ASSERT( path.intersects(RectF(PointF(50.0, 50.0), PointF(150.0, 150.0)), FillRule::EvenOdd));
        }

        void IntersectsRectOutside()
        {
            Path path;
            path.addRect(RectF(PointF(0.0, 0.0), PointF(100.0, 100.0)));

            PT_UNIT_ASSERT(!path.intersects(RectF(PointF(200.0, 0.0),   PointF(300.0, 100.0))));
            PT_UNIT_ASSERT(!path.intersects(RectF(PointF(-100.0, -50.0), PointF(-10.0, 50.0))));
        }

        void IntersectsRectContains()
        {
            // Rect is small and fully inside the path — rect corners are inside path.
            Path path;
            path.addRect(RectF(PointF(0.0, 0.0), PointF(100.0, 100.0)));

            PT_UNIT_ASSERT( path.intersects(RectF(PointF(20.0, 20.0), PointF(80.0, 80.0))));
        }

        void IntersectsPathInsideRect()
        {
            // Path is entirely inside the test rect — no rect corners inside path,
            // no crossings, but bbox subset of rect triggers true.
            Path path;
            path.addRect(RectF(PointF(20.0, 20.0), PointF(80.0, 80.0)));

            PT_UNIT_ASSERT( path.intersects(RectF(PointF(10.0, 10.0), PointF(90.0, 90.0))));
        }

        void IntersectsRectEllipse()
        {
            // Rect crossing the ellipse boundary.
            Path path;
            path.addEllipse(PointF(0.0, 0.0), SizeF(100.0, 100.0));

            // Rect partly inside — corner (45,45) is inside the circle.
            PT_UNIT_ASSERT( path.intersects(RectF(PointF(45.0, 45.0), PointF(150.0, 150.0))));
            // Rect entirely outside the circle.
            PT_UNIT_ASSERT(!path.intersects(RectF(PointF(200.0, 200.0), PointF(300.0, 300.0))));
        }

};

} // namespace Gfx

} // namespace Pt

Pt::Unit::RegisterTest<Pt::Gfx::PathTest> register_PathTest;
