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

#include <limits>
#include <vector>


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
                /** @brief Non-reference type (container type) of this color.
                 */
                typedef Color<ARgb> NonRefT;

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

        /** @brief Convenience access to the reference-type 64-Bit ARGB color model.
         *  @ingroup Gfx
         */
       typedef Color<ARgbProxy> ARgbColorProxy;


        /** @brief Full specialisation of the color traits class for ARgbColorProxy.
         */
        template <>
        struct ColorTraits<ARgbColorProxy> {
            typedef uint16_t ComponentT; // Value type of the components
            typedef uint32_t TmpValueT;  // Temporary value type to be used when
                                         // performing arithmetic on the components

            // Color pointer class for ARgbColorProxy color model.
            class ColorPtrT {
                public:
                    inline ColorPtrT(void*)
                    : _chnA(0), _chnR(0), _chnG(0), _chnB(0)
                    {}

                    inline ColorPtrT(std::vector<ComponentT*>& chanPtr,
                                     size_t                    imageWidth,
                                     size_t                    imageHeight,
                                     size_t                    posX,
                                     size_t                    posY)
                    : _imgW(imageWidth), _imgH(imageHeight)
                    {
                        // In this ARgbColorProxy, all channel have the same full size
                        // (no subsampling) and thus simple pointer arithmetic will
                        // does the job :)
                        const size_t pos = posY*imageWidth + posX;

                        _chnAStart = chanPtr[0];

                        _chnA = chanPtr[0] + pos; // A is channel #0
                        _chnR = chanPtr[1] + pos; // R is channel #1
                        _chnG = chanPtr[2] + pos; // G is channel #2
                        _chnB = chanPtr[3] + pos; // B is channel #3
                    }

                    inline ARgbColorProxy operator*()
                    { return ARgbColorProxy(*_chnA, *_chnR, *_chnG, *_chnB); }

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

                    inline Math::Point currentXYPosition() const
                    {
                        const size_t pos = _chnA - _chnAStart;
                        return Math::Point(pos/_imgH, pos/_imgW);
                    }

                private:
                    uint16_t* _chnAStart; // Becase all channel are at the same size, just
                    uint16_t* _chnA;      // use the alpha channel as the master channel
                    uint16_t* _chnR;
                    uint16_t* _chnG;
                    uint16_t* _chnB;

                    size_t    _imgW, _imgH;
            };

            // Constant color pointer class for ARgbColorProxy color model.
            class ConstColorPtrT {
                public:
                    inline ConstColorPtrT(void*)
                    : _chnA(0), _chnR(0), _chnG(0), _chnB(0)
                    {}

                    inline ConstColorPtrT(const std::vector<ComponentT*>& chanPtr,
                                          size_t                          imageWidth,
                                          size_t                          imageHeight,
                                          size_t                          posX,
                                          size_t                          posY)
                    : _imgW(imageWidth), _imgH(imageHeight)
                    {
                        // In this ARgbColorProxy, all channel have the same full size
                        // (no subsampling) and thus simple pointer arithmetic will
                        // does the job :)
                        const size_t pos = posY*imageWidth + posX;

                        _chnAStart = chanPtr[0];

                        _chnA = chanPtr[0] + pos; // A is channel #0
                        _chnR = chanPtr[1] + pos; // R is channel #1
                        _chnG = chanPtr[2] + pos; // G is channel #2
                        _chnB = chanPtr[3] + pos; // B is channel #3
                    }

                    inline ARgbColor operator*() const
                    { return ARgbColor(*_chnA, *_chnR, *_chnG, *_chnB); }

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

                    inline Math::Point currentXYPosition() const
                    {
                        const size_t pos = _chnA - _chnAStart;
                        return Math::Point(pos/_imgH, pos/_imgW);
                    }

                private:
                    const uint16_t* _chnAStart; // Becase all channel are at the same size, just
                    const uint16_t* _chnA;      // use the alpha channel as the master channel
                    const uint16_t* _chnR;
                    const uint16_t* _chnG;
                    const uint16_t* _chnB;

                    size_t          _imgW, _imgH;
            };


            // Scanline class for ARgbColorProxy color model.
            class ScanlineT {
                public:
                    inline ScanlineT(std::vector<ComponentT*>& chanPtr,
                                     size_t                    imageWidth,
                                     size_t                    posY)
                    {
                        // In this ARgbColorProxy, all channel have the same full size
                        // (no subsampling) and thus simple pointer arithmetic will
                        // does the job :)
                        const size_t pos = posY*imageWidth;

                        _chnA = chanPtr[0] + pos; // A is channel #0
                        _chnR = chanPtr[1] + pos; // R is channel #1
                        _chnG = chanPtr[2] + pos; // G is channel #2
                        _chnB = chanPtr[3] + pos; // B is channel #3
                    }

                    inline ARgbColorProxy operator[](size_t x)
                    { return ARgbColorProxy( *(_chnA+x), *(_chnR+x), *(_chnG+x), *(_chnB+x) ); }

                private:
                    uint16_t* _chnA;
                    uint16_t* _chnR;
                    uint16_t* _chnG;
                    uint16_t* _chnB;
            };

            // Constant scanline class for ARgbColorProxy color model.
            class ConstScanlineT {
                public:
                    inline ConstScanlineT(const std::vector<ComponentT*>& chanPtr,
                                          size_t                          imageWidth,
                                          size_t                          posY)
                    {
                        // In this ARgbColorProxy, all channel have the same full size
                        // (no subsampling) and thus simple pointer arithmetic will
                        // does the job :)
                        const size_t pos = posY*imageWidth;

                        _chnA = chanPtr[0] + pos; // A is channel #0
                        _chnR = chanPtr[1] + pos; // R is channel #1
                        _chnG = chanPtr[2] + pos; // G is channel #2
                        _chnB = chanPtr[3] + pos; // B is channel #3
                    }

                    inline ARgbColor operator[](size_t x)
                    { return ARgbColor( *(_chnA+x), *(_chnR+x), *(_chnG+x), *(_chnB+x) ); }

                private:
                    const uint16_t* _chnA;
                    const uint16_t* _chnR;
                    const uint16_t* _chnG;
                    const uint16_t* _chnB;
            };


            //
            // Below are specific to reference-type color only
            //

            // We have 4 channels (A, R, G and B)
            static const size_t ChannelCount = 4;

            // In this ARgbColorProxy, all channel have the same full size
            // (no subsampling) and thus just return a constant value of '1'
            static inline size_t ChannelSubsamplingX(size_t channelIndex)
            {
                assert(channelIndex <= ChannelCount);
                return 1;
            }

            // In this ARgbColorProxy, all channel have the same full size
            // (no subsampling) and thus just return a constant value of '1'
            static inline size_t ChannelSubsamplingY(size_t channelIndex)
            {
                assert(channelIndex <= ChannelCount);
                return 1;
            }
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

