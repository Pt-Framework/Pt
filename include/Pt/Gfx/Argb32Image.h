/* Copyright (C) 2016 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_GFX_ARGB32IMAGE_H
#define PT_GFX_ARGB32IMAGE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/BasicImage.h>
#include <Pt/Types.h>

namespace Pt {
namespace Gfx {


/** @brief ARGB-32 image model.
*/
class Argb32Model
{
    public:
        class Pixel;
        class ConstPixel;

        static std::size_t imageSize(const Size& size, Pt::ssize_t padding)
        {
            std::size_t l = (size.width() * 4) + padding;
            std::size_t n = l * size.height();
            return n;
        }

        static Pt::ssize_t pixelStride()
        {
            return 4;
        }

    public:

// Enabling this one seems to does not matter in an x86_64
//#define USE_MULTIPLY_SHIFT_FOR_CONSTANT_DIVISION

// Enabling this one seems to make everything faster in an x86_64
#define USE_DUFFS_DEVICE

        /*
         * http://embeddedgurus.com/stack-overflow/2009/06/division-of-integers-by-constants
         *
         * Convert to binary using Javascript console: alert((1/255).toString(2))
         *     1/255
         *     => 0.00000001000000010000000100000001000000010000000100000001
         *
         * Take all the bits to the right of the binary point:
         *     => 00000001000000010000000100000001000000010000000100000001
         *
         * Left shift them until the bit to the right of the binary point is 1 and
         * record the required number of shifts as S:
         *        00000001000000010000000100000001000000010000000100000001
         *        *******
         *     => 1000000010000000100000001000000010000000100000001
         *     => S = 7
         *
         * Take the most significant 17 bits:
         *        1000000010000000100000001000000010000000100000001
         *        *****************
         *     => 10000000100000001
         *
         * Add one:
         *        10000000100000001
         *                        1
         *        ----------------- +
         *     => 10000000100000010
         *
         * Truncate to 16 bits and convert to 4-digit hexadecimal as M:
         *        10000000100000010
         *        ****************
         *     => 8081
         *
         * The formula will be RESULT = (((uint32_t) VALUE * (uint32_t) M) >> 16) >> S:
         *     => RESULT = (((uint32_t) VALUE * 0x00008081) >> 16) >> 7
         *
         *
         * For 1/257:
         *     => 0.00000000111111110000000011111111000000001111111100000001
         *     => 00000000111111110000000011111111000000001111111100000001
         *        ********
         *     => 111111110000000011111111000000001111111100000001 (S = 8)
         *        *****************
         *     => 11111111000000001
         *                        1
         *        ----------------- +
         *     => 11111111000000010
         *        ****************
         *     => FF01
         *     => RESULT = (((uint32_t) VALUE * 0x0000FF01) >> 16) >> 8
         */
#ifndef USE_MULTIPLY_SHIFT_FOR_CONSTANT_DIVISION
    #define DIV_BY_255(V) ( (uint32_t) V / 255 )
    #define DIV_BY_257(V) ( (uint32_t) V / 257 )
#else
    #define DIV_BY_255(V) ( (((uint32_t) V * 0x00008081) >> 16) >> 7 )
    #define DIV_BY_257(V) ( (((uint32_t) V * 0x0000FF01) >> 16) >> 8 )
#endif

        static Color toColor(const Pt::uint8_t* p)
        {
            Pt::uint32_t pixel = *reinterpret_cast<const Pt::uint32_t*>(p);

            const uint16_t ta =  pixel               >> 24;
            const uint16_t tr = (pixel & 0x00FF0000) >> 16;
            const uint16_t tg = (pixel & 0x0000FF00) >>  8;
            const uint16_t tb =  pixel & 0x000000FF;

            Pt::uint16_t a = (ta << 8) + ta;
            Pt::uint16_t r = (tr << 8) + tr;
            Pt::uint16_t g = (tg << 8) + tg;
            Pt::uint16_t b = (tb << 8) + tb;

            return Color(a, r, g, b);
        }

