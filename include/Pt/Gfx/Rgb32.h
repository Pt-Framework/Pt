/* Copyright (C) 2015 Marc Boris Duerner

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

#ifndef PT_GFX_RGB32_H
#define PT_GFX_RGB32_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Types.h>
#include <cstring>

namespace Pt {

namespace Gfx {

/** @brief Premultiplied standard color type.
*/
class Rgb32Color
{
    public:
        Rgb32Color()
        : _value(0)
        { }
        
        Rgb32Color(const Rgb32Color& color)
        : _value(color._value)
        { }

        explicit Rgb32Color(uint32_t val)
        : _value(val)
        { }

        explicit Rgb32Color(const uint8_t* base)
        : _value()
        {
            std::memcpy( &_value, base, sizeof(uint32_t) );
        }

        Rgb32Color(Pt::uint8_t a, Pt::uint8_t pr, Pt::uint8_t pg, Pt::uint8_t pb)
        : _value( (uint32_t(a) << 24) | (uint32_t(pr) << 16) | (uint32_t(pg) << 8) | uint32_t(pb) )
        {
        }
        
        Rgb32Color& operator=(const Rgb32Color& color)
        { 
            _value = color._value;
            return *this;
        }
        
        Pt::uint8_t alpha() const
        {
            return _value >> 24;
        }

        Pt::uint8_t red() const
        {
            return (_value & 0x00FF0000) >> 16;
        }

        Pt::uint8_t green() const
        {
            return (_value & 0x0000FF00) >> 8;
        }

        Pt::uint8_t blue() const
        {
            return _value & 0x000000FF;
        }

        void setAlpha(Pt::uint8_t a)
        {
            _value = (_value & 0x00FFFFFF) | (uint32_t(a) << 24);
        }

        void setRed(Pt::uint8_t pr)
        {
            _value = (_value & 0xFF00FFFF) | (uint32_t(pr) << 16);
        }

        void setGreen(Pt::uint8_t pg)
        {
            _value = (_value & 0xFFFF00FF) | (uint32_t(pg) << 8);
        }

        void setBlue(Pt::uint8_t pb)
        {
            _value = (_value & 0xFFFFFF00) | uint32_t(pb);
        }

        const uint32_t& value() const
        {
            return _value;
        }

    private:
        Pt::uint32_t _value;
};

/** @brief RGB-32 pixel as premultiplied ARGB-32.

    When converting to a format without alpha the premultiplied RGB values
    are used directly, which is equivalent to compositing over black.
*/
class Rgb32Pixel
{
    friend class Rgb32ConstPixel;

    public:
        typedef Rgb32 FormatType;
        typedef Argb32Color ColorType;

    public:
        Rgb32Pixel(BasicView<Rgb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        Rgb32Pixel(const Rgb32Pixel& p) = default;

        ~Rgb32Pixel() = default;

        Rgb32Pixel& operator=(const Argb32Color& color);

        Rgb32Pixel& operator=(const Gfx::ColorF& color);

        Rgb32Pixel& operator=(const Rgb32Color& color);

        void reset(BasicView<Rgb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const Rgb32Pixel& p);

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

        /** @brief Returns the native premultiplied Rgb32Color.
        */
        Rgb32Color color() const;

        /** @brief Returns the pixel as straight (un-premultiplied) Argb32Color.
        */
        Argb32Color toColor() const;

        void advance();

        void skipPadding();

        void advance(Pt::ssize_t n);

        void advanceLines(Pt::ssize_t n);

        void assign(const Rgb32Pixel& p);

        void assign(const Rgb32ConstPixel& p);

        void assign(const Rgb32Pixel& p, std::size_t length);

        void assign(const Rgb32ConstPixel& p, std::size_t length);

        void assign(const Argb32Color* colors, std::size_t length);

        void fill(std::size_t n, const Argb32Color& color);

        void fill(std::size_t n, const Rgb32Color& color);

        bool equals(const Rgb32Pixel& p) const;

        bool equals(const Rgb32ConstPixel& p) const;

    private:
        const ViewBase* _view;
        Pt::uint8_t*    _base;
};

/** @brief Premultiplied ARGB-32 const pixel.
*/
class Rgb32ConstPixel
{
    friend class Rgb32Pixel;

