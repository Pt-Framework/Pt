/***************************************************************************
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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
#ifndef Pt_Gfx_CmykColor_h
#define Pt_Gfx_CmykColor_h

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ARgbColor.h>


namespace Pt {

	namespace Gfx {

		//!
		//! \brief Cmyk color space.
		//!
		//! This struct is used just for differentiating between various image formats.
		//!
		struct Cmyk {};


		//!
		//! \brief Cmyk color class
		//!
		//! Valid range of the color components for this color model:\n
		//!    Cyan    : 0 - 0xFFFF\n
		//!    Magenta : 0 - 0xFFFF\n
		//!    Yellow  : 0 - 0xFFFF\n
		//!    Black   : 0 - 0xFFFF
		template <>
		class PT_GFX_API BasicColor<Cmyk> {
			public:
				typedef Cmyk ColorSpaceT;

			public:
				//! Default ctor, will generate default color (black)
				inline BasicColor()
				: _c(0), _m(0), _y(0), _k(0xFFFF)
				{}

				//! Copy ctor
				inline BasicColor(const BasicColor<Cmyk>& c)
				: _c(c._c), _m(c._m), _y(c._y), _k(c._k)
				{}

				//! Construct color using the given ARgbColor
				inline BasicColor(const BasicColor<ARgb>& c)
				{ fromARgb(c.alpha(), c.red(), c.green(), c.blue()); }

				//! Construct color using the given source color (from any color space)
				template <typename SrcColorSpaceT> inline
				BasicColor(const BasicColor<SrcColorSpaceT>& c)
				: _c(0), _m(0), _y(0), _k(0)
				{ convert(*this, c); }


				//! Construct color using the given components
				inline BasicColor(uint16_t c, uint16_t m, uint16_t y, uint16_t k)
				: _c(0), _m(0), _y(0), _k(0)
				{
					setCyan(c);
					setMagenta(m);
					setYellow(y);
					setBlack(k);
				}

				//! Convert this color to ARGB components of the master color model
				//! (destinations range from 0 to 0xFFFF)
				void toARgb(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b) const;

				//! Set this color from the given ARGB components of the master color model
				//! (sources range from 0 to 0xFFFF)
				void fromARgb(uint16_t a, uint16_t r, uint16_t g, uint16_t b);

				//! Assignment operator from the same color space
				inline BasicColor<Cmyk>& operator=(const BasicColor<Cmyk>& c)
				{ _c = c._c; _m = c._m; _y = c._y; _k = c._k; return *this; }

				//! Assignment operator from different color space
				template <typename T> inline
				BasicColor<Cmyk>& operator=(const BasicColor<T>& c)
				{ assign(*this, c); return *this; }

				//! Equality comparison operator
				inline bool eq(const BasicColor<ColorSpaceT>& c) const
				{ return _c==c._c && _m==c._m && _y==c._y && _k==c._k; }

				//! Lessthan comparison operator
				bool lt(const BasicColor<ColorSpaceT>& c) const;

				//! Greaterthan comparison operator
				bool gt(const BasicColor<ColorSpaceT>& c) const;

				//! Return the cyan component of this color
				inline uint16_t cyan() const
				{ return _c; }

				//! Return the magenta component of this color
				inline uint16_t magenta() const
				{ return _m; }

				//! Return the yellow component of this color
				inline uint16_t yellow() const
				{ return _y; }

				//! Return the black (darkness / reverse intensity) component of this color
				inline uint16_t black() const
				{ return _k; }

				//! Set the cyan component of this color
				inline void setCyan(uint16_t c)
				{ _c = c; }

				//! Set the magenta component of this color
				inline void setMagenta(uint16_t m)
				{ _m = m; }

				//! Set the yellow component of this color
				inline void setYellow(uint16_t y)
				{ _y = y; }

				//! Set the black (darkness / reverse intensity) component of this color
				inline void setBlack(uint16_t k)
				{ _k = k; }

				//! Get brightness
				inline uint16_t brightness() const
				{ ARgbColor t; t = *this; return t.brightness(); }

				//! Set brightness
				inline void setBrightness(uint16_t l)
				{ ARgbColor t; t = *this; t.setBrightness(l); *this = t; }

			protected:
				uint16_t _c, _m, _y, _k;
		};

		// For convenience
		typedef BasicColor<Cmyk> CmykColor;

	} // namespace Gfx

} // namespace Pt

#endif

