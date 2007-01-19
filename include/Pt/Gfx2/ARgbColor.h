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
#ifndef Pt_Gfx2_ARgbColor_h
#define Pt_Gfx2_ARgbColor_h

#include <limits>
#include <Pt/IfElse.h>
#include <Pt/Gfx2/Color.h>


namespace Pt {

	namespace Gfx {

		struct ARgb {};


		/** @brief 64-Bit ARGB color model.
		 *  @ingroup Gfx
		 *
		 *  This is the master color model for Pt::Gfx.
		 *  \n\n
		 *  Valid range of the color components for this color model:
		 *  <TABLE>
		 *    <TR> <TD>Alpha</TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
		 *    <TR> <TD>Red  </TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
		 *    <TR> <TD>Green</TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
		 *    <TR> <TD>Blue </TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
		 *  </TABLE>
		 */
		template <>
		class PT_API PT_PACKED Color<ARgb> {
			public:
				/** @brief The default constructor, will generate the default color (black).
				 */
				inline Color()
				: _a(0xFFFF), _r(0), _g(0), _b(0)
				{}

				/** @brief Copy constructor.
				 */
				inline Color(const Color<ARgb>& c)
				: _a(c._a), _r(c._r), _g(c._g), _b(c._b)
				{}

				/** @brief Construct color using the given components.
				 */
				inline Color(uint16_t a, uint16_t r, uint16_t g, uint16_t b)
				: _a(a), _r(r), _g(g), _b(b)
				{}

				/** @brief Construct color using the given components.
				 */
				inline Color(uint16_t r, uint16_t g, uint16_t b)
				: _a(0xFFFF), _r(r), _g(g), _b(b)
				{}


				/** @brief Assignment operator.
				 */
				inline Color<ARgb>& operator=(const Color<ARgb>& c)
				{ _a = c._a; _r = c._r; _g = c._g; _b = c._b; return *this; }

				/** @brief Assignment-addition operator (beware of overflow).
				 */
				inline Color<ARgb>& operator+=(const Color<ARgb>& c)
				{ _a += c._a; _r += c._r; _g += c._g; _b += c._b; return *this; }

				/** @brief Assignment-substraction operator (beware of underflow).
				 */
				inline Color<ARgb>& operator-=(const Color<ARgb>& c)
				{ _a -= c._a; _r -= c._r; _g -= c._g; _b -= c._b; return *this; }


				/** @brief Return the alpha component of this color (range 0 to 65535).
				 */
				inline uint16_t alpha() const
				{ return _a; }

				/** @brief Return the red component of this color (range 0 to 65535).
				 */
				inline uint16_t red() const
				{ return _r; }

				/** @brief Return the green component of this color (range 0 to 65535).
				 */
				inline uint16_t green() const
				{ return _g; }

				/** @brief Return the blue component of this color (range 0 to 65535).
				 */
				inline uint16_t blue() const
				{ return _b; }


				/** @brief Set the alpha component of this color (range 0 to 65535).
				 */
				inline void setAlpha(uint16_t a)
				{ _a = a; }

				/** @brief Set the red component of this color (range 0 to 65535).
				 */
				inline void setRed(uint16_t r)
				{ _r = r; }

				/** @brief Set the green component of this color (range 0 to 65535).
				 */
				inline void setGreen(uint16_t g)
				{ _g = g; }

				/** @brief Set the blue component of this color (range 0 to 65535).
				 */
				inline void setBlue(uint16_t b)
				{ _b = b; }

			public:
				//friend bool operator==(const Color<ARgb>& c1, const Color<ARgb>& c2);
				//friend bool operator<(const Color<ARgb>& c1, const Color<ARgb>& c2);
				//friend bool operator>(const Color<ARgb>& c1, const Color<ARgb>& c2);

			protected:
				uint16_t _a, _r, _g, _b;
		};


		/** @brief Convenience access to the 64-Bit ARGB color model.
		 *  @ingroup Gfx
		 */
		typedef Color<ARgb> ARgbColor;


		/** @brief Dummy function for the sake of completeness.
		 */
		inline const Color<ARgb> toARgb(const Color<ARgb>& from)
		{ return from; }

		/** @brief Dummy function for the sake of completeness.
		 */
		inline void fromARgb(Color<ARgb>& to, const Color<ARgb>& from)
		{ to = from; }