    public:
        typedef Rgb32 FormatType;
        typedef Argb32Color ColorType;

    protected:
        Rgb32ConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                        Pt::ssize_t x, Pt::ssize_t y);

    public:
        Rgb32ConstPixel(const BasicConstView<Rgb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        Rgb32ConstPixel(const BasicView<Rgb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        Rgb32ConstPixel(const Rgb32ConstPixel& p) = default;

        Rgb32ConstPixel(const Rgb32Pixel& p);

        ~Rgb32ConstPixel() = default;

        void reset(const BasicConstView<Rgb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const BasicView<Rgb32>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const Rgb32ConstPixel& p);

        void reset(const Rgb32Pixel& p);

        const ViewBase& view() const
        { return *_view; }

        const Pt::uint8_t* base() const
        { return _base; }

        Pt::uint8_t alpha() const;

        Pt::uint8_t red() const;

        Pt::uint8_t green() const;

        Pt::uint8_t blue() const;

        /** @brief Returns the native premultiplied Rgb32Color.
        */
        Rgb32Color color() const;

        /** @brief Returns the pixel as straight (un-premultiplied) Argb32Color.
     */
        Argb32Color toColor() const;

        void advance();

        void skipPadding();

        void advance(Pt::ssize_t n);

        void advanceLines(Pt::ssize_t n);

        void getColors(Argb32Color* colors, std::size_t length) const;

        bool equals(const Rgb32ConstPixel& p) const;

        bool equals(const Rgb32Pixel& p) const;

    private:
        const ViewBase*     _view;
        const Pt::uint8_t*  _base;
};

/** @brief Premultiplied ARGB-32 image format.

    Stores pixel data as premultiplied ARGB-32. When colors are written
    from straight-alpha sources (ColorF, Argb32Color), the RGB channels
    are premultiplied by alpha. The sourceOver compositing uses the
    simplified premultiplied formula: dst = src + dst * (1 - src_alpha).
*/
class PT_GFX_API Rgb32 final : public ImageFormat
{
    static const unsigned PixelWidth = 4;

    public:    
        typedef Rgb32Pixel PixelType;
        typedef Rgb32ConstPixel ConstPixelType;
        typedef Argb32Color ColorType;

    public:
        static const Rgb32& get()
        {
            static Rgb32 _rgb32;
            return _rgb32;
        }

    public:
        Rgb32()
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
            return std::unique_ptr<ImageFormat>(new Rgb32);
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
        static BasePtr skipPadding(const ViewBase& view, BasePtr base)
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

        /** @brief Read premultiplied pixel as ColorF (returns premultiplied values).
        */
        static ColorF getColor(const Pt::uint8_t* p);

        /** @brief Read premultiplied pixel as Argb32Color (returns premultiplied values).
        */
        static Argb32Color getArgb32Color(const Pt::uint8_t* p);

        static void getColors(const Pt::uint8_t* p, Gfx::ColorF* colors, std::size_t n);

        static void getColors(const Pt::uint8_t* p, Argb32Color* colors, std::size_t n);

        /** @brief Read premultiplied pixel and un-premultiplied to straight ColorF.
        */
        static ColorF getStraightColor(const Pt::uint8_t* p);

        /** @brief Read premultiplied pixel and un-premultiplied to straight Argb32Color.
        */
        static Argb32Color getStraightArgb32Color(const Pt::uint8_t* p);

        static void getStraightColors(const Pt::uint8_t* p, Gfx::ColorF* colors, std::size_t n);

        static void getStraightColors(const Pt::uint8_t* p, Argb32Color* colors, std::size_t n);

        //
        // SourceCopy - writes premultiplied values
        //

        /** @brief Copy raw premultiplied pixel data.
        */
        static void sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from);

        /** @brief Write Argb32Color, premultiplying RGB by alpha.
        */
        static void sourceCopy(Pt::uint8_t* to, const Argb32Color& from);

        /** @brief Write ColorF, premultiplying RGB by alpha.
        */
        static void sourceCopy(Pt::uint8_t* to, const ColorF& c);

        /** @brief Fill with ColorF, premultiplying RGB by alpha.
        */
        static void sourceCopy(Pt::uint8_t* to, std::size_t length, const ColorF& c);

        /** @brief Fill with Argb32Color, premultiplying RGB by alpha.
        */
        static void sourceCopy(Pt::uint8_t* to, std::size_t length, const Argb32Color& c);

        /** @brief Fill with premultiplied raw pixel.
        */
        static void sourceCopy(Pt::uint8_t* to, std::size_t length, const Pt::uint8_t* from);

        /** @brief Write ColorF array, premultiplying each by alpha.
        */
        static void sourceCopy(Pt::uint8_t* to, const ColorF* colors, std::size_t length);

        /** @brief Copy premultiplied pixel array.
        */
        static void sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length);

