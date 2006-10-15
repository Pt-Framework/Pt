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
#ifndef Pt_Gfx_YuvColor_h
#define Pt_Gfx_YuvColor_h

#include <Pt/Gfx/ARgbColor.h>
#include <algorithm>


namespace Pt {

namespace Gfx {

	//! \brief Yuv color space
	//!
	//! This struct is used just for differentiating between various image formats.
	struct Yuv { };


	//! \brief Yuv color class
	//!
	//! Valid range of the color components for this color model:\n
	//!    Luminance     :      0 to 32767\n
	//!    U-Chrominance : -16384 to 16384\n
	//!    V-Chrominance : -16384 to 16384
	template <>
	class PT_EXPORT BasicColor<Yuv> {
		public:
			typedef Yuv ColorSpaceT;

		public:
			//! Default ctor, will generate default color (black)
			inline BasicColor()
			: _y(0), _u(0), _v(0)
			{}

			//! Copy ctor
			inline BasicColor(const BasicColor<Yuv>& c)
			: _y(c._y), _u(c._u), _v(c._v)
			{}

			//! Construct color using the given ARgbColor
			inline BasicColor(const BasicColor<ARgb>& c)
			{ fromARgb(c.alpha(), c.red(), c.green(), c.blue()); }

			//! Construct color using the given source color (from any color space)
			template <typename SrcColorSpaceT> inline
			BasicColor(const BasicColor<SrcColorSpaceT>& c)
			: _y(0), _u(0), _v(0)
			{ convert(*this, c); }

			//! Construct color using the given components
			inline BasicColor(int16_t y, int16_t u, int16_t v)
			: _y(0), _u(0), _v(0)
			{
				setLuminance(y);
				setChrominanceU(u);
				setChrominanceV(v);
			}

			//! Convert this color to ARGB components of the master color model
			//! (destinations range from 0 to 32767)
			void toARgb(Pt::uint16_t& a, Pt::uint16_t& r, Pt::uint16_t& g, Pt::uint16_t& b) const;

			//! Set this color from the given ARGB components of the master color model
			//! (sources range from 0 to 32767)
			void fromARgb(uint16_t a, uint16_t r, uint16_t g, uint16_t b);

			//! Assignment operator from the same color space
			inline BasicColor<Yuv>& operator=(const BasicColor<Yuv>& c)
			{ _y = c._y; _u = c._u; _v = c._v; return *this; }

			//! Assignment operator from different color space
			template <typename SrcColorSpaceT> inline
			BasicColor<Yuv>& operator=(const BasicColor<SrcColorSpaceT>& c)
			{ assign(*this, c); return *this; }

			//! Equality comparison operator
			inline bool eq(const BasicColor<ColorSpaceT>& c) const
			{ return _y==c._y && _u==c._u && _v==c._v; }

			//! Lessthan comparison operator
			bool lt(const BasicColor<ColorSpaceT>& c) const;

			//! Greaterthan comparison operator
			bool gt(const BasicColor<ColorSpaceT>& c) const;

			//! Return the luminance component of this color
			inline int16_t luminance() const
			{ return _y; }

			//! Return the u-chrominance component of this color
			inline int16_t chrominanceU() const
			{ return _u; }

			//! Return the v-chrominance component of this color
			int16_t chrominanceV() const
			{ return _v; }

			//! Set the luminance component of this color
			inline void setLuminance (int16_t y)
			{ _y = std::max<int16_t>(0, y); }

			//! Set the u-chrominance component of this color
			inline void setChrominanceU(int16_t u)
			{ _u = (u < -16384) ? -16384 : ((u > 16384) ? 16384 : u); }

			//! Set the v-chrominance component of this color
			inline void setChrominanceV(int16_t v)
			{ _v = (v < -16384) ? -16384 : ((v > 16384) ? 16384 : v); }

			//! Get brightness
			inline int16_t brightness()
			{ ARgbColor t; t = *this; return t.brightness(); }

			//! Set brightness
			inline void setBrightness(int16_t l)
			{
				ARgbColor t;
				assign(t, *this);
				t.setBrightness(l);
				assign(*this, t);
			}

		protected:
			int16_t _y, _u, _v;
	};

	// For convenience
	typedef BasicColor<Yuv> YuvColor;

} // namespace Gfx

} // namespace Pt

#endif