        static void fromColor(Pt::uint8_t* p, const Color& c)
        {
            Pt::uint32_t* pixel = reinterpret_cast<Pt::uint32_t*>(p);

            *pixel = ( Pt::uint32_t(c.alpha() & 0xFF00) << 16 ) |
                     ( Pt::uint32_t(c.red  () & 0xFF00) <<  8 ) |
                       Pt::uint32_t(c.green() & 0xFF00)         |
                     ( Pt::uint32_t(c.blue ()         ) >>  8 );
        }

        static void sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from)
        {
            Pt::uint8_t  alpha    = *((const Pt::uint32_t*) (from)) >> 24;
            Pt::uint32_t alphaSrc = alpha;
            Pt::uint32_t alphaInv = 255 - alpha;

            to[0] = (Pt::uint8_t)((alphaSrc * from[0] + alphaInv * to[0]) >> 8);
            to[1] = (Pt::uint8_t)((alphaSrc * from[1] + alphaInv * to[1]) >> 8);
            to[2] = (Pt::uint8_t)((alphaSrc * from[2] + alphaInv * to[2]) >> 8);
            to[3] = (Pt::uint8_t)((alphaSrc * from[3] + alphaInv * to[3]) >> 8);
        }

        static void sourceOver(Pt::uint8_t* to, const Pt::Gfx::Color& from)
        {
            Pt::uint32_t alpha    = DIV_BY_257(from.alpha());
            Pt::uint32_t alphaSrc = alpha;
            Pt::uint32_t alphaInv = 255 - alpha;

            to[0] = (Pt::uint8_t)((alphaSrc * DIV_BY_257(from.blue ()) + alphaInv * to[0]) >> 8);
            to[1] = (Pt::uint8_t)((alphaSrc * DIV_BY_257(from.green()) + alphaInv * to[1]) >> 8);
            to[2] = (Pt::uint8_t)((alphaSrc * DIV_BY_257(from.red  ()) + alphaInv * to[2]) >> 8);
            to[3] = (Pt::uint8_t)((alphaSrc * alpha                    + alphaInv * to[3]) >> 8);
        }

        static void assign(Pt::uint8_t* to, const Color& c,
                           CompositionMode mode)
        {
            switch(mode) {
                default:
                case CompositionMode::SourceCopy:
                    Argb32Model::fromColor(to, c);
                    break;

                case CompositionMode::SourceOver:
                    Argb32Model::sourceOver(to, c);
                    break;
            }
        }

        static void assign(Pt::uint8_t* to, const Pt::uint8_t* from,
                           CompositionMode mode)
        {
            switch(mode) {
                default:
                case CompositionMode::SourceCopy:
                    *((Pt::uint32_t*) to) = *((const Pt::uint32_t*) from);
                    break;

                case CompositionMode::SourceOver:
                    Argb32Model::sourceOver(to, from);
                    break;
            }
        }

        static void assign(Pt::uint8_t* to, const Color& c,
                           CompositionMode mode, Pt::uint8_t blendingAlpha)
        {
            switch(mode) {
                default:
                case CompositionMode::SourceCopy: {
                    const Pt::uint32_t blendAlphaSrc = blendingAlpha;
                    const Pt::uint32_t blendAlphaInv = 255 - blendingAlpha;
                    to[0] = (blendAlphaSrc * DIV_BY_257(c.blue ()) + blendAlphaInv * to[0]) >> 8;
                    to[1] = (blendAlphaSrc * DIV_BY_257(c.green()) + blendAlphaInv * to[1]) >> 8;
                    to[2] = (blendAlphaSrc * DIV_BY_257(c.red  ()) + blendAlphaInv * to[2]) >> 8;
                    to[3] = (blendAlphaSrc * DIV_BY_257(c.alpha()) + blendAlphaInv * to[3]) >> 8;
                    break;
                }

                case CompositionMode::SourceOver:
                {
                    const Pt::uint32_t colorAlpha    = DIV_BY_257(c.alpha());
                    const Pt::uint32_t blendAlphaSrc = DIV_BY_255(colorAlpha * blendingAlpha);
                    const Pt::uint32_t blendAlphaInv = 255 - blendAlphaSrc;
                    to[0] = (blendAlphaSrc * DIV_BY_257(c.blue ()) + blendAlphaInv * to[0]) >> 8;
                    to[1] = (blendAlphaSrc * DIV_BY_257(c.green()) + blendAlphaInv * to[1]) >> 8;
                    to[2] = (blendAlphaSrc * DIV_BY_257(c.red  ()) + blendAlphaInv * to[2]) >> 8;
                    to[3] = (blendAlphaSrc * colorAlpha            + blendAlphaInv * to[3]) >> 8;
                    break;
                }
            }
        }

        static void assign(Pt::uint8_t* to, const Pt::uint8_t* from,
                           CompositionMode mode, Pt::uint8_t blendingAlpha)
        {
            switch(mode) {
                default:
                case CompositionMode::SourceCopy: {
                    const Pt::uint32_t blendAlphaSrc = blendingAlpha;
                    const Pt::uint32_t blendAlphaInv = 255 - blendingAlpha;
                    to[0] = (blendAlphaSrc * from[0] + blendAlphaInv * to[0]) >> 8;
                    to[1] = (blendAlphaSrc * from[1] + blendAlphaInv * to[1]) >> 8;
                    to[2] = (blendAlphaSrc * from[2] + blendAlphaInv * to[2]) >> 8;
                    to[3] = (blendAlphaSrc * from[3] + blendAlphaInv * to[3]) >> 8;
                    break;
                }

                case CompositionMode::SourceOver:
                {
                    const Pt::uint32_t colorAlpha    = from[3];
                    const Pt::uint32_t blendAlphaSrc = DIV_BY_255(colorAlpha * blendingAlpha);
                    const Pt::uint32_t blendAlphaInv = 255 - blendAlphaSrc;
                    to[0] = (blendAlphaSrc * from[0]    + blendAlphaInv * to[0]) >> 8;
                    to[1] = (blendAlphaSrc * from[1]    + blendAlphaInv * to[1]) >> 8;
                    to[2] = (blendAlphaSrc * from[2]    + blendAlphaInv * to[2]) >> 8;
                    to[3] = (blendAlphaSrc * colorAlpha + blendAlphaInv * to[3]) >> 8;
                    break;
                }
            }
        }

        static void assign(Pt::uint8_t* to, const Color& c, size_t length,
                           CompositionMode mode)
        {
            switch(mode) {
                default:
                case CompositionMode::SourceCopy: {
                    Pt::uint32_t src = ( Pt::uint32_t(c.alpha() & 0xFF00) << 16 ) |
                                       ( Pt::uint32_t(c.red  () & 0xFF00) <<  8 ) |
                                       ( Pt::uint32_t(c.green() & 0xFF00)       )  |
                                       ( Pt::uint32_t(c.blue ()         ) >>  8 );
                    Pt::uint32_t* dst =  reinterpret_cast<Pt::uint32_t*>(to);
#ifndef USE_DUFFS_DEVICE
                    for(size_t i = 0; i < length; ++i) *dst++ = src;
#else
                    register Pt::uint32_t* dst_ = dst;
                    register Pt::uint32_t  src_ = src;
                    register Pt::uint32_t  cnt  = length;
                    register Pt::uint32_t  n    = (cnt + 7) / 8;
                    switch(cnt % 8) {
                            case 0 : do { *dst_++ = src_;
                            case 7 :      *dst_++ = src_;
                            case 6 :      *dst_++ = src_;
                            case 5 :      *dst_++ = src_;
                            case 4 :      *dst_++ = src_;
                            case 3 :      *dst_++ = src_;
                            case 2 :      *dst_++ = src_;
                            case 1 :      *dst_++ = src_;
                                     } while (--n > 0);
                    }
#endif
                    break;
                }

                case CompositionMode::SourceOver: {
                    const Pt::uint32_t  blend    = DIV_BY_257(c.alpha());
                    const Pt::uint32_t  blendInv = 255 - blend;
                    const Pt::uint32_t  srcR     = DIV_BY_257(c.red  ()) * blend;
                    const Pt::uint32_t  srcG     = DIV_BY_257(c.green()) * blend;
                    const Pt::uint32_t  srcB     = DIV_BY_257(c.blue ()) * blend;
                    const Pt::uint32_t  srcA     = blend * blend;
                          Pt::uint8_t*  dst      = to;
#ifndef USE_DUFFS_DEVICE
                    for(size_t i = 0; i < length; ++i) {
                        dst[0] = (srcB + blendInv * dst[0]) >> 8;
                        dst[1] = (srcG + blendInv * dst[1]) >> 8;
                        dst[2] = (srcR + blendInv * dst[2]) >> 8;
                        dst[3] = (srcA + blendInv * dst[3]) >> 8;
                        dst += 4;
                    }
#else
                    #define STORE_VALUES()                            \
                        dst_[0] = (srcB_ + blendInv_ * dst_[0]) >> 8; \
                        dst_[1] = (srcG_ + blendInv_ * dst_[1]) >> 8; \
                        dst_[2] = (srcR_ + blendInv_ * dst_[2]) >> 8; \
                        dst_[3] = (srcA_ + blendInv_ * dst_[3]) >> 8; \
                        dst_ += 4
                    register Pt::uint32_t srcR_     = srcR;
                    register Pt::uint32_t srcG_     = srcG;
                    register Pt::uint32_t srcB_     = srcB;
                    register Pt::uint32_t srcA_     = srcA;
                    register Pt::uint32_t blendInv_ = blendInv;
                    register Pt::uint8_t* dst_ = dst;
                    register Pt::uint32_t cnt  = length;
                    register Pt::uint32_t n    = (cnt + 7) / 8;
                    switch(cnt % 8) {
                            case 0 : do { STORE_VALUES();
                            case 7 :      STORE_VALUES();
                            case 6 :      STORE_VALUES();
                            case 5 :      STORE_VALUES();
                            case 4 :      STORE_VALUES();
                            case 3 :      STORE_VALUES();
                            case 2 :      STORE_VALUES();
                            case 1 :      STORE_VALUES();
                                     } while (--n > 0);
                    }
                    #undef STORE_VALUES
#endif
                    break;
                }
            }
        }

        static void assign(Pt::uint8_t* to, const Pt::uint8_t* from, size_t length,
                           CompositionMode mode)
        {
            switch(mode) {
                default:
                case CompositionMode::SourceCopy: {
                    Pt::uint32_t  src = *reinterpret_cast<const Pt::uint32_t*>(from);
                    Pt::uint32_t* dst =  reinterpret_cast<Pt::uint32_t*>(to);
#ifndef USE_DUFFS_DEVICE
                    for(size_t i = 0; i < length; ++i) *dst++ = src;
#else
                    register Pt::uint32_t* dst_ = dst;
                    register Pt::uint32_t  src_ = src;
                    register Pt::uint32_t  cnt  = length;
                    register Pt::uint32_t  n    = (cnt + 7) / 8;
                    switch(cnt % 8) {
                            case 0 : do { *dst_++ = src_;
                            case 7 :      *dst_++ = src_;
                            case 6 :      *dst_++ = src_;
                            case 5 :      *dst_++ = src_;
                            case 4 :      *dst_++ = src_;
                            case 3 :      *dst_++ = src_;
                            case 2 :      *dst_++ = src_;
                            case 1 :      *dst_++ = src_;
                                     } while (--n > 0);
                    }
#endif
                    break;
                }

                case CompositionMode::SourceOver: {
                    const Pt::uint32_t  blend    = from[3];
                    const Pt::uint32_t  blendInv = 255 - blend;
                    const Pt::uint32_t  srcR     = from[2] * blend;
                    const Pt::uint32_t  srcG     = from[1] * blend;
                    const Pt::uint32_t  srcB     = from[0] * blend;
                    const Pt::uint32_t  srcA     = blend   * blend;
                          Pt::uint8_t*  dst      = to;
#ifndef USE_DUFFS_DEVICE
                    for(size_t i = 0; i < length; ++i) {
                        dst[0] = (srcB + blendInv * dst[0]) >> 8;
                        dst[1] = (srcG + blendInv * dst[1]) >> 8;
                        dst[2] = (srcR + blendInv * dst[2]) >> 8;
                        dst[3] = (srcA + blendInv * dst[3]) >> 8;
                        dst += 4;
                    }
#else
                    #define STORE_VALUES()                            \
                        dst_[0] = (srcB_ + blendInv_ * dst_[0]) >> 8; \
                        dst_[1] = (srcG_ + blendInv_ * dst_[1]) >> 8; \
                        dst_[2] = (srcR_ + blendInv_ * dst_[2]) >> 8; \
                        dst_[3] = (srcA_ + blendInv_ * dst_[3]) >> 8; \
                        dst_ += 4
                    register Pt::uint32_t srcR_     = srcR;
                    register Pt::uint32_t srcG_     = srcG;
                    register Pt::uint32_t srcB_     = srcB;
                    register Pt::uint32_t srcA_     = srcA;
                    register Pt::uint32_t blendInv_ = blendInv;
                    register Pt::uint8_t* dst_ = dst;
                    register Pt::uint32_t cnt  = length;
                    register Pt::uint32_t n    = (cnt + 7) / 8;
                    switch(cnt % 8) {
                            case 0 : do { STORE_VALUES();
                            case 7 :      STORE_VALUES();
                            case 6 :      STORE_VALUES();
                            case 5 :      STORE_VALUES();
                            case 4 :      STORE_VALUES();
                            case 3 :      STORE_VALUES();
                            case 2 :      STORE_VALUES();
                            case 1 :      STORE_VALUES();
                                     } while (--n > 0);
                    }
                    #undef STORE_VALUES
#endif
                    break;
                }
            }
        }

        template <typename T>
        static void advance(T*& p, Pt::ssize_t& xpos, Pt::ssize_t& ypos,
                            const BasicView<Argb32Model>& view)
        {
            if( ++xpos >= view.width() )
            {
                xpos = 0;
                ++ypos;

                p += view.padding();
            }

            p += 4;
        }

        template <typename T>
        static void advance(T*& p, Pt::ssize_t n, Pt::ssize_t& xpos, Pt::ssize_t& ypos,
                            const BasicView<Argb32Model>& view, T* data)
        {
            Pt::ssize_t off = xpos + n;
            ypos += off / view.width();
            xpos  = off % view.width();

            p = data + view.stride() * ypos + xpos * 4;
        }
