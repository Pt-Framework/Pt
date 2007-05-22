/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_GFX_RECT_H
#define PT_GFX_RECT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Math/Math.h>
#include <Pt/Math/Rect.h>


namespace Pt {

    namespace Gfx {

        //! \brief A generic Rect class
        template<typename PointT, typename SizeT>
        class BasicRect {
            public:
                //! Construct a BasicRect at with a given Math::Rect
                BasicRect(const Pt::Math::BasicRect<PointT,SizeT>& val)
                : _r(val)
                {}

                //! Construct a BasicRect at a given position and BasicSize<SizeT>
                BasicRect(const Pt::Math::BasicPoint<PointT>& p = Pt::Math::BasicPoint<PointT>(0, 0), const Pt::Math::BasicSize<SizeT>& s = Pt::Math::BasicSize<SizeT>(1, 1))
                : _r(p,s)
                {}

                BasicRect( const Pt::Math::BasicPoint<PointT>& p1, const Pt::Math::BasicPoint<PointT>& p2 )
                : _r(p1, Math::Size( p2.x() - p1.x() + 1, p2.y() - p1.y() + 1 ) )
                {}


                Pt::Math::BasicRect<PointT,SizeT>& mathRect()
                { return _r; }

                const Pt::Math::BasicRect<PointT,SizeT>& mathRect() const
                { return _r; }

                void set(const Pt::Math::BasicRect<PointT,SizeT>& val)
                { _r = val; }

                //! Return the BasicSize<SizeT> as a BasicSize<SizeT>
                void setSize(const Pt::Math::BasicSize<SizeT>& s)
                { _r.setSize(s); }


                const Pt::Math::BasicSize<SizeT>& size() const
                { return _r.size(); }


                PointT left() const
                { return _r.x(); }


                PointT top() const
                { return _r.y(); }


                PointT x() const
                { return _r.x(); }


                PointT y() const
                { return _r.y(); }

                BasicRect& setX(PointT x)
                {
                    _r.setX( x );
                    return *this;
                }

                BasicRect& setY(PointT y)
                {
                    _r.setY( y );
                    return *this;
                }

                PointT right() const
                { return _r.x() + _r.width() - 1; }

                PointT bottom() const
                {
                    return _r.y() + _r.height() - 1;
                }

                void setLeft(PointT value)
                {
                    _r.setWidth( _r.width() + _r.x() - value );
                    _r.setX( value );
                }

                void setTop(PointT value)
                {
                    _r.setHeight( _r.y() - value  + _r.height());
                    _r.setY( value );
                }

                void setRight( PointT value )
                {
                    _r.setWidth( width() + (value - right()) );
                }

                void setBottom( PointT value )
                {
                    _r.setHeight( height() + value - this->bottom() );
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
                { return _r.width(); }

                SizeT height() const
                { return _r.height(); }

                BasicRect& setWidth(SizeT w)
                {
                     _r.setWidth(w);
                     return *this;
                }

                BasicRect& setHeight(SizeT h)
                {
                    _r.setHeight(h);
                     return *this;
                }

                BasicRect& setGeometry(const Pt::Math::BasicPoint<PointT>& p, const Pt::Math::BasicSize<SizeT>& s)
                {
                    _r.setGeometry(p,s);
                    return *this;
                }

                BasicRect& setGeometry(const Pt::Math::BasicPoint<PointT>& p1, const Pt::Math::BasicPoint<PointT>& p2)
                {
                    _r.setOrigin( p1 );
                    _r.setWidth(p2.x() - p1.x() + 1);
                    _r.setHeight(p2.y() - p1.y() + 1);
                    return *this;
                }

                bool isNull() const
                {
                    return (_r.width() == 0 || _r.height() == 0 );
                }

                //! Return the top left coordinates as a const BasicPoint<SizeT>
                const Pt::Math::BasicPoint<PointT>& topLeft() const
                { return _r.origin(); }

                //! Return the top right coordinates as a const BasicPoint<SizeT>
                const Pt::Math::BasicPoint<PointT> topRight() const
                { return Pt::Math::BasicPoint<PointT>(_r.x() + _r.width(), _r.y()); }

                //! Return the bottom left coordinates as a const BasicPoint<SizeT>
                const Pt::Math::BasicPoint<PointT> bottomLeft() const
                { return Pt::Math::BasicPoint<PointT>(_r.x(), _r.y() + _r.height()); }

                //! Return the bottom right coordinates as a const BasicPoint<SizeT>
                const Pt::Math::BasicPoint<PointT> bottomRight() const
                { return Pt::Math::BasicPoint<PointT>(_r.x() + _r.width(), _r.y() + _r.height()); }

                bool operator==(const BasicRect& other) const
                { return _r == other._r; }

                bool operator!=(const BasicRect& other) const
                { return _r != other._r; }

                BasicRect<PointT,SizeT>& operator = (const BasicRect<PointT,SizeT>& rhs)
                {
                    _r = rhs._r;
                    return *this;
                }

                BasicRect<PointT,SizeT>& operator = (const Math::BasicRect<PointT,SizeT>& mr)
                {
                    _r = mr;
                    return *this;
                }

            protected:
                Pt::Math::BasicRect<PointT,SizeT> _r;

        };

    } // namespace Math

} // namespace Pt

#endif