        static void sourceCopy(Pt::uint8_t* to, Pt::ssize_t toStride,
                               const Pt::uint8_t* from, Pt::ssize_t fromStride,
                               Pt::ssize_t width, Pt::ssize_t height);

        /** @brief Copy raw Rgb32Color data.
        */
        static void sourceCopy(Pt::uint8_t* to, const Rgb32Color& from);

        /** @brief Fill with raw Rgb32Color.
        */
        static void sourceCopy(Pt::uint8_t* to, std::size_t length, const Rgb32Color& c);

        //
        // SourceOver - premultiplied compositing: dst = src + dst * (1 - src_alpha)
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
            const Pt::uint32_t a = c.alpha() >> 8;
            const Pt::uint32_t blendAlphaSrc = alpha;
            const Pt::uint32_t blendAlphaInv = 255 - alpha;
            const Pt::uint32_t pr = (Pt::uint32_t)(c.red  () >> 8) * a / 255;
            const Pt::uint32_t pg = (Pt::uint32_t)(c.green() >> 8) * a / 255;
            const Pt::uint32_t pb = (Pt::uint32_t)(c.blue () >> 8) * a / 255;
            to[0] = (blendAlphaSrc * pb + blendAlphaInv * to[0]) >> 8;
            to[1] = (blendAlphaSrc * pg + blendAlphaInv * to[1]) >> 8;
            to[2] = (blendAlphaSrc * pr + blendAlphaInv * to[2]) >> 8;
            to[3] = (blendAlphaSrc * a  + blendAlphaInv * to[3]) >> 8;
        }
        
        static void blendSourceOver(Pt::uint8_t* to, const ColorF& c, Pt::uint8_t alpha)
        {
            const Pt::uint32_t a = c.alpha() >> 8;
            const Pt::uint32_t blendAlpha = a * alpha / 255;
            const Pt::uint32_t blendAlphaInv = 255 - blendAlpha;
            const Pt::uint32_t pr = (Pt::uint32_t)(c.red  () >> 8) * blendAlpha / 255;
            const Pt::uint32_t pg = (Pt::uint32_t)(c.green() >> 8) * blendAlpha / 255;
            const Pt::uint32_t pb = (Pt::uint32_t)(c.blue () >> 8) * blendAlpha / 255;
            to[0] = (Pt::uint8_t)(pb + ((blendAlphaInv * to[0]) >> 8));
            to[1] = (Pt::uint8_t)(pg + ((blendAlphaInv * to[1]) >> 8));
            to[2] = (Pt::uint8_t)(pr + ((blendAlphaInv * to[2]) >> 8));
            to[3] = (Pt::uint8_t)(blendAlpha + ((blendAlphaInv * to[3]) >> 8));
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
            // from[] is already premultiplied, scale by extra blend alpha
            const Pt::uint32_t blendAlpha    = from[3] * alpha / 255;
            const Pt::uint32_t blendAlphaInv = 255 - blendAlpha;
            const Pt::uint32_t srcB = from[0] * alpha / 255;
            const Pt::uint32_t srcG = from[1] * alpha / 255;
            const Pt::uint32_t srcR = from[2] * alpha / 255;
            to[0] = (Pt::uint8_t)(srcB + ((blendAlphaInv * to[0]) >> 8));
            to[1] = (Pt::uint8_t)(srcG + ((blendAlphaInv * to[1]) >> 8));
            to[2] = (Pt::uint8_t)(srcR + ((blendAlphaInv * to[2]) >> 8));
            to[3] = (Pt::uint8_t)(blendAlpha + ((blendAlphaInv * to[3]) >> 8));
        }

    private:
        /** @brief Premultiply 8-bit ARGB to a packed premultiplied uint32_t.
        */
        static Pt::uint32_t premultiply(Pt::uint8_t a, Pt::uint8_t r, 
                                        Pt::uint8_t g, Pt::uint8_t b)
        {
            const Pt::uint32_t pr = (r * a + 127) / 255;
            const Pt::uint32_t pg = (g * a + 127) / 255;
            const Pt::uint32_t pb = (b * a + 127) / 255;

            return (Pt::uint32_t(a)  << 24) |
                   (Pt::uint32_t(pr) << 16) |
                   (Pt::uint32_t(pg) <<  8) |
                    Pt::uint32_t(pb);
        }

        /** @brief Premultiply a ColorF to a packed premultiplied uint32_t.
        */
        static Pt::uint32_t premultiplyColorF(const ColorF& c)
        {
            const Pt::uint32_t a = c.alpha() >> 8;
            const Pt::uint32_t r = (Pt::uint32_t)(c.red  () >> 8) * a / 255;
            const Pt::uint32_t g = (Pt::uint32_t)(c.green() >> 8) * a / 255;
            const Pt::uint32_t b = (Pt::uint32_t)(c.blue () >> 8) * a / 255;

            return (a << 24) | (r << 16) | (g << 8) | b;
        }
};

} // namespace

} // namespace

