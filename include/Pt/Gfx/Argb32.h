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
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Types.h>
#include <cstring>

namespace Pt {

namespace Gfx {

/** @brief ARGB-32 pixel.
*/
class Argb32Pixel
{
    friend class Argb32ConstPixel;

    public:
        typedef Argb32 FormatType;
        typedef Argb32Color ColorType;

    public:
        Argb32Pixel(BasicView<Argb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        Argb32Pixel(const Argb32Pixel& p) = default;

        ~Argb32Pixel() = default;

        Argb32Pixel& operator=(const Argb32Color& color);

        Argb32Pixel& operator=(const Gfx::ColorF& color);

        void reset(BasicView<Argb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const Argb32Pixel& p);

        const ViewBase& view() const
        { return *_view; }

        Pt::uint8_t* base()
        { return _base; }

        const Pt::uint8_t* base() const
        { return _base; }

        Pt::uint8_t alpha() const;

        void setAlpha(Pt::uint8_t a);

        Pt::uint8_t red() const;

        void setRed(Pt::uint8_t r);

        Pt::uint8_t green() const;

        void setGreen(Pt::uint8_t g);

        Pt::uint8_t blue() const;

        void setBlue(Pt::uint8_t b);

        Argb32Color toColor() const;

        void advance();

        void advanceLine();

        void advance(Pt::ssize_t n);

        void advanceLines(Pt::ssize_t n);

        void assign(const Argb32Pixel& p);

        void assign(const Argb32ConstPixel& p);

        void assign(const Argb32Pixel& p, std::size_t length);

        void assign(const Argb32ConstPixel& p, std::size_t length);

        void assign(const Argb32Color* colors, std::size_t length);

        void fill(std::size_t n, const Argb32Color& color);

        bool equals(const Argb32Pixel& p) const;

        bool equals(const Argb32ConstPixel& p) const;

    private:
        const ViewBase* _view;
        Pt::uint8_t*    _base;
};

/** @brief ARGB-32 const pixel.
*/
class Argb32ConstPixel
{
    friend class Argb32Pixel;

    public:
        typedef Argb32 FormatType;
        typedef Argb32Color ColorType;

    protected:
        Argb32ConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                         Pt::ssize_t x, Pt::ssize_t y);

    public:
        Argb32ConstPixel(const BasicConstView<Argb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        Argb32ConstPixel(const BasicView<Argb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        Argb32ConstPixel(const Argb32ConstPixel& p) = default;

        Argb32ConstPixel(const Argb32Pixel& p);

        ~Argb32ConstPixel() = default;

        void reset(const BasicConstView<Argb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const BasicView<Argb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const Argb32ConstPixel& p);

        void reset(const Argb32Pixel& p);

        const ViewBase& view() const
        { return *_view; }

        const Pt::uint8_t* base() const
        { return _base; }

        Pt::uint8_t alpha() const;

        Pt::uint8_t red() const;

        Pt::uint8_t green() const;

        Pt::uint8_t blue() const;

        Argb32Color toColor() const;

        void advance();

        void advanceLine();

        void advance(Pt::ssize_t n);

        void advanceLines(Pt::ssize_t n);

        void getColors(Argb32Color* colors, std::size_t length) const;

        bool equals(const Argb32ConstPixel& p) const;
        
        bool equals(const Argb32Pixel& p) const;

    private:
        const ViewBase*    _view;
        const Pt::uint8_t* _base;
};

/** @brief ARGB-32 image format.
*/
class PT_GFX_API Argb32 final : public ImageFormat
{
    static const unsigned PixelWidth = 4;

    public:    
        typedef Argb32Pixel PixelType;
        typedef Argb32ConstPixel ConstPixelType;
        typedef Argb32Color ColorType;

    public:
        static const Argb32& get()
        {
            static Argb32 _argb32;
            return _argb32;
        }

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
        virtual std::unique_ptr<ImageFormat> onClone() const override
        {          
            return std::unique_ptr<ImageFormat>(new Argb32);
        }

        virtual const std::type_info& onGetType() const override
        { 
            return typeid(*this); 
        }

        virtual std::size_t onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                        std::size_t padding) const override;

        virtual PixelBase* onCreatePixel(Pt::uint8_t* data, const ViewBase& view, 
                                         Pt::ssize_t x, Pt::ssize_t y, 
                                         PixelStorage& store) const override;

    public:
        template <typename BasePtr>
        static BasePtr getPixel(const ViewBase& view, BasePtr base, 
                                Pt::ssize_t xpos, Pt::ssize_t ypos)
        {
            ypos += view.ypos();
            xpos += view.xpos();

            base += ypos * view.stride();
            base += xpos * PixelWidth; 
            return base;
        }

        template <typename BasePtr>
        static BasePtr advance(const ViewBase& view, BasePtr base)
        {
            return base + PixelWidth;
        }

        template <typename BasePtr>
        static BasePtr advance(const ViewBase& view, BasePtr base, Pt::ssize_t n)
        {
            return base + n * PixelWidth;
        }

        template <typename BasePtr>
        static BasePtr advanceLine(const ViewBase& view, BasePtr base)
        {
            Pt::ssize_t w = view.width() * PixelWidth;
            Pt::ssize_t off = view.stride() - w;
            return base + off;
        }

        template <typename BasePtr>
        static BasePtr advanceLines(const ViewBase& view, BasePtr base, Pt::ssize_t n)
        {
            return base + n * view.stride();
        }

        static ColorF getColor(const Pt::uint8_t* p);

        static Argb32Color getArgb32Color(const Pt::uint8_t* p);

        static void getColors(const Pt::uint8_t* p, Gfx::ColorF* colors, std::size_t n);

        static void getColors(const Pt::uint8_t* p, Argb32Color* colors, std::size_t n);

        //
        // SourceCopy
        //
        static void sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from);

