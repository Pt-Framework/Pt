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
#ifndef Pt_Gfx2_Rgb565Color_h
#define Pt_Gfx2_Rgb565Color_h

#include <Pt/Gfx2/ARgbFColor.h>


namespace Pt {

	namespace Gfx {

		struct Rgb565 {};


		/** @brief 16-Bit RGB color model.
		 *  @ingroup Gfx
		 *
		 *  This class is exist so that the raw memory buffer of an image implementation
		 *  which use this color model could be casted directly to hardware image buffer
		 *  with format RRRRRGGGGGGBBBBB
		 *  \n\n
		 *  Valid range of the color components for this color model:
		 *  <TABLE>
		 *    <TR> <TD>Red  </TD> <TD>0</TD> <TD>to</TD> <TD>31 (0x1F)</TD> </TR>
		 *    <TR> <TD>Green</TD> <TD>0</TD> <TD>to</TD> <TD>63 (0x3F)</TD> </TR>
		 *    <TR> <TD>Blue </TD> <TD>0</TD> <TD>to</TD> <TD>31 (0x1F)</TD> </TR>
		 *  </TABLE>
		 */
		template <>
		class PT_API PT_PACKED Color<Rgb565> {
			public:
				/** @brief The default constructor, will generate the default color (black).
				 */
				inline Color()
				: _val(0x0000)
				{}

				/** @brief Copy constructor.
				 */
				inline Color(const Color& c)
				: _val(c._val)
				{}

				/** @brief Construct color using the given packed color constant.
				 */
				inline Color(uint16_t val)
				: _val(val)
				{}

				/** @brief Construct color using the given components.
				 */
				inline Color(uint8_t r, uint8_t g, uint8_t b)
				: _val(uint16_t(r & 0xF8) << 8)
				{
					// 1111111100000000
					// 7654321076543210
					// RRRRRGGGGGGBBBBB
					//         CCCCCCCC
					_val |= (uint16_t(g & 0xFC) << 3);
					_val |=  uint16_t(b       ) >> 3;
				}


				/** @brief Assignment operator.
				 */
				inline Color& operator=(const Color& c)
				{ _val = c._val; return *this; }

				/** @brief Assignment-addition operator (beware of overflow).
				 */
				inline Color& operator+=(const Color& c)
				{
					const uint16_t r = red()   + c.red();
					const uint16_t g = green() + c.green();
					const uint16_t b = blue()  + c.blue();

					// 1111111100000000
					// 7654321076543210
					// RRRRRGGGGGGBBBBB
					//            CCCCC
					// and
					//           CCCCCC
					_val  = (uint16_t(r) << 11);
					_val |= (uint16_t(g) <<  5);
					_val |=  uint16_t(b);

					return *this;
				}

				/** @brief Assignment-substraction operator (beware of underflow).
				 */
				inline Color& operator-=(const Color& c)
				{
					const uint16_t r = red()   - c.red();
					const uint16_t g = green() - c.green();
					const uint16_t b = blue()  - c.blue();

					_val  = (uint16_t(r) << 11);
					_val |= (uint16_t(g) <<  5);
					_val |=  uint16_t(b);

					return *this;
				}


				/** @brief Return the packed color value of this color.
				 */
				inline uint16_t value() const
				{ return _val; }

				/** @brief Return the red component of this color (range 0 to 31).
				 */
				inline uint8_t red() const
				{ return (_val & 0xF800) >> 11; }

				/** @brief Return the green component of this color (range 0 to 63).
				 */
				inline uint8_t green() const
				{ return (_val & 0x07E0) >> 5; }

				/** @brief Return the blue component of this color (range 0 to 31).
				 */
				inline uint8_t blue() const
				{ return _val & 0x001F; }


				/** @brief Set the packed color value of this color.
				 */
				void setValue(uint16_t c)
				{ _val = c; }

				/** @brief Set the red component of this color (range 0 to 31).
				 */
				inline void setRed(uint8_t r)
				{ _val = _val & 0x07FF | (uint16_t(r & 0xF8) << 8); }

