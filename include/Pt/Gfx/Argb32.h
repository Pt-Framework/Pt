/* Copyright (C) 2016-2016 Marc Boris Duerner
   Copyright (C) 2017-2017 Aloysius Indrayanto

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef PT_GFX_ARGB32_H
#define PT_GFX_ARGB32_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/BasicView.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

class Argb32;
class Argb32Pixel;
class Argb32ConstPixel;

/** @brief ARGB-32 pixel reference.
*/
class Argb32Pixel
{
    friend class Argb32;
    friend class Argb32ConstPixel;

    public:
        typedef BasicView<Argb32> View;

    public:
        Argb32Pixel(View& view, Pt::ssize_t x, Pt::ssize_t y);

        Argb32Pixel(const Argb32Pixel& p)
        : _base(p._base)
        , _x(p._x)
        , _y(p._y)
        {  }

        Pt::ssize_t x() const
        { return _x; }

        Pt::ssize_t y() const
        { return _y; }

        Pt::uint8_t* base()
        { return _base; }

        const Pt::uint8_t* base() const
        { return _base; }

        Pt::uint8_t alpha() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_base);
            return *val >> 24;
        }

        Pt::uint8_t red() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_base);
            return (*val & 0x00FF0000) >> 16;
        }

        Pt::uint8_t green() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_base);
            return (*val & 0x0000FF00) >> 8;
        }

        Pt::uint8_t blue() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_base);
            return *val & 0x000000FF;
        }

        void setAlpha(Pt::uint8_t a)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(_base);
            *val = (*val & 0x00FFFFFF) | (uint32_t(a) << 24);
        }

        void setRed(Pt::uint8_t r)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(_base);
            *val = (*val & 0xFF00FFFF) | (uint32_t(r) << 16);
        }

        void setGreen(Pt::uint8_t g)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(_base);
            *val = (*val & 0xFFFF00FF) | (uint32_t(g) << 8);
        }

        void setBlue(Pt::uint8_t b)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(_base);
            *val = (*val & 0xFFFFFF00) | uint32_t(b);
        }

        bool operator==(const Argb32Pixel& p) const;

        bool operator==(const Argb32ConstPixel& p) const;

    private:
        Pt::uint8_t*  _base;
        Pt::ssize_t   _x;
        Pt::ssize_t   _y;
};

/** @brief ARGB-32 const pixel reference.
*/
class Argb32ConstPixel
{
    friend class Argb32;
    friend class Argb32Pixel;
    
    public:
        typedef BasicView<Argb32> View;

    public:
        Argb32ConstPixel(const View& view, Pt::ssize_t x, Pt::ssize_t y);

        Argb32ConstPixel(const Argb32ConstPixel& p)
        : _base(p._base)
        , _x(p._x)
        , _y(p._y)
        {  }

        Argb32ConstPixel(const Argb32Pixel& p)
        : _base(p.base())
        , _x(p.x())
        , _y(p.y())
        {  }

        Pt::ssize_t x() const
        { return _x; }

        Pt::ssize_t y() const
        { return _y; }

        const Pt::uint8_t* base() const
        { return _base; }

        Pt::uint8_t alpha() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_base);
            return *val >> 24;
        }

        Pt::uint8_t red() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_base);
            return (*val & 0x00FF0000) >> 16;
        }

        Pt::uint8_t green() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_base);
            return (*val & 0x0000FF00) >> 8;
        }

        Pt::uint8_t blue() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_base);
            return *val & 0x000000FF;
        }

        bool operator==(const Argb32Pixel& p) const
        { return _base == p._base; }

        bool operator==(const Argb32ConstPixel& p) const
        { return _base == p._base; }

    private:
        const Pt::uint8_t* _base;
        Pt::ssize_t        _x;
        Pt::ssize_t        _y;
};

/** @brief ARGB-32 image format.
*/
class Argb32
{
    public:
        typedef BasicView<Argb32>  View;
        
        typedef Argb32Pixel         Pixel;
        typedef Argb32ConstPixel    ConstPixel;

    public:
        static Pt::ssize_t pixelStride()
        {
            return 4;
        }

        static std::size_t imageSize(std::size_t width, std::size_t height,
                                     std::size_t padding)
        {
            std::size_t l = (width * 4) + padding;
            std::size_t n = l * height;
            return n;
        }

        bool operator==(const Argb32& ) const
        {
            return true;
        }

        bool operator!=(const Argb32& ) const
        {
            return false;
        }

        // planes(), planeStride()
    