        static void sourceCopy(Pt::uint8_t* to, const Argb32Color& from);

        static void sourceCopy(Pt::uint8_t* to, const ColorF& c);

        static void sourceCopy(Pt::uint8_t* to, std::size_t length, const ColorF& c);

        static void sourceCopy(Pt::uint8_t* to, std::size_t length, const Argb32Color& c);

        static void sourceCopy(Pt::uint8_t* to, std::size_t length, const Pt::uint8_t* from);

        static void sourceCopy(Pt::uint8_t* to, const ColorF* colors, std::size_t length);

        static void sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length);

        static void sourceCopy(Pt::uint8_t* to, Pt::ssize_t toStride,
                               const Pt::uint8_t* from, Pt::ssize_t fromStride,
                               Pt::ssize_t width, Pt::ssize_t height);

        //
        // SourceOver
        //
        static void sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from);
        
        static void sourceOver(Pt::uint8_t* to, const Pt::Gfx::ColorF& from);

        static void sourceOver(Pt::uint8_t* to, std::size_t length, const ColorF& c);

        static void sourceOver(Pt::uint8_t* to, std::size_t length, const Pt::uint8_t* from);

        static void sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length);

        static void sourceOver(Pt::uint8_t* to, Pt::ssize_t toStride,
                               const Pt::uint8_t* from, Pt::ssize_t fromStride,
                               Pt::ssize_t width, Pt::ssize_t height);

    public:
        static void blendSourceCopy(Pt::uint8_t* to, const ColorF& c, Pt::uint8_t alpha)
        {
            const Pt::uint32_t blendAlphaSrc = alpha;
            const Pt::uint32_t blendAlphaInv = 255 - alpha;
            to[0] = (blendAlphaSrc * (c.blue () >> 8) + blendAlphaInv * to[0]) >> 8;
            to[1] = (blendAlphaSrc * (c.green() >> 8) + blendAlphaInv * to[1]) >> 8;
            to[2] = (blendAlphaSrc * (c.red  () >> 8) + blendAlphaInv * to[2]) >> 8;
            to[3] = (blendAlphaSrc * (c.alpha() >> 8) + blendAlphaInv * to[3]) >> 8;
        }
        
        static void blendSourceOver(Pt::uint8_t* to, const ColorF& c, Pt::uint8_t alpha)
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

} // namespace

} // namespace

#include <Pt/Gfx/BasicView.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// Argb32
///////////////////////////////////////////////////////////////////////

inline ColorF Argb32::getColor(const Pt::uint8_t* p)
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

    return ColorF(a, r, g, b);
}


inline Argb32Color Argb32::getArgb32Color(const Pt::uint8_t* p)
{
    return Argb32Color(p);
}


inline void Argb32::getColors(const Pt::uint8_t* p, ColorF* colors, std::size_t n)
{
    for(std::size_t i = 0; i < n; ++i)
    {
        colors[i] = getColor(p);
        p += PixelWidth;
    }
}


