/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_MATH_SIZE_H
#define PT_MATH_SIZE_H

#include <Pt/Types.h>
#include <Pt/Api.h>
#include <Pt/Math/Math.h>


namespace Pt {

	namespace Math {

		//! \brief The Size class describes the heigth and width of an object
		template<typename T>
		class PT_API BasicSize {
			public:
				//! @brief Construct a BasicSize of (w,h)
				inline BasicSize(T w = 0, T h = 0)
				: _w(w), _h(h)
				{}

				//! @brief Returns the width
				inline T width() const
				{ return _w; }

				//! @brief Returns the height
				inline T height() const
				{ return _h; }

				//! @brief Sets the width
				inline void setWidth(T w)
				{ _w = w; }

				//! @brief Sets the height
				inline void setHeight(T h)
				{ _h = h; }

                const BasicSize& addWidth(T w)
				{
					_w += w;
					return *this;
				}

                const BasicSize& subWidth(T w)
				{
					_w -= w;
					return *this;
				}

                const BasicSize& addHeight(T h)
				{
					_h += h;
					return *this;
				}
				

                const BasicSize& subHeight(T h)
				{
					_h -= h;
					return *this;
				}
				
				const BasicSize& operator=(const BasicSize& other)
				{
					_w = other._w; _h = other._h;
					return *this;
				}

				bool operator==(const BasicSize& other) const
				{ return (_w == other._w && _h == other._h); }

				bool operator!=(const BasicSize& other) const
				{ return (_w != other._w || _h != other._h); }

				bool operator>(const BasicSize& other) const
				{
					if ( _w < other._w || _h < other._h)
						return false;

					return ( (*this) != other );
				}

				bool operator<(const BasicSize& other) const
				{
					if ( _w > other._w || _h > other._h )
						return false;

					return ( other != (*this) );
				}

			protected:
				T _w;
				T _h;
		};

	} // namespace Math

} // namespace Pt

#endif