		/** @brief Equality operator for Color<ARgb> comparison.
		 */
		inline bool operator==(const Color<ARgb>& c1, const Color<ARgb>& c2)
		{ return c1.alpha()==c2.alpha() && c1.red()==c2.red() && c1.green()==c2.green() && c1.blue()==c2.blue(); }

		/** @brief Less-than operator for Color<ARgb> comparison.
		 */
		inline bool operator<(const Color<ARgb>& c1, const Color<ARgb>& c2)
		{ return c1.alpha()<c2.alpha() || c1.red()<c2.red() || c1.green()<c2.green() || c1.blue()<c2.blue(); }

		/** @brief Greater-than operator for Color<ARgb> comparison.
		 */
		inline bool operator>(const Color<ARgb>& c1, const Color<ARgb>& c2)
		{ return c1.alpha()>c2.alpha() || c1.red()>c2.red() || c1.green()>c2.green() || c1.blue()>c2.blue(); }


		/** @brief Make the greyscale version of the source Color<ARgb>.
		 */
		inline Color<ARgb>& greyscale(Color<ARgb>& to, const Color<ARgb>& from)
		{
			const uint16_t s = (from.red()*77 + from.green()*128 + from.blue()*51) >> 8;

			to.setAlpha(from.alpha());
			to.setRed  (s);
			to.setGreen(s);
			to.setBlue (s);

			return to;
		}


		template <typename Type1, typename Type2>
		struct LargestType {
			typedef typename IfElse< (sizeof(Type1) >= sizeof(Type2)), Type1, Type2 >::ResultT ResultT;
		};

		template <typename> struct TmpType;
		template <> struct TmpType<uint8_t > { typedef uint16_t ValueT; };
		template <> struct TmpType<uint16_t> { typedef uint32_t ValueT; };
		template <> struct TmpType<uint32_t> { typedef uint64_t ValueT; };

		template <typename T1, typename T2>
		struct ResultType {
			typedef typename TmpType< typename LargestType<T1, T2>::ResultT >::ValueT ValueT;
		};


		/** @brief Choose the type which has greater size (from the two given types).
		 */
		template <typename A, typename B>
		struct LargestSizeOf {
			typedef typename IfElse< (sizeof(A) >= sizeof(B)), A, B >::ResultT Result;
		};


		// Pure conceptional traits class. Later it may contain much more than just
		// a typedef for temporary color values
		template <typename ColorT>
		struct ColorTraits;

		// Full specialisation for ARgbColor
		template <>
		struct ColorTraits<ARgbColor> {
			typedef uint32_t TmpValue;
		};


		/** @brief Mix two Color<ARgb>s using the given mixing factor
		 *
		 *  This could be the implementation detail for overloaded functions.
		 *  This would give us the ability to react to defect compilers that
		 *  are not very good with template specialisations. However we should
		 *  be pretty safe as long as we avoid partial specialisation.
		 */
		template <typename FactorT>
		inline void mixColor(Color<ARgb>& dst, const Color<ARgb>& src, const FactorT& factor)
		{
			assert(  std::numeric_limits<FactorT>::is_integer() );
			assert( !std::numeric_limits<FactorT>::is_signed () );

			typedef ColorTraits<ARgbColor> Traits;
			typedef typename LargestSizeOf< Traits::TmpValue, FactorT >::Result ValueT;

			const ValueT oF = factor;
			const ValueT rF = std::numeric_limits<FactorT>::max() - oF;

			const ValueT dA = ValueT( dst.alpha() ) * rF;
			const ValueT dR = ValueT( dst.red()   ) * rF;
			const ValueT dG = ValueT( dst.green() ) * rF;
			const ValueT dB = ValueT( dst.blue()  ) * rF;

			const ValueT sA = ValueT( src.alpha() ) * oF;
			const ValueT sR = ValueT( src.red()   ) * oF;
			const ValueT sG = ValueT( src.green() ) * oF;
			const ValueT sB = ValueT( src.blue()  ) * oF;

			dst.setAlpha( (dA + sA) >> (8*sizeof(factor)) );
			dst.setRed  ( (dR + sR) >> (8*sizeof(factor)) );
			dst.setGreen( (dG + sG) >> (8*sizeof(factor)) );
			dst.setBlue ( (dB + sB) >> (8*sizeof(factor)) );
		}

	} // namespace Gfx

} // namespace Pt

#endif