    public:
        template <typename P>
        void advance(const View& view, P& p) const
        {
            if( ++p._x >= view.width() )
            {
                p._x = 0;
                ++p._y;

                p._base += view.padding();
            }

            p._base += view.pixelStride();
        }
        
        template <typename P>
        void advance(const View& view, P& p, Pt::ssize_t n) const
        {
            Pt::ssize_t off = p._x + n;

            std::size_t dy = off / view.width();
            std::size_t dx = off % view.width() - p._x;

            p._x += dx;
            p._y += dy;
            p._base += dy * view.stride() + dx * view.pixelStride();
        }

    public:
        /** @brief Get pixel color.
        */
        template <typename P>
        Color getColor(const View& view, const P& p) const
        {
            return Argb32::getColor( p.base() );
        }

        /** @brief Assign pixels.
        */
        template <typename P>
        void sourceCopy(View& view, Pixel& to, 
                        const View& from, const P& p) const
        {
            Argb32::sourceCopy( to.base(), p.base() );
        }
        
        template <typename P>
        void sourceOver(View& view, Pixel& to, 
                        const View& from, const P& p) const
        {
            Argb32::sourceOver(to.base(), p.base());
        }
        
        /** @brief Assign pixels.
        */
        void sourceCopy(View& view, Pixel& to, const Color& c) const
        {
            Argb32::sourceCopy(to.base(), c);
        }
        
        void sourceOver(View& view, Pixel& to, const Color& c) const
        {
            Argb32::sourceOver(to.base(), c);
        }
 
        /** @brief Fill pixels.
        */
        void sourceCopy(View& view, Pixel& to, std::size_t n, const Color& c) const
        {
            Argb32::sourceCopy(to.base(), n, c);
        }

        void sourceOver(View& view, Pixel& to, std::size_t n, const Color& c) const
        {
            Argb32::sourceOver(to.base(), n, c);
        }
    
        /** @brief Fill pixels.
        */
        void sourceCopy(View& view, Pixel& to, std::size_t n, 
                        const View& from, const ConstPixel& p) const
        {
            Argb32::sourceCopy(to.base(), n, p.base());
        }


        void sourceOver(View& view, Pixel& to, std::size_t n, 
                        const View& from, const ConstPixel& p) const
        {
            Argb32::sourceOver(to.base(), n, p.base());
        }

        /** @brief Copy pixels.
        */
        void sourceCopy(View& view, Pixel& to, 
                        const View& from, const ConstPixel& p, std::size_t n) const
        { 
            Argb32::sourceCopy(to.base(), p.base(), n);
        }

        void sourceOver(View& view, Pixel& to, 
                        const View& from, const ConstPixel& p, std::size_t n) const
        { 
            Argb32::sourceOver(to.base(), p.base(), n);
        }

        void sourceCopy(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                        const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                        Pt::ssize_t width, Pt::ssize_t height)
        {
            Argb32::sourceCopy(to, toX, toY, from, fromX, fromY, width, height);
        }


        void sourceOver(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                        const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                        Pt::ssize_t width, Pt::ssize_t height)
        {
            Argb32::sourceOver(to, toX, toY, from, fromX, fromY, width, height);
        }

    public:
        static Color getColor(const Pt::uint8_t* p);

