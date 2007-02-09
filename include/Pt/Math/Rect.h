/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

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
                          const BasicSize<SizeT>& s = BasicSize<SizeT>(1, 1))
                : _p(p)
                , _s(s)
                {}

                BasicRect( const BasicPoint<PointT>& p1, const BasicPoint<PointT>& p2 )
                : _p(p1)
                , _s( p2.x() - p1.x() + 1, p2.y() - p1.y() + 1 )
                {}

                //! Return the BasicSize<SizeT> as a BasicSize<SizeT>
                void setSize(const BasicSize<SizeT>& s)
                { _s = s; }


                const BasicSize<SizeT>& size() const
                { return _s; }


                PointT left() const
                { return _p.x(); }


                PointT top() const
                { return _p.y(); }


                PointT x() const
                { return _p.x(); }


                PointT y() const
                { return _p.y(); }

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

                PointT right() const
                { return _p.x() + _s.width() - 1; }

                PointT bottom() const
                {
                    return _p.y() + _s.height() - 1;
                }

                void setLeft(PointT value)
                {
                    _s.setWidth( _s.width() + _p.x() - value );
                    _p.setX( value );
                }

                void setTop(PointT value)
                {
                    _s.setHeight( _p.y() - value  + _s.height());
                    _p.setY( value );
                }

                void setRight( PointT value )
                {
                    _s.setWidth( width() + (value - right()) );
                }

                void setBottom( PointT value )
                {
                    _s.setHeight( height() + value - this->bottom() );
                }

                BasicRect& addLeft(PointT delta)
                {
                  setLeft( left() + delta);
                  return *this;
                }

                BasicRect& subLeft(PointT delta)
                {
                  setLeft( left() - delta);
                  return *this;
                }

                BasicRect& addTop(PointT delta)
                {
                  setTop( top() +  delta);
                  return *this;
                }

                BasicRect& subTop(PointT delta)
                {
                  setTop( top() -  delta);
                  return *this;
                }

                BasicRect& addRight(PointT delta)
                {
                  setRight( right() +  delta);
                  return *this;
                }

                BasicRect& subRight(PointT delta)
                {
                  setRight( right() -  delta);
                  return *this;
                }

                BasicRect& addBottom(PointT delta)
                {
                  setBottom( bottom() +  delta);
                  return *this;
                }

                BasicRect& subBottom(PointT delta)
                {
                  setBottom( bottom() -  delta);
                  return * this;
                }

                SizeT width() const
                { return _s.width(); }

                SizeT height() const
                { return _s.height(); }

                BasicRect& setWidth(SizeT w)
                {
                     _s.setWidth(w);
                     return *this;
                }

                BasicRect& setHeight(SizeT h)
                {
                    _s.setHeight(h);
                     return *this;
                }

                BasicRect& setGeometry(const BasicPoint<PointT>& p, const BasicSize<SizeT>& s)
                {
                    _p = p;
                    _s = s;
                    return *this;
                }

                BasicRect& setGeometry(const BasicPoint<PointT>& p1, const BasicPoint<PointT>& p2)
                {
                    _p = p1;
                    _s.setWidth(p2.x() - p1.x() + 1);
                    _s.setHeight(p2.y() - p1.y() + 1);
                    return *this;
                }

                bool isNull() const
                {
                    return (_s.width() == 0 || _s.height() == 0 );
                }

                //! Return the top left coordinates as a const BasicPoint<SizeT>
                const BasicPoint<PointT>& topLeft() const
                { return _p; }

                //! Return the top right coordinates as a const BasicPoint<SizeT>
                const BasicPoint<PointT> topRight() const
                { return BasicPoint<PointT>(_p.x() + _s.width(), _p.y()); }

                //! Return the bottom left coordinates as a const BasicPoint<SizeT>
                const BasicPoint<PointT> bottomLeft() const
                { return BasicPoint<PointT>(_p.x(), _p.y() + _s.height()); }

                //! Return the bottom right coordinates as a const BasicPoint<SizeT>
                const BasicPoint<PointT> bottomRight() const
                { return BasicPoint<PointT>(_p.x() + _s.width(), _p.y() + _s.height()); }

                bool operator==(const BasicRect& other) const
                { return _p == other._p && _s == other._s; }

                bool operator!=(const BasicRect& other) const
                { return _p != other._p || _s != other._s; }

            protected:
                BasicPoint<PointT> _p;
                BasicSize<SizeT>  _s;

        };

	} // namespace Math

} // namespace Pt

#endif