inline void Argb32::getColors(const Pt::uint8_t* p, Argb32Color* colors, std::size_t n)
{
    void* cp = reinterpret_cast<void*>(colors); 
    std::memcpy(cp, p, n * PixelWidth);
}

//
// Implementation SourceCopy
//

inline void Argb32::sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from)
{
    std::memcpy(to, from, PixelWidth);
}


inline void Argb32::sourceCopy(Pt::uint8_t* to, const Argb32Color& from)
{
    std::memcpy(to, &from.value(), PixelWidth);
}


inline void Argb32::sourceCopy(Pt::uint8_t* to, const ColorF& c)
{
    Pt::uint32_t* pixel = reinterpret_cast<Pt::uint32_t*>(to);

    *pixel = ( Pt::uint32_t(c.alpha() & 0xFF00) << 16 ) |
             ( Pt::uint32_t(c.red  () & 0xFF00) <<  8 ) |
               Pt::uint32_t(c.green() & 0xFF00)         |
             ( Pt::uint32_t(c.blue ()         ) >>  8 );
}


inline void Argb32::sourceCopy(Pt::uint8_t* to, std::size_t length, const ColorF& c)
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


inline void Argb32::sourceCopy(Pt::uint8_t* to, std::size_t length, const Argb32Color& c)
{
    const Pt::uint8_t* p = reinterpret_cast<const Pt::uint8_t*>( &c.value() );
    sourceCopy(to, length, p);
}


inline void Argb32::sourceCopy(Pt::uint8_t* to, std::size_t length, const Pt::uint8_t* from)
{
    //Argb32Ops::pixelOps_SourceCopy(to, from, length);

    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(to);
    const Pt::uint32_t fromARGB = *reinterpret_cast<const Pt::uint32_t*>(from);
    for(std::size_t i = 0; i < length; ++i) 
        *dst++ = fromARGB;
}


inline void Argb32::sourceCopy(Pt::uint8_t* to, const ColorF* colors, std::size_t length)
{          
    for(std::size_t n = 0; n < length; ++n)
    {
        Argb32::sourceCopy( to, 1, colors[n] );
        to += Argb32::PixelWidth;
    }
}


inline void Argb32::sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length)
{
    std::memcpy(to, from, length * 4);
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


inline void Argb32::sourceOver(Pt::uint8_t* to, const Pt::Gfx::ColorF& from)
{
    const Pt::uint32_t alpha    = from.alpha() >> 8;
    const Pt::uint32_t alphaSrc = alpha;
    const Pt::uint32_t alphaInv = 255 - alpha;

    to[0] = (Pt::uint8_t) ( (alphaSrc * (from.blue () >> 8) + alphaInv * to[0]) >> 8 );
    to[1] = (Pt::uint8_t) ( (alphaSrc * (from.green() >> 8) + alphaInv * to[1]) >> 8 );
    to[2] = (Pt::uint8_t) ( (alphaSrc * (from.red  () >> 8) + alphaInv * to[2]) >> 8 );
    to[3] = (Pt::uint8_t) ( (alphaSrc *  alpha              + alphaInv * to[3]) >> 8 );
}


inline void Argb32::sourceOver(Pt::uint8_t* to, std::size_t length, const ColorF& c)
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
, _base( Argb32::getPixel(view, view.data(), x, y) )
{
}


inline void Argb32Pixel::reset(BasicView<Argb32>& view, Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Argb32::getPixel(view, view.data(), x, y);
    
}


inline void Argb32Pixel::reset(const Argb32Pixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Argb32Pixel::advance()
{
    _base = Argb32::advance(*_view, _base);
}


inline void Argb32Pixel::advanceLine()
{
    _base = Argb32::advanceLine(*_view, _base);
}


inline void Argb32Pixel::advance(Pt::ssize_t n)
{
    _base = Argb32::advance(*_view, _base, n);
}


inline void Argb32Pixel::advanceLines(Pt::ssize_t n)
{
    _base = Argb32::advanceLines(*_view, _base, n);
}


inline bool Argb32Pixel::equals(const Argb32Pixel& p) const
{
    return base() == p.base();
}


inline bool Argb32Pixel::equals(const Argb32ConstPixel& p) const
{
    return base() == p.base();
}


inline void Argb32Pixel::assign(const Argb32Pixel& p, std::size_t length)
{
    Argb32::sourceCopy(base(), p.base(), length);
}


inline void Argb32Pixel::assign(const Argb32ConstPixel& p, std::size_t length)
{
    Argb32::sourceCopy(base(), p.base(), length);
}


inline void Argb32Pixel::assign(const Argb32Color* colors, std::size_t length)
{
    const Pt::uint8_t* p = reinterpret_cast<const Pt::uint8_t*>(colors);
    Argb32::sourceCopy(base(), p, length);
}


inline void Argb32Pixel::fill(std::size_t n, const Argb32Color& color)
{
    Argb32::sourceCopy(base(), n, color);
}


inline Argb32Pixel& Argb32Pixel::operator=(const Gfx::ColorF& color)
{ 
    Argb32::sourceCopy(base(), color);
    return *this;
}


inline Argb32Pixel& Argb32Pixel::operator=(const Argb32Color& color)
{ 
    Argb32::sourceCopy(base(), color);
    return *this;
}


inline void Argb32Pixel::assign(const Argb32Pixel& p)
{
    Argb32::sourceCopy( base(), p.base() );
}


inline void Argb32Pixel::assign(const Argb32ConstPixel& p)
{
    Argb32::sourceCopy( base(), p.base() );
}


inline Pt::uint8_t Argb32Pixel::alpha() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return *val >> 24;
}


inline void Argb32Pixel::setAlpha(Pt::uint8_t a)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0x00FFFFFF) | (uint32_t(a) << 24);
}


