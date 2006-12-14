/***************************************************************************
 *   Copyright (C) 2006 by Aloysius Indrayanto                             *
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
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
#ifndef Pt_Gfx2_ARgb8888Color_h
#define Pt_Gfx2_ARgb8888Color_h

#include <Pt/Gfx2/FloatedColor.h>


namespace Pt {

	namespace Gfx {

		/** @brief 32-Bit ARGB color model.
		 *  @ingroup Gfx
		 *
		 *  This class is exist so that the raw memory buffer of an image implementation
		 *  which use this color model could be casted directly to hardware image buffer
		 *  with format AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB.
		 *  \n\n
		 *  Valid range of the color components for this color model:
		 *  <TABLE>
		 *    <TR> <TD>Alpha</TD> <TD>0</TD> <TD>to</TD> <TD>255 (0xFF)</TD> </TR>
		 *    <TR> <TD>Red  </TD> <TD>0</TD> <TD>to</TD> <TD>255 (0xFF)</TD> </TR>
		 *    <TR> <TD>Green</TD> <TD>0</TD> <TD>to</TD> <TD>255 (0xFF)</TD> </TR>
		 *    <TR> <TD>Blue </TD> <TD>0</TD> <TD>to</TD> <TD>255 (0xFF)</TD> </TR>
		 *  </TABLE>
		 */
		class PT_EXPORT PT_PACKED ARgb8888Color {
			public:
				/** @brief The default constructor, will generate the default color (black).
				 */
				inline ARgb8888Color()
				: _val(0xFF000000)
				{}

				/** @brief Copy constructor.
				 */
				inline ARgb8888Color(const ARgb8888Color& c)
				: _val(c._val)
				{}

				/** @brief Construct color using the given packed color constant.
				 */
				inline ARgb8888Color(uint32_t val)
				: _val(val)
				{}

				/** @brief Construct color using the given components.
				 */
				inline ARgb8888Color(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
				: _val(uint32_t(a) << 24)
				{
					// 33333333222222221111111100000000
					// 76543210765432107654321076543210
					// AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB
					_val |= (uint32_t(r) << 16);
					_val |= (uint32_t(g) <<  8);
					_val |=  uint32_t(b);
				}


				/** @brief Construct color using the given components.
				 */
				inline ARgb8888Color(uint8_t r, uint8_t g, uint8_t b)
				: _val(0xFF000000)
				{
					_val |= (uint32_t(r) << 16);
					_val |= (uint32_t(g) <<  8);
					_val |=  uint32_t(b);
				}


				/** @brief Assignment operator.
				 */
				inline const ARgb8888Color& operator=(const ARgb8888Color& c)
				{ _val = c._val; return *this; }

				/** @brief Assignment-addition operator (beware of overflow).
				 */
				inline const ARgb8888Color& operator+=(const ARgb8888Color& c);

				/** @brief Assignment-substraction operator (beware of underflow).
				 */
				inline const ARgb8888Color& operator-=(const ARgb8888Color& c);


				/** @brief Return the alpha component of this color.
				 */
				inline uint8_t alpha() const
				{ return _val >> 24; }

				/** @brief Return the red component of this color.
				 */
				inline uint8_t red() const
				{ return (_val & 0x00FF0000) >> 16; }

				/** @brief Return the green component of this color.
				 */
				inline uint8_t green() const
				{ return (_val & 0x0000FF00) >> 8; }

				/** @brief Return the blue component of this color.
				 */
				inline uint8_t blue() const
				{ return _val & 0x000000FF; }


				/** @brief Set the packed color value of this color.
				 */
				void setColor(uint32_t c)
				{ _val = c; }

				/** @brief Set the alpha component of this color.
				 */
				inline void setAlpha(uint8_t a)
				{ _val = _val & 0x00FFFFFF | (uint32_t(a) << 24); }

				/** @brief Set the red component of this color.
				 */
				inline void setRed(uint8_t r)
				{ _val = _val & 0xFF00FFFF | (uint32_t(r) << 16); }

				/** @brief Set the green component of this color.
				 */
				inline void setGreen(uint8_t g)
				{ _val = _val & 0xFFFF00FF | (uint32_t(g) << 8); }

				/** @brief Set the blue component of this color.
				 */
				inline void setBlue(uint8_t b)
				{ _val = _val & 0xFFFFFF00 | uint32_t(b); }

			public:
				friend void toARgb<ARgb8888Color>(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b, const ARgb8888Color& from);
				friend void toARgb_fast<ARgb8888Color>(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b, const ARgb8888Color& from);
				friend void fromARgb<ARgb8888Color>(ARgb8888Color& to, const uint16_t a, const uint16_t r, const uint16_t g, const uint16_t b);
				friend bool operator==<ARgb8888Color>(const ARgb8888Color& c1, const ARgb8888Color& c2);
				friend bool operator< <ARgb8888Color>(const ARgb8888Color& c1, const ARgb8888Color& c2);
				friend bool operator> <ARgb8888Color>(const ARgb8888Color& c1, const ARgb8888Color& c2);

			protected:
				uint32_t _val;
		};


		/** @brief Convert an ARgb8888Color to ARgbColor's components.
		 *
		 *  Valid range of the individual color components (a, r, g, and b) are
		 *  from 0 to 65535 (0xFFFF).
		 */
		template <> inline
		void toARgb<ARgb8888Color>(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b, const ARgb8888Color& from)
		{
			// 33333333222222221111111100000000
			// 76543210765432107654321076543210
			// AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB
			a = ((from._val & 0xFF000000) >> 24) * 257;
			r = ((from._val & 0x00FF0000) >> 16) * 257;
			g = ((from._val & 0x0000FF00) >> 8 ) * 257;
			b = ( from._val & 0x000000FF       ) * 257;
		}

		/** @brief Convert an ARgb8888Color to ARgbColor's components (faster, but less precision version).
		 *
		 *  Valid range of the individual color components (a, r, g, and b) are
		 *  from 0 to 65535 (0xFFFF).
		 */
		template <> inline
		void toARgb_fast<ARgb8888Color>(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b, const ARgb8888Color& from)
		{
			a = (from._val & 0xFF000000) >> 16;
			r = (from._val & 0x00FF0000) >>  8;
			g = (from._val & 0x0000FF00)      ;
			b = (from._val & 0x000000FF) <<  8;
		}

		/** @brief Convert ARgbColor's components to an ARgbColor.
		 *
		 *  Valid range of the individual color components (a, r, g, and b) are
		 *  from 0 to 65535 (0xFFFF).
		 */
		template <> inline
		void fromARgb<ARgb8888Color>(ARgb8888Color& to, const uint16_t a, const uint16_t r, const uint16_t g, const uint16_t b)
		{
			// 33333333222222221111111100000000
			// 76543210765432107654321076543210
			// AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB
			//                 CCCCCCCCCCCCCCCC
			to._val  = ( uint32_t(a & 0xFF00) << 16 );
			to._val |= ( uint32_t(r & 0xFF00) <<  8 );
			to._val |=   uint32_t(g & 0xFF00)        ;
			to._val |=   uint32_t(b)          >>  8  ;
		}


		/** @brief Assign an ARgb8888Color to a FloatedARgbColor model.
		 */
		template <> inline
		void assign<FloatedARgbColor, ARgb8888Color>(FloatedARgbColor& to, const ARgb8888Color& from)
		{
			to.setAlpha( float(from.alpha()) / 255.0f );
			to.setRed  ( float(from.red  ()) / 255.0f );
			to.setGreen( float(from.green()) / 255.0f );
			to.setBlue ( float(from.blue ()) / 255.0f );
		}

		/** @brief Faster (but less precision) version of assign().
		 *
		 *  In this full specialization for FloatedARgbColor <- ARgb8888Color, this
		 *  function will just actually call assign<FloatedARgbColor, ARgb8888Color>().
		 */
		template <> inline
		void assign_fast<FloatedARgbColor, ARgb8888Color>(FloatedARgbColor& to, const ARgb8888Color& from)
		{ assign<FloatedARgbColor, ARgb8888Color>(to, from); }


		/** @brief Equality operator for ARgb8888Color comparison.
		 */
		template <> inline
		bool operator==<ARgb8888Color>(const ARgb8888Color& c1, const ARgb8888Color& c2)
		{ return c1._val==c2._val; }

		/** @brief Less-than operator for ARgb8888Color comparison.
		 */
		template <> inline
		bool operator< <ARgb8888Color>(const ARgb8888Color& c1, const ARgb8888Color& c2)
		{ return c1._val<c2._val; }

		/** @brief Greater-than operator for ARgb8888Color comparison.
		 */
		template <> inline
		bool operator> <ARgb8888Color>(const ARgb8888Color& c1, const ARgb8888Color& c2)
		{ return c1._val>c2._val; }


		/** @brief Assignment-addition operator (beware of overflow).
		 */
		inline const ARgb8888Color& ARgb8888Color::operator+=(const ARgb8888Color& c)
		{
			uint16_t a1, r1, g1, b1; toARgb_fast(a1, r1, g1, b1, *this);
			uint16_t a2, r2, g2, b2; toARgb_fast(a2, r2, g2, b2, c);

			a1 += a2; r1 += r2; g1 += g2; b1 += b2;
			fromARgb_fast(*this, a1, r1, g1, b1);

			return *this;
		}

		/** @brief Assignment-substraction operator (beware of underflow).
		 */
		inline const ARgb8888Color& ARgb8888Color::operator-=(const ARgb8888Color& c)
		{
			uint16_t a1, r1, g1, b1; toARgb_fast(a1, r1, g1, b1, *this);
			uint16_t a2, r2, g2, b2; toARgb_fast(a2, r2, g2, b2, c);

			a1 -= a2; r1 -= r2; g1 -= g2; b1 -= b2;
			fromARgb_fast(*this, a1, r1, g1, b1);

			return *this;
		}
	} // namespace Gfx

} // namespace Pt

#endif

