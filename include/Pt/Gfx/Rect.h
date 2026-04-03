/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2015 Marc Boris Duerner
 * Copyright (C) 2010 Aloysius Indrayanto
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef PT_GFX_RECT_H
#define PT_GFX_RECT_H

#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <algorithm>

namespace Pt {

namespace Gfx {

/** @brief %Rect with floating-point coordinates.
*/
class Rect
{
    public:
        explicit Rect(const Point& p = Point(0, 0),
                      const Size& s = Size(0, 0))
        : _p(p)
        , _s(s)
        {
        }

        explicit Rect(const Size& s)
        : _p()
        , _s(s)
        {
        }

        Rect(Float width, Float height)
        : _p()
        , _s(width, height)
        {
        }

        Rect(const Point& p1, const Point& p2)
        : _p(p1)
        , _s(p2.x() - p1.x(), p2.y() - p1.y())
        {
        }

        Rect(Float left, Float right, Float top, Float bottom)
        {
            set(left, right, top, bottom);
        }

        Rect(const Rect& val)
        : _p(val._p)
        , _s(val._s)
        {
        }

        bool isNull() const
        {
            return (_s.width() == 0 || _s.height() == 0);
        }

        void clear()
        {
            _p.clear();
            _s.clear();
        }

        void set(const Point& p, const Size& s)
        {
            _p = p;
            _s = s;
        }

        void set(const Point& p1, const Point& p2)
        {
            this->setOrigin(p1);
            this->setWidth(p2.x() - p1.x());
            this->setHeight(p2.y() - p1.y());
        }

        void set(Float width, Float height)
        {
            _p.clear();
            _s.set(width, height);
        }

        void set(Float left, Float right, Float top, Float bottom)
        {
            _p = Point(left, top);
            _s = Size(right - left, bottom - top);
        }

        const Point& origin() const
        {
            return _p;
        }

        void setOrigin(const Point& p)
        {
            _p = p;
        }

        void setSize(const Size& s)
        {
            _s = s;
        }

        void setWidth(Float w)
        {
            _s.setWidth(w);
        }

        void setHeight(Float h)
        {
            _s.setHeight(h);
        }

        Float x() const
        {
            return _p.x();
        }

        Float y() const
        {
            return _p.y();
        }

        const Size& size() const
        {
            return _s;
        }

        Float width() const
        {
            return _s.width();
        }

        Float height() const
        {
            return _s.height();
        }

        Float left() const
        {
            return _p.x();
        }

        Float top() const
        {
            return _p.y();
        }

        Float right() const
        {
            return _p.x() + _s.width();
        }

        Float bottom() const
        {
            return _p.y() + _s.height();
        }

        const Point& topLeft() const
        {
            return _p;
        }

        Point topRight() const
        {
            return Point(this->x() + this->width(), this->y());
        }

        Point bottomLeft() const
        {
            return Point(this->x(), this->y() + this->height());
        }

        Point bottomRight() const
        {
            return Point(this->x() + this->width(),
                         this->y() + this->height());
        }

        bool operator==(const Rect& other) const
        {
            return _p == other._p && _s == other._s;
        }

        bool operator!=(const Rect& other) const
        {
            return _p != other._p || _s != other._s;
        }

        void shift(Float dx, Float dy)
        {
            _p.addX(dx);
            _p.addY(dy);
        }

        void expand(Float dw, Float dh)
        {
            _s.addWidth(dw);
            _s.addHeight(dh);
        }

        void shrink(Float dw, Float dh)
        {
            _s.addWidth(-dw);
            _s.addHeight(-dh);
        }

        void unify(const Rect& rect)
        {
            if(rect.isNull())
                return;

            if(this->isNull())
            {
                _p = rect._p;
                _s = rect._s;
                return;
            }

            const Float l = std::min(this->left(), rect.left());
            const Float t = std::min(this->top(), rect.top());
            const Float r = std::max(this->right(), rect.right());
            const Float b = std::max(this->bottom(), rect.bottom());

            set(l, r, t, b);
        }

        Rect intersect(const Rect& rect) const
        {
            const Float l = std::max(this->left(), rect.left());
            const Float t = std::max(this->top(), rect.top());
            const Float r = std::min(this->right(), rect.right());
            const Float b = std::min(this->bottom(), rect.bottom());

            return r >= l && b >= t ? Rect(l, r, t, b)
                                    : Rect();
        }

        bool contains(const Point& p) const
        {
            return p.x() >= _p.x() &&
                   p.x() < _p.x() + _s.width() &&
                   p.y() >= _p.y() &&
                   p.y() < _p.y() + _s.height();
        }

