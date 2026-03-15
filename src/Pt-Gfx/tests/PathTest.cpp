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
            registerMethod("AppendPath", *this, &PathTest::AppendPath);
            registerMethod("AddPath",       *this, &PathTest::AddPath);
            registerMethod("ToPolygons",    *this, &PathTest::ToPolygons);
            registerMethod("Transform",     *this, &PathTest::Transform);
            registerMethod("BoundingRect",  *this, &PathTest::BoundingRect);
            registerMethod("ArcTo",         *this, &PathTest::ArcTo);
            registerMethod("AddRect",       *this, &PathTest::AddRect);
            registerMethod("AddPie",     *this, &PathTest::AddPie);
            registerMethod("AddChord",      *this, &PathTest::AddChord);
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

        void AppendPath()
        {
            Path path;
            path.moveTo(PointF(0.0, 0.0));
            path.lineTo(PointF(1.0, 0.0));

            Path other;
            other.moveTo(PointF(2.0, 0.0));
            other.lineTo(PointF(3.0, 0.0));

            path.appendPath(other);

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

        void AddPath()
        {
            Path path;
            path.moveTo(PointF(0.0, 0.0));
            path.lineTo(PointF(1.0, 0.0));

            Path other;
            other.moveTo(PointF(2.0, 0.0));
            other.lineTo(PointF(3.0, 0.0));

            path.addPath(other);

            Path::Iterator it = path.begin();
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 1.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 0.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 0.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::Close);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 2.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 3.0);

            ++it;
            PT_UNIT_ASSERT(it == path.end());
        }

        void ToPolygons()
        {
            {
                Path path;
                path.moveTo(PointF(0.0, 0.0));
                path.lineTo(PointF(4.0, 0.0));
                path.lineTo(PointF(4.0, 3.0));
                path.close();

                std::vector<Polygon> polygons;
                path.toPolygons(polygons);

                PT_UNIT_ASSERT_EQUAL(polygons.size(), std::size_t(1));

                const Polygon& tri = polygons[0];
                PT_UNIT_ASSERT_EQUAL(tri.size(), std::size_t(4));
                PT_UNIT_ASSERT_NEAR(tri[0].x(), 0.0);
                PT_UNIT_ASSERT_NEAR(tri[0].y(), 0.0);
                PT_UNIT_ASSERT_NEAR(tri[1].x(), 4.0);
                PT_UNIT_ASSERT_NEAR(tri[1].y(), 0.0);
                PT_UNIT_ASSERT_NEAR(tri[2].x(), 4.0);
                PT_UNIT_ASSERT_NEAR(tri[2].y(), 3.0);
                PT_UNIT_ASSERT_NEAR(tri[3].x(), 0.0);
                PT_UNIT_ASSERT_NEAR(tri[3].y(), 0.0);
            }

            {
                Path path;
                path.moveTo(PointF(0.0, 0.0));
                path.lineTo(PointF(2.0, 0.0));
                path.lineTo(PointF(2.0, 2.0));

                std::vector<Polygon> polygons;
                path.toPolygons(polygons);

                PT_UNIT_ASSERT_EQUAL(polygons.size(), std::size_t(0));
            }

            {
                Path path;
                path.moveTo(PointF(0.0, 0.0));
                path.lineTo(PointF(1.0, 0.0));
                path.lineTo(PointF(0.0, 0.0));
                path.close();

                path.moveTo(PointF(5.0, 5.0));
                path.lineTo(PointF(6.0, 5.0));
                path.lineTo(PointF(5.0, 5.0));
                path.close();

                std::vector<Polygon> polygons;
                path.toPolygons(polygons);

                PT_UNIT_ASSERT_EQUAL(polygons.size(), std::size_t(2));

                PT_UNIT_ASSERT_EQUAL(polygons[0].size(), std::size_t(3));
                PT_UNIT_ASSERT_NEAR(polygons[0][0].x(), 0.0);
                PT_UNIT_ASSERT_NEAR(polygons[0][1].x(), 1.0);
                PT_UNIT_ASSERT_NEAR(polygons[0][2].x(), 0.0);

                PT_UNIT_ASSERT_EQUAL(polygons[1].size(), std::size_t(3));
                PT_UNIT_ASSERT_NEAR(polygons[1][0].x(), 5.0);
                PT_UNIT_ASSERT_NEAR(polygons[1][1].x(), 6.0);
                PT_UNIT_ASSERT_NEAR(polygons[1][2].x(), 5.0);
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

            // arc: bounding box contains both endpoints and the arc apex (1,1)
            {
                Path path;
                path.moveTo(PointF(0.0, 0.0));
                path.arcTo(PointF(2.0, 0.0), 1.0);

                RectF r = path.boundingRect();
                PT_UNIT_ASSERT(r.left()   <= 0.0);
                PT_UNIT_ASSERT(r.right()  >= 2.0);
                PT_UNIT_ASSERT(r.top()    <= 0.0);
                PT_UNIT_ASSERT(r.bottom() >= 1.0);
            }
        }

        void ArcTo()
        {
            Path path;
            path.moveTo(PointF(0.0, 0.0));
            path.arcTo(PointF(2.0, 0.0), 1.0);

             // moveTo + 2x cubicTo
            PT_UNIT_ASSERT_EQUAL(path.size(), std::size_t(3));

            Path::Iterator it = path.begin();
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::CubicTo);
            PT_UNIT_ASSERT_EQUAL(it->size(), std::size_t(3));
            // curve 1 must end at the mid-arc-point (1, 1)
            PT_UNIT_ASSERT_NEAR(it->point(2).x(), 1.0);
            PT_UNIT_ASSERT_NEAR(it->point(2).y(), 1.0);

            ++it;
            PT_UNIT_ASSERT(it->type() == Path::CubicTo);
            // curve 2 must end at the target point (2, 0)
            PT_UNIT_ASSERT_NEAR(it->point(2).x(), 2.0);
            PT_UNIT_ASSERT_NEAR(it->point(2).y(), 0.0);

            ++it;
            PT_UNIT_ASSERT(it == path.end());
        }

        void AddRect()
        {
            Path path;
            path.moveTo(PointF(10.0, 20.0));
            path.addRect(SizeF(100.0, 50.0));

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
            path.moveTo(PointF(0.0, 0.0));
            path.addPie(SizeF(100.0, 100.0), 0.0f, 90.0f);

            PT_UNIT_ASSERT(!path.isEmpty());

            Path::Iterator it = path.begin();
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::MoveTo);
            PT_UNIT_ASSERT_NEAR(it->point(0).x(), 50.0);
            PT_UNIT_ASSERT_NEAR(it->point(0).y(), 50.0);


            ++it;
            PT_UNIT_ASSERT(it != path.end());
            PT_UNIT_ASSERT(it->type() == Path::LineTo);
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

        void AddChord()
        {
          Path path;
          path.moveTo(PointF(0.0, 0.0));
          path.addChord(SizeF(100.0, 100.0), 0.0f, 90.0f);

          PT_UNIT_ASSERT( ! path.isEmpty() );

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
};

} // namespace

} // namespace

Pt::Unit::RegisterTest<Pt::Gfx::PathTest> register_PathTest;
