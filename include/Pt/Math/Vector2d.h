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

/*
            BasicVector2d(const BasicPoint2d& startPt, const BasicPoint2d& endPt)
            {
                m_x = entPt.x - startPt.x;
                m_x = entPt.y - startPt.y;
            }

            BasicVector2d(const BasicPoint2d& point)
            {
                m_x = point.m_x;
                m_y = point.m_y;
            }

            void set(const BasicPoint2d& startPt, const BasicPoint2d& endPt)
            {
                m_x = entPt.x - startPt.x;
                m_x = entPt.y - startPt.y;
            }

            void set(const BasicPoint2d& point)
            {
                m_x = point.m_x;
                m_y = point.m_y;
            }
*/

			void set(const T& x, const T& y)
            {
				this->m_x = x;
                this->m_y = y;
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


            const BasicVector2d<T>& operator=(const BasicVector2d<T>& vector)
            {
                this->m_x = vector.m_x;
                this->m_y = vector.m_y;
				return *this;
			}


            bool operator==(const BasicVector2d<T>& vector) const
			{
                return (this->m_x == vector.m_x) && (this->m_y == vector.m_y);
            }


            bool operator!=(const BasicVector2d<T>& vector) const
			{
                return (this->m_x != vector.m_x) || (this->m_y != vector.m_y);
            }


            const BasicVector2d<T>& operator+=(const BasicVector2d<T>& vector)
            {
                this->m_x += vector.m_x;
                this->m_y += vector.m_y;
				return *this;
			}


            BasicVector2d<T> operator+(const BasicVector2d<T>& vector)
            {
                BasicVector2d<T> resVector(*this);
                resVector += vector;
				return resVector;
			}

            const BasicVector2d<T>& operator-=(const BasicVector2d<T>& vector)
            {
                this->m_x -= vector.m_x;
                this->m_y -= vector.m_y;
				return *this;
			}


            BasicVector2d<T> operator-(const BasicVector2d<T>& vector)
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
            BasicVector2d<T> operator*(const valT& factor)
            {
                BasicVector2d<T> resVector(*this);
                resVector *= factor;
				return resVector;
			}


            /** \brief Calculate the scalar product of two vectors.
                \param
                \return
			*/
            double operator*(const BasicVector2d<T>& vector)
            {
                return this->m_x * vector.m_x + this->m_y * vector.m_y;
			}


            double length() const
            {
                double val = (double)(this->m_x * this->m_x + this->m_y * this->m_y);
                return sqrt(val);
            }


            double sqrtLength() const
            {
                return (double)(this->m_x * this->m_x + this->m_y * this->m_y);
            }


            T x() const
            {
                return this->m_x;
            }


            T y() const
            {
                return this->m_y;
            }


            void setX(const T& x)
            {
                this->m_x = x;
            }


            void setY(const T& y)
            {
                this->m_y = y;
            }


        protected:
            T m_x;
            T m_y;

};

} // namespace Math

} // namespace Pt

#endif // PTV_GFX_VECTOR2D_H
