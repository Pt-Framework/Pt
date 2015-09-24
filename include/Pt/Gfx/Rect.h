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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
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
      BasicRect( const BasicPoint<T>& p = BasicPoint<T>(0, 0), const BasicSize<T>& s = BasicSize<T>(0, 0) )
      : _p(p)
			, _s(s)
      {
			}

      BasicRect( const BasicPoint<T>& p1, const BasicPoint<T>& p2 )
      : _p(p1)
			, _s( p2.x() - p1.x() + 1, p2.y() - p1.y() + 1 )
      {				
			}
        
			BasicRect( const T left, const T right, const T top, const T bottom )
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

      void set( const BasicPoint<T>& p, const BasicSize<T>& s ) 
      {
          _p = p;
          _s = s;
      }

      void set( const BasicPoint<T>& p1, const BasicPoint<T>& p2 )
      {
          this->setOrigin( p1 );
          this->setWidth(p2.x() - p1.x() + 1);
          this->setHeight(p2.y() - p1.y() + 1);
      }

      void set( const T left, const T right, const T top, const T bottom )
      {
          _p = BasicPoint<T>( left, right );
          _s = BasicSize<T>(  right- left + 1 , bottom - top  + 1);
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
          return _p.x() + _s.width() - 1;
      }

      T bottom() const
      {
          return _p.y() + _s.height() - 1;
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
				return BasicPoint<T>(this->x() + this->width(), this->y() + this->height()); 
			}

      BasicRect<T>& operator = (const BasicRect<T>& val)
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

      std::vector<BasicPoint<T> > points() const
      {
        std::vector<BasicPoint<T> > point;

        point.push_back( topLeft() );
        point.push_back( topRight() );
        point.push_back( bottomRight() );
        point.push_back( bottomLeft() );
        return point;
      }


			BasicRect<T> intersect( const BasicRect<T>& r ) const 
			{
				const T left	 = std::max( this->left(), r.left() );
				const T top		 = std::max( this->top(), r.top() ) ;
				const T right	 = std::min( this->right(), r.right() );
				const T bottom = std::min( this->bottom(), r.bottom() );

				return ( (right > left && bottom > top ) ) ? BasicRect<T>(left, right, top, bottom ) : BasicRect<T>() ;
			}


			bool contains( const BasicPoint<T>& p ) const 
			{
					return (  p.x() >= _p.x() &&  p.x() < ( _p.x()  + _s.width() ) && p.y() >= _p.y() &&  p.y() <  ( _p.y()  + _s.height() ) );
			}

  protected:
      BasicPoint<T> _p;
      BasicSize<T>  _s;
};

typedef BasicRect<Pt::ssize_t>  Rect;
typedef BasicRect<double>       RectF;

}} // namespace

#endif
