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
#ifndef Pt_Gfx_XRgb8888Color_h
#define Pt_Gfx_XRgb8888Color_h

#include <Pt/Api.h>
#include <Pt/Gfx/ARgbColor.h>


namespace Pt {

	namespace Gfx {

		//! \brief XRgb8888 color space
		struct XRgb8888 {};


		//! \brief XRgb8888 color class
		//!
		//! Valid range of the color components for this color model:\n
		//!    Red   : 0 to 255\n
		//!    Green : 0 to 255\n
		//!    Blue  : 0 to 255
		template <>
		class PT_API PT_PACKED BasicColor<XRgb8888> {
			public:
				typedef XRgb8888 ColorSpaceT;

			public:
				//! Default ctor, will generate default color (black)
				inline BasicColor()
				: _val(0)
				{}

				//! Copy ctor
				inline BasicColor(const BasicColor<XRgb8888>& c)
				: _val(c._val)
				{}

				//! Construct color using the given ARgbColor
				inline BasicColor(const BasicColor<ARgb>& c)
				{ fromARgb(c.alpha(), c.red(), c.green(), c.blue()); }

				//! Construct color using the given source color (from any color space)
				template <typename SrcColorSpaceT> inline
				BasicColor(const BasicColor<SrcColorSpaceT>& c)
				: _val(0)
				{ assign(*this, c); }

				//! Construct color using the given packed color constant
				inline BasicColor(uint32_t val)
				: _val(val & 0x00FFFFFF)
				{}

				//! Construct color using the given components
				inline BasicColor(uint8_t r, uint8_t g, uint8_t b)
				: _val(0)
				{
					setRed(r);
					setGreen(g);
					setBlue(b);
				}

				//!
				//! Convert this color to ARGB components of the master color model
				//! (destinations range from 0 to 65535)
				inline void toARgb(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b) const
				{
					// 33333333222222221111111100000000
					// 76543210765432107654321076543210
					// XXXXXXXXRRRRRRRRGGGGGGGGBBBBBBBB
					a = 65535;
					r = ((_val & 0x00FF0000) >> 16) * 257;
					g = ((_val & 0x0000FF00) >> 8 ) * 257;
					b = ( _val & 0x000000FF       ) * 257;
				}
				//!
				//! Set this color from the given ARGB components of the master color model
				//! (sources range from 0 to 65535)
				inline void fromARgb(uint16_t a, uint16_t r, uint16_t g, uint16_t b)
				{
					setRed  (uint8_t(r >> 8));
					setGreen(uint8_t(g >> 8));
					setBlue (uint8_t(b >> 8));
				}

				//! Assignment operator from the same color space
				inline BasicColor<XRgb8888>& operator=(const BasicColor<XRgb8888>& c)
				{ _val = c._val; return *this; }

				//! Assignment operator from different color space
				template <typename SrcColorSpaceT> inline
				BasicColor<XRgb8888>& operator=(const BasicColor<SrcColorSpaceT>& c)
				{ convert(*this, c); return *this; }

				//! Equality comparison operator
				inline bool eq(const BasicColor<ColorSpaceT>& c) const
				{ return _val == c._val; }
				//!
				//! Lessthan comparison operator
				inline bool lt(const BasicColor<ColorSpaceT>& c) const
				{ return _val < c._val; }
				//!
				//! Greaterthan comparison operator
				inline bool gt(const BasicColor<ColorSpaceT>& c) const
				{ return _val > c._val; }

				//! Return the packed color value of this color
				inline uint32_t color() const
				{ return _val; }

				//! Return the red component of this color (in range 0 to 255)
				inline uint8_t red() const
				{ return (_val & 0x00FF0000) >> 16; }

				//! Return the green component of this color (in range 0 to 255)
				inline uint8_t green() const
				{ return (_val & 0x0000FF00) >> 8; }

				//! Return the blue component of this color (in range 0 to 255)
				inline uint8_t blue() const
				{ return _val & 0x000000FF; }

				//! Set the packed color value of this color
				void setColor(uint32_t c)
				{ _val = c; }

				//! Set the red component of this color (in range 0 to 255)
				inline void setRed(uint8_t r)
				{ _val = _val & 0xFF00FFFF | (uint32_t(r) << 16); }

				//! Set the green component of this color (in range 0 to 255)
				inline void setGreen(uint8_t g)
				{ _val = _val & 0xFFFF00FF | (uint32_t(g) << 8); }

				//! Set the blue component of this color (in range 0 to 255)
				inline void setBlue(uint8_t b)
				{ _val = _val & 0xFFFFFF00 | uint32_t(b); }

				//! Get brightness (in range 0 to 255)
				inline uint8_t brightness() const;

				//! Set brightness (in range 0 to 255)
				inline void setBrightness(uint8_t l);

			protected:
				uint32_t _val;
		};

		// For convenience
		typedef BasicColor<XRgb8888> XRgb8888Color;


	} // namespace Gfx

} // namespace Pt

#endif
