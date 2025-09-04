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
#include <Pt/Gfx/Rect.h>
#include <Pt/Gfx/Polygon.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/SmartPtr.h>
#include <vector>

namespace Pt {

namespace Gfx {

class PathData;

/* @brief Element of a path.
*/
struct Element
{
    enum Type
    {
        IT_Close,
        IT_MoveTo,
        IT_LineTo,
        IT_QuadBezierTo,
        IT_CubicBezierTo,
        IT_GenNBezierTo
    };

    Element(Type type_)
    : type(type_)
    {}

    Element(Type type_, double x0, double y0)
    : type(type_), pxy(2)
    { pxy[0] = x0; pxy[1] = y0; }

    Element(Type type_, double x0, double y0, double x1, double y1)
    : type(type_), pxy(4)
    { pxy[0] = x0; pxy[1] = y0; pxy[2] = x1; pxy[3] = y1; }

    Element(Type type_, double x0, double y0, double x1, double y1, double x2, double y2)
    : type(type_), pxy(6)
    { pxy[0] = x0; pxy[1] = y0; pxy[2] = x1; pxy[3] = y1; pxy[4] = x2; pxy[5] = y2; }

    Element(Type type_, const std::vector<double>& pxy_)
    : type(type_), pxy(pxy_)
    {}

    Type                type;
    std::vector<double> pxy;
};

/* @brief Graphics path.
*/
class PT_GFX_API Path
{
    public:
        typedef std::vector<Element> Elements;

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

        /** @brief Appends a path into the current subpath.
        */
        void appendPath(const Path& p);

        void addRect(const SizeF& size);

        void addRoundedRect(const SizeF& size, float radius);

        void addEllipse(const SizeF& size);

        void addPie(const SizeF& size, float degBegin, float degEnd);

        void addChord(const SizeF& size,  float degBegin, float degEnd);

        void transform(const Transform& transform);

        void toPolygons(std::vector<Polygon>& polygons, float smoothness = 1) const;

        void detach();

    private:
        SmartPtr<PathData> _pathData;
};

/* @internal Path data.
*/
class PathData
{
    public:
        typedef std::vector<Element> Elements;

    public:
        PathData()
        {}

        ~PathData()
        {
        }

        void clear()
        {
            return _elements.clear();
        }

        const PointF& currentPosition() const
        {
            return _position;
        }

        void setCurrentPosition(const PointF& p)
        {
            _position = p;
        }

        Elements& elements() 
        {
            return _elements;
        }

        const Elements& elements() const 
        {
            return _elements;
        }

    private:
        typedef std::vector<Element> ElementVector;

    private:
        ElementVector _elements;
        PointF        _position;
};

} // namespace

} // namespace

#endif