#include <Pt/Gfx/BasicView.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// Rgb32 - color conversion
///////////////////////////////////////////////////////////////////////

inline ColorF Rgb32::getColor(const Pt::uint8_t* p)
{
    // Returns premultiplied values as-is
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


inline Argb32Color Rgb32::getArgb32Color(const Pt::uint8_t* p)
{
    // Returns premultiplied values as-is
    return Argb32Color(p);
}


inline void Rgb32::getColors(const Pt::uint8_t* p, ColorF* colors, std::size_t n)
{
    for(std::size_t i = 0; i < n; ++i)
    {
        colors[i] = getColor(p);
        p += PixelWidth;
    }
}


inline void Rgb32::getColors(const Pt::uint8_t* p, Argb32Color* colors, std::size_t n)
{
    void* cp = reinterpret_cast<void*>(colors); 
    std::memcpy(cp, p, n * PixelWidth);
}


inline ColorF Rgb32::getStraightColor(const Pt::uint8_t* p)
{
    const Pt::uint32_t pixel = *reinterpret_cast<const Pt::uint32_t*>(p);
    const Pt::uint32_t a = pixel >> 24;

    if (a == 0)
        return ColorF(0, 0, 0, 0);

    const Pt::uint32_t r = ((pixel & 0x00FF0000) >> 16);
    const Pt::uint32_t g = ((pixel & 0x0000FF00) >>  8);
    const Pt::uint32_t b =  (pixel & 0x000000FF);

    const Pt::uint16_t a16 = a * 257;
    const Pt::uint16_t r16 = (r * 255 * 257) / a;
    const Pt::uint16_t g16 = (g * 255 * 257) / a;
    const Pt::uint16_t b16 = (b * 255 * 257) / a;

    return ColorF(a16, r16, g16, b16);
}


inline Argb32Color Rgb32::getStraightArgb32Color(const Pt::uint8_t* p)
{
    const Pt::uint32_t pixel = *reinterpret_cast<const Pt::uint32_t*>(p);
    const Pt::uint32_t a = pixel >> 24;

    if (a == 0)
        return Argb32Color(0, 0, 0, 0);

    const Pt::uint32_t r = ((pixel & 0x00FF0000) >> 16);
    const Pt::uint32_t g = ((pixel & 0x0000FF00) >>  8);
    const Pt::uint32_t b =  (pixel & 0x000000FF);

    return Argb32Color( Pt::uint8_t(a), 
                        Pt::uint8_t((r * 255) / a), 
                        Pt::uint8_t((g * 255) / a), 
                        Pt::uint8_t((b * 255) / a) );
}


inline void Rgb32::getStraightColors(const Pt::uint8_t* p, ColorF* colors, std::size_t n)
{
    for(std::size_t i = 0; i < n; ++i)
    {
        colors[i] = getStraightColor(p);
        p += PixelWidth;
    }
}


inline void Rgb32::getStraightColors(const Pt::uint8_t* p, Argb32Color* colors, std::size_t n)
{
    for(std::size_t i = 0; i < n; ++i)
    {
        colors[i] = getStraightArgb32Color(p);
        p += PixelWidth;
    }
}

//
// Implementation SourceCopy - premultiplies when writing from straight-alpha sources
//

inline void Rgb32::sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from)
{
    // Raw copy: data is already premultiplied
    std::memcpy(to, from, PixelWidth);
}