        //
        // SourceCopy
        //
        static void sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from);

        static void sourceCopy(Pt::uint8_t* to, const Color& c);

        static void sourceCopy(Pt::uint8_t* to, size_t length, const Color& c);

        static void sourceCopy(Pt::uint8_t* to, size_t length, const Pt::uint8_t* from);

        static void sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from, size_t length);

        static void sourceCopy(ViewBase& to, Pt::ssize_t toX, Pt::ssize_t toY,
                               const ViewBase& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                               Pt::ssize_t width, Pt::ssize_t height);

        //
        // SourceOver
        //
        static void sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from);
        
        static void sourceOver(Pt::uint8_t* to, const Pt::Gfx::Color& from);

        static void sourceOver(Pt::uint8_t* to, size_t length, const Color& c);

        static void sourceOver(Pt::uint8_t* to, size_t length, const Pt::uint8_t* from);

        static void sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from, size_t length);

        static void sourceOver(ViewBase& to, Pt::ssize_t toX, Pt::ssize_t toY,
                               const ViewBase& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                               Pt::ssize_t width, Pt::ssize_t height);

    public:
        static void blendSourceCopy(Pt::uint8_t* to, const Color& c, Pt::uint8_t alpha)
        {
            const Pt::uint32_t blendAlphaSrc = alpha;
            const Pt::uint32_t blendAlphaInv = 255 - alpha;
            to[0] = (blendAlphaSrc * (c.blue () >> 8) + blendAlphaInv * to[0]) >> 8;
            to[1] = (blendAlphaSrc * (c.green() >> 8) + blendAlphaInv * to[1]) >> 8;
            to[2] = (blendAlphaSrc * (c.red  () >> 8) + blendAlphaInv * to[2]) >> 8;
            to[3] = (blendAlphaSrc * (c.alpha() >> 8) + blendAlphaInv * to[3]) >> 8;
        }
        
        static void blendSourceOver(Pt::uint8_t* to, const Color& c, Pt::uint8_t alpha)
        {
            const Pt::uint32_t colorAlpha    = c.alpha() >> 8;
            const Pt::uint32_t blendAlphaSrc = colorAlpha * alpha / 255;
            const Pt::uint32_t blendAlphaInv = 255 - blendAlphaSrc;
            to[0] = (blendAlphaSrc * (c.blue () >> 8) + blendAlphaInv * to[0]) >> 8;
            to[1] = (blendAlphaSrc * (c.green() >> 8) + blendAlphaInv * to[1]) >> 8;
            to[2] = (blendAlphaSrc * (c.red  () >> 8) + blendAlphaInv * to[2]) >> 8;
            to[3] = (blendAlphaSrc *  colorAlpha      + blendAlphaInv * to[3]) >> 8;
        }

        static void blendSourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from, Pt::uint8_t alpha)
        {
            const Pt::uint32_t blendAlphaSrc = alpha;
            const Pt::uint32_t blendAlphaInv = 255 - alpha;
            to[0] = (blendAlphaSrc * from[0] + blendAlphaInv * to[0]) >> 8;
            to[1] = (blendAlphaSrc * from[1] + blendAlphaInv * to[1]) >> 8;
            to[2] = (blendAlphaSrc * from[2] + blendAlphaInv * to[2]) >> 8;
            to[3] = (blendAlphaSrc * from[3] + blendAlphaInv * to[3]) >> 8;
        }

        static void blendSourceOver(Pt::uint8_t* to, const Pt::uint8_t* from, Pt::uint8_t alpha)
        {
            const Pt::uint32_t colorAlpha    = from[3];
            const Pt::uint32_t blendAlphaSrc = colorAlpha * alpha / 255;
            const Pt::uint32_t blendAlphaInv = 255 - blendAlphaSrc;
            to[0] = (blendAlphaSrc * from[0]    + blendAlphaInv * to[0]) >> 8;
            to[1] = (blendAlphaSrc * from[1]    + blendAlphaInv * to[1]) >> 8;
            to[2] = (blendAlphaSrc * from[2]    + blendAlphaInv * to[2]) >> 8;
            to[3] = (blendAlphaSrc * colorAlpha + blendAlphaInv * to[3]) >> 8;
        }
};

///////////////////////////////////////////////////////////////////////
// Argb32Pixel
///////////////////////////////////////////////////////////////////////

inline Argb32Pixel::Argb32Pixel(View& view, Pt::ssize_t x, Pt::ssize_t y)
: _base(0)
, _x(x)
, _y(y)
{
    _base = view.data() + view.stride() * y + x * view.pixelStride();
}


inline bool Argb32Pixel::operator==(const Argb32Pixel& p) const
{ 
    return _base == p._base; 
}


inline bool Argb32Pixel::operator==(const Argb32ConstPixel& p) const
{ 
    return _base == p._base; 
}

///////////////////////////////////////////////////////////////////////
// Argb32ConstPixel
///////////////////////////////////////////////////////////////////////

inline Argb32ConstPixel::Argb32ConstPixel(const View& view, Pt::ssize_t x, Pt::ssize_t y)
: _base(0)
, _x(x)
, _y(y)
{
    _base = view.data() + view.stride() * y + x * view.pixelStride();
}

///////////////////////////////////////////////////////////////////////
// Argb32
///////////////////////////////////////////////////////////////////////

inline Color Argb32::getColor(const Pt::uint8_t* p)
{
    const Pt::uint32_t pixel = *reinterpret_cast<const Pt::uint32_t*>(p);

    const Pt::uint16_t ta =  pixel               >> 24;
    const Pt::uint16_t tr = (pixel & 0x00FF0000) >> 16;
    const Pt::uint16_t tg = (pixel & 0x0000FF00) >>  8;
    const Pt::uint16_t tb =  pixel & 0x000000FF;

    Pt::uint16_t a = (ta << 8) + ta;
    Pt::uint16_t r = (tr << 8) + tr;
    Pt::uint16_t g = (tg << 8) + tg;
    Pt::uint16_t b = (tb << 8) + tb;

    return Color(a, r, g, b);
}