inline Pt::uint8_t Argb32Pixel::red() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return (*val & 0x00FF0000) >> 16;
}


inline void Argb32Pixel::setRed(Pt::uint8_t r)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0xFF00FFFF) | (uint32_t(r) << 16);
}


inline Pt::uint8_t Argb32Pixel::green() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return (*val & 0x0000FF00) >> 8;
}


inline void Argb32Pixel::setGreen(Pt::uint8_t g)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0xFFFF00FF) | (uint32_t(g) << 8);
}


inline Pt::uint8_t Argb32Pixel::blue() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return *val & 0x000000FF;
}


inline void Argb32Pixel::setBlue(Pt::uint8_t b)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0xFFFFFF00) | uint32_t(b);
}


inline Argb32Color Argb32Pixel::toColor() const
{
    return Argb32Color( base() );
}


inline Argb32Color Argb32ConstPixel::toColor() const
{
    return Argb32Color( base() );
}

///////////////////////////////////////////////////////////////////////
// Argb32ConstPixel
///////////////////////////////////////////////////////////////////////

inline Argb32ConstPixel::Argb32ConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                                          Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Argb32::getPixel(view, data, x, y) )
{
}


inline Argb32ConstPixel::Argb32ConstPixel(const BasicConstView<Argb32>& view,
                                          Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Argb32::getPixel(view, view.data(), x, y) )
{
}


inline Argb32ConstPixel::Argb32ConstPixel(const BasicView<Argb32>& view,
                                          Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Argb32::getPixel(view, view.data(), x, y) )
{
}


inline Argb32ConstPixel::Argb32ConstPixel(const Argb32Pixel& p)
: _view(p._view)
, _base(p._base)
{ 
}


inline void Argb32ConstPixel::reset(const BasicConstView<Argb32>& view, 
                                     Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Argb32::getPixel(view, view.data(), x, y);
}


inline void Argb32ConstPixel::reset(const BasicView<Argb32>& view, 
                                    Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Argb32::getPixel(view, view.data(), x, y);
}


inline void Argb32ConstPixel::reset(const Argb32ConstPixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Argb32ConstPixel::reset(const Argb32Pixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Argb32ConstPixel::advance()
{
    _base = Argb32::advance(*_view, _base);
}


inline void Argb32ConstPixel::advanceLine()
{
    _base = Argb32::advanceLine(*_view, _base);
}


inline void Argb32ConstPixel::advance(Pt::ssize_t n)
{
    _base = Argb32::advance(*_view, _base, n);
}


inline void Argb32ConstPixel::advanceLines(Pt::ssize_t n)
{
    _base = Argb32::advanceLines(*_view, _base, n);
}



inline void Argb32ConstPixel::getColors(Argb32Color* colors, std::size_t length) const
{
    Argb32::getColors(base(), colors, length);
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