inline void Rgb32::sourceCopy(Pt::uint8_t* to, const Argb32Color& from)
{
    // Premultiply straight-alpha Argb32Color
    Pt::uint32_t* pixel = reinterpret_cast<Pt::uint32_t*>(to);
    *pixel = premultiply(from.alpha(), from.red(), from.green(), from.blue());
}


inline void Rgb32::sourceCopy(Pt::uint8_t* to, const ColorF& c)
{
    // Premultiply straight-alpha ColorF
    Pt::uint32_t* pixel = reinterpret_cast<Pt::uint32_t*>(to);
    *pixel = premultiplyColorF(c);
}


inline void Rgb32::sourceCopy(Pt::uint8_t* to, std::size_t length, const ColorF& c)
{
    const Pt::uint32_t value = premultiplyColorF(c);

    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(to);
    for(std::size_t i = 0; i < length; ++i) 
        *dst++ = value;
}


inline void Rgb32::sourceCopy(Pt::uint8_t* to, std::size_t length, const Argb32Color& c)
{
    const Pt::uint32_t value = premultiply(c.alpha(), c.red(), c.green(), c.blue());

    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(to);
    for(std::size_t i = 0; i < length; ++i) 
        *dst++ = value;
}


inline void Rgb32::sourceCopy(Pt::uint8_t* to, std::size_t length, const Pt::uint8_t* from)
{
    // Raw fill: data is already premultiplied
    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(to);
    const Pt::uint32_t fromValue = *reinterpret_cast<const Pt::uint32_t*>(from);
    for(std::size_t i = 0; i < length; ++i) 
        *dst++ = fromValue;
}


inline void Rgb32::sourceCopy(Pt::uint8_t* to, const ColorF* colors, std::size_t length)
{          
    for(std::size_t n = 0; n < length; ++n)
    {
        Rgb32::sourceCopy(to, colors[n]);
        to += Rgb32::PixelWidth;
    }
}


inline void Rgb32::sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length)
{
    // Raw array copy: data is already premultiplied
    std::memcpy(to, from, length * 4);
}