    private:
        Point _p;
        Size  _s;
};

typedef Rect RectF;

/** @brief %Rect with integer coordinates.
*/
class RectI
{
    public:
        explicit RectI(const PointI& p = PointI(0, 0),
                       const SizeI& s = SizeI(0, 0))
        : _p(p)
        , _s(s)
        {
        }

        explicit RectI(const SizeI& s)
        : _p()
        , _s(s)
        {
        }

        RectI(Int width, Int height)
        : _p()
        , _s(width, height)
        {
        }

        RectI(const PointI& p1, const PointI& p2)
        : _p(p1)
        , _s(p2.x() - p1.x(), p2.y() - p1.y())
        {
        }

        RectI(Int left, Int right, Int top, Int bottom)
        {
            set(left, right, top, bottom);
        }

        RectI(const RectI& val)
        : _p(val._p)
        , _s(val._s)
        {
        }

        bool isNull() const
        {
            return (_s.width() == 0 || _s.height() == 0);
        }

        void clear()
        {
            _p.clear();
            _s.clear();
        }

        void set(const PointI& p, const SizeI& s)
        {
            _p = p;
            _s = s;
        }

        void set(const PointI& p1, const PointI& p2)
        {
            this->setOrigin(p1);
            this->setWidth(p2.x() - p1.x());
            this->setHeight(p2.y() - p1.y());
        }

        void set(Int width, Int height)
        {
            _p.clear();
            _s.set(width, height);
        }

        void set(Int left, Int right, Int top, Int bottom)
        {
            _p = PointI(left, top);
            _s = SizeI(right - left, bottom - top);
        }

        const PointI& origin() const
        {
            return _p;
        }

        void setOrigin(const PointI& p)
        {
            _p = p;
        }

        void setSize(const SizeI& s)
        {
            _s = s;
        }

        void setWidth(Int w)
        {
            _s.setWidth(w);
        }

        void setHeight(Int h)
        {
            _s.setHeight(h);
        }

        Int x() const
        {
            return _p.x();
        }

        Int y() const
        {
            return _p.y();
        }

        const SizeI& size() const
        {
            return _s;
        }

        Int width() const
        {
            return _s.width();
        }

        Int height() const
        {
            return _s.height();
        }

        Int left() const
        {
            return _p.x();
        }

        Int top() const
        {
            return _p.y();
        }

        Int right() const
        {
            return _p.x() + _s.width();
        }

        Int bottom() const
        {
            return _p.y() + _s.height();
        }

        const PointI& topLeft() const
        {
            return _p;
        }

        PointI topRight() const
        {
            return PointI(this->x() + this->width(), this->y());
        }

        PointI bottomLeft() const
        {
            return PointI(this->x(), this->y() + this->height());
        }

        PointI bottomRight() const
        {
            return PointI(this->x() + this->width(),
                          this->y() + this->height());
        }

        bool operator==(const RectI& other) const
        {
            return _p == other._p && _s == other._s;
        }

        bool operator!=(const RectI& other) const
        {
            return _p != other._p || _s != other._s;
        }

        void shift(Int dx, Int dy)
        {
            _p.addX(dx);
            _p.addY(dy);
        }

        void expand(Int dw, Int dh)
        {
            _s.addWidth(dw);
            _s.addHeight(dh);
        }

        void shrink(Int dw, Int dh)
        {
            _s.addWidth(-dw);
            _s.addHeight(-dh);
        }

        void unify(const RectI& rect)
        {
            if(rect.isNull())
                return;

            if(this->isNull())
            {
                _p = rect._p;
                _s = rect._s;
                return;
            }

            const Int l = std::min(this->left(), rect.left());
            const Int t = std::min(this->top(), rect.top());
            const Int r = std::max(this->right(), rect.right());
            const Int b = std::max(this->bottom(), rect.bottom());

            set(l, r, t, b);
        }

        RectI intersect(const RectI& rect) const
        {
            const Int l = std::max(this->left(), rect.left());
            const Int t = std::max(this->top(), rect.top());
            const Int r = std::min(this->right(), rect.right());
            const Int b = std::min(this->bottom(), rect.bottom());

            return r >= l && b >= t ? RectI(l, r, t, b)
                                    : RectI();
        }

        bool contains(const PointI& p) const
        {
            return p.x() >= _p.x() &&
                   p.x() < _p.x() + _s.width() &&
                   p.y() >= _p.y() &&
                   p.y() < _p.y() + _s.height();
        }

    private:
        PointI _p;
        SizeI  _s;
};

}  // namespace

} // namespace

#endif
