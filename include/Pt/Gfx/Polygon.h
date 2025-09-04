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

#ifndef PT_GFX_PPOLYGON_H
#define PT_GFX_PPOLYGON_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Point.h>
#include <vector>

namespace Pt {

namespace Gfx {

/* @brief A polygon consisting of multiple points.
*/
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

    private:
        std::vector<PointF> _points;
};

} // namespace

} // namespace

#endif