//
// Implementation SourceCopy
//

inline void Argb32::sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from)
{
    *((Pt::uint32_t*) to) = *((const Pt::uint32_t*) from);
}


inline void Argb32::sourceCopy(Pt::uint8_t* to, const Color& c)
{
    Pt::uint32_t* pixel = reinterpret_cast<Pt::uint32_t*>(to);

    *pixel = ( Pt::uint32_t(c.alpha() & 0xFF00) << 16 ) |
             ( Pt::uint32_t(c.red  () & 0xFF00) <<  8 ) |
               Pt::uint32_t(c.green() & 0xFF00)         |
             ( Pt::uint32_t(c.blue ()         ) >>  8 );
}


inline void Argb32::sourceCopy(Pt::uint8_t* to, size_t length, const Color& c)
{
    const Pt::uint32_t fromARGB = ( Pt::uint32_t(c.alpha() & 0xFF00) << 16 ) |
                                  ( Pt::uint32_t(c.red  () & 0xFF00) <<  8 ) |
                                  ( Pt::uint32_t(c.green() & 0xFF00)       ) |
                                  ( Pt::uint32_t(c.blue ()         ) >>  8 );
            
    //Argb32Ops::pixelOps_SourceCopy(to, fromARGB, length);

    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(to);
    for(size_t i = 0; i < length; ++i) 
        *dst++ = fromARGB;
}


inline void Argb32::sourceCopy(Pt::uint8_t* to, size_t length, const Pt::uint8_t* from)
{
    //Argb32Ops::pixelOps_SourceCopy(to, from, length);

    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(to);
    const Pt::uint32_t fromARGB = *reinterpret_cast<const Pt::uint32_t*>(from);
    for(size_t i = 0; i < length; ++i) 
        *dst++ = fromARGB;
}


inline void Argb32::sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from, size_t length)
{
    memcpy(to, from, length * 4);
}


inline void Argb32::sourceCopy(ViewBase& toView, Pt::ssize_t toX, Pt::ssize_t toY,
                               const ViewBase& fromView, Pt::ssize_t fromX, Pt::ssize_t fromY,
                               Pt::ssize_t width, Pt::ssize_t height)
{
    Pt::ssize_t bytesPerPixel = Argb32::pixelStride();
    Pt::ssize_t n = width * bytesPerPixel;

    Pt::uint8_t* to = toView.data() + (toY * toView.stride()) 
                                    + (toX * bytesPerPixel);
    
    const Pt::uint8_t* from = fromView.data() + (fromY * fromView.stride()) 
                                              + (fromX * bytesPerPixel);

    for(Pt::ssize_t y = 0; y < height; ++y)
    {
        memcpy(to, from, n);

        to += toView.stride();
        from += fromView.stride();
    }
}

//
// Implementation SourceOver
//

inline void Argb32::sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from)
{
    const Pt::uint32_t alphaSrc = from[3];
    const Pt::uint32_t alphaInv = 255 - alphaSrc;

    to[0] = (Pt::uint8_t) ( (alphaSrc * from[0]  + alphaInv * to[0]) >> 8 );
    to[1] = (Pt::uint8_t) ( (alphaSrc * from[1]  + alphaInv * to[1]) >> 8 );
    to[2] = (Pt::uint8_t) ( (alphaSrc * from[2]  + alphaInv * to[2]) >> 8 );
    to[3] = (Pt::uint8_t) ( (alphaSrc * alphaSrc + alphaInv * to[3]) >> 8 );
}


inline void Argb32::sourceOver(Pt::uint8_t* to, const Pt::Gfx::Color& from)
{
    const Pt::uint32_t alpha    = from.alpha() >> 8;
    const Pt::uint32_t alphaSrc = alpha;
    const Pt::uint32_t alphaInv = 255 - alpha;

    to[0] = (Pt::uint8_t) ( (alphaSrc * (from.blue () >> 8) + alphaInv * to[0]) >> 8 );
    to[1] = (Pt::uint8_t) ( (alphaSrc * (from.green() >> 8) + alphaInv * to[1]) >> 8 );
    to[2] = (Pt::uint8_t) ( (alphaSrc * (from.red  () >> 8) + alphaInv * to[2]) >> 8 );
    to[3] = (Pt::uint8_t) ( (alphaSrc *  alpha              + alphaInv * to[3]) >> 8 );
}


