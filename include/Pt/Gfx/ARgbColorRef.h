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
#ifndef Pt_Gfx_ARgbColorRef_h
#define Pt_Gfx_ARgbColorRef_h

#include <Pt/Gfx/ARgbColor.h>
#include <limits>
#include <vector>


namespace Pt {

    namespace Gfx {

        /** @brief An empty structure used for tagging reference-type 64-bit ARGB color class.
         */
        struct ARgbRef {};


        /** @brief Reference-type 64-Bit ARGB color class.
         *  @ingroup Gfx
         *
         *  Valid range of the color components for this color model:
         *  <TABLE>
         *    <TR> <TD>Alpha</TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
         *    <TR> <TD>Red  </TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
         *    <TR> <TD>Green</TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
         *    <TR> <TD>Blue </TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
         *  </TABLE>
         */
        template <>
        class PT_GFX_API Color<ARgbRef> {
            public:
                /** @brief This constructor will take reference to the real storage.
                 */
                inline Color(uint16_t &a, uint16_t &r, uint16_t &g, uint16_t &b)
                : _a(a), _r(r), _g(g), _b(b)
                {}

                /** @brief This constructor will take reference to the real storage.
                 */
                inline Color(const uint16_t &a, const uint16_t &r, const uint16_t &g, const uint16_t &b)
                : _a(const_cast<uint16_t&>(a)), _r(const_cast<uint16_t&>(r)), _g(const_cast<uint16_t&>(g)), _b(const_cast<uint16_t&>(b))
                {}

                /** @brief Copy constructor.
                 */
                inline Color(const Color<ARgbRef>& c)
                : _a(c._a), _r(c._r), _g(c._g), _b(c._b)
                {}


                /** @brief Assignment operator.
                 */
                inline Color<ARgbRef>& operator=(const Color<ARgbRef>& c)
                { _a = c._a; _r = c._r; _g = c._g; _b = c._b; return *this; }

                /** @brief Assignment operator.
                 *
                 *  This assigns color with different type to this one by calling
                 *  assign(), which can be overloaded to allow new color types to
                 *  be assigned to this one.
                 */
                template <typename ColorT>
                inline Color<ARgbRef>& operator=(const ColorT& color)
                { assign(*this, color); return *this; }


                /** @brief Assignment-addition operator (beware of overflow).
                 */
                inline Color<ARgbRef>& operator+=(const Color<ARgbRef>& c)
                { _a += c._a; _r += c._r; _g += c._g; _b += c._b; return *this; }

                /** @brief Assignment-substraction operator (beware of underflow).
                 */
                inline Color<ARgbRef>& operator-=(const Color<ARgbRef>& c)
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

            protected:
                uint16_t &_a, &_r, &_g, &_b;
        };


        /** @brief Convenience access to the 64-Bit ARGB color model.
         *  @ingroup Gfx
         */
        typedef Color<ARgbRef> ARgbColorRef;


        /** @brief Full specialisation of the color traits class for ARgbColorRef.
         */
        template <>
        struct ColorTraits<ARgbColorRef> {
            typedef uint16_t ComponentT;
            typedef uint32_t TmpValueT;

            // Color pointer class for ARgbColorRef color model.
            class ColorPtrT {
                public:
                    inline ColorPtrT(std::vector<ComponentT*>& chanPtr,
                                     size_t                    imageWidth,
                                     size_t                    posX,
                                     size_t                    posY)
                    {
                        // In this ARgbColorRef, all channel have the same full size
                        // (no subsampling) and thus simple pointer arithmetic will
                        // does the job :)
                        const size_t pos = posY*imageWidth + posX;

                        _chnA = chanPtr[0] + pos; // A is channel #0
                        _chnR = chanPtr[1] + pos; // R is channel #0
                        _chnG = chanPtr[2] + pos; // G is channel #0
                        _chnB = chanPtr[3] + pos; // B is channel #0
                    }

                    inline ARgbColorRef operator*()
                    { return ARgbColorRef(*_chnA, *_chnR, *_chnG, *_chnB); }

                    inline ColorPtrT& operator++()
                    { ++_chnA; ++_chnR; ++_chnG; ++_chnB; return *this; }

                    inline ColorPtrT& operator--()
                    { --_chnA; --_chnR; --_chnG; --_chnB; return *this; }

                    inline ColorPtrT& operator+=(size_t n)
                    { _chnA+=n; _chnR+=n; _chnG+=n; _chnB+=n; return *this; }

                    inline ColorPtrT& operator-=(size_t n)
                    { _chnA-=n; _chnR-=n; _chnG-=n; _chnB-=n; return *this; }

                private:
                    uint16_t* _chnA;
                    uint16_t* _chnR;
                    uint16_t* _chnG;
                    uint16_t* _chnB;
            };

