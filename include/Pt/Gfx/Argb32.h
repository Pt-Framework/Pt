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
#include <Pt/Gfx/BasicView.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Location.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

class Argb32PixelBase;
class Argb32ConstPixelBase;

class Argb32;
class Argb32Pixel;
class Argb32ConstPixel;

/** @brief ARGB-32 pixel.
*/
class Argb32Pixel
{
    friend class Argb32Format;
    friend class Argb32ConstPixel;
    friend class Argb32;

    public:
        typedef Argb32Color ColorType;

    protected:
        Argb32Pixel(Pt::uint8_t* data, ViewBase& view, Pt::ssize_t x, Pt::ssize_t y);

    public:
        Argb32Pixel(BasicView<Argb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        Argb32Pixel(const Argb32Pixel& p) = default;

        void reset(const Argb32Pixel& p);

        ViewBase& view()
        { return *_view; }

        const ViewBase& view() const
        { return *_view; }

        Location& location()
        { return _loc; }

        const Location& location() const
        { return _loc; }

        Pt::ssize_t xpos() const
        { return _loc.xpos(); }

        Pt::ssize_t ypos() const
        { return _loc.ypos(); }

        Pt::uint8_t* base()
        { return _loc.base(); }

        const Pt::uint8_t* base() const
        { return _loc.base(); }

        Pt::uint8_t alpha() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
            return *val >> 24;
        }

        Pt::uint8_t red() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
            return (*val & 0x00FF0000) >> 16;
        }

        Pt::uint8_t green() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
            return (*val & 0x0000FF00) >> 8;
        }

        Pt::uint8_t blue() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
            return *val & 0x000000FF;
        }

        void setAlpha(Pt::uint8_t a)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
            *val = (*val & 0x00FFFFFF) | (uint32_t(a) << 24);
        }

        void setRed(Pt::uint8_t r)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
            *val = (*val & 0xFF00FFFF) | (uint32_t(r) << 16);
        }

        void setGreen(Pt::uint8_t g)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
            *val = (*val & 0xFFFF00FF) | (uint32_t(g) << 8);
        }

        void setBlue(Pt::uint8_t b)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
            *val = (*val & 0xFFFFFF00) | uint32_t(b);
        }

        Argb32Color color() const;

        Argb32Pixel& operator=(const Gfx::Color& color);

        Argb32Pixel& operator=(const Argb32Color& color);

        Argb32Pixel& operator=(const Argb32Pixel& p);

        Argb32Pixel& operator=(const Argb32ConstPixel& p);

        Argb32Pixel& operator=(const Pixel& p);

        Argb32Pixel& operator=(const ConstPixel& p);

        void copy(const Argb32Pixel& p, std::size_t length);

        bool equals(const Argb32Pixel& p) const;

        bool equals(const Argb32ConstPixel& p) const;

        void advance();

        void advance(Pt::ssize_t n);

    private:
        ViewBase*    _view;
        Location     _loc;
};

/** @brief ARGB-32 const pixel.
*/
class Argb32ConstPixel
{
    friend class Argb32Format;
    friend class Argb32Pixel;
    friend class Argb32;

    public:
        typedef Argb32Color ColorType;

    protected:
        Argb32ConstPixel(const Pt::uint8_t* data, const ViewBase& view, Pt::ssize_t x, Pt::ssize_t y)
        : _view(view)
        , _loc()
        {
            const Pt::uint8_t* _p = data;
            _p += y * _view.stride();
            _p += x * 4; 

            _loc = ConstLocation(_p, x, y);
        }

    public:
        Argb32ConstPixel(BasicConstView<Argb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        Argb32ConstPixel(const BasicView<Argb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        Argb32ConstPixel(const Argb32ConstPixel& p) = default;

        explicit Argb32ConstPixel(const Argb32Pixel& p)
        : _view(*p._view)
        , _loc( p.location() )
        { }

        const ViewBase& view() const
        { return _view; }

        const ConstLocation& location() const
        { return _loc; }

        Pt::ssize_t xpos() const
        { return _loc.xpos(); }

        Pt::ssize_t ypos() const
        { return _loc.ypos(); }

        const Pt::uint8_t* base() const
        { return _loc.base(); }

        Pt::uint8_t alpha() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
            return *val >> 24;
        }

        Pt::uint8_t red() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
            return (*val & 0x00FF0000) >> 16;
        }

        Pt::uint8_t green() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
            return (*val & 0x0000FF00) >> 8;
        }

        Pt::uint8_t blue() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
            return *val & 0x000000FF;
        }

        Argb32Color color() const;

        bool equals(const Argb32ConstPixel& p) const;
        
        bool equals(const Argb32Pixel& p) const;

        void advance();

        void advance(Pt::ssize_t n);

    private:
        const ViewBase&   _view;
        ConstLocation     _loc;
};