#undef DIV_BY_255
#undef DIV_BY_257
};


/** @brief Const pixel in a ARGB-32 Image.
*/
class Argb32Model::ConstPixel
{
    friend class Pixel;

    public:
        ConstPixel(const BasicView<Argb32Model>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        : _view(0)
        , _xpos(0)
        , _ypos(0)
        , _p(0)
        {
            reset(view, xpos, ypos);
        }

        ConstPixel(const ConstPixel& p)
        : _view(p._view)
        , _xpos(p._xpos)
        , _ypos(p._ypos)
        , _p(p._p)
        { }

        void reset(const BasicView<Argb32Model>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        {
            _view = &view;
            _xpos = xpos;
            _ypos = ypos;

            Pt::ssize_t off = view.stride() * ypos + (xpos * 4);
            _p = view.data() + off;
        }

        void reset(const ConstPixel& p)
        {
            _view = p._view;
            _xpos = p._xpos;
            _ypos = p._ypos;

            _p = p._p;
        }

        void advance()
        {
            Argb32Model::advance(_p, _xpos, _ypos, *_view);
        }

        void advance( Pt::ssize_t n )
        {
            Argb32Model::advance(_p, n, _xpos, _ypos, *_view, _view->data());
        }

        Color toColor() const
        {
            return Argb32Model::toColor(_p);
        }

        Pt::uint8_t alpha() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return *val >> 24;
        }

        Pt::uint8_t red() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return (*val & 0x00FF0000) >> 16;
        }

