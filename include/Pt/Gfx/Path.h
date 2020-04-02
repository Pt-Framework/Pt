/* Copyright (C) 2017 Marc Boris Duerner
   Copyright (C) 2017 Aloysius Indrayanto

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

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Transform.h>
#include <vector>
#include <limits>

namespace Pt {

namespace Gfx {


class Polygon
{
    public:
        Polygon(const PointF* ps, std::size_t n)
        : _points(ps, ps + n)
        {
        }

        Polygon()
        {
        }

        void assign(const PointF* ps, std::size_t n)
        {
            _points.assign(ps, ps+n);
        }


        const PointF& operator[](std::size_t n) const
        {
            return _points[n];
        }

        PointF& operator[](std::size_t n)
        {
            return _points[n];
        }

        const PointF& at(std::size_t n) const
        {
            return _points[n];
        }

        PointF& at(std::size_t n)
        {
            return _points[n];
        }

        void clear()
        {
            _points.clear();
        }

        bool empty() const
        {
            return _points.empty();
        }

        std::size_t size() const
        {
            return _points.size();
        }

        void push_back(const PointF& p)
        {
            _points.push_back(p);
        }

        std::vector<PointF>& points()
        {
            return _points;
        }

        const std::vector<PointF>& points() const
        {
            return _points;
        }

#if 0
        Polygon toPixel() const
        {

            double xmin = std::numeric_limits<double>::max();
            double xmax = std::numeric_limits<double>::min();
            double ymin = std::numeric_limits<double>::max();
            double ymax = std::numeric_limits<double>::min();

            for (size_t i = 0; i < _points.size(); ++i)
            {
                xmin = std::min(xmin, _points[i].x());
                xmax = std::max(xmax, _points[i].x());
                ymin = std::min(ymin, _points[i].y());
                ymax = std::max(ymax, _points[i].y());
            }

            const double w2 = (xmax - xmin) / 2;
            const double h2 = (ymax - ymin) / 2;
            const double transx = xmin + w2;
            const double transy = ymin + h2;

            Polygon polygon;

            for (size_t i = 0; i < _points.size(); ++i)
            {
                PointF point = _points[i];

                point.addX(-transx);
                point.addY(-transy);

                const double distx = std::abs(point.x());

                if (distx >= std::numeric_limits<double>::epsilon())
                {
                    const double xscale = (distx - 0.5) / distx;
                    point.setX(point.x() * xscale);
                }

                point.addX(transx - 0.5);

                const double disty = std::abs(point.y());

                if (disty >= std::numeric_limits<double>::epsilon())
                {
                    const double yscale = (disty - 0.5) / disty;
                    point.setY(point.y() * yscale);
                }

                point.addY(transy - 0.5);

                polygon.push_back(point);
            }

            return polygon;
        }
#endif

    private:
        std::vector<PointF> _points;
};


struct Element
{
    enum ElementType
    {
        IT_Close,
        IT_MoveTo,
        IT_LineTo,
        IT_QuadBezierTo,
        IT_CubicBezierTo,
        IT_GenNBezierTo
    };

    Element(ElementType type_)
    : type(type_)
    {}

    Element(ElementType type_, double x0, double y0)
    : type(type_), pxy(2)
    { pxy[0] = x0; pxy[1] = y0; }

    Element(ElementType type_, double x0, double y0, double x1, double y1)
    : type(type_), pxy(4)
    { pxy[0] = x0; pxy[1] = y0; pxy[2] = x1; pxy[3] = y1; }

    Element(ElementType type_, double x0, double y0, double x1, double y1, double x2, double y2)
    : type(type_), pxy(6)
    { pxy[0] = x0; pxy[1] = y0; pxy[2] = x1; pxy[3] = y1; pxy[4] = x2; pxy[5] = y2; }

    Element(ElementType type_, const std::vector<double>& pxy_)
    : type(type_), pxy(pxy_)
    {}


    ElementType         type;
    std::vector<double> pxy;
};


class PT_GFX_API Path
{
    public:
        Path();

        ~Path();

        std::size_t size() const;

        bool isEmpty() const;

        const Element& at(std::size_t n) const;

        void clear();

        RectF boundingRect() const;

        const PointF& currentPosition() const;

        void moveTo(const PointF& p);

        void lineTo(const PointF& p);

        void arcTo(const PointF& p, double r);

        void quadraticBezierTo(const PointF &c, const PointF& to);

        void cubicBezierTo(const PointF &c1, const PointF &c2, const PointF& to);

        void bezierTo(const PointF* controlPoints, size_t n, const PointF& to);

        /** @brief closes the current subpath.
        */
        void close();

        /** @brief Adds a path as a new subpath.
        */
        void addPath(const Path& p);

        /** @brief Inserts a path into the current subpath.
        */
        void insertPath(const Path& p);

        void addRect(const SizeF& size);

        void addRoundedRect(const SizeF& size, float radius);

        void addEllipse(const SizeF& size);

        void addPie(const SizeF& size, float degBegin, float degEnd);

        void addChord(const SizeF& size,  float degBegin, float degEnd);

        void transform(const Transform& transform);

        void toPolygons(std::vector<Polygon>& polygons, float smoothness = 1) const;

    private:
        typedef std::vector<Element> ElementVector;

    private:
        ElementVector _elements;
        PointF        _position;
};


} // namespace

} // namespace

#endif
