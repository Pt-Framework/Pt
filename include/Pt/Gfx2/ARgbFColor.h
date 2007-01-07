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
#ifndef Pt_Gfx2_ARgbFColor_h
#define Pt_Gfx2_ARgbFColor_h

#include <Pt/Gfx2/ARgbColor.h>


namespace Pt {

	namespace Gfx {

		struct ARgbF {};


		/** @brief Floated ARGB color model.
		 *  @ingroup Gfx
		 *
		 *  This is the temporary color model for Pt::Gfx.
		 *  \n\n
		 *  Valid range of the color components for this color model:
		 *  <TABLE>
		 *    <TR> <TD>Alpha</TD> <TD>0.0f</TD> <TD>to</TD> <TD>1.0f</TD> </TR>
		 *    <TR> <TD>Red  </TD> <TD>0.0f</TD> <TD>to</TD> <TD>1.0f</TD> </TR>
		 *    <TR> <TD>Green</TD> <TD>0.0f</TD> <TD>to</TD> <TD>1.0f</TD> </TR>
		 *    <TR> <TD>Blue </TD> <TD>0.0f</TD> <TD>to</TD> <TD>1.0f</TD> </TR>
		 *  </TABLE>
		 *  However, values <0.0f and >1.0f are allowed exist for temporal calculation
		 *  results.
		 *  \n\n
		 *  Complex color algorithms such as cubic-scale, dithering, etc. are suggested
		 *  to use this color model to minimize rounding error propagation.
		 */
		template <>
		class PT_API PT_PACKED Color<ARgbF> {
			public:
				/** @brief The default constructor, will generate the default color (black).
				 */
				inline Color()
				: _a(1.0f), _r(0.0f), _g(0.0f), _b(0.0f)
				{}

				/** @brief Copy constructor.
				 */
				inline Color(const Color<ARgbF>& c)
				: _a(c._a), _r(c._r), _g(c._g), _b(c._b)
				{}

				/** @brief Construct color using the given components.
				 */
				inline Color(float a, float r, float g, float b)
				: _a(a), _r(r), _g(g), _b(b)
				{}

				/** @brief Construct color using the given components.
				 */
				inline Color(float r, float g, float b)
				: _a(1.0f), _r(r), _g(g), _b(b)
				{}


				/** @brief Assignment operator.
				 */
				inline const Color<ARgbF>& operator=(const Color<ARgbF>& c)
				{ _a = c._a; _r = c._r; _g = c._g; _b = c._b; return *this; }

				/** @brief Assignment-addition operator.
				 */
				inline const Color<ARgbF>& operator+=(const Color<ARgbF>& c)
				{ _a += c._a; _r += c._r; _g += c._g; _b += c._b; return *this; }

				/** @brief Assignment-substraction operator.
				 */
				inline const Color<ARgbF>& operator-=(const Color<ARgbF>& c)
				{ _a -= c._a; _r -= c._r; _g -= c._g; _b -= c._b; return *this; }


				/** @brief Return the alpha component of this color.
				 */
				inline float alpha() const
				{ return _a; }

				/** @brief Return the red component of this color.
				 */
				inline float red() const
				{ return _r; }

				/** @brief Return the green component of this color.
				 */
				inline float green() const
				{ return _g; }

				/** @brief Return the blue component of this color.
				 */
				inline float blue() const
				{ return _b; }


				/** @brief Set the alpha component of this color.
				 */
				inline void setAlpha(float a)
				{ _a = a; }

				/** @brief Set the red component of this color.
				 */
				inline void setRed(float r)
				{ _r = r; }

				/** @brief Set the green component of this color.
				 */
				inline void setGreen(float g)
				{ _g = g; }

				/** @brief Set the blue component of this color.
				 */
				inline void setBlue(float b)
				{ _b = b; }

			public:
				friend void toARgb(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b, const Color<ARgbF>& from);
				friend void fromARgb(Color<ARgbF>& to, const uint16_t a, const uint16_t r, const uint16_t g, const uint16_t b);
				friend bool operator==(const Color<ARgbF>& c1, const Color<ARgbF>& c2);
				friend bool operator<(const Color<ARgbF>& c1, const Color<ARgbF>& c2);
				friend bool operator>(const Color<ARgbF>& c1, const Color<ARgbF>& c2);

				friend const Color<ARgbF>& greyscale(Color<ARgbF>& to, const Color<ARgbF>& from);

			protected:
				float _a, _r, _g, _b;
		};


		/** @brief Convenience access to the Floated ARGB color model.
		 *  @ingroup Gfx
		 */
		typedef Color<ARgbF> ARgbFColor;


		/** @brief Convert a Color<ARgbF> to ARgbColor's components.
		 *
		 *  Valid range of the individual color components (a, r, g, and b) are
		 *  from 0 to 65535 (0xFFFF).
		 */
		inline void toARgb(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b, const Color<ARgbF>& from)
		{
			a = from._a<0.0f ? 0 : ( from._a>1.0f ? 0xFFFF : uint16_t(from._a*65535.0f) );
			r = from._r<0.0f ? 0 : ( from._r>1.0f ? 0xFFFF : uint16_t(from._r*65535.0f) );
			g = from._g<0.0f ? 0 : ( from._g>1.0f ? 0xFFFF : uint16_t(from._g*65535.0f) );
			b = from._b<0.0f ? 0 : ( from._b>1.0f ? 0xFFFF : uint16_t(from._b*65535.0f) );
		}

		/** @brief Convert ARgbColor's components to a Color<ARgbF>.
		 *
		 *  Valid range of the individual color components (a, r, g, and b) are
		 *  from 0 to 65535 (0xFFFF).
		 */
		inline void fromARgb(Color<ARgbF>& to, const uint16_t a, const uint16_t r, const uint16_t g, const uint16_t b)
		{
			to._a = float(a) / 65535.0;
			to._r = float(r) / 65535.0;
			to._g = float(g) / 65535.0;
			to._b = float(b) / 65535.0;
		}


		/** @brief Equality operator for Color<ARgbF> comparison.
		 */
		inline bool operator==(const Color<ARgbF>& c1, const Color<ARgbF>& c2)
		{ return c1._a==c2._a && c1._r==c2._r && c1._g==c2._g && c1._b==c2._b; }

		/** @brief Less-than operator for Color<ARgbF> comparison.
		 */
		inline bool operator<(const Color<ARgbF>& c1, const Color<ARgbF>& c2)
		{ return c1._a<c2._a || c1._r<c2._r || c1._g<c2._g || c1._b<c2._b; }

		/** @brief Greater-than operator for Color<ARgbF> comparison.
		 */
		inline bool operator>(const Color<ARgbF>& c1, const Color<ARgbF>& c2)
		{ return c1._a>c2._a || c1._r>c2._r || c1._g>c2._g || c1._b>c2._b; }


		/** @brief Make the greyscale version of the source Color<ARgbF> color.
		 */
		inline const Color<ARgbF>& greyscale(Color<ARgbF>& to, const Color<ARgbF>& from)
		{
			const float s = from._r*0.3f + from._g*0.5f + from._b*0.2f;

			to._a = from._a;
			to._r = s;
			to._g = s;
			to._b = s;

			return to;
		}

	} // namespace Gfx

} // namespace Pt

#endif

