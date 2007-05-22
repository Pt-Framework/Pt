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

