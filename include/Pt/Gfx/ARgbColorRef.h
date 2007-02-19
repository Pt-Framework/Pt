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

        /** @brief Reference-type 64-Bit ARGB color class.
         *  @ingroup Gfx
         *
         *  @see ARgbColor.
         */
        template <>
        class PT_GFX_API Color<ARgb, ReferenceType> {
            public:
                /** @brief Non-reference type (container type) of this color.
                 */
                typedef Color<ARgb, ContainerType> NonRefT;

                /** @brief Constant reference type of this color.
                 */
                typedef Color<ARgb, ConstReferenceType> ConstRefT;

            public:
                /** @brief This constructor will take reference to the real storage.
                 */
                inline Color(uint16_t &a, uint16_t &r, uint16_t &g, uint16_t &b)
                : _a(a), _r(r), _g(g), _b(b)
                {}

                /** @brief Copy constructor.
                 */
                inline Color(const Color& c)
                : _a(c._a), _r(c._r), _g(c._g), _b(c._b)
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

        /** @brief Constant reference-type 64-Bit ARGB color class.
         *  @ingroup Gfx
         *
         *  @see  ARgbColor.
         *  @note This class is not meant to be used directly by the user.
         */
        template <>
        class PT_GFX_API Color<ARgb, ConstReferenceType> {
            public:
                /** @brief Non-reference type (container type) of this color.
                 */
                typedef Color<ARgb, ContainerType> NonRefT;

                /** @brief Non-constant reference type of this color.
                 */
                typedef Color<ARgb, ReferenceType> NonConstRefT;

            public:
                /** @brief This constructor will take reference to the real storage.
                 */
                inline Color(const NonRefT& c)
                : _a(c.alpha()), _r(c.red()), _g(c.green()), _b(c.blue())
                {}

                /** @brief This constructor will take reference to the real storage.
                 */
                inline Color(const uint16_t &a, const uint16_t &r, const uint16_t &g, const uint16_t &b)
                : _a(a), _r(r), _g(g), _b(b)
                {}

                /** @brief Copy constructor.
                 */
                inline Color(const Color& c)
                : _a(c._a), _r(c._r), _g(c._g), _b(c._b)
                {}


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

            protected:
                const uint16_t &_a, &_r, &_g, &_b;
        };


        /** @brief Convenience access to the reference-type 64-Bit ARGB color model.
         *  @ingroup Gfx
         */
        typedef Color<ARgb, ReferenceType> ARgbColorRef;

        /** @brief Convenience access to the constant reference-type 64-Bit ARGB color model.
         *  @ingroup Gfx
         */
        typedef Color<ARgb, ConstReferenceType> ARgbColorConstRef;


        /** @brief Full specialisation of the color traits class for ARgbColorRef.
         */
        template <>
        struct ColorTraits<ARgbColorRef> {
            // Value type of the components
            typedef uint16_t ComponentT;

            // Temporary value type to be used when performing arithmetic on
            // the components
            typedef uint32_t TmpValueT;


            // Color pointer class for ARgbColorRef color model.
            class ColorPtrT {
                public:
                    inline ColorPtrT(void*)
                    : _chnA(0), _chnR(0), _chnG(0), _chnB(0)
                    {}

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

                    inline bool operator==(const ColorPtrT& c) const
                    { return _chnA==c._chnA && _chnR==c._chnR && _chnG==c._chnG && _chnB==c._chnB; }

                    inline bool operator!=(const ColorPtrT& c) const
                    { return _chnA!=c._chnA || _chnR!=c._chnR || _chnG!=c._chnG || _chnB!=c._chnB; }

                private:
                    uint16_t* _chnA;
                    uint16_t* _chnR;
                    uint16_t* _chnG;
                    uint16_t* _chnB;
            };

            // Constant color pointer class for ARgbColorRef color model.
            class ConstColorPtrT {
                public:
                    inline ConstColorPtrT(void*)
                    : _chnA(0), _chnR(0), _chnG(0), _chnB(0)
                    {}

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

                    inline ARgbColorConstRef operator*() const
                    { return ARgbColorConstRef(*_chnA, *_chnR, *_chnG, *_chnB); }

                    inline ConstColorPtrT& operator++()
                    { ++_chnA; ++_chnR; ++_chnG; ++_chnB; return *this; }

                    inline ConstColorPtrT& operator--()
                    { --_chnA; --_chnR; --_chnG; --_chnB; return *this; }

                    inline ConstColorPtrT& operator+=(size_t n)
                    { _chnA+=n; _chnR+=n; _chnG+=n; _chnB+=n; return *this; }

                    inline ConstColorPtrT& operator-=(size_t n)
                    { _chnA-=n; _chnR-=n; _chnG-=n; _chnB-=n; return *this; }

                    inline bool operator==(const ConstColorPtrT& c) const
                    { return _chnA==c._chnA && _chnR==c._chnR && _chnG==c._chnG && _chnB==c._chnB; }

                    inline bool operator!=(const ConstColorPtrT& c) const
                    { return _chnA!=c._chnA || _chnR!=c._chnR || _chnG!=c._chnG || _chnB!=c._chnB; }

                private:
                    const uint16_t* _chnA;
                    const uint16_t* _chnR;
                    const uint16_t* _chnG;
                    const uint16_t* _chnB;
            };


            // Scanline class for ARgbColorRef color model.
            class ScanlineT {
                // TODO: WRITE IT !!!
                public:
                    inline ScanlineT(std::vector<ComponentT*>& chanPtr,
                                     size_t                    imageWidth,
                                     size_t                    posY)
                    {}

                    //inline ARgbColorRef operator[](size_t x)
                    //{}
            };

            // Constant scanline class for ARgbColorRef color model.
            class ConstScanlineT {
                // TODO: WRITE IT !!!
                public:
                    inline ConstScanlineT(const std::vector<ComponentT*>& chanPtr,
                                          size_t                          imageWidth,
                                          size_t                          posY)
                    {}

                    //inline ARgbColorConstRef operator[](size_t x)
                    //{}
            };


            //
            // Below are specific to reference-type color only
            //

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


    } // namespace Gfx

} // namespace Pt

#endif

