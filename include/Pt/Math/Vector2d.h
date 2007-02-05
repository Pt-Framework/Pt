/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PTV_GFX_VECTOR2D_H
#define PTV_GFX_VECTOR2D_H

#include <Pt/Types.h>
#include <Pt/Math/Api.h>
#include <Pt/Math/Math.h>
#include <math.h>


namespace Pt {

namespace Math {

    /** \brief BasicVector2d class
	*/
	template <typename T>
	class BasicVector2d {

        public:

			BasicVector2d():
				m_x(0),
                m_y(0)
			{
            }


            BasicVector2d(const BasicVector2d<T>& vector)
            {
                this->m_x = vector.m_x;
                this->m_y = vector.m_y;
            }


			BasicVector2d(const T& x, const T& y):
				 m_x(x),
                 m_y(y)
            {
            }

            template<typename PointType>
            BasicVector2d(const BasicPoint<PointType>& startPt, const BasicPoint<PointType>& endPt)
            : m_x( (T)(endPt.x() - startPt.x()) )
            , m_y( (T)(endPt.y() - startPt.y()) )
            {
            }

            template<typename PointType>
            inline void set(const BasicPoint<PointType>& startPt, const BasicPoint<PointType>& endPt)
            {
                m_x = (T)(endPt.x() - startPt.x());
                m_y = (T)(endPt.y() - startPt.y());
            }

            void normalize()
            {
                double val = (double)(this->m_x * this->m_x + this->m_y * this->m_y);
	            double length = sqrt(val);
	            if( (length != 0.0) && (length != 1.0) )
                {
		            this->m_x = (T)(this->m_x / length);
		            this->m_y = (T)(this->m_y / length);
    	        }
            }


            inline const BasicVector2d<T>& operator=(const BasicVector2d<T>& vector)
            {
                this->m_x = vector.m_x;
                this->m_y = vector.m_y;
				return *this;
			}


            inline bool operator==(const BasicVector2d<T>& vector) const
			{
                return (this->m_x == vector.m_x) && (this->m_y == vector.m_y);
            }


            inline bool operator!=(const BasicVector2d<T>& vector) const
			{
                return (this->m_x != vector.m_x) || (this->m_y != vector.m_y);
            }


            inline const BasicVector2d<T>& operator+=(const BasicVector2d<T>& vector)
            {
                this->m_x += vector.m_x;
                this->m_y += vector.m_y;
				return *this;
			}


            inline BasicVector2d<T> operator+(const BasicVector2d<T>& vector)
            {
                BasicVector2d<T> resVector(*this);
                resVector += vector;
				return resVector;
			}

            inline const BasicVector2d<T>& operator-=(const BasicVector2d<T>& vector)
            {
                this->m_x -= vector.m_x;
                this->m_y -= vector.m_y;
				return *this;
			}


            inline BasicVector2d<T> operator-(const BasicVector2d<T>& vector)
            {
                BasicVector2d<T> resVector(*this);
                resVector -= vector;
				return resVector;
			}


            template <typename valT>
            const BasicVector2d<T>& operator*=(const valT& factor)
            {
                this->m_x = (T)(this->m_x * factor);
                this->m_y = (T)(this->m_y * factor);
				return *this;
			}


            template <typename valT>
            inline BasicVector2d<T> operator*(const valT& factor)
            {
                BasicVector2d<T> resVector(*this);
                resVector *= factor;
				return resVector;
			}


            /** \brief Calculate the scalar product of two vectors.
                \param
                \return
			*/
            inline double operator*(const BasicVector2d<T>& vector)
            {
                return this->m_x * vector.m_x + this->m_y * vector.m_y;
			}

            
            inline const BasicVector2d<T> operator+=(const BasicPoint<T>& point)
            {
                m_x += point.x();
                m_y += point.y();
			    return *this;
		    }

            
            inline BasicVector2d<T> operator+(const BasicPoint<T>& point) const
			{
                BasicVector2d<T> newVector( (m_x+point.x()), (m_y+point.y()) );
                return newVector;
            }


            inline const BasicVector2d<T> operator-=(const BasicPoint<T>& point)
            {
                m_x -= point.x();
                m_y -= point.y();
			    return *this;
		    }

            
            inline BasicVector2d<T> operator-(const BasicPoint<T>& point) const
			{
                BasicVector2d<T> newVector( (m_x-point.x()), (m_y-point.y()) );
                return newVector;
            }


            inline double length() const
            {
                double val = (double)(this->m_x * this->m_x + this->m_y * this->m_y);
                return sqrt(val);
            }


            inline double sqrtLength() const
            {
                return (double)(this->m_x * this->m_x + this->m_y * this->m_y);
            }


            inline T x() const
            {
                return this->m_x;
            }


            inline T y() const
            {
                return this->m_y;
            }


            inline void setX(const T& x)
            {
                this->m_x = x;
            }


            inline void setY(const T& y)
            {
                this->m_y = y;
            }

            inline void set(const T& x, const T& y)
            {
				this->m_x = x;
                this->m_y = y;
            }

        protected:
            T m_x;
            T m_y;

};

} // namespace Math

} // namespace Pt

#endif // PTV_GFX_VECTOR2D_H
