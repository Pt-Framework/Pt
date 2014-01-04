/*
 * Copyright (C) 2006-2007 by Aloysius Indrayanto
 * Copyright (C) 2006-2007 by Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef Pt_Gfx_ARgbColor_h
#define Pt_Gfx_ARgbColor_h

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Color.h>
#include <Pt/String.h>
#include <Pt/SourceInfo.h>
#include <Pt/SerializationInfo.h>
#include <limits>
#include <vector>
#include <string.h>


namespace Pt {

    namespace Gfx {

        /** @brief An empty structure used for tagging 64-bit ARGB color class.
         */
        struct ARgb {};

#include <Pt/Pack.h>
        /** @brief 64-Bit ARGB color class.
         *
         *  This is the master color model for Pt::Gfx, since it is used by
         *  the Painter interface. Each channel has 16 bit.
         */
        template <>
        class PT_GFX_API Color<ARgb> {
            public:
                /** @brief The default constructor, will generate the default color (black).
                 */
                inline Color()
                : _a(0xFFFF), _r(0), _g(0), _b(0)
                {}

                /** @brief Copy constructor.
                 */
                inline Color(const Color& c)
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

                /** @brief Construct color using the given components (in vector of pointers form).
                 */
                inline Color(const uint16_t* const chanPtr[4])
                : _a(*chanPtr[0]), _r(*chanPtr[1]), _g(*chanPtr[2]), _b(*chanPtr[3])
                {}

                /** @brief Construct color using the given components (in vector of pointers form plus offset).
                 */
                inline Color(const uint16_t* const chanPtr[4], size_t offset)
                : _a(*(chanPtr[0]+offset)), _r(*(chanPtr[1]+offset)), _g(*(chanPtr[2]+offset)), _b(*(chanPtr[3]+offset))
                {}


                /** @brief Assignment operator.
                 */
                inline Color& operator=(const Color& c)
                { _a = c._a; _r = c._r; _g = c._g; _b = c._b; return *this; }

                /** @brief Assignment operator.
                 *
                 *  This assigns color with different type to this one by calling
                 *  assign(), which can be overloaded to allow new color types to
                 *  be assigned to this one.
                 */
                template <typename ColorT>
                inline Color& operator=(const ColorT& color)
                { assign(*this, color); return *this; }


                /** @brief Assignment-addition operator (beware of overflow).
                 */
                inline Color& operator+=(const Color& c)
                { _a += c._a; _r += c._r; _g += c._g; _b += c._b; return *this; }

                /** @brief Assignment-substraction operator (beware of underflow).
                 */
                inline Color& operator-=(const Color& c)
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

                Pt::String toHtml() const;

                static Color fromHtml(const Pt::String& s);

            protected:
                uint16_t _a, _r, _g, _b;
        } PT_PACKED ;

#include <Pt/PackPop.h>

        /** @brief Convenience access to the 64-Bit ARGB color model.
         *  @ingroup Gfx
         */
        typedef Color<ARgb> ARgbColor;


        /** @brief Convert a Color<ARgb> to a Color<ARgb>.
         *
         *  This function is implemented just for the sake of completeness.
         */
        inline const Color<ARgb>& toARgb(Color<ARgb>& to, const Color<ARgb>& from)
        { to = from; return to; }

        /** @brief Convert a Color<ARgb> to a Color<ARgb>.
         *
         *  This function is implemented just for the sake of completeness.
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


        /** @brief Mix two Color<ARgb>s using the given mixing factor.
         */
        inline void blend(Color<ARgb>& dst, const Color<ARgb>& src, uint16_t factor)
        {
            typedef uint32_t ValueT;

            // A factor of 0 means that the src color is not used at all. Thus the
            // dst color keeps the same; just return.
            if (factor == 0)
            {
                return;
            }

            // The maximum factor means that only the src color is used. The dst color
            // is completely covered by the src color. Just copy the src color to the
            // dst color and return.
            if (factor == (std::numeric_limits<Pt::uint16_t>::max)())
            {
                dst.setAlpha(src.alpha());
                dst.setRed  (src.red());
                dst.setGreen(src.green());
                dst.setBlue (src.blue());
                return;
            }


            const ValueT oF = factor;

            const ValueT srcAlpha = ValueT(src.alpha());
            const ValueT dstAlpha = ValueT(dst.alpha());
            const ValueT srcRed   = ValueT(src.red());
            const ValueT dstRed   = ValueT(dst.red());
            const ValueT srcGreen = ValueT(src.green());
            const ValueT dstGreen = ValueT(dst.green());
            const ValueT srcBlue  = ValueT(src.blue());
            const ValueT dstBlue  = ValueT(dst.blue());

            const int shiftWidth = 8 * sizeof(factor);

            // The lines are basically the same as dst.X = factor * src.X + (1 - factor) * dst
            // where factor is of type float with a value between 0 and 1.
            dst.setAlpha((((srcAlpha - dstAlpha) * oF) >> shiftWidth) + dstAlpha);
            dst.setRed  ((((srcRed   - dstRed)   * oF) >> shiftWidth) + dstRed);
            dst.setGreen((((srcGreen - dstGreen) * oF) >> shiftWidth) + dstGreen);
            dst.setBlue ((((srcBlue  - dstBlue)  * oF) >> shiftWidth) + dstBlue);
        }

        /*
           Note: mixing equ for true ARgb alpha blending without mask:

           A: upper layer (the color to be put)
           B: lower layer (the color already in the image)
           D: the result  (mixing result)

           aD = 1 - (1-aA) * (1-aB)
           [rD, gD, bD] = [rA, gA, bA] * aA/aD        +
                          [rB, gB, bB] * aB*(1-aA)/aD
        */


    PT_GFX_API void operator >>=( const SerializationInfo& si, Gfx::Color<Pt::Gfx::ARgb>& x );

    PT_GFX_API void operator <<=( SerializationInfo& si, const Gfx::Color<Pt::Gfx::ARgb>& x );

} // namespace Gfx

} // namespace Pt

#endif