inline void Rgb32::sourceCopy(Pt::uint8_t* to, Pt::ssize_t toStride,
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


inline void Rgb32::sourceCopy(Pt::uint8_t* to, const Rgb32Color& from)
{
    // Raw copy: Rgb32Color is already premultiplied
    std::memcpy(to, &from.value(), PixelWidth);
}


inline void Rgb32::sourceCopy(Pt::uint8_t* to, std::size_t length, const Rgb32Color& c)
{
    // Raw fill: Rgb32Color is already premultiplied
    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(to);
    const Pt::uint32_t value = c.value();
    for(std::size_t i = 0; i < length; ++i) 
        *dst++ = value;
}

//
// Implementation SourceOver - premultiplied compositing
// Formula: dst = src + dst * (1 - src_alpha)
//

inline void Rgb32::sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from)
{
    // from[] is already premultiplied
    const Pt::uint32_t alphaInv = 255 - from[3];

    to[0] = (Pt::uint8_t) ( from[0] + ((alphaInv * to[0]) >> 8) );
    to[1] = (Pt::uint8_t) ( from[1] + ((alphaInv * to[1]) >> 8) );
    to[2] = (Pt::uint8_t) ( from[2] + ((alphaInv * to[2]) >> 8) );
    to[3] = (Pt::uint8_t) ( from[3] + ((alphaInv * to[3]) >> 8) );
}


inline void Rgb32::sourceOver(Pt::uint8_t* to, const Pt::Gfx::ColorF& from)
{
    // Premultiply the straight-alpha ColorF, then composite
    const Pt::uint32_t alpha    = from.alpha() >> 8;
    const Pt::uint32_t alphaInv = 255 - alpha;
    const Pt::uint32_t srcR = (Pt::uint32_t)(from.red  () >> 8) * alpha / 255;
    const Pt::uint32_t srcG = (Pt::uint32_t)(from.green() >> 8) * alpha / 255;
    const Pt::uint32_t srcB = (Pt::uint32_t)(from.blue () >> 8) * alpha / 255;

    to[0] = (Pt::uint8_t) ( srcB  + ((alphaInv * to[0]) >> 8) );
    to[1] = (Pt::uint8_t) ( srcG  + ((alphaInv * to[1]) >> 8) );
    to[2] = (Pt::uint8_t) ( srcR  + ((alphaInv * to[2]) >> 8) );
    to[3] = (Pt::uint8_t) ( alpha + ((alphaInv * to[3]) >> 8) );
}


inline void Rgb32::sourceOver(Pt::uint8_t* to, std::size_t length, const ColorF& c)
{
    // Premultiply once, then apply to all pixels
    const Pt::uint32_t alpha = c.alpha() >> 8;
    const Pt::uint32_t alphaInv = 255 - alpha;
    const Pt::uint32_t srcR = (Pt::uint32_t)(c.red  () >> 8) * alpha / 255;
    const Pt::uint32_t srcG = (Pt::uint32_t)(c.green() >> 8) * alpha / 255;
    const Pt::uint32_t srcB = (Pt::uint32_t)(c.blue () >> 8) * alpha / 255;

    Pt::uint8_t* dst = to;

    for(std::size_t i = 0; i < length; ++i) {
        dst[0] = (Pt::uint8_t)(srcB  + ((alphaInv * dst[0]) >> 8));
        dst[1] = (Pt::uint8_t)(srcG  + ((alphaInv * dst[1]) >> 8));
        dst[2] = (Pt::uint8_t)(srcR  + ((alphaInv * dst[2]) >> 8));
        dst[3] = (Pt::uint8_t)(alpha + ((alphaInv * dst[3]) >> 8));
        dst += 4;
    }
}


inline void Rgb32::sourceOver(Pt::uint8_t* to, std::size_t length, const Pt::uint8_t* from)
{
    // from[] is already premultiplied, fill dst with same src pixel
    const Pt::uint32_t alphaInv = 255 - from[3];

    Pt::uint8_t* dst = to;

    for(std::size_t i = 0; i < length; ++i) {
        dst[0] = (Pt::uint8_t)(from[0] + ((alphaInv * dst[0]) >> 8));
        dst[1] = (Pt::uint8_t)(from[1] + ((alphaInv * dst[1]) >> 8));
        dst[2] = (Pt::uint8_t)(from[2] + ((alphaInv * dst[2]) >> 8));
        dst[3] = (Pt::uint8_t)(from[3] + ((alphaInv * dst[3]) >> 8));
        dst += 4;
    }
}