inline void Argb32::sourceOver(Pt::uint8_t* to, size_t length, const Color& c)
{
    const Pt::uint32_t blend = c.alpha() >> 8;
    const Pt::uint32_t bfcI  = 255 - blend;
    const Pt::uint32_t srcR  = (Pt::uint32_t) (c.red  () >> 8) * blend;
    const Pt::uint32_t srcG  = (Pt::uint32_t) (c.green() >> 8) * blend;
    const Pt::uint32_t srcB  = (Pt::uint32_t) (c.blue () >> 8) * blend;
    const Pt::uint32_t srcA  = blend * blend;
            
    //Argb32Ops::pixelOps_SourceOver(to, srcA, srcR, srcG, srcB, blendInv, length);

    Pt::uint8_t* dst = to;

    for(size_t i = 0; i < length; ++i) {
        dst[0] = (srcB + bfcI * dst[0]) >> 8;
        dst[1] = (srcG + bfcI * dst[1]) >> 8;
        dst[2] = (srcR + bfcI * dst[2]) >> 8;
        dst[3] = (srcA + bfcI * dst[3]) >> 8;
        dst += 4;
    }
}


inline void Argb32::sourceOver(Pt::uint8_t* to, size_t length, const Pt::uint8_t* from)
{
    //Argb32Ops::pixelOps_SourceOver( to, from, length);

    const Pt::uint32_t blend    = from[3];
    const Pt::uint32_t blendInv = 255 - blend;
    const Pt::uint32_t srcR     = from[2] * blend;
    const Pt::uint32_t srcG     = from[1] * blend;
    const Pt::uint32_t srcB     = from[0] * blend;
    const Pt::uint32_t srcA     = blend   * blend;
            
    //Argb32::pixelOps_SourceOver(to, srcA, srcR, srcG, srcB, blendInv, length);

    Pt::uint8_t* dst = to;
    Pt::uint32_t bfcI = blendInv;

    for(size_t i = 0; i < length; ++i) {
        dst[0] = (srcB + bfcI * dst[0]) >> 8;
        dst[1] = (srcG + bfcI * dst[1]) >> 8;
        dst[2] = (srcR + bfcI * dst[2]) >> 8;
        dst[3] = (srcA + bfcI * dst[3]) >> 8;
        dst += 4;
    }
}


inline void Argb32::sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from, size_t length)
{
    const Pt::uint8_t* src = from;
          Pt::uint8_t* dst = to;

    for(size_t i = 0; i < length; ++i) 
    {
        const Pt::uint32_t alphaSrc = src[3];
        const Pt::uint32_t alphaInv = 255 - alphaSrc;
        dst[0] = (Pt::uint8_t) ( (alphaSrc * src[0]   + alphaInv * dst[0]) >> 8 );
        dst[1] = (Pt::uint8_t) ( (alphaSrc * src[1]   + alphaInv * dst[1]) >> 8 );
        dst[2] = (Pt::uint8_t) ( (alphaSrc * src[2]   + alphaInv * dst[2]) >> 8 );
        dst[3] = (Pt::uint8_t) ( (alphaSrc * alphaSrc + alphaInv * dst[3]) >> 8 );
        src += 4;
        dst += 4;
    }
}


inline void Argb32::sourceOver(ViewBase& toView, Pt::ssize_t toX, Pt::ssize_t toY,
                               const ViewBase& fromView, Pt::ssize_t fromX, Pt::ssize_t fromY,
                               Pt::ssize_t width, Pt::ssize_t height)
{
    Pt::ssize_t bytesPerPixel = Argb32::pixelStride();

    Pt::uint8_t* to = toView.data() + (toY * toView.stride()) 
                                    + (toX * bytesPerPixel);
    
    const Pt::uint8_t* from = fromView.data() + (fromY * fromView.stride()) 
                                              + (fromX * bytesPerPixel);

    for(int y = 0; y < height; ++y)
    {
        Pt::uint8_t* toLine = to;
        const Pt::uint8_t* fromLine = from;
    
        for(int x = 0; x < width ; ++x)
        {
            Argb32::sourceOver(toLine, fromLine);
            toLine += 4;
            fromLine += 4;
        }

        to += toView.stride();
        from += fromView.stride();
    }
}
} // namespace

} // namespace

#endif
