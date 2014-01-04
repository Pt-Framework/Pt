/*
 * Copyright (C) 2006 PTV AG
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PT_GFX_RECT_H
#define PT_GFX_RECT_H

#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>


namespace Pt {

    namespace Gfx {
        //! \brief A generic Rect class
        template<typename PointT, typename SizeT>
        class BasicRect {
            public:
                //! @brief Construct a BasicRect using the given BasicPoint and BasicSize<SizeT>
                BasicRect(const BasicPoint<PointT>& p = BasicPoint<PointT>(0, 0), const BasicSize<SizeT>& s = BasicSize<SizeT>(0, 0))
                : _p(p), _s(s)
                {}

                //! @brief Construct a BasicRect at with the given BasicPoints
                BasicRect( const Pt::Gfx::BasicPoint<PointT>& p1, const Pt::Gfx::BasicPoint<PointT>& p2 )
                : _p(p1), _s( p2.x() - p1.x() + 1, p2.y() - p1.y() + 1 )
                {}

                //! @brief Construct a BasicRect at with the given BasicRect
                BasicRect(const Pt::Gfx::BasicRect<PointT,SizeT>& val)
                : _p(val._p), _s(val._s)
                {}

                //! @brief Returns true if the BasicRect has a zero width and height
                bool isNull() const
                {
                    return (_s.width() == 0 || _s.height() == 0 );
                }

                //! @brief Set this BasicRect using the given BasicRect (copy)
                void set(const Pt::Gfx::BasicRect<PointT,SizeT>& val)
                {
                    _p = val._p;
                    _s = val._s;
                }

                //! @brief Set this BasicRect using the given BasicPoint and BasicSize
                BasicRect& setGeometry(const Pt::Gfx::BasicPoint<PointT>& p,
                                       const Pt::Gfx::BasicSize<SizeT>& s)
                {
                    _p = p;
                    _s = s;
                    return *this;
                }

                //! @brief Set this BasicRect using the given BasicPoints
                BasicRect& setGeometry(const Pt::Gfx::BasicPoint<PointT>& p1, const Pt::Gfx::BasicPoint<PointT>& p2)
                {
                    this->setOrigin( p1 );
                    this->setWidth(p2.x() - p1.x() + 1);
                    this->setHeight(p2.y() - p1.y() + 1);
                    return *this;
                }

                //! @brief Set the origin of this BasicRect using the given BasicPoint
                void setOrigin(const Pt::Gfx::BasicPoint<PointT>& p)
                {
                    _p = p;
                }

                //! @brief Set the origin of this BasicRect using the given X and Y values
                void setOrigin(PointT x, PointT y)
                {
                    _p.set(x, y);
                }

                //! @brief Return the origin of this BasicRect as a const BasicPoint
                const Pt::Gfx::BasicPoint<PointT>& origin() const
                {
                    return _p;
                }

                //! @brief Set the X origin of this BasicRect
                BasicRect& setX(PointT x)
                {
                    _p.setX( x );
                    return *this;
                }

                //! @brief Set the Y origin of this BasicRect
                BasicRect& setY(PointT y)
                {
                    _p.setY( y );
                    return *this;
                }

                //! @brief Return the X origin of this BasicRect
                PointT x() const
                {
                    return _p.x();
                }

                //! @brief Return the Y origin of this BasicRect
                PointT y() const
                {
                    return _p.y();
                }

                //! @brief Set the width and height of this BasicRect using the given BasicSize
                void setSize(const Pt::Gfx::BasicSize<SizeT>& s)
                {
                    _s = s;
                }

                //! @brief Set the width and height of this BasicRect using the given width and height values
                void setSize(SizeT width, SizeT height)
                {
                    _s.setWidthHeight(width, height);
                }

                //! @brief Return the width and height of this BasicRect as a const BasicSize
                const Pt::Gfx::BasicSize<SizeT>& size() const
                {
                    return _s;
                }

                //! @brief Set the width of this BasicRect
                void setWidth(SizeT w)
                {
                    _s.setWidth(w);
                }

                //! @brief Set the height of this BasicRect
                void setHeight(SizeT w)
                {
                    _s.setHeight(w);
                }

                //! @brief Return the width of this BasicRect
                SizeT width() const
                {
                    return _s.width();
                }

                //! @brief Return the height of this BasicRect
                SizeT height() const
                {
                    return _s.height();
                }

                //! @brief Move the left side of this BasicRect to the given coordinate (does not resize the BasicRect)
                void setLeft(PointT value)
                {
                    setWidth( this->width() + this->x() - value );
                    setX( value );
                }

                //! @brief Move the top side of this BasicRect to the given coordinate (does not resize the BasicRect)
                void setTop(PointT value)
                {
                    setHeight( this->height() + this->y() - value );
                    setY( value );
                }

                //! @brief Move the right side of this BasicRect to the given coordinate (resize the BasicRect)
                void setRight( PointT value )
                {
                    setWidth( this->width() + (value - right()) );
                }

                //! @brief Move the bottom side of this BasicRect to the given coordinate (resize the BasicRect)
                void setBottom( PointT value )
                {
                    setHeight( this->height() + value - this->bottom() );
                }

                //! @brief Return the X coordinate of the left side of this BasicRect
                PointT left() const
                {
                    return _p.x();
                }

                //! @brief Return the Y coordinate of the top side of this BasicRect
                PointT top() const
                {
                    return _p.y();
                }

                //! @brief Return the X coordinate of the right side of this BasicRect
                PointT right() const
                {
                    return _p.x() + _s.width() - 1;
                }

                //! @brief Return the Y coordinate of the bottom side of this BasicRect
                PointT bottom() const
                {
                    return _p.y() + _s.height() - 1;
                }

                //! @brief Increment the position of the left side of this BasicRect by the given value
                BasicRect& addLeft(PointT delta)
                {
                  setLeft( left() + delta);
                  return *this;
                }

                //! @brief Decrement the position of the left side of this BasicRect by the given value
                BasicRect& subLeft(PointT delta)
                {
                  setLeft( left() - delta);
                  return *this;
                }

                //! @brief Increment the position of the top side of this BasicRect by the given value
                BasicRect& addTop(PointT delta)
                {
                  setTop( top() +  delta);
                  return *this;
                }

                //! @brief Decrement the position of the top side of this BasicRect by the given value
                BasicRect& subTop(PointT delta)
                {
                  setTop( top() -  delta);
                  return *this;
                }

                //! @brief Increment the position of the right side of this BasicRect by the given value
                BasicRect& addRight(PointT delta)
                {
                  setRight( right() +  delta);
                  return *this;
                }

                //! @brief Decrement the position of the right side of this BasicRect by the given value
                BasicRect& subRight(PointT delta)
                {
                  setRight( right() -  delta);
                  return *this;
                }

                //! @brief Increment the position of the bottom side of this BasicRect by the given value
                BasicRect& addBottom(PointT delta)
                {
                  setBottom( bottom() +  delta);
                  return *this;
                }

                //! @brief Decrement the position of the bottom side of this BasicRect by the given value
                BasicRect& subBottom(PointT delta)
                {
                  setBottom( bottom() -  delta);
                  return * this;
                }

                //! @brief Return the top left coordinates as a const BasicPoint<SizeT>
                const Pt::Gfx::BasicPoint<PointT>& topLeft() const
                { return this->origin(); }

                //! @brief Return the top right coordinates as a const BasicPoint<SizeT>
                const Pt::Gfx::BasicPoint<PointT> topRight() const
                { return Pt::Gfx::BasicPoint<PointT>(this->x() + this->width(), this->y()); }

                //! @brief Return the bottom left coordinates as a const BasicPoint<SizeT>
                const Pt::Gfx::BasicPoint<PointT> bottomLeft() const
                { return Pt::Gfx::BasicPoint<PointT>(this->x(), this->y() + this->height()); }

                //! @brief Return the bottom right coordinates as a const BasicPoint<SizeT>
                const Pt::Gfx::BasicPoint<PointT> bottomRight() const
                { return Pt::Gfx::BasicPoint<PointT>(this->x() + this->width(), this->y() + this->height()); }

                BasicRect<PointT,SizeT>& operator = (const BasicRect<PointT,SizeT>& val)
                {
                    _p = val._p;
                    _s = val._s;
                    return *this;
                }

                bool operator==(const BasicRect& other) const
                {
                    return _p == other._p && _s == other._s;
                }

                bool operator!=(const BasicRect& other) const
                {
                    return _p != other._p || _s != other._s;
                }

            protected:
                Pt::Gfx::BasicPoint<PointT> _p;
                Pt::Gfx::BasicSize<SizeT>  _s;
        };


        typedef BasicRect<Pt::ssize_t, Pt::size_t>  Rect;
        typedef BasicRect<double, double>           RectF;

    } // namespace Gfx

} // namespace Pt

#endif
