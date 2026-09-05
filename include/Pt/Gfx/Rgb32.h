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
        
        Rgb32Color(const Rgb32Color& color) = default;

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
        
        Rgb32Color& operator=(const Rgb32Color& color) = default;
        
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
        typedef Color ColorType;

    public:
        template <typename T>
        Rgb32Pixel(T& view, Pt::ssize_t x, Pt::ssize_t y);

        Rgb32Pixel(const Rgb32Pixel& p) = default;

        ~Rgb32Pixel() = default;

        Rgb32Pixel& operator=(const Rgb32Pixel&) = delete;

        Rgb32Pixel& operator=(const Color& color);

        Rgb32Pixel& operator=(const Rgb32Color& color);

        template <typename T>
        void reset(T& view, Pt::ssize_t x, Pt::ssize_t y);

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

        /** @brief Returns the premultiplied RGB-32 color.
        */
        Rgb32Color color() const;

        /** @brief Returns ARGB-32 color.
        */
        Color getColor() const;

        void advance();

        void skipPadding();

        void advance(Pt::ssize_t n);

        void advanceLines(Pt::ssize_t n);

        void assign(const Rgb32Pixel& p);

        void assign(const Rgb32ConstPixel& p);

        void assign(const Rgb32Pixel& p, std::size_t length);

        void assign(const Rgb32ConstPixel& p, std::size_t length);

        void assign(const Color* colors, std::size_t length);

        void assign(const Rgb32Color* colors, std::size_t length);

        void getColors(Color* colors, std::size_t length) const;

        /** @brief Get as premultiplied RGB-32 colors.
        */
        void getColors(Rgb32Color* colors, std::size_t length) const;

        void fill(std::size_t n, const Color& color);

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
        typedef Color ColorType;

    protected:
        Rgb32ConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                        Pt::ssize_t x, Pt::ssize_t y);

    public:
        template <typename T>
        Rgb32ConstPixel(const T& view, Pt::ssize_t x, Pt::ssize_t y);

        template <typename T>
        Rgb32ConstPixel(T& view, Pt::ssize_t x, Pt::ssize_t y);

        Rgb32ConstPixel(const Rgb32ConstPixel& p) = default;

        Rgb32ConstPixel(const Rgb32Pixel& p);

        ~Rgb32ConstPixel() = default;

        Rgb32ConstPixel& operator=(const Rgb32ConstPixel&) = delete;

        template <typename T>
        void reset(const T& view, Pt::ssize_t x, Pt::ssize_t y);

        template <typename T>
        void reset(T& view, Pt::ssize_t x, Pt::ssize_t y);

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

        /** @brief Returns a premultiplied RGB-32 color.
        */
        Rgb32Color color() const;

        /** @brief Returns a ARGB-32 color.
     */
        Color getColor() const;

        void advance();

        void skipPadding();

        void advance(Pt::ssize_t n);

        void advanceLines(Pt::ssize_t n);

        void getColors(Color* colors, std::size_t length) const;

        /** @brief Get as premultiplied RGB-32 colors.
 */
        void getColors(Rgb32Color* colors, std::size_t length) const;

        bool equals(const Rgb32ConstPixel& p) const;

        bool equals(const Rgb32Pixel& p) const;

    private:
        const ViewBase*     _view;
        const Pt::uint8_t*  _base;
};

/** @brief Premultiplied ARGB-32 image format.

    Stores pixel data as premultiplied ARGB-32. When colors are written
    from straight-alpha sources (ColorF, Color), the RGB channels
    are premultiplied by alpha. The sourceOver compositing uses the
    simplified premultiplied formula: dst = src + dst * (1 - src_alpha).
*/
class PT_GFX_API Rgb32 final : public ImageFormat
{
    static const unsigned PixelWidth = 4;

    public:    
        typedef Rgb32Pixel      Pixel;
        typedef Rgb32ConstPixel ConstPixel;

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

        static Rgb32Color getRgb32Color(const Pt::uint8_t* p);

        static ColorF getColorF(const Pt::uint8_t* p);

        static Color getColor(const Pt::uint8_t* p);

        static void getColors(const Pt::uint8_t* p, Rgb32Color* colors, std::size_t n);

        static void getColors(const Pt::uint8_t* p, Color* colors, std::size_t n);

        static void getColors(const Pt::uint8_t* p, Gfx::ColorF* colors, std::size_t n);

        static void assign(Pt::uint8_t* to, const Color& from);

        static void assign(Pt::uint8_t* to, const ColorF& c);

        static void fill(Pt::uint8_t* to, std::size_t length, const Rgb32Color& c);

        static void fill(Pt::uint8_t* to, std::size_t length, const Color& c);

        static void fill(Pt::uint8_t* to, std::size_t length, const ColorF& c);

        static void assign(Pt::uint8_t* to, const Rgb32Color* colors, std::size_t length);

        static void assign(Pt::uint8_t* to, const Color* colors, std::size_t length);

        static void assign(Pt::uint8_t* to, const ColorF* colors, std::size_t length);

        static void copy(Pt::uint8_t* to, const Pt::uint8_t* from);

        static void copy(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length);

    private:
        /** @brief Premultiply 8-bit ARGB.
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

        /** @brief Premultiply a ColorF.
        */
        static Pt::uint32_t premultiply(const ColorF& c)
        {
            const float a = c.alpha();
            const Pt::uint32_t au = ColorF::toChannel8(a);
            const Pt::uint32_t r = ColorF::toChannel8(c.red() * a);
            const Pt::uint32_t g = ColorF::toChannel8(c.green() * a);
            const Pt::uint32_t b = ColorF::toChannel8(c.blue() * a);

            return (au << 24) | (r << 16) | (g << 8) | b;
        }
};

/** @brief Copies one color to N destination pixels.
*/
inline void sourceCopy(Rgb32Pixel& to, std::size_t length, const Rgb32Color& from);

/** @brief Blends one color over N destination pixels.
*/
inline void sourceOver(Rgb32Pixel& to, std::size_t length, const Rgb32Color& from);

/** @brief Copies N source pixels to N destination pixels.
*/
inline void sourceCopy(Rgb32Pixel& to, const Rgb32ConstPixel& from, std::size_t length);

/** @brief Blends N source pixels over N destination pixels.
*/
inline void sourceOver(Rgb32Pixel& to, const Rgb32ConstPixel& from, std::size_t length);

} // namespace

} // namespace

#include <Pt/Gfx/Rgb32.hpp>

#endif
