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

#include <Pt/Gfx2/ARgb8888Color.h>


namespace Pt {

	namespace Gfx {

		/** @brief 32-Bit Packed ARGB Color.
		 *  @ingroup Gfx
		 *
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
				/** @brief The default constructor, will generate default color (black).
				 */
				inline ARgb8888Color()
				: _val(0)
				{}

				/** @brief Copy constructor.
				 */
				inline ARgb8888Color(const ARgb8888Color& c)
				: _val(c._val)
				{}

				/** @brief Construct color using the given packed color constant.
				 */
				inline ARgb8888Color(uint32_t val)
				: _val(val & 0xFFFFFFFF)
				{}

				/** @brief Construct color using the given components.
				 */
				inline ARgb8888Color(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
				: _val(0)
				{
					setAlpha(a);
					setRed(r);
					setGreen(g);
					setBlue(b);
				}

				/** @brief Construct color using the given components.
				 */
				inline ARgb8888Color(uint8_t r, uint8_t g, uint8_t b)
				: _val(0)
				{
					setAlpha(0xFF);
					setRed(r);
					setGreen(g);
					setBlue(b);
				}


				/** @brief Assignment operator.
				 */
				inline ARgb8888Color& operator=(const ARgb8888Color& c)
				{ _val = c._val; return *this; }


				/** @brief Return the alpha component of this color.
				 */
				inline uint8_t alpha() const
				{ return (_val & 0xFF000000) >> 24; }

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

			protected:
				uint32_t _val;
		};


		/** @brief Convert an ARgb8888Color to an ARgbColor.
		 */
		inline void toARgb(ARgbColor& dst, const ARgb8888Color& src)
		{
		}

		/** @brief Convert an ARgbColor to an ARgb8888Color.
		 */
		inline void fromARgb(ARgb8888Color& dst, const ARgbColor& src)
		{
		}


		/** @brief Convert an ARgb8888Color to an ARgbColor.
		 */
		inline void toARgb_fast(ARgbColor& dst, const ARgb8888Color& src)
		{
		}

		/** @brief Convert an ARgbColor to an ARgb8888Color.
		 */
		inline void fromARgb_fast(ARgb8888Color& dst, const ARgbColor& src)
		{
		}

	} // namespace Gfx

} // namespace Pt

#endif

