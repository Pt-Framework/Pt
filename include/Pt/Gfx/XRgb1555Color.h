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
#ifndef Pt_Gfx_XRgb1555Color_h
#define Pt_Gfx_XRgb1555Color_h

#include <Pt/Api.h>
#include <Pt/Gfx/ARgbColor.h>


namespace Pt {

	namespace Gfx {

		//! \brief XRgb1555 color space
		//!
		//! This struct is used just for differentiating between various image formats.
		struct XRgb1555 {};


		//! \brief XRgb1555 color class
		//!
		//! Valid range of the color components for this color model:\n
		//!    Red   : 0 to 255 (clipped to 5 bit on storage)\n
		//!    Green : 0 to 255 (clipped to 5 bit on storage)\n
		//!    Blue  : 0 to 255 (clipped to 5 bit on storage)
		template <>
		class PT_EXPORT PT_PACKED BasicColor<XRgb1555> {
			public:
				typedef XRgb1555 ColorSpaceT;

			public:
				//! Default ctor, will generate default color (black)
				inline BasicColor()
				: _val(0)
				{}

				//! Copy ctor
				inline BasicColor(const BasicColor<XRgb1555>& c)
				: _val(c._val)
				{}

				//! Construct color using the given ARgbColor
				inline BasicColor(const BasicColor<ARgb>& c)
				{ fromARgb(c.alpha(), c.red(), c.green(), c.blue()); }

				//! Construct color using the given source color (from any color space)
				template <typename SrcColorSpaceT> inline
				BasicColor(const BasicColor<SrcColorSpaceT>& c)
				: _val(0)
				{ convert(*this, c); }

				//! Construct color using the given packed color constant
				inline BasicColor(uint16_t val)
				: _val(val & 0x7FFF)
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
					a = 65535;
					r = ((uint32_t(_val) & 0x7C00) >> 10) * 65535 / 31;
					g = ((uint32_t(_val) & 0x03E0) >> 5 ) * 65535 / 31;
					b = ((uint32_t(_val) & 0x001F) >> 0 ) * 65535 / 31;
				}
				//!
				//! Set this color from the given ARGB components of the master color model
				//! (sources range from 0 to 65535)
				inline void fromARgb(uint16_t a, uint16_t r, uint16_t g, uint16_t b)
				{
					setRed  (r / 2048);
					setGreen(g / 2048);
					setBlue (b / 2048);
				}

				//! Assignment operator from the same color space
				inline BasicColor<XRgb1555>& operator=(const BasicColor<XRgb1555>& c)
				{ _val = c._val; return *this; }

				//! Assignment operator from different color space
				template <typename SrcColorSpaceT> inline
				BasicColor<XRgb1555>& operator=(const BasicColor<SrcColorSpaceT>& c)
				{ convert(*this, c); return *this; }

				//! Equality comparison operator
				inline bool eq(const BasicColor<ColorSpaceT>& c) const
				{ return _val == c._val; }

				//! Lessthan comparison operator
				inline bool lt(const BasicColor<ColorSpaceT>& c) const
				{ return _val < c._val; }

				//! Greaterthan comparison operator
				inline bool gt(const BasicColor<ColorSpaceT>& c) const
				{ return _val > c._val; }

				//! Return the packed color value of this color
				inline uint16_t color() const
				{ return _val; }

				//! Return the red component of this color
				inline uint8_t red() const
				{ return (_val & 0x7C00) >> 7; }

				//! Return the green component of this color
				inline uint8_t green() const
				{ return (_val & 0x03E0) >> 2; }

				//! Return the blue component of this color
				inline uint8_t blue() const
				{ return (_val & 0x001F) << 3; }

				//! Set the packed color value of this color
				inline void setColor(uint16_t c)
				{ _val = c & 0x7FFF; }

				//! Set the red component of this color
				inline void setRed(uint8_t r)
				{ _val &= 0x83FF; _val |= (uint16_t(r&0xF8) << 7); }

				//! Set the green component of this color
				inline void setGreen(uint8_t g)
				{ _val &= 0xFC1F; _val |= (uint16_t(g&0xF8) << 2); }

				//! Set the blue component of this color
				inline void setBlue(uint8_t b)
				{ _val &= 0xFFE0; _val |= (uint16_t(b&0xF8) >> 3); }

				//! Get brightness
				inline uint8_t brightness() const;

				//! Set brightness
				inline void setBrightness(uint8_t l);

			protected:
				uint16_t _val;
		};

		// For convenience
		typedef BasicColor<XRgb1555> XRgb1555Color;

	} //! namespace Gfx

} //! namespace Pt

#endif

