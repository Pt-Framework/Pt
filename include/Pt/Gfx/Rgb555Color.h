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
#ifndef Pt_Gfx_Rgb555Color_h
#define Pt_Gfx_Rgb555Color_h

#include <Pt/Gfx/ARgbFColor.h>


namespace Pt {

    namespace Gfx {

        /** @brief An empty structure used for tagging packed 15-bit RGB color class.
         */
        struct Rgb555 {};

#include <Pt/Pack.h>
        /** @brief Packed 15-bit RGB color class.
         *
         *  This class is exist so that the raw memory buffer of an image implementation
         *  which use this color model could be casted directly to hardware image buffer
         *  with format XRRRRRGGGGGBBBBB
         *  \n\n
         *  Valid range of the color components for this color model:
         *  <TABLE>
         *    <TR> <TD>Red  </TD> <TD>0</TD> <TD>to</TD> <TD>31 (0x1F)</TD> </TR>
         *    <TR> <TD>Green</TD> <TD>0</TD> <TD>to</TD> <TD>31 (0x1F)</TD> </TR>
         *    <TR> <TD>Blue </TD> <TD>0</TD> <TD>to</TD> <TD>31 (0x1F)</TD> </TR>
         *  </TABLE>
         */
        template <>
        class PT_GFX_API Color<Rgb555> {
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
                : _val(uint16_t(r & 0xF8) << 7)
                {
                    // 1111111100000000
                    // 7654321076543210
                    // 0RRRRRGGGGGBBBBB
                    //         CCCCCCCC
                    _val |= (uint16_t(g & 0xF8) << 2);
                    _val |=  uint16_t(b       ) >> 3;
                }


                /** @brief Assignment operator.
                 */
                inline Color& operator=(const Color& c)
                { _val = c._val; return *this; }

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
                {
                    const uint16_t r = red()   + c.red();
                    const uint16_t g = green() + c.green();
                    const uint16_t b = blue()  + c.blue();

                    // 1111111100000000
                    // 7654321076543210
                    // 0RRRRRGGGGGBBBBB
                    //            CCCCC
                    _val  = (uint16_t(r) << 10);
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

                    _val  = (uint16_t(r) << 10);
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
                { return (_val & 0x7C00) >> 10; }

                /** @brief Return the green component of this color (range 0 to 31).
                 */
                inline uint8_t green() const
                { return (_val & 0x03E0) >> 5; }

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
                { _val = (_val & 0x83FF) | (uint16_t(r & 0xF8) << 7); }

                /** @brief Set the green component of this color (range 0 to 31).
                 */
                inline void setGreen(uint8_t g)
                { _val = (_val & 0xFC1F) | (uint16_t(g & 0xF8) << 2); }

                /** @brief Set the blue component of this color (range 0 to 31).
                 */
                inline void setBlue(uint8_t b)
                { _val = (_val & 0xFFE0) | (uint16_t(b) >> 3); }

            protected:
                uint16_t _val;
        } PT_PACKED ;

#include <Pt/PackPop.h>

        /** @brief Convenience access to the 32-Bit ARGB color model.
         *  @ingroup Gfx
         */
        typedef Color<Rgb555> Rgb555Color;


        /** @brief Convert a Color<Rgb555> to a Color<ARgb>.
         */
        inline const Color<ARgb>& toARgb(Color<ARgb>& to, const Color<Rgb555>& from)
        {
            const uint16_t tr = from.red();
            const uint16_t tg = from.green();
            const uint16_t tb = from.blue();

            to.setAlpha( 0xFFFF                     );
            to.setRed  ( ((tr + !!tr) << 11) - !!tr ); // Thanks to Mike Sharov for this algorithm
            to.setGreen( ((tg + !!tg) << 11) - !!tg );
            to.setBlue ( ((tb + !!tb) << 11) - !!tb );
            return to;
        }

        /** @brief Convert a Color<ARgb> to a Color<Rgb555>.
         */
        inline void fromARgb(Color<Rgb555>& to, const Color<ARgb>& from)
        {
            // 1111111100000000
            // 7654321076543210
            // 0RRRRRGGGGGBBBBB
            // CCCCCCCCCCCCCCCC
            const uint32_t val = ( uint32_t(from.red  () & 0xF800) /*>>  1*/ ) |
                                 ( uint32_t(from.green() & 0xF800) >>  6 ) |
                                 ( uint32_t(from.blue ()         ) >> 11 );
            to.setValue(val);
        }


        /** @brief Assign a Color<ARgb> to a Color<Rgb555>.
         */
        inline void assign(Color<Rgb555>& to, const Color<ARgb>& from)
        { fromARgb(to, from); }

        /** @brief Assign a Color<Rgb555> to a Color<ARgb>.
         */
        inline void assign(Color<ARgb>& to, const Color<Rgb555>& from)
        {
            const uint16_t tr = from.red();
            const uint16_t tg = from.green();
            const uint16_t tb = from.blue();

            to.setAlpha(0xFFFF);
            to.setRed  ( ((tr + !!tr) << 11) - !!tr ); // Thanks to Mike Sharov for this algorithm
            to.setGreen( ((tg + !!tg) << 11) - !!tg );
            to.setBlue ( ((tb + !!tb) << 11) - !!tb );
        }

        /** @brief Assign a Color<Rgb555> to a Color<ARgbF>.
         */
        inline void assign(Color<ARgbF>& to, const Color<Rgb555>& from)
        {
            to.setAlpha( 1.0f                        );
            to.setRed  ( float(from.red  ()) / 31.0f );
            to.setGreen( float(from.green()) / 31.0f );
            to.setBlue ( float(from.blue ()) / 31.0f );
        }

        // No need to overload for:
        //   void assign(Color<Rgb555>& to, const Color<ARgbF>& from)
        // beause there is no more direct method to do this yet instead of
        // converting to the master color format first.


        /** @brief Equality operator for Color<Rgb555> comparison.
         */
        inline bool operator==(const Color<Rgb555>& c1, const Color<Rgb555>& c2)
        { return c1.value()==c2.value(); }

        /** @brief Less-than operator for Color<Rgb555> comparison.
         */
        inline bool operator<(const Color<Rgb555>& c1, const Color<Rgb555>& c2)
        { return c1.value()<c2.value(); }

        /** @brief Greater-than operator for Color<Rgb555> comparison.
         */
        inline bool operator>(const Color<Rgb555>& c1, const Color<Rgb555>& c2)
        { return c1.value()>c2.value(); }


        /** @brief Make the greyscale version of the source Color<Rgb555> color.
         */
        inline Color<Rgb555>& greyscale(Color<Rgb555>& to, const Color<Rgb555>& from)
        {
            const uint16_t r = from.red();
            const uint16_t g = from.green();
            const uint16_t b = from.blue();

            const uint16_t s = (r*77 + g*128 + b*51) >> 8;

            // 1111111100000000
            // 7654321076543210
            // 0RRRRRGGGGGBBBBB
            //            CCCCC
            to.setValue( (s<<10) | (s<<5) | s );

            return to;
        }


        /** @brief Mix two Color<Rgb555>s using the given mixing factor.
         */
        inline void mixColor(Color<Rgb555>& dst, const Color<Rgb555>& src, uint8_t factor)
        {
            // TODO: Write it !!!
        }

    } // namespace Gfx

} // namespace Pt

#endif