/** @brief ARGB-32 image format.
*/
class PT_GFX_API Argb32 final : public ImageFormat
{
    static const unsigned PixelWidth = 4;

    public:    
        typedef Argb32Pixel Pixel;
        typedef Argb32ConstPixel ConstPixel;

    public:
        Argb32()
        : ImageFormat(PixelWidth)
        { }

        /** @brief Returns the distance between two pixel base pointers in bytes.
        */
        std::size_t pixelStride() const
        {
            return PixelWidth;
        }

        std::size_t imageSize(std::size_t width, std::size_t height,
                              std::size_t padding) const
        {
            std::size_t stride = (width * 4) + padding;
            std::size_t n = stride * height;
            return n;
        }

    protected:
        virtual ImageFormat* onClone() const override
        {          
            return new Argb32();
        }

        virtual void onRelease() const override
        {
            delete this;
        }

        virtual const std::type_info& onGetType() const override
        { 
            return typeid(*this); 
        }

        virtual std::size_t onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                        std::size_t padding) const override;

        virtual PixelBase* onCreatePixel(Pt::uint8_t* data, ViewBase& view, 
                                         Pt::ssize_t x, Pt::ssize_t y, 
                                         PixelStorage& store) const override;
    
        virtual ConstPixelBase* onCreateConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                                                    Pt::ssize_t x, Pt::ssize_t y, 
                                                    PixelStorage& store) const override;
    
    public:
        static void sourceCopy(Argb32Pixel& p, const Color& c)
        {
            sourceCopy(p.base(), c);
        }

        static void sourceOver(Argb32Pixel& p, const Color& c)
        {
            sourceOver(p.base(), c);
        }

    public:
        static void advance(const ViewBase& view, Pt::uint8_t*& p, 
                            Pt::ssize_t& xpos, Pt::ssize_t& ypos)
        {
            if( ++xpos >= view.width() )
            {
                xpos = 0;
                ++ypos;

                p += view.padding();
            }

            p += 4;
        }

        static void advance(const ViewBase& view, Pt::uint8_t*& p, Pt::ssize_t n,
                            Pt::ssize_t& xpos, Pt::ssize_t& ypos)
        {
            Pt::ssize_t off = xpos + n;

            std::size_t dy = off / view.width();
            std::size_t dx = off % view.width() - xpos;

            xpos += dx;
            ypos += dy;
            p += dy * view.stride() + dx * 4;
        }

        static Color getColor(const Pt::uint8_t* p);

        static Argb32Color getArgb32Color(const Pt::uint8_t* p);

        static void getColors(const Pt::uint8_t* p, Color* colors, std::size_t n);

        static void getColors(const Pt::uint8_t* p, Argb32Color* colors, std::size_t n);

        //
        // SourceCopy
        //
        static void sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from);

        static void sourceCopy(Pt::uint8_t* to, const Color& c);

        static void sourceCopy(Pt::uint8_t* to, std::size_t length, const Color& c);

        static void sourceCopy(Pt::uint8_t* to, std::size_t length, const Pt::uint8_t* from);

        static void sourceCopy(Pt::uint8_t* to, const Color* colors, std::size_t length);

        static void sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length);

        static void sourceCopy(Pt::uint8_t* to, Pt::ssize_t toStride,
                               const Pt::uint8_t* from, Pt::ssize_t fromStride,
                               Pt::ssize_t width, Pt::ssize_t height);

        //
        // SourceOver
        //
        static void sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from);
        
        static void sourceOver(Pt::uint8_t* to, const Pt::Gfx::Color& from);

        static void sourceOver(Pt::uint8_t* to, std::size_t length, const Color& c);

        static void sourceOver(Pt::uint8_t* to, std::size_t length, const Pt::uint8_t* from);

        static void sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length);

        static void sourceOver(Pt::uint8_t* to, Pt::ssize_t toStride,
                               const Pt::uint8_t* from, Pt::ssize_t fromStride,
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


inline Argb32Color Argb32::getArgb32Color(const Pt::uint8_t* p)
{
    return Argb32Color(p);
}


inline void Argb32::getColors(const Pt::uint8_t* p, Color* colors, std::size_t n)
{
    for(int i = 0; i < n; ++i)
    {
        colors[i] = getColor(p);
        p += PixelWidth;
    }
}


inline void Argb32::getColors(const Pt::uint8_t* p, Argb32Color* colors, std::size_t n)
{
    std::memcpy(colors, p, n * PixelWidth);
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


inline void Argb32::sourceCopy(Pt::uint8_t* to, std::size_t length, const Color& c)
{
    const Pt::uint32_t value = ( Pt::uint32_t(c.alpha() & 0xFF00) << 16 ) |
                               ( Pt::uint32_t(c.red  () & 0xFF00) <<  8 ) |
                               ( Pt::uint32_t(c.green() & 0xFF00)       ) |
                               ( Pt::uint32_t(c.blue ()         ) >>  8 );
            
    //Argb32Ops::pixelOps_SourceCopy(to, fromARGB, length);

    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(to);
    for(std::size_t i = 0; i < length; ++i) 
        *dst++ = value;
}


inline void Argb32::sourceCopy(Pt::uint8_t* to, std::size_t length, const Pt::uint8_t* from)
{
    //Argb32Ops::pixelOps_SourceCopy(to, from, length);

    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(to);
    const Pt::uint32_t fromARGB = *reinterpret_cast<const Pt::uint32_t*>(from);
    for(std::size_t i = 0; i < length; ++i) 
        *dst++ = fromARGB;
}


inline void Argb32::sourceCopy(Pt::uint8_t* to, const Color* colors, std::size_t length)
{          
    for(int n = 0; n < length; ++n)
    {
        Argb32::sourceCopy( to, 1, colors[n] );
        to += Argb32::PixelWidth;
    }
}


inline void Argb32::sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length)
{
    memcpy(to, from, length * 4);
}


inline void Argb32::sourceCopy(Pt::uint8_t* to, Pt::ssize_t toStride,
                               const Pt::uint8_t* from, Pt::ssize_t fromStride,
                               Pt::ssize_t width, Pt::ssize_t height)
{
    for(Pt::ssize_t y = 0; y < height; ++y)
    {
        sourceCopy(to, from, width);

        to += toStride;
        from += fromStride; 
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


inline void Argb32::sourceOver(Pt::uint8_t* to, std::size_t length, const Color& c)
{
    const Pt::uint32_t blend = c.alpha() >> 8;
    const Pt::uint32_t bfcI  = 255 - blend;
    const Pt::uint32_t srcR  = (Pt::uint32_t) (c.red  () >> 8) * blend;
    const Pt::uint32_t srcG  = (Pt::uint32_t) (c.green() >> 8) * blend;
    const Pt::uint32_t srcB  = (Pt::uint32_t) (c.blue () >> 8) * blend;
    const Pt::uint32_t srcA  = blend * blend;
            
    //Argb32Ops::pixelOps_SourceOver(to, srcA, srcR, srcG, srcB, blendInv, length);

    Pt::uint8_t* dst = to;

    for(std::size_t i = 0; i < length; ++i) {
        dst[0] = (srcB + bfcI * dst[0]) >> 8;
        dst[1] = (srcG + bfcI * dst[1]) >> 8;
        dst[2] = (srcR + bfcI * dst[2]) >> 8;
        dst[3] = (srcA + bfcI * dst[3]) >> 8;
        dst += 4;
    }
}


inline void Argb32::sourceOver(Pt::uint8_t* to, std::size_t length, const Pt::uint8_t* from)
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

    for(std::size_t i = 0; i < length; ++i) {
        dst[0] = (srcB + bfcI * dst[0]) >> 8;
        dst[1] = (srcG + bfcI * dst[1]) >> 8;
        dst[2] = (srcR + bfcI * dst[2]) >> 8;
        dst[3] = (srcA + bfcI * dst[3]) >> 8;
        dst += 4;
    }
}


inline void Argb32::sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length)
{
    const Pt::uint8_t* src = from;
          Pt::uint8_t* dst = to;

    for(std::size_t i = 0; i < length; ++i) 
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


inline void Argb32::sourceOver(Pt::uint8_t* to, Pt::ssize_t toStride,
                               const Pt::uint8_t* from, Pt::ssize_t fromStride,
                               Pt::ssize_t width, Pt::ssize_t height)
{
    for(Pt::ssize_t y = 0; y < height; ++y)
    {
        sourceOver(to, from, width);

        to += toStride;
        from += fromStride; 
    }
}

///////////////////////////////////////////////////////////////////////
// Argb32Pixel
///////////////////////////////////////////////////////////////////////

inline Argb32Pixel::Argb32Pixel(BasicView<Argb32>& view, 
                                Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _loc()
{
    Pt::uint8_t* _p = view.data();
    _p += y * _view->stride();
    _p += x * 4; 
    
    _loc = Location(_p, x, y);
}


inline Argb32Pixel::Argb32Pixel(Pt::uint8_t* data, ViewBase& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _loc()
{
    Pt::uint8_t* _p = data;
    _p += y * _view->stride();
    _p += x * 4; 
    
    _loc = Location(_p, x, y);
}


inline void Argb32Pixel::reset(const Argb32Pixel& p)
{
    _view = p._view;
    _loc = p._loc;
}


inline void Argb32Pixel::advance()
{
    Pt::ssize_t _x = _loc.xpos();
    Pt::ssize_t _y = _loc.ypos();
    Pt::uint8_t* _p = _loc.base();

    if( ++_x >= _view->width() )
    {
        _x = 0;
        ++_y;

        _p += _view->padding();
    }

    _p += 4;

    _loc.setXPos(_x);
    _loc.setYPos(_y);
    _loc.setBase(_p);
}


inline void Argb32Pixel::advance(Pt::ssize_t n)
{
    Pt::ssize_t _x = _loc.xpos();
    Pt::ssize_t _y = _loc.ypos();
    Pt::uint8_t* _p = _loc.base();

    Pt::ssize_t off = _x + n;

    std::size_t dy = off / _view->width();
    std::size_t dx = off % _view->width() - _x;

    _loc.setXPos(_x + dx);
    _loc.setYPos(_y + dy);
    _loc.setBase(_p + dy * _view->stride() + dx * 4);
}


inline bool Argb32Pixel::equals(const Argb32Pixel& p) const
{
    return base() == p.base();
}


inline bool Argb32Pixel::equals(const Argb32ConstPixel& p) const
{
    return base() == p.base();
}


inline void Argb32Pixel::copy(const Argb32Pixel& p, std::size_t length)
{
    Argb32::sourceCopy(base(), p.base(), length);
}


inline Argb32Pixel& Argb32Pixel::operator=(const Gfx::Color& color)
{ 
    Argb32::sourceCopy(base(), color);
    return *this;
}


inline Argb32Pixel& Argb32Pixel::operator=(const Argb32Color& color)
{ 
    const Pt::uint8_t* p = reinterpret_cast<const Pt::uint8_t*>( color.value() );
    Argb32::sourceCopy(base(), p);
    return *this;
}


inline Argb32Pixel& Argb32Pixel::operator=(const Argb32Pixel& p)
{
    Argb32::sourceCopy( base(), p.base() );
    return *this;
}


inline Argb32Pixel& Argb32Pixel::operator=(const Argb32ConstPixel& p)
{
    Argb32::sourceCopy( base(), p.base() );
    return *this;
}


inline Argb32Pixel& Argb32Pixel::operator=(const Pixel& p)
{
    return *this = p.toArgb32Color();
}


inline Argb32Pixel& Argb32Pixel::operator=(const ConstPixel& p)
{
    return *this = p.toArgb32Color();
}


inline Argb32Color Argb32Pixel::color() const
{
    return Argb32Color(*base());
}


inline Argb32Color Argb32ConstPixel::color() const
{
    return Argb32Color(*base());
}

///////////////////////////////////////////////////////////////////////
// Argb32ConstPixel
///////////////////////////////////////////////////////////////////////

inline Argb32ConstPixel::Argb32ConstPixel(BasicConstView<Argb32>& view,
                                          Pt::ssize_t x, Pt::ssize_t y)
: _view(view)
, _loc()
{
    const Pt::uint8_t* _p = view.data();
    _p += y * _view.stride();
    _p += x * 4; 
    
    _loc = ConstLocation(_p, x, y);
}


inline Argb32ConstPixel::Argb32ConstPixel(const BasicView<Argb32>& view,
                                          Pt::ssize_t x, Pt::ssize_t y)
: _view(view)
, _loc()
{
    const Pt::uint8_t* _p = view.data();
    _p += y * _view.stride();
    _p += x * 4; 
    
    _loc = ConstLocation(_p, x, y);
}


inline void Argb32ConstPixel::advance()
{
    Pt::ssize_t _x = _loc.xpos();
    Pt::ssize_t _y = _loc.ypos();
    const Pt::uint8_t* _p = _loc.base();

    if( ++_x >= _view.width() )
    {
        _x = 0;
        ++_y;

        _p += _view.padding();
    }

    _p += 4;

    _loc.setXPos(_x);
    _loc.setYPos(_y);
    _loc.setBase(_p);
}


inline void Argb32ConstPixel::advance(Pt::ssize_t n)
{
    Pt::ssize_t _x = _loc.xpos();
    Pt::ssize_t _y = _loc.ypos();
    const Pt::uint8_t* _p = _loc.base();

    Pt::ssize_t off = _x + n;

    std::size_t dy = off / _view.width();
    std::size_t dx = off % _view.width() - _x;

    _loc.setXPos(_x + dx);
    _loc.setYPos(_y + dy);
    _loc.setBase(_p + dy * _view.stride() + dx * 4);
}


inline bool Argb32ConstPixel::equals(const Argb32ConstPixel& p) const
{
    return base() == p.base();
}


inline bool Argb32ConstPixel::equals(const Argb32Pixel& p) const
{
    return base() == p.base();
}

} // namespace

} // namespace

#endif
