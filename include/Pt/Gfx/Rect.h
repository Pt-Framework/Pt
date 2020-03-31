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


template<typename T>
class BasicRect
{
    public:
        typedef T ValueT;

    public:
        explicit BasicRect( const BasicPoint<T>& p = BasicPoint<T>(0, 0),
                            const BasicSize<T>& s = BasicSize<T>(0, 0) )
        : _p(p)
        , _s(s)
        {
        }

        explicit BasicRect(const BasicSize<T>& s)
        : _p()
        , _s(s)
        {
        }

        BasicRect(const BasicPoint<T>& p1, const BasicPoint<T>& p2)
        : _p(p1)
        , _s( p2.x() - p1.x(), p2.y() - p1.y())
        {
        }

        BasicRect(const T left, const T right, const T top, const T bottom)
        {
            set( left, right, top, bottom );
        }

        BasicRect(const BasicRect<T>& val)
        : _p(val._p)
        , _s(val._s)
        {
        }

        bool isNull() const
        {
            return (_s.width() == 0 || _s.height() == 0 );
        }

        void clear()
        {
            _p.set(0, 0);
            _s.set(0, 0);
        }

        void set(const BasicPoint<T>& p, const BasicSize<T>& s)
        {
            _p = p;
            _s = s;
        }

        void set(const BasicPoint<T>& p1, const BasicPoint<T>& p2)
        {
            this->setOrigin( p1 );
            this->setWidth(p2.x() - p1.x() );
            this->setHeight(p2.y() - p1.y());
        }

        void set(const T left, const T right, const T top, const T bottom)
        {
            _p = BasicPoint<T>( left, top );
            _s = BasicSize<T>(  right - left , bottom - top );
        }

        void setOrigin(const BasicPoint<T>& p)
        {
            _p = p;
        }

        void setSize(const BasicSize<T>& s)
        {
            _s = s;
        }

        void setWidth(T w)
        {
            _s.setWidth(w);
        }

        void setHeight(T h)
        {
            _s.setHeight(h);
        }

        T x() const
        {
            return _p.x();
        }

        T y() const
        {
            return _p.y();
        }

        const BasicSize<T>& size() const
        {
            return _s;
        }

        T width() const
        {
            return _s.width();
        }

        T height() const
        {
            return _s.height();
        }

        T left() const
        {
            return _p.x();
        }

        T top() const
        {
            return _p.y();
        }

        T right() const
        {
            return _p.x() + _s.width();
        }

        T bottom() const
        {
            return _p.y() + _s.height();
        }

        const BasicPoint<T>& topLeft() const
        {
            return _p;
        }

        const BasicPoint<T> topRight() const
        {
            return BasicPoint<T>(this->x() + this->width(), this->y());
        }

        const BasicPoint<T> bottomLeft() const
        {
            return BasicPoint<T>(this->x(), this->y() + this->height());
        }

        const BasicPoint<T> bottomRight() const
        {
            return BasicPoint<T>(this->x() + this->width(),
                                 this->y() + this->height() );
        }

        bool operator==(const BasicRect& other) const
        {
            return _p == other._p && _s == other._s;
        }

        bool operator!=(const BasicRect& other) const
        {
            return _p != other._p || _s != other._s;
        }

        void shift(T dx, T dy)
        {
            _p.addX(dx);
            _p.addY(dy);
        }

        void expand(T dw, T dh)
        {
            _s.addWidth(dw);
            _s.addHeight(dh);
        }

        void shrink(T dw, T dh)
        {
            _s.addWidth(-dw);
            _s.addHeight(-dh);
        }

        void unify(const BasicRect<T>& rect)
        {
            if( rect.isNull() )
                return;

            if( this->isNull() )
            {
                _p = rect._p;
                _s = rect._s;
                return;
            }

            const T l     = std::min( this->left(), rect.left() );
            const T t     = std::min( this->top(), rect.top() );
            const T r     = std::max( this->right(), rect.right() );
            const T b  = std::max( this->bottom(), rect.bottom() );

            set(l, r, t, b);
        }

        BasicRect<T> intersect(const BasicRect<T>& rect) const
        {
            const T l     = std::max( this->left(), rect.left() );
            const T t     = std::max( this->top(), rect.top() );
            const T r     = std::min( this->right(), rect.right() );
            const T b  = std::min( this->bottom(), rect.bottom() );

            return r >= l && b >= t ? BasicRect<T>(l, r, t, b)
                          : BasicRect<T>();
        }

        bool contains(const BasicPoint<T>& p) const
        {
            return p.x() >= _p.x() &&
                    p.x() < _p.x() + _s.width() &&
                    p.y() >= _p.y() &&
                    p.y() <  _p.y() + _s.height();
        }

    protected:
        BasicPoint<T> _p;
        BasicSize<T>  _s;
};


typedef BasicRect<Pt::ssize_t> Rect;
typedef BasicRect<double> RectF;
//typedef BasicRect<float> RectF;


inline Rect round(const RectF& r)
{
  return Rect( round(r.topLeft()),
               round(r.size()) );
}


}  // namespace

} // namespace

#endif