            // Constant color pointer class for ARgbColorRef color model.
            class ConstColorPtrT {
                public:
                    inline ConstColorPtrT(const std::vector<ComponentT*>& chanPtr,
                                          size_t                          imageWidth,
                                          size_t                          posX,
                                          size_t                          posY)
                    {
                        // In this ARgbColorRef, all channel have the same full size
                        // (no subsampling) and thus simple pointer arithmetic will
                        // does the job :)
                        const size_t pos = posY*imageWidth + posX;

                        _chnA = chanPtr[0] + pos; // A is channel #0
                        _chnR = chanPtr[1] + pos; // R is channel #0
                        _chnG = chanPtr[2] + pos; // G is channel #0
                        _chnB = chanPtr[3] + pos; // B is channel #0
                    }

                    inline const ARgbColorRef operator*() const
                    { return ARgbColorRef(*_chnA, *_chnR, *_chnG, *_chnB); }

                    inline ConstColorPtrT& operator++()
                    { ++_chnA; ++_chnR; ++_chnG; ++_chnB; return *this; }

                    inline ConstColorPtrT& operator--()
                    { --_chnA; --_chnR; --_chnG; --_chnB; return *this; }

                    inline ConstColorPtrT& operator+=(size_t n)
                    { _chnA+=n; _chnR+=n; _chnG+=n; _chnB+=n; return *this; }

                    inline ConstColorPtrT& operator-=(size_t n)
                    { _chnA-=n; _chnR-=n; _chnG-=n; _chnB-=n; return *this; }

                private:
                    const uint16_t* _chnA;
                    const uint16_t* _chnR;
                    const uint16_t* _chnG;
                    const uint16_t* _chnB;
            };


            // We have 4 channels (A, R, G and B)
            static const size_t ChannelCount = 4;


            // In this ARgbColorRef, all channel have the same full size
            // (no subsampling) and thus just return a constant value of '1'
            static inline size_t ChannelSubsamplingX(size_t channelIndex)
            {
                assert(channelIndex <= ChannelCount);
                return 1;
            }

            // In this ARgbColorRef, all channel have the same full size
            // (no subsampling) and thus just return a constant value of '1'
            static inline size_t ChannelSubsamplingY(size_t channelIndex)
            {
                assert(channelIndex <= ChannelCount);
                return 1;
            }
        };


        /** @brief Convert a Color<ARgbRef> to a Color<ARgb>.
         */
        inline const Color<ARgb> toARgb(const Color<ARgbRef>& from)
        { return Color<ARgb>(from.alpha(), from.red(), from.green(), from.blue()); }

        /** @brief Convert a Color<ARgb> to a Color<ARgb8888>.
         */
        inline void fromARgb(Color<ARgbRef>& to, const Color<ARgb>& from)
        {
            to.setAlpha(from.alpha());
            to.setRed  (from.red  ());
            to.setGreen(from.green());
            to.setBlue (from.blue ());
        }


        /** @brief Equality operator for Color<ARgbRef> comparison.
         */
        inline bool operator==(const Color<ARgbRef>& c1, const Color<ARgbRef>& c2)
        { return c1.alpha()==c2.alpha() && c1.red()==c2.red() && c1.green()==c2.green() && c1.blue()==c2.blue(); }

        /** @brief Less-than operator for Color<ARgbRef> comparison.
         */
        inline bool operator<(const Color<ARgbRef>& c1, const Color<ARgbRef>& c2)
        { return c1.alpha()<c2.alpha() || c1.red()<c2.red() || c1.green()<c2.green() || c1.blue()<c2.blue(); }

        /** @brief Greater-than operator for Color<ARgbRef> comparison.
         */
        inline bool operator>(const Color<ARgbRef>& c1, const Color<ARgbRef>& c2)
        { return c1.alpha()>c2.alpha() || c1.red()>c2.red() || c1.green()>c2.green() || c1.blue()>c2.blue(); }


        /** @brief Make the greyscale version of the source Color<ARgbRef>.
         */
        inline Color<ARgbRef>& greyscale(Color<ARgbRef>& to, const Color<ARgbRef>& from)
        {
            Color<ARgb> tmp;
            fromARgb(to, greyscale( tmp, toARgb(from) ) );
            return to;
        }


        /** @brief Mix two Color<ARgbRef>s using the given mixing factor.
         */
        template <typename FactorT>
        inline void blend(Color<ARgbRef>& dst, const Color<ARgbRef>& src, const FactorT& factor)
        {
            Color<ARgb> tmp;
            fromARgb(dst, blend( tmp, toARgb(src), factor ) );
        }

    } // namespace Gfx

} // namespace Pt

#endif

