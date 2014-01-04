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
#ifndef Pt_Gfx_ARgbFColor_h
#define Pt_Gfx_ARgbFColor_h

#include <Pt/Gfx/ARgbColor.h>


namespace Pt {

    namespace Gfx {

        /** @brief An empty structure used for tagging floated ARGB color class.
         */
        struct ARgbF {};


#include <Pt/Pack.h>
        /** @brief Floated ARGB color class.
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
        class PT_GFX_API Color<ARgbF> {
            public:
                /** @brief The default constructor, will generate the default color (black).
                 */
                inline Color()
                : _a(1.0f), _r(0.0f), _g(0.0f), _b(0.0f)
                {}

                /** @brief Copy constructor.
                 */
                inline Color(const Color& c)
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


                /** @brief Assignment-addition operator.
                 */
                inline Color& operator+=(const Color& c)
                { _a += c._a; _r += c._r; _g += c._g; _b += c._b; return *this; }

                /** @brief Assignment-substraction operator.
                 */
                inline Color& operator-=(const Color& c)
                { _a -= c._a; _r -= c._r; _g -= c._g; _b -= c._b; return *this; }


                /** @brief Return the alpha component of this color (range 0.0f to 1.0f).
                 */
                inline float alpha() const
                { return _a; }

                /** @brief Return the red component of this color (range 0.0f to 1.0f).
                 */
                inline float red() const
                { return _r; }

                /** @brief Return the green component of this color (range 0.0f to 1.0f).
                 */
                inline float green() const
                { return _g; }

                /** @brief Return the blue component of this color (range 0.0f to 1.0f).
                 */
                inline float blue() const
                { return _b; }


                /** @brief Set the alpha component of this color (range 0.0f to 1.0f).
                 */
                inline void setAlpha(float a)
                { _a = a; }

                /** @brief Set the red component of this color (range 0.0f to 1.0f).
                 */
                inline void setRed(float r)
                { _r = r; }

                /** @brief Set the green component of this color (range 0.0f to 1.0f).
                 */
                inline void setGreen(float g)
                { _g = g; }

                /** @brief Set the blue component of this color (range 0.0f to 1.0f).
                 */
                inline void setBlue(float b)
                { _b = b; }

            protected:
                float _a, _r, _g, _b;
        } PT_PACKED ;

#include <Pt/PackPop.h>


        /** @brief Convenience access to the Floated ARGB color model.
         *  @ingroup Gfx
         */
        typedef Color<ARgbF> ARgbFColor;


// This helper macro is implemented to make the compiler tries to generate the 'cmov'
// instruction if available. This macro will be undefined before leaving this file.
#define Pt_Gfx_ARgbFColor_h_convert(dVar, sVar, sChn) \
    const float&      sChn = sVar.sChn();             \
    register uint16_t dVar = 0;                       \
    if(sChn > 0.0f) dVar = uint16_t(sChn * 65535.0f); \
    if(sChn > 1.0f) dVar = 65535

        /** @brief Convert a Color<ARgbF> to a Color<ARgb>.
         */
        inline const Color<ARgb>& toARgb(Color<ARgb> &to, const Color<ARgbF>& from)
        {
            Pt_Gfx_ARgbFColor_h_convert(a, from, alpha);
            Pt_Gfx_ARgbFColor_h_convert(r, from, red  );
            Pt_Gfx_ARgbFColor_h_convert(g, from, green);
            Pt_Gfx_ARgbFColor_h_convert(b, from, blue );

            to.setAlpha(a);
            to.setRed  (r);
            to.setGreen(g);
            to.setBlue (b);
            return to;
        }

        /** @brief Convert a Color<ARgb> to a Color<ARgbF>.
         */
        inline void fromARgb(Color<ARgbF>& to, const Color<ARgb>& from)
        {
            to.setAlpha( float( from.alpha() ) / 65535.0f );
            to.setRed  ( float( from.red  () ) / 65535.0f );
            to.setGreen( float( from.green() ) / 65535.0f );
            to.setBlue ( float( from.blue () ) / 65535.0f );
        }


        /** @brief Assign a Color<ARgb> to a Color<ARgbF>.
         */
        inline void assign(Color<ARgbF>& to, const Color<ARgb>& from)
        { fromARgb(to, from); }

        /** @brief Assign a Color<RgbF> to a Color<ARgb>.
         */
        inline void assign(Color<ARgb>& to, const Color<ARgbF>& from)
        {
            Pt_Gfx_ARgbFColor_h_convert(a, from, alpha);
            Pt_Gfx_ARgbFColor_h_convert(r, from, red  );
            Pt_Gfx_ARgbFColor_h_convert(g, from, green);
            Pt_Gfx_ARgbFColor_h_convert(b, from, blue );

            to.setAlpha(a);
            to.setRed  (r);
            to.setGreen(g);
            to.setBlue (b);
        }

#undef Pt_Gfx_ARgbFColor_h_convert


        /** @brief Equality operator for Color<ARgbF> comparison.
         */
        inline bool operator==(const Color<ARgbF>& c1, const Color<ARgbF>& c2)
        { return c1.alpha()==c2.alpha() && c1.red()==c2.red() && c1.green()==c2.green() && c1.blue()==c2.blue(); }

        /** @brief Less-than operator for Color<ARgbF> comparison.
         */
        inline bool operator<(const Color<ARgbF>& c1, const Color<ARgbF>& c2)
        { return c1.alpha()<c2.alpha() || c1.red()<c2.red() || c1.green()<c2.green() || c1.blue()<c2.blue(); }

        /** @brief Greater-than operator for Color<ARgbF> comparison.
         */
        inline bool operator>(const Color<ARgbF>& c1, const Color<ARgbF>& c2)
        { return c1.alpha()>c2.alpha() || c1.red()>c2.red() || c1.green()>c2.green() || c1.blue()>c2.blue(); }


        /** @brief Make the greyscale version of the source Color<ARgbF> color.
         */
        inline Color<ARgbF>& greyscale(Color<ARgbF>& to, const Color<ARgbF>& from)
        {
            const float s = from.red()*0.3f + from.green()*0.5f + from.blue()*0.2f;

            to.setAlpha(from.alpha());
            to.setRed  (s);
            to.setGreen(s);
            to.setBlue (s);

            return to;
        }


        /** @brief Mix two Color<ARgbF>s using the given mixing factor.
         */
        inline void mixColor(Color<ARgbF>& dst, const Color<ARgbF>& src, float factor)
        {
            // TODO: Write it !!!
        }

    } // namespace Gfx

} // namespace Pt

#endif

