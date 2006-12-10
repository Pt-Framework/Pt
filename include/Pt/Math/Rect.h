/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_MATH_RECT_H
#define PT_MATH_RECT_H

#include <Pt/Math/Point.h>
#include <Pt/Math/Size.h>
#include <Pt/Math/Math.h>


namespace Pt {

	namespace Math {

		//! \brief A generic Rect class
		template<typename T>
		class PT_API BasicRect {
			public:
				//! Construct a BasicRect at a given position and BasicSize<T>
				BasicRect(const BasicPoint<T>& p = BasicPoint<T>(0, 0), const BasicSize<T>& s = BasicSize<T>(1, 1))
				: _p(p)
				, _s(s)
				{}
				
				BasicRect( const BasicPoint<T>& p1, const BasicPoint<T>& p2 )
				: _p(p1)
				, _s( p2.x() - p1.x() + 1, p2.y() - p1.y() + 1 )
				{}
				
				//! Return the BasicSize<T> as a BasicSize<T>
				void setSize(const BasicSize<T>& s)
				{ _s = s; }

				const BasicSize<T>& size() const
				{ return _s; }


				T left() const
				{ return _p.x(); }


				T top() const
				{ return _p.y(); }


				T x() const
				{ return _p.x(); }


				T y() const
				{ return _p.y(); }

                BasicRect& setX(T x)
                {
                    _p.setX( x );
                    return *this;
                }
                
                BasicRect& setY(T y)
                {
                    _p.setY( y );
                    return *this;
                }
                
				T right() const
				{ return _p.x() + _s.width() - 1; }

				T bottom() const
				{ 
				    return _p.y() + _s.height() - 1; 
                }

				void setLeft(T value)
				{ 
				    _s.setWidth( _s.width() + _p.x() - value );
				    _p.setX( value ); 				    
				}

				void setTop(T value)
				{ 
				    _s.setHeight( _p.y() - value  + _s.height());
                    _p.setY( value ); 
				}

				void setRight( T value )
				{ 
				    _s.setWidth( width() + (value - right()) ); 
				}
				
				void setBottom( T value )
				{ 
				    _s.setHeight( height() + value - this->bottom() ); 
                }

                BasicRect& addLeft(T delta)
                {
                  setLeft( left() + delta);                  
                  return *this;
                }
                
                BasicRect& subLeft(T delta)
                {
                  setLeft( left() - delta); 
                  return *this;
                }
                
                BasicRect& addTop(T delta)
                {
                  setTop( top() +  delta); 
                  return *this;
                }
                
                BasicRect& subTop(T delta)
                {
                  setTop( top() -  delta); 
                  return *this;
                }       
                     
                BasicRect& addRight(T delta)
                {
                  setRight( right() +  delta); 
                  return *this;
                }
                
                BasicRect& subRight(T delta)
                {
                  setRight( right() -  delta); 
                  return *this;
                }
               
                BasicRect& addBottom(T delta)
                {
                  setBottom( bottom() +  delta); 
                  return *this;
                }
                
                BasicRect& subBottom(T delta)
                {
                  setBottom( bottom() -  delta); 
                  return * this;
                }      
                                            
				T width() const
				{ return _s.width(); }

				T height() const
				{ return _s.height(); }

				BasicRect& setWidth(T w)
				{
				     _s.setWidth(w); 
				     return *this;				
				}

				BasicRect& setHeight(T h)
				{
				    _s.setHeight(h); 
				     return *this;			
				}

				BasicRect& setGeometry(const BasicPoint<T>& p, const BasicSize<T>& s)
				{
					_p = p;
					_s = s;
					return *this;
				}

				BasicRect& setGeometry(const BasicPoint<T>& p1, const BasicPoint<T>& p2)
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

				//! Return the top left coordinates as a const BasicPoint<T>
				const BasicPoint<T>& topLeft() const
				{ return _p; }

				//! Return the top right coordinates as a const BasicPoint<T>
				const BasicPoint<T> topRight() const
				{ return BasicPoint<T>(_p.x() + _s.width(), _p.y()); }

				//! Return the bottom left coordinates as a const BasicPoint<T>
				const BasicPoint<T> bottomLeft() const
				{ return BasicPoint<T>(_p.x(), _p.y() + _s.height()); }

				//! Return the bottom right coordinates as a const BasicPoint<T>
				const BasicPoint<T> bottomRight() const
				{ return BasicPoint<T>(_p.x() + _s.width(), _p.y() + _s.height()); }

				bool operator==(const BasicRect& other) const
				{ return _p == other._p && _s == other._s; }

				bool operator!=(const BasicRect& other) const
				{ return _p != other._p || _s != other._s; }

			protected:
				BasicPoint<T> _p;
				BasicSize<T>  _s;
				
		};

	} // namespace Math

} // namespace Pt

#endif