inline void Rgb32::sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length)
{
    // from[] array is already premultiplied, each pixel has different alpha
    const Pt::uint8_t* src = from;
          Pt::uint8_t* dst = to;

    for(std::size_t i = 0; i < length; ++i) 
    {
        const Pt::uint32_t alphaInv = 255 - src[3];
        dst[0] = (Pt::uint8_t) ( src[0] + ((alphaInv * dst[0]) >> 8) );
        dst[1] = (Pt::uint8_t) ( src[1] + ((alphaInv * dst[1]) >> 8) );
        dst[2] = (Pt::uint8_t) ( src[2] + ((alphaInv * dst[2]) >> 8) );
        dst[3] = (Pt::uint8_t) ( src[3] + ((alphaInv * dst[3]) >> 8) );
        src += 4;
        dst += 4;
    }
}


inline void Rgb32::sourceOver(Pt::uint8_t* to, Pt::ssize_t toStride,
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
// Rgb32Pixel
///////////////////////////////////////////////////////////////////////

inline Rgb32Pixel::Rgb32Pixel(BasicView<Rgb32>& view, 
                              Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Rgb32::getPixel(view, view.data(), x, y) )
{
}


inline void Rgb32Pixel::reset(BasicView<Rgb32>& view, Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Rgb32::getPixel(view, view.data(), x, y);
}


inline void Rgb32Pixel::reset(const Rgb32Pixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Rgb32Pixel::advance()
{
    _base = Rgb32::advance(*_view, _base);
}


inline void Rgb32Pixel::skipPadding()
{
    _base = Rgb32::skipPadding(*_view, _base);
}


inline void Rgb32Pixel::advance(Pt::ssize_t n)
{
    _base = Rgb32::advance(*_view, _base, n);
}


inline void Rgb32Pixel::advanceLines(Pt::ssize_t n)
{
    _base = Rgb32::advanceLines(*_view, _base, n);
}


inline Rgb32Pixel& Rgb32Pixel::operator=(const Gfx::ColorF& color)
{ 
    Rgb32::sourceCopy(base(), color);
    return *this;
}


inline Rgb32Pixel& Rgb32Pixel::operator=(const Argb32Color& color)
{ 
    Rgb32::sourceCopy(base(), color);
    return *this;
}


inline void Rgb32Pixel::assign(const Rgb32Pixel& p)
{
    Rgb32::sourceCopy( base(), p.base() );
}


inline void Rgb32Pixel::assign(const Rgb32ConstPixel& p)
{
    Rgb32::sourceCopy( base(), p.base() );
}


inline void Rgb32Pixel::assign(const Rgb32Pixel& p, std::size_t length)
{
    Rgb32::sourceCopy(base(), p.base(), length);
}


inline void Rgb32Pixel::assign(const Rgb32ConstPixel& p, std::size_t length)
{
    Rgb32::sourceCopy(base(), p.base(), length);
}


inline void Rgb32Pixel::assign(const Argb32Color* colors, std::size_t length)
{
    // Premultiply each straight-alpha color individually
    Pt::uint8_t* p = base();
    for(std::size_t i = 0; i < length; ++i)
    {
        Rgb32::sourceCopy(p, colors[i]);
        p += 4;
    }
}


inline void Rgb32Pixel::fill(std::size_t n, const Argb32Color& color)
{
    Rgb32::sourceCopy(base(), n, color);
}


inline void Rgb32Pixel::fill(std::size_t n, const Rgb32Color& color)
{
    Rgb32::sourceCopy(base(), n, color);
}


inline bool Rgb32Pixel::equals(const Rgb32Pixel& p) const
{
    return base() == p.base();
}


inline bool Rgb32Pixel::equals(const Rgb32ConstPixel& p) const
{
    return base() == p.base();
}


inline Pt::uint8_t Rgb32Pixel::alpha() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return *val >> 24;
}


inline void Rgb32Pixel::setAlpha(Pt::uint8_t a)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0x00FFFFFF) | (uint32_t(a) << 24);
}


