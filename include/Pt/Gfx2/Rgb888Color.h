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
#ifndef Pt_Gfx2_Rgb888Color_h
#define Pt_Gfx2_Rgb888Color_h

#include <Pt/Gfx2/ARgbFColor.h>


namespace Pt {

	namespace Gfx {

		struct Rgb888 {};


		/** @brief 32-Bit RGB color model.
		 *  @ingroup Gfx
		 *
		 *  This class is exist so that the raw memory buffer of an image implementation
		 *  which use this color model could be casted directly to hardware image buffer
		 *  with format XXXXXXXXRRRRRRRRGGGGGGGGBBBBBBBB.
		 *  \n\n
		 *  Valid range of the color components for this color model:
		 *  <TABLE>
		 *    <TR> <TD>Red  </TD> <TD>0</TD> <TD>to</TD> <TD>255 (0xFF)</TD> </TR>
		 *    <TR> <TD>Green</TD> <TD>0</TD> <TD>to</TD> <TD>255 (0xFF)</TD> </TR>
		 *    <TR> <TD>Blue </TD> <TD>0</TD> <TD>to</TD> <TD>255 (0xFF)</TD> </TR>
		 *  </TABLE>
		 */
		template <>
		class PT_PACKED Color<Rgb888> {
			public:
				/** @brief The default constructor, will generate the default color (black).
				 */
				inline Color()
				: _val(0x00000000)
				{}

				/** @brief Copy constructor.
				 */
				inline Color(const Color& c)
				: _val(c._val)
				{}

				/** @brief Construct color using the given packed color constant.
				 */
				inline Color(uint32_t val)
				: _val(val)
				{}

				/** @brief Construct color using the given components.
				 */
				inline Color(uint8_t r, uint8_t g, uint8_t b)
				: _val(uint32_t(r) << 16)
				{
					// 33333333222222221111111100000000
					// 76543210765432107654321076543210
					// 00000000RRRRRRRRGGGGGGGGBBBBBBBB
					_val |= (uint32_t(g) << 8);
					_val |=  uint32_t(b);
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

					_val  = (uint32_t(r) << 16);
					_val |= (uint32_t(g) <<  8);
					_val |=  uint32_t(b);

					return *this;
				}

				/** @brief Assignment-substraction operator (beware of underflow).
				 */
				inline Color& operator-=(const Color& c)
				{
					const uint16_t r = red()   - c.red();
					const uint16_t g = green() - c.green();
					const uint16_t b = blue()  - c.blue();

					_val  = (uint32_t(r) << 16);
					_val |= (uint32_t(g) <<  8);
					_val |=  uint32_t(b);

					return *this;
				}


				/** @brief Return the packed color value of this color.
				 */
				inline uint32_t value() const
				{ return _val; }

				/** @brief Return the red component of this color (range 0 to 255).
				 */
				inline uint8_t red() const
				{ return (_val & 0x00FF0000) >> 16; }

				/** @brief Return the green component of this color (range 0 to 255).
				 */
				inline uint8_t green() const
				{ return (_val & 0x0000FF00) >> 8; }

				/** @brief Return the blue component of this color (range 0 to 255).
				 */
				inline uint8_t blue() const
				{ return _val & 0x000000FF; }


				/** @brief Set the packed color value of this color.
				 */
				void setValue(uint32_t c)
				{ _val = c; }

				/** @brief Set the red component of this color (range 0 to 255).
				 */
				inline void setRed(uint8_t r)
				{ _val = _val & 0xFF00FFFF | (uint32_t(r) << 16); }

				/** @brief Set the green component of this color (range 0 to 255).
				 */
				inline void setGreen(uint8_t g)
				{ _val = _val & 0xFFFF00FF | (uint32_t(g) << 8); }

				/** @brief Set the blue component of this color (range 0 to 255).
				 */
				inline void setBlue(uint8_t b)
				{ _val = _val & 0xFFFFFF00 | uint32_t(b); }

			public:
				//friend bool operator==(const Color& c1, const Color& c2);
				//friend bool operator<(const Color& c1, const Color& c2);
				//friend bool operator>(const Color& c1, const Color& c2);

			protected:
				uint32_t _val;
		};


		/** @brief Convenience access to the 32-Bit ARGB color model.
		 *  @ingroup Gfx
		 */
		typedef Color<Rgb888> Rgb888Color;


		/** @brief Convert a Color<Rgb888> to a Color<ARgb>.
		 */
		inline const Color<ARgb> toARgb(const Color<Rgb888>& from)
		{
			const uint16_t tr = from.red();
			const uint16_t tg = from.green();
			const uint16_t tb = from.blue();

			return Color<ARgb>( 0xFFFF,
			                    ((tr + !!tr) << 8) - !!tr, // Thanks to Mike Sharov for this algorithm
			                    ((tg + !!tg) << 8) - !!tg,
			                    ((tb + !!tb) << 8) - !!tb );
		}

		/** @brief Convert a Color<ARgb> to a Color<Rgb888>.
		 */
		inline void fromARgb(Color<Rgb888>& to, const Color<ARgb>& from)
		{
			// 33333333222222221111111100000000
			// 76543210765432107654321076543210
			// 00000000RRRRRRRRGGGGGGGGBBBBBBBB
			//                 CCCCCCCCCCCCCCCC
			const uint32_t val = ( uint32_t(from.red  () & 0xFF00) <<  8 ) |
			                       uint32_t(from.green() & 0xFF00)         |
			                     ( uint32_t(from.blue ()         ) >>  8 );
			to.setValue(val);
		}


		/** @brief Assign an Color<Rgb888> to an ARgbFColor.
		 */
		inline void assign(ARgbFColor& to, const Color<Rgb888>& from)
		{
			to.setAlpha( 1.0f                         );
			to.setRed  ( float(from.red  ()) / 255.0f );
			to.setGreen( float(from.green()) / 255.0f );
			to.setBlue ( float(from.blue ()) / 255.0f );
		}


		/** @brief Equality operator for Color<Rgb888> comparison.
		 */
		inline bool operator==(const Color<Rgb888>& c1, const Color<Rgb888>& c2)
		{ return c1.value()==c2.value(); }

		/** @brief Less-than operator for Color<Rgb888> comparison.
		 */
		inline bool operator<(const Color<Rgb888>& c1, const Color<Rgb888>& c2)
		{ return c1.value()<c2.value(); }

		/** @brief Greater-than operator for Color<Rgb888> comparison.
		 */
		inline bool operator>(const Color<Rgb888>& c1, const Color<Rgb888>& c2)
		{ return c1.value()>c2.value(); }


		/** @brief Make the greyscale version of the source Color<Rgb888> color.
		 */
		inline Color<Rgb888>& greyscale(Color<Rgb888>& to, const Color<Rgb888>& from)
		{
			const uint32_t r = from.red();
			const uint32_t g = from.green();
			const uint32_t b = from.blue();

			const uint32_t s = (r*77 + g*128 + b*51) >> 8;

			// 33333333222222221111111100000000
			// 76543210765432107654321076543210
			// 00000000RRRRRRRRGGGGGGGGBBBBBBBB
			//                         SSSSSSSS
			to.setValue( (s<<24) | (s<<16) | s );

			return to;
		}

	} // namespace Gfx

} // namespace Pt

#endif

