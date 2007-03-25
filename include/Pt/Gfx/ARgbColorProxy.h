/***************************************************************************
 *   Copyright (C) 2006-2007 by Aloysius Indrayanto                        *
 *   Copyright (C) 2006-2007 by Marc Boris Dürner                          *
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
#ifndef Pt_Gfx_ARgbColorProxy_h
#define Pt_Gfx_ARgbColorProxy_h

#include <Pt/Gfx/ARgbColor.h>
#include <Pt/Math/Rect.h>


namespace Pt {

    namespace Gfx {

        /** @brief An empty structure used for tagging 64-bit ARGB color proxy class.
         */
        struct ARgbProxy : public ARgb {};


        /** @brief Reference-type 64-Bit ARGB color class.
         *  @ingroup Gfx
         *
         *  @see ARgbColor.
         */
        template <>
        class PT_GFX_API Color<ARgbProxy> {

            public:
                /** @brief Number of channels of this color. */
                static const size_t NumberOfChannels = 4;

                /** @brief Value type of each individual component of this color */
                typedef uint16_t ComponentT;

                /** @brief Non-reference type (value type) of this color. */
                typedef Color<ARgb> ValueT;

            public:
                /** @brief Copy constructor.
                 */
                inline Color(const Color& c)
                : _a(c._a), _r(c._r), _g(c._g), _b(c._b)
                {}

                /** @brief This constructor will take reference to the real storage.
                 */
                inline Color(uint16_t &a, uint16_t &r, uint16_t &g, uint16_t &b)
                : _a(a), _r(r), _g(g), _b(b)
                {}

                /** @brief This constructor will take reference to the real storage.
                 */
                inline Color(ComponentT* const chanPtr[4])
                : _a(*chanPtr[0]), _r(*chanPtr[1]), _g(*chanPtr[2]), _b(*chanPtr[3])
                {}

                /** @brief This constructor will take reference to the real storage (plus offset).
                 */
                inline Color(ComponentT* const chanPtr[4], size_t offset)
                : _a(*(chanPtr[0]+offset)), _r(*(chanPtr[1]+offset)), _g(*(chanPtr[2]+offset)), _b(*(chanPtr[3]+offset))
                {}


                /** @see ARgbColor. */
                inline Color& operator=(const Color& c)
                { _a = c._a; _r = c._r; _g = c._g; _b = c._b; return *this; }

                /** @see ARgbColor. */
                inline Color& operator=(const Color<ARgb>& c)
                { _a = c.alpha(); _r = c.red(); _g = c.green(); _b = c.blue(); return *this; }

                /** @see ARgbColor. */
                template <typename ColorT>
                inline Color& operator=(const ColorT& color)
                { assign(*this, color); return *this; }

                /** @see ARgbColor. */
                inline Color& operator+=(const Color& c)
                { _a += c._a; _r += c._r; _g += c._g; _b += c._b; return *this; }

                /** @see ARgbColor. */
                inline Color& operator-=(const Color& c)
                { _a -= c._a; _r -= c._r; _g -= c._g; _b -= c._b; return *this; }

                /** @see ARgbColor. */
                inline uint16_t alpha() const
                { return _a; }

                /** @see ARgbColor. */
                inline uint16_t red() const
                { return _r; }

                /** @see ARgbColor. */
                inline uint16_t green() const
                { return _g; }

                /** @see ARgbColor. */
                inline uint16_t blue() const
                { return _b; }


                /** @see ARgbColor. */
                inline void setAlpha(uint16_t a)
                { _a = a; }

                /** @see ARgbColor. */
                inline void setRed(uint16_t r)
                { _r = r; }

                /** @see ARgbColor. */
                inline void setGreen(uint16_t g)
                { _g = g; }

                /** @see ARgbColor. */
                inline void setBlue(uint16_t b)
                { _b = b; }

            protected:
                uint16_t &_a, &_r, &_g, &_b;
        };


        /** @brief Convenience access to the proxy-type 64-Bit ARGB color model.
         *  @ingroup Gfx
         */
        typedef Color<ARgbProxy> ARgbColorProxy;


        /** @brief Color model for planar YUV images
        */
        struct PlanarARgb
        {
            static const size_t NumberOfChannels = 4;

            /** @brief Value type of each individual component of this color */
            typedef uint16_t Component;

            /** @brief Non-reference type (value type) of this color. */
            typedef ARgbColor Color;

            typedef const ARgbColor ConstColor;

            typedef ARgbColorProxy ColorRef;

            typedef ARgbColor ConstColorRef;

            typedef Component* ColorData [ NumberOfChannels ];

            typedef const Component* ConstColorData [ NumberOfChannels ];
        };


        /** @brief Convert a Color<ARgbProxy> to a Color<ARgb>.
         */
        inline const Color<ARgb>& toARgb(Color<ARgb>& to, const Color<ARgbProxy>& from)
        {
            to.setAlpha(from.alpha());
            to.setRed  (from.red  ());
            to.setGreen(from.green());
            to.setBlue (from.blue ());
            return to;
        }

        /** @brief Convert a Color<ARgb> to a Color<ARgbProxy>.
         */
        inline void fromARgb(Color<ARgbProxy>& to, const Color<ARgb>& from)
        { to = from; }

        /** @brief Assign a Color<ARgbProxy> to a Color<ARgb>.
         */
        inline void assign(Color<ARgb>& to, const Color<ARgbProxy>& from)
        { toARgb(to, from); }

        /** @brief Assign a Color<ARgb> to a Color<ARgbProxy>.
         */
        inline void assign(Color<ARgbProxy>& to, const Color<ARgb>& from)
        { to = from; }

    } // namespace Gfx

} // namespace Pt

#endif