        Pt::uint8_t green() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return (*val & 0x0000FF00) >> 8;
        }

        Pt::uint8_t blue() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return *val & 0x000000FF;
        }

        bool operator!=(const ConstPixel& p) const
        {
            return _p != p._p;
        }

        bool operator==(const ConstPixel& p) const
        {
            return _p == p._p;
        }

    private:
        ConstPixel& operator=(const ConstPixel&);

    private:
        const BasicView<Argb32Model>* _view;
        Pt::ssize_t                   _xpos;
        Pt::ssize_t                   _ypos;
        const Pt::uint8_t*            _p;
};


/** @brief Const pixel in a ARGB-32 Image.
*/
class Argb32Model::Pixel
{
    public:
        Pixel(BasicView<Argb32Model>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        : _view(0)
        , _xpos(0)
        , _ypos(0)
        , _p(0)
        {
            reset(view, xpos, ypos);
        }

        Pixel(const Pixel& p)
        : _view(p._view)
        , _xpos(p._xpos)
        , _ypos(p._ypos)
        , _p(p._p)
        { }

        Pixel& operator=(const Pixel& p)
        {
            assign(p, CompositionMode::SourceCopy);
            return *this;
        }

        Pixel& operator=(const ConstPixel& p)
        {
            assign(p, CompositionMode::SourceCopy);
            return *this;
        }

        Pixel& operator=(const Color& color)
        {
            assign(color, CompositionMode::SourceCopy);
            return *this;
        }

        void reset(BasicView<Argb32Model>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        {
            _view = &view;
            _xpos = xpos;
            _ypos = ypos;

            Pt::ssize_t off = view.stride() * ypos + (xpos * 4);
            _p = view.data() + off;
        }

        void reset(const Pixel& p)
        {
            _view = p._view;
            _xpos = p._xpos;
            _ypos = p._ypos;

            _p = p._p;
        }

        void advance()
        {
            Argb32Model::advance(_p, _xpos, _ypos, *_view);
        }

        void advance( Pt::ssize_t n )
        {
            Argb32Model::advance(_p, n, _xpos, _ypos, *_view, _view->data());
        }

        void assign(const Color& c, CompositionMode mode)
        {
            Argb32Model::assign(_p, c, mode);
        }

        void assign(const Pixel& p, CompositionMode mode)
        {
            Argb32Model::assign(_p, p._p, mode);
        }

        void assign(const ConstPixel& p, CompositionMode mode)
        {
            Argb32Model::assign(_p, p._p, mode);
        }

        Color toColor() const
        {
            return Argb32Model::toColor(_p);
        }

        Pt::uint8_t alpha() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return *val >> 24;
        }

        Pt::uint8_t red() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return (*val & 0x00FF0000) >> 16;
        }

