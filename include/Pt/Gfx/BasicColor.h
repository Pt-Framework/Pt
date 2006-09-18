/***************************************************************************
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef Pt_Gfx_BasicColor_h
#define Pt_Gfx_BasicColor_h

#include <Pt/Api.h>
#include <Pt/Types.h>


namespace Pt {

	namespace Gfx {

		//! \brief BasicColor<ColorSpaceT> interface class
		//!
		//! All template specializations must at least implement the functions
		//! defined here, however adding as many as additional functions is allowed
		//!
		template <typename ColorSpaceT>
		class BasicColor {
			public:
				typedef ColorSpaceT ColorSpace;

			public:
				//! Default ctor, will generate default color (black)
				BasicColor();

				//! Copy ctor
				BasicColor(const BasicColor<ColorSpace>& c);

				//! Construct color using the given source color (from any color space)
				template <typename C>
				BasicColor(const BasicColor<C>& c);

				//! Convert this color to its ARGB components
				void toARgb(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b) const;

				//! Set this color from the given ARGB components
				void fromARgb(uint16_t a, uint16_t r, uint16_t g, uint16_t b);

				//! Assignment operator from the same color space
				BasicColor<ColorSpace>& operator=(const BasicColor<ColorSpace>& c);

				//! Assignment operator from different color space
				template <typename C>
				BasicColor<ColorSpace>& operator=(const BasicColor<C>& c);

				//! Equality comparison operator
				bool eq(const BasicColor<ColorSpace>& c) const;

				//! Lessthan comparison operator
				bool lt(const BasicColor<ColorSpace>& c) const;

				//! Get brightness
				uint16_t brightness() const;

				//! Set brightness
				void setBrightness(uint16_t l);
		};


		//! Equality comparison operator
		template <typename ColorSpaceT> inline
		bool operator==(const BasicColor<ColorSpaceT>& c1, const BasicColor<ColorSpaceT>& c2)
		{ return c1.eq(c2); }

		//! Unequality comparison operator
		template <typename ColorSpaceT> inline
		bool operator!=(const BasicColor<ColorSpaceT>& c1, const BasicColor<ColorSpaceT>& c2)
		{ return !c1.eq(c2); }

		//! Less-than comparison operator
		template <typename ColorSpaceT> inline
		bool operator<(const BasicColor<ColorSpaceT>& c1, const BasicColor<ColorSpaceT>& c2)
		{ return c1.lt(c2); }

		//! Greater-than comparison operator
		template <typename ColorSpaceT> inline
		bool operator>(const BasicColor<ColorSpaceT>& c1, const BasicColor<ColorSpaceT>& c2)
		{ return c1.lt(c2) ? false : !c1.eq(c2) ; }

		//! Less-than or equal comparison operator
		template <typename ColorSpaceT> inline
		bool operator<=(const BasicColor<ColorSpaceT>& c1, const BasicColor<ColorSpaceT>& c2)
		{ return c1.lt(c2) ? true : c1.eq(c2) ; }

		//! Greater-than or equal comparison operator
		template <typename ColorSpaceT> inline
		bool operator>=(const BasicColor<ColorSpaceT>& c1, const BasicColor<ColorSpaceT>& c2)
		{ return c1.lt(c2) ? false : c1.eq(c2) ; }


		//! Assign color value
		template <typename CS1, typename CS2>
		void assign(BasicColor<CS1>& to, const BasicColor<CS2>& from)
		{
			uint16_t a, r, g, b;
			from.toARgb(a, r, g, b);
			to.fromARgb(a, r, g, b);
		}


		//! \brief Greyscale a color
		template <typename ColorT>
		class Greyscale
		{
			public:
				typedef ColorT Color;

				void operator()(Color& color)
				{ this->operator()(color, color); }

				template <typename Other>
				void operator()(Color& to, const Other& from)
				{
					from.toARgb(_a, _r, _g, _b);
					_f = static_cast<uint16_t>(_r*0.3f + _g*0.5f + 0.2f*_b);
					to.fromARgb(_a, _f, _f, _f);
				}

			private:
				uint16_t _a, _r, _g, _b, _f;
		};


	} // namespace Gfx

} // namespace Pt

#endif