inline Pt::uint8_t Rgb32Pixel::red() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return (*val & 0x00FF0000) >> 16;
}


inline void Rgb32Pixel::setRed(Pt::uint8_t r)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0xFF00FFFF) | (uint32_t(r) << 16);
}


inline Pt::uint8_t Rgb32Pixel::green() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return (*val & 0x0000FF00) >> 8;
}


inline void Rgb32Pixel::setGreen(Pt::uint8_t g)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0xFFFF00FF) | (uint32_t(g) << 8);
}


inline Pt::uint8_t Rgb32Pixel::blue() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return *val & 0x000000FF;
}


inline void Rgb32Pixel::setBlue(Pt::uint8_t b)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0xFFFFFF00) | uint32_t(b);
}


inline Rgb32Color Rgb32Pixel::color() const
{
    return Rgb32Color( base() );
}


inline Argb32Color Rgb32Pixel::toColor() const
{
    // Un-premultiply to return straight Argb32Color
    return Rgb32::getStraightArgb32Color( base() );
}


///////////////////////////////////////////////////////////////////////
// Rgb32ConstPixel
///////////////////////////////////////////////////////////////////////

inline Rgb32ConstPixel::Rgb32ConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                                        Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Rgb32::getPixel(view, data, x, y) )
{
}


inline Rgb32ConstPixel::Rgb32ConstPixel(const BasicConstView<Rgb32>& view,
                                        Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Rgb32::getPixel(view, view.data(), x, y) )
{
}


inline Rgb32ConstPixel::Rgb32ConstPixel(const BasicView<Rgb32>& view,
                                        Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Rgb32::getPixel(view, view.data(), x, y) )
{
}


inline Rgb32ConstPixel::Rgb32ConstPixel(const Rgb32Pixel& p)
: _view(p._view)
, _base(p._base)
{ 
}


inline void Rgb32ConstPixel::reset(const BasicConstView<Rgb32>& view, 
                                   Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Rgb32::getPixel(view, view.data(), x, y);
}


inline void Rgb32ConstPixel::reset(const BasicView<Rgb32>& view, 
                                   Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Rgb32::getPixel(view, view.data(), x, y);
}


inline void Rgb32ConstPixel::reset(const Rgb32ConstPixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Rgb32ConstPixel::reset(const Rgb32Pixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Rgb32ConstPixel::advance()
{
    _base = Rgb32::advance(*_view, _base);
}


inline void Rgb32ConstPixel::skipPadding()
{
    _base = Rgb32::skipPadding(*_view, _base);
}


inline void Rgb32ConstPixel::advance(Pt::ssize_t n)
{
    _base = Rgb32::advance(*_view, _base, n);
}


inline void Rgb32ConstPixel::advanceLines(Pt::ssize_t n)
{
    _base = Rgb32::advanceLines(*_view, _base, n);
}


inline Pt::uint8_t Rgb32ConstPixel::alpha() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return *val >> 24;
}


inline Pt::uint8_t Rgb32ConstPixel::red() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return (*val & 0x00FF0000) >> 16;
}


inline Pt::uint8_t Rgb32ConstPixel::green() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return (*val & 0x0000FF00) >> 8;
}


inline Pt::uint8_t Rgb32ConstPixel::blue() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return *val & 0x000000FF;
}


inline Rgb32Color Rgb32ConstPixel::color() const
{
    return Rgb32Color( base() );
}


inline Argb32Color Rgb32ConstPixel::toColor() const
{
    // Un-premultiply to return straight Argb32Color
    return Rgb32::getStraightArgb32Color( base() );
}


inline void Rgb32ConstPixel::getColors(Argb32Color* colors, std::size_t length) const
{
    // Un-premultiply to return straight Argb32Color values
    Rgb32::getStraightColors(base(), colors, length);
}


inline bool Rgb32ConstPixel::equals(const Rgb32ConstPixel& p) const
{
    return base() == p.base();
}


inline bool Rgb32ConstPixel::equals(const Rgb32Pixel& p) const
{
    return base() == p.base();
}

} // namespace

} // namespace

#endif