        Pt::uint8_t green() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return (*val & 0x0000FF00) >> 8;
        }

        Pt::uint8_t blue() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return *val & 0x000000FF;
        }

        void setAlpha(Pt::uint8_t a)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(_p);
            *val = (*val & 0x00FFFFFF) | (uint32_t(a) << 24);
        }

        void setRed(Pt::uint8_t r)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(_p);
            *val = (*val & 0xFF00FFFF) | (uint32_t(r) << 16);
        }

        void setGreen(Pt::uint8_t g)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(_p);
            *val = (*val & 0xFFFF00FF) | (uint32_t(g) << 8);
        }

        void setBlue(Pt::uint8_t b)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(_p);
            *val = (*val & 0xFFFFFF00) | uint32_t(b);
        }

        bool operator!=(const Pixel& p) const
        { return _p != p._p; }

        bool operator==(const Pixel& p) const
        { return _p == p._p; }

    private:
        BasicView<Argb32Model>* _view;
        Pt::ssize_t             _xpos;
        Pt::ssize_t             _ypos;
        Pt::uint8_t*            _p;
};


/** @brief ARGB-32 image.
*/
class Argb32Image : public BasicImage<Argb32Model>
{
    public:
        /** @brief Constructor.
        */
        Argb32Image(const Size& size, size_t padding = 0)
        : BasicImage(size, padding)
        { }

        /** @brief Construct from external buffer.
        */
        Argb32Image(Pt::uint8_t* data, const Size& size, size_t padding = 0)
        : BasicImage(data, size, padding)
        { }
};


} // namespace
} // namespace

#endif