				/** @brief Set the green component of this color (range 0 to 63).
				 */
				inline void setGreen(uint8_t g)
				{ _val = _val & 0xF81F | (uint16_t(g & 0xFC) << 3); }

				/** @brief Set the blue component of this color (range 0 to 31).
				 */
				inline void setBlue(uint8_t b)
				{ _val = _val & 0xFFE0 | (uint16_t(b) >> 3); }

			public:
				friend bool operator==(const Color& c1, const Color& c2);
				friend bool operator<(const Color& c1, const Color& c2);
				friend bool operator>(const Color& c1, const Color& c2);

			protected:
				uint16_t _val;
		};


		/** @brief Convenience access to the 32-Bit ARGB color model.
		 *  @ingroup Gfx
		 */
		typedef Color<Rgb565> Rgb565Color;


		/** @brief Convert an Color<Rgb565> to ARgbColor's components.
		 *
		 *  Valid range of the individual color components (a, r, g, and b) are
		 *  from 0 to 65535 (0xFFFF).
		 */
		inline void toARgb(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b, const Color<Rgb565>& from)
		{
			const uint16_t tr = from.red();
			const uint16_t tg = from.green();
			const uint16_t tb = from.blue();

			a = 0xFFFF;
			r = ( (tr + !!tr) << 11 ) - !!tr; // Thanks to Mike Sharov for this algorithm
			g = ( (tg + !!tg) << 10 ) - !!tg;
			b = ( (tb + !!tb) << 11 ) - !!tb;
		}

		/** @brief Convert ARgbColor's components to an ARgbColor.
		 *
		 *  Valid range of the individual color components (a, r, g, and b) are
		 *  from 0 to 65535 (0xFFFF).
		 */
		inline void fromARgb(Color<Rgb565>& to, const uint16_t a, const uint16_t r, const uint16_t g, const uint16_t b)
		{
			// 1111111100000000
			// 7654321076543210
			// RRRRRGGGGGGBBBBB
			// CCCCCCCCCCCCCCCC
			const uint16_t val  =  uint16_t(r & 0xF800)        |
			                      (uint16_t(g & 0xFC00) >>  5) |
			                      (uint16_t(b)          >> 11);
			to.setValue(val);
		}


		/** @brief Assign an Color<Rgb565> to an ARgbFColor.
		 */
		inline void assign(ARgbFColor& to, const Color<Rgb565>& from)
		{
			to.setAlpha( 1.0f                        );
			to.setRed  ( float(from.red  ()) / 31.0f );
			to.setGreen( float(from.green()) / 63.0f );
			to.setBlue ( float(from.blue ()) / 31.0f );
		}


		/** @brief Equality operator for Color<Rgb565> comparison.
		 */
		inline bool operator==(const Color<Rgb565>& c1, const Color<Rgb565>& c2)
		{ return c1._val==c2._val; }

		/** @brief Less-than operator for Color<Rgb565> comparison.
		 */
		inline bool operator<(const Color<Rgb565>& c1, const Color<Rgb565>& c2)
		{ return c1._val<c2._val; }

		/** @brief Greater-than operator for Color<Rgb565> comparison.
		 */
		inline bool operator>(const Color<Rgb565>& c1, const Color<Rgb565>& c2)
		{ return c1._val>c2._val; }


		/** @brief Make the greyscale version of the source Color<Rgb565> color.
		 */
		inline Color<Rgb565>& greyscale(Color<Rgb565>& to, const Color<Rgb565>& from)
		{
			const uint16_t r = from.red();
			const uint16_t g = from.green();
			const uint16_t b = from.blue();

			const uint16_t h = (r*77 + g*128 + b*51) >> 7;
			const uint16_t s = h >> 1;

			// 1111111100000000
			// 7654321076543210
			// RRRRRGGGGGGBBBBB
			//            CCCCC
			// and
			//           CCCCCC
			to.setValue( (s<<11) | (h<<5) | s );

			return to;
		}

	} // namespace Gfx

} // namespace Pt

#endif

