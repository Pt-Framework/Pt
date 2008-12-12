/*
 * Copyright (C) 2006 PTV AG
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

#ifndef PT_MATH_RECT_H
#define PT_MATH_RECT_H

#include <Pt/Math/Api.h>
#include <Pt/Math/Point.h>
#include <Pt/Math/Size.h>
#include <Pt/Math/Math.h>


namespace Pt {

    namespace Math {

        //! \brief A generic Rect class
        template<typename PointT, typename SizeT>
        class BasicRect {
            public:
                //! Construct a BasicRect at a given position and BasicSize<SizeT>
                BasicRect(const BasicPoint<PointT>& p = BasicPoint<PointT>(0, 0),
                          const BasicSize<SizeT>& s = BasicSize<SizeT>(0, 0))
                : _p(p)
                , _s(s)
                {}


                BasicRect& setGeometry(const Pt::Math::BasicPoint<PointT>& p,
                                       const Pt::Math::BasicSize<SizeT>& s)
                {
                    _p = p;
                    _s = s;
                    return *this;
                }

                //! Return the BasicSize<SizeT> as a BasicSize<SizeT>
                void setOrigin(const Pt::Math::BasicPoint<PointT>& p)
                {
                    _p = p;
                }

                void setOrigin(PointT x, PointT y)
                {
                    _p.set(x, y);
                }

                BasicRect& setX(PointT x)
                {
                    _p.setX( x );
                    return *this;
                }

                BasicRect& setY(PointT y)
                {
                    _p.setY( y );
                    return *this;
                }



                //! Return the BasicPoint<PointT> as a BasicPoint<PointT>
                const Pt::Math::BasicPoint<PointT>& origin() const
                {
                    return _p;
                }

                //! Return the BasicSize<SizeT> as a BasicSize<SizeT>
                void setSize(const Pt::Math::BasicSize<SizeT>& s)
                {
                    _s = s;
                }

                //! Return the BasicSize<SizeT> as a BasicSize<SizeT>
                void setSize(SizeT width, SizeT height)
                {
                    _s.setWidthHeight(width, height);
                }

                //! Return the BasicSize<SizeT> as a BasicSize<SizeT>
                void setWidth(SizeT w)
                {
                    _s.setWidth(w);
                }

                //! Return the BasicSize<SizeT> as a BasicSize<SizeT>
                void setHeight(SizeT w)
                {
                    _s.setHeight(w);
                }

                const Pt::Math::BasicSize<SizeT>& size() const
                {
                    return _s;
                }

                PointT x() const
                {
                    return _p.x();
                }

                PointT y() const
                {
                    return _p.y();
                }

                SizeT width() const
                {
                    return _s.width();
                }

                SizeT height() const
                {
                    return _s.height();
                }

                bool isNull() const
                {
                    return (_s.width() == 0 || _s.height() == 0 );
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
                Pt::Math::BasicPoint<PointT> _p;
                Pt::Math::BasicSize<SizeT>  _s;
        };

    } // namespace Math

} // namespace Pt

#endif // PT_MATH_RECT_H

