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
#ifndef Pt_Gfx_ARgb8888Color_h
#define Pt_Gfx_ARgb8888Color_h

#include <Pt/Api.h>
#include <Pt/Gfx/ARgbColor.h>


namespace Pt {

	namespace Gfx {

		//! \brief ARgb8888 color space
		struct ARgb8888 {};


		//! \brief ARgb8888 color class
		//!
		//! Valid range of the color components for this color model:\n
		//!    Red   : 0 to 255\n
		//!    Green : 0 to 255\n
		//!    Blue  : 0 to 255
		template <>
		class PT_EXPORT PT_PACKED BasicColor<ARgb8888> {
			public:
				typedef ARgb8888 ColorSpaceT;

			public:
				//! Default ctor, will generate default color (black)
				inline BasicColor()
				: _val(0)
				{}

				//! Copy ctor
				inline BasicColor(const BasicColor<ARgb8888>& c)
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
				inline BasicColor(uint32_t val)
				: _val(val & 0xFFFFFFFF)
				{}

				//! Construct color using the given components
				inline BasicColor(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
				: _val(0)
				{
					setAlpha(a);
					setRed(r);
					setGreen(g);
					setBlue(b);
				}

				//! Construct color using the given components
				inline BasicColor(uint8_t r, uint8_t g, uint8_t b)
				: _val(0)
				{
					setAlpha(0xFF);
					setRed(r);
					setGreen(g);
					setBlue(b);
				}

				//! Convert this color to ARGB components of the master color model
				//! (destinations range from 0 to 0xFFFF)
				inline void toARgb(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b) const
				{
					//                     33333333222222221111111100000000
					//                     76543210765432107654321076543210
					//                     AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB
					//                                     CCCCCCCCCCCCCCCC
					a = (_val & 0xFF000000) >> 16;
					r = (_val & 0x00FF0000) >> 8;
					g = (_val & 0x0000FF00)     ;
					b = (_val & 0x000000FF) << 8;
				}

				//! Set this color from the given ARGB components of the master color model
				//! (sources range from 0 to 0xFFFF)
				inline void fromARgb(uint16_t a, uint16_t r, uint16_t g, uint16_t b)
				{
					//int32_t rr = static_cast<int32_t>(r) * a / 0xFFFF;
					//int32_t gg = static_cast<int32_t>(g) * a / 0xFFFF;
					//int32_t bb = static_cast<int32_t>(b) * a / 0xFFFF;
					//setRed  ( uint8_t(rr>>8) );
					//setGreen( uint8_t(gg>>8) );
					//setBlue ( uint8_t(bb>>8) );

					setAlpha( uint8_t(a>>8) );
					setRed  ( uint8_t(r>>8) );
					setGreen( uint8_t(g>>8) );
					setBlue ( uint8_t(b>>8) );
				}

				//! Assignment operator from the same color space
				inline BasicColor<ARgb8888>& operator=(const BasicColor<ARgb8888>& c)
				{ _val = c._val; return *this; }

				//! Assignment operator from different color space
				template <typename SrcColorSpaceT> inline
				BasicColor<ARgb8888>& operator=(const BasicColor<SrcColorSpaceT>& c)
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
				inline uint32_t color() const
				{ return _val; }

				//! Return the alpha component of this color
				inline uint8_t alpha() const
				{ return (_val & 0xFF000000) >> 24; }

				//! Return the red component of this color
				inline uint8_t red() const
				{ return (_val & 0x00FF0000) >> 16; }

				//! Return the green component of this color
				inline uint8_t green() const
				{ return (_val & 0x0000FF00) >>  8; }

				//! Return the blue component of this color
				inline uint8_t blue() const
				{ return (_val & 0x000000FF) >>  0; }

				//! Set the packed color value of this color
				void setColor(uint32_t c)
				{ _val = c & 0x00FFFFFF; }

				//! Set the alpha component of this color
				inline void setAlpha(uint8_t a)
				{ _val &= 0x00FFFFFF; _val |= (uint32_t(a) << 24); }

				//! Set the red component of this color
				inline void setRed(uint8_t r)
				{ _val &= 0xFF00FFFF; _val |= (uint32_t(r) << 16); }

				//! Set the green component of this color
				inline void setGreen(uint8_t g)
				{ _val &= 0xFFFF00FF; _val |= (uint32_t(g) <<  8); }

				//! Set the blue component of this color
				inline void setBlue(uint8_t b)
				{ _val &= 0xFFFFFF00; _val |= (uint32_t(b) <<  0); }

				//! Get brightness
				inline uint8_t brightness() const;

				//! Set brightness
				inline void setBrightness(uint8_t l);

			protected:
				uint32_t _val;
		};

		// For convenience
		typedef BasicColor<ARgb8888> ARgb8888Color;

	} // namespace Gfx

} // namespace Pt

#endif
