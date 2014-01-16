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
#ifndef Pt_Gfx_Rgb888Color_h
#define Pt_Gfx_Rgb888Color_h

#include <Pt/Gfx/ARgbFColor.h>


namespace Pt {

    namespace Gfx {

        /** @brief An empty structure used for tagging packed 32-bit RGB color class.
         */
        struct Rgb888 {};

#include <Pt/Pack.h>
        /** @brief Packed 32-bit RGB color class.
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
        class PT_GFX_API Color<Rgb888> {
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
                { _val = (_val & 0xFF00FFFF) | (uint32_t(r) << 16); }

                /** @brief Set the green component of this color (range 0 to 255).
                 */
                inline void setGreen(uint8_t g)
                { _val = (_val & 0xFFFF00FF) | (uint32_t(g) << 8); }

                /** @brief Set the blue component of this color (range 0 to 255).
                 */
                inline void setBlue(uint8_t b)
                { _val = (_val & 0xFFFFFF00) | uint32_t(b); }

            protected:
                uint32_t _val;
        } PT_PACKED ;

#include <Pt/PackPop.h>

        /** @brief Convenience access to the 32-Bit ARGB color model.
         *  @ingroup Gfx
         */
        typedef Color<Rgb888> Rgb888Color;


        /** @brief Convert a Color<Rgb888> to a Color<ARgb>.
         */
        inline const Color<ARgb>& toARgb(Color<ARgb>& to, const Color<Rgb888>& from)
        {
            const uint16_t tr = from.red();
            const uint16_t tg = from.green();
            const uint16_t tb = from.blue();

            to.setAlpha( 0xFFFF                    );
            to.setRed  ( ((tr + !!tr) << 8) - !!tr ); // Thanks to Mike Sharov for this algorithm
            to.setGreen( ((tg + !!tg) << 8) - !!tg );
            to.setBlue ( ((tb + !!tb) << 8) - !!tb );
            return to;
        }

        /** @brief Convert a Color<ARgb> to a Color<Rgb888>.
         */
        inline void fromARgb(Color<Rgb888>& to, const Color<ARgb>& from)
        {
            // 33333333222222221111111100000000
            // 76543210765432107654321076543210
            // 00000000RRRRRRRRGGGGGGGGBBBBBBBB
            //                 CCCCCCCCCCCCCCCC
            const uint32_t val = ( uint32_t(from.red  () & 0xFF) <<  16 ) |
                                 ( uint32_t(from.green() & 0xFF) <<   8)  |
                                 ( uint32_t(from.blue () & 0xFF)  );
            to.setValue(val);
        }


        /** @brief Assign a Color<ARgb> to a Color<Rgb888>.
         */
        inline void assign(Color<Rgb888>& to, const Color<ARgb>& from)
        { fromARgb(to, from); }

        /** @brief Assign a Color<Rgb888> to a Color<ARgb>.
         */
        inline void assign(Color<ARgb>& to, const Color<Rgb888>& from)
        {
            const uint16_t tr = from.red();
            const uint16_t tg = from.green();
            const uint16_t tb = from.blue();

            to.setAlpha(0xFFFF);
            to.setRed  ( ((tr + !!tr) << 8) - !!tr ); // Thanks to Mike Sharov for this algorithm
            to.setGreen( ((tg + !!tg) << 8) - !!tg );
            to.setBlue ( ((tb + !!tb) << 8) - !!tb );
        }

        /** @brief Assign an Color<Rgb888> to an Color<ARgbF>.
         */
        inline void assign(Color<ARgbF>& to, const Color<Rgb888>& from)
        {
            to.setAlpha( 1.0f                         );
            to.setRed  ( float(from.red  ()) / 255.0f );
            to.setGreen( float(from.green()) / 255.0f );
            to.setBlue ( float(from.blue ()) / 255.0f );
        }

        // No need to overload for:
        //   void assign(Color<Rgb888>& to, const Color<ARgbF>& from)
        // beause there is no more direct method to do this yet instead of
        // converting to the master color format first.


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


        /** @brief Mix two Color<Rgb888>s using the given mixing factor.
         */
        inline void mixColor(Color<Rgb888>& dst, const Color<Rgb888>& src, uint8_t factor)
        {
            // TODO: Write it !!!
        }

    } // namespace Gfx

} // namespace Pt

#endif

