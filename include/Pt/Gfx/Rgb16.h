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

#ifndef PT_GFX_RGB16_H
#define PT_GFX_RGB16_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Types.h>
#include <cstring>

namespace Pt {

namespace Gfx {

/** @brief RGB-565 pixel.
*/
class Rgb16Pixel
{
    friend class Rgb16ConstPixel;

    public:
        typedef Rgb16 FormatType;
        typedef Argb32Color ColorType;

    public:
        Rgb16Pixel(BasicImageView<Rgb16>& view, Pt::ssize_t x, Pt::ssize_t y);

        Rgb16Pixel(const Rgb16Pixel& p) = default;

        ~Rgb16Pixel() = default;

        Rgb16Pixel& operator=(const Argb32Color& color);

        Rgb16Pixel& operator=(const Gfx::ColorF& color);

        void reset(BasicImageView<Rgb16>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const Rgb16Pixel& p);

        const ViewBase& view() const
        { return *_view; }

        Pt::uint8_t* base()
        { return _base; }

        const Pt::uint8_t* base() const
        { return _base; }

        Pt::uint8_t red() const;

        Pt::uint8_t green() const;

        Pt::uint8_t blue() const;

        Argb32Color toColor() const;

        void advance();

        void skipPadding();

        void advance(Pt::ssize_t n);

        void advanceLines(Pt::ssize_t n);

        void assign(const Rgb16Pixel& p);

        void assign(const Rgb16ConstPixel& p);

        void assign(const Rgb16Pixel& p, std::size_t length);

        void assign(const Rgb16ConstPixel& p, std::size_t length);

        void assign(const Argb32Color* colors, std::size_t length);

        void fill(std::size_t n, const Argb32Color& color);

        bool equals(const Rgb16Pixel& p) const;

        bool equals(const Rgb16ConstPixel& p) const;

    private:
        const ViewBase* _view;
        Pt::uint8_t*    _base;
};

/** @brief RGB-565 const pixel.
*/
class Rgb16ConstPixel
{
    friend class Rgb16Pixel;

    public:
        typedef Rgb16 FormatType;
        typedef Argb32Color ColorType;

    protected:
        Rgb16ConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                        Pt::ssize_t x, Pt::ssize_t y);

    public:
        Rgb16ConstPixel(const BasicConstImageView<Rgb16>& view, Pt::ssize_t x, Pt::ssize_t y);

        Rgb16ConstPixel(const BasicImageView<Rgb16>& view, Pt::ssize_t x, Pt::ssize_t y);

        Rgb16ConstPixel(const Rgb16ConstPixel& p) = default;

        Rgb16ConstPixel(const Rgb16Pixel& p);

        ~Rgb16ConstPixel() = default;

        void reset(const BasicConstImageView<Rgb16>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const BasicImageView<Rgb16>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const Rgb16ConstPixel& p);

        void reset(const Rgb16Pixel& p);

        const ViewBase& view() const
        { return *_view; }

        const Pt::uint8_t* base() const
        { return _base; }

        Pt::uint8_t red() const;

        Pt::uint8_t green() const;

        Pt::uint8_t blue() const;

        Argb32Color toColor() const;

        void advance();

        void skipPadding();

        void advance(Pt::ssize_t n);

        void advanceLines(Pt::ssize_t n);

        void getColors(Argb32Color* colors, std::size_t length) const;

        bool equals(const Rgb16ConstPixel& p) const;

        bool equals(const Rgb16Pixel& p) const;

    private:
        const ViewBase*     _view;
        const Pt::uint8_t* _base;
};

/** @brief RGB-565 image format.
*/
class PT_GFX_API Rgb16 final : public ImageFormat
{
    static const unsigned PixelWidth = 2;

    public:
        typedef Rgb16Pixel PixelType;
        typedef Rgb16ConstPixel ConstPixelType;
        typedef Argb32Color ColorType;

  public:
        static const Rgb16& get()
        {
            static Rgb16 _rgb16;
            return _rgb16;
        }

    public:
        Rgb16()
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
            std::size_t stride = (width * PixelWidth) + padding;
            std::size_t n = stride * height;
            return n;
      }

    protected:
        virtual std::unique_ptr<ImageFormat> onClone() const override
        {
            return std::unique_ptr<ImageFormat>(new Rgb16);
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

    private:
        /** @brief Encode 8-bit r/g/b to a 16-bit RGB565 value.
        */
        static Pt::uint16_t encode(Pt::uint8_t r, Pt::uint8_t g, Pt::uint8_t b)
        {
            return (Pt::uint16_t(r & 0xF8) << 8) |
                   (Pt::uint16_t(g & 0xFC) << 3) |
                   (Pt::uint16_t(b)      >> 3);
        }

        /** @brief Encode a ColorF (16-bit channels) to a 16-bit RGB565 value.
        */
        static Pt::uint16_t encodeColorF(const ColorF& c)
        {
              return Pt::uint16_t(c.red  () & 0xF800)        |
                    (Pt::uint16_t(c.green() & 0xFC00) >> 5)  |
                    (Pt::uint16_t(c.blue ()        )  >> 11);
        }
};

} // namespace

} // namespace

#include <Pt/Gfx/ImageView.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// Rgb16 - color conversion
///////////////////////////////////////////////////////////////////////

inline ColorF Rgb16::getColor(const Pt::uint8_t* p)
{
    const Pt::uint16_t pixel = *reinterpret_cast<const Pt::uint16_t*>(p);

    const Pt::uint16_t tr = (pixel & 0xF800) >> 11;
    const Pt::uint16_t tg = (pixel & 0x07E0) >>  5;
    const Pt::uint16_t tb =  pixel & 0x001F;

    // expand 5/6/5 to 16-bit range
    Pt::uint16_t a = 0xFFFF;
    Pt::uint16_t r = (tr << 11) | (tr << 6) | (tr << 1) | (tr >> 4);
    Pt::uint16_t g = (tg << 10) | (tg << 4) | (tg >> 2);
    Pt::uint16_t b = (tb << 11) | (tb << 6) | (tb << 1) | (tb >> 4);

    return ColorF(a, r, g, b);
}


inline Argb32Color Rgb16::getArgb32Color(const Pt::uint8_t* p)
{
    const Pt::uint16_t pixel = *reinterpret_cast<const Pt::uint16_t*>(p);

    Pt::uint8_t r5 = (pixel & 0xF800) >> 11;
    Pt::uint8_t g6 = (pixel & 0x07E0) >>  5;
    Pt::uint8_t b5 =  pixel & 0x001F;

    // expand 5/6/5 to 8-bit
    Pt::uint8_t r = (r5 << 3) | (r5 >> 2);
    Pt::uint8_t g = (g6 << 2) | (g6 >> 4);
    Pt::uint8_t b = (b5 << 3) | (b5 >> 2);

    return Argb32Color(255, r, g, b);
}


inline void Rgb16::getColors(const Pt::uint8_t* p, ColorF* colors, std::size_t n)
{
    for(std::size_t i = 0; i < n; ++i)
    {
        colors[i] = getColor(p);
        p += PixelWidth;
    }
}


inline void Rgb16::getColors(const Pt::uint8_t* p, Argb32Color* colors, std::size_t n)
{
    for(std::size_t i = 0; i < n; ++i)
    {
        colors[i] = getArgb32Color(p);
        p += PixelWidth;
    }
}

///////////////////////////////////////////////////////////////////////
// Rgb16 - SourceCopy
///////////////////////////////////////////////////////////////////////

inline void Rgb16::sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from)
{
    std::memcpy(to, from, PixelWidth);
}


inline void Rgb16::sourceCopy(Pt::uint8_t* to, const Argb32Color& from)
{
    Pt::uint16_t* dst = reinterpret_cast<Pt::uint16_t*>(to);
    *dst = encode(from.red(), from.green(), from.blue());
}


inline void Rgb16::sourceCopy(Pt::uint8_t* to, const ColorF& c)
{
    Pt::uint16_t* dst = reinterpret_cast<Pt::uint16_t*>(to);
    *dst = encodeColorF(c);
}


inline void Rgb16::sourceCopy(Pt::uint8_t* to, std::size_t length, const ColorF& c)
{
    const Pt::uint16_t val = encodeColorF(c);

    Pt::uint16_t* dst = reinterpret_cast<Pt::uint16_t*>(to);
    for(std::size_t i = 0; i < length; ++i) 
        *dst++ = val;
}


inline void Rgb16::sourceCopy(Pt::uint8_t* to, std::size_t length, const Argb32Color& c)
{
    const Pt::uint16_t val = encode(c.red(), c.green(), c.blue());

    Pt::uint16_t* dst = reinterpret_cast<Pt::uint16_t*>(to);
    for(std::size_t i = 0; i < length; ++i) 
        *dst++ = val;
}


inline void Rgb16::sourceCopy(Pt::uint8_t* to, std::size_t length, const Pt::uint8_t* from)
{
  const Pt::uint16_t val = *reinterpret_cast<const Pt::uint16_t*>(from);

    Pt::uint16_t* dst = reinterpret_cast<Pt::uint16_t*>(to);
    for(std::size_t i = 0; i < length; ++i) 
        *dst++ = val;
}


inline void Rgb16::sourceCopy(Pt::uint8_t* to, const ColorF* colors, std::size_t length)
{
    for(std::size_t n = 0; n < length; ++n)
    {
        Rgb16::sourceCopy(to, colors[n]);
        to += PixelWidth;
    }
}


inline void Rgb16::sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length)
{
    std::memcpy(to, from, length * PixelWidth);
}


inline void Rgb16::sourceCopy(Pt::uint8_t* to, Pt::ssize_t toStride,
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

///////////////////////////////////////////////////////////////////////
// Rgb16Pixel
///////////////////////////////////////////////////////////////////////

inline Rgb16Pixel::Rgb16Pixel(BasicImageView<Rgb16>& view, 
                              Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Rgb16::getPixel(view, view.data(), x, y) )
{
}


inline void Rgb16Pixel::reset(BasicImageView<Rgb16>& view, Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Rgb16::getPixel(view, view.data(), x, y);
}


inline void Rgb16Pixel::reset(const Rgb16Pixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Rgb16Pixel::advance()
{
    _base = Rgb16::advance(*_view, _base);
}


inline void Rgb16Pixel::skipPadding()
{
    _base = Rgb16::skipPadding(*_view, _base);
}


inline void Rgb16Pixel::advance(Pt::ssize_t n)
{
    _base = Rgb16::advance(*_view, _base, n);
}


inline void Rgb16Pixel::advanceLines(Pt::ssize_t n)
{
    _base = Rgb16::advanceLines(*_view, _base, n);
}


inline Rgb16Pixel& Rgb16Pixel::operator=(const Gfx::ColorF& color)
{ 
    Rgb16::sourceCopy(base(), color);
    return *this;
}


inline Rgb16Pixel& Rgb16Pixel::operator=(const Argb32Color& color)
{ 
    Rgb16::sourceCopy(base(), color);
    return *this;
}


inline void Rgb16Pixel::assign(const Rgb16Pixel& p)
{
    Rgb16::sourceCopy(base(), p.base());
}


inline void Rgb16Pixel::assign(const Rgb16ConstPixel& p)
{
    Rgb16::sourceCopy(base(), p.base());
}


inline void Rgb16Pixel::assign(const Rgb16Pixel& p, std::size_t length)
{
    Rgb16::sourceCopy(base(), p.base(), length);
}


inline void Rgb16Pixel::assign(const Rgb16ConstPixel& p, std::size_t length)
{
    Rgb16::sourceCopy(base(), p.base(), length);
}


inline void Rgb16Pixel::assign(const Argb32Color* colors, std::size_t length)
{
    Pt::uint8_t* p = _base;
    for(std::size_t i = 0; i < length; ++i)
    {
        Rgb16::sourceCopy(p, colors[i]);
        p += 2;
    }
}


inline void Rgb16Pixel::fill(std::size_t n, const Argb32Color& color)
{
    Rgb16::sourceCopy(base(), n, color);
}


inline bool Rgb16Pixel::equals(const Rgb16Pixel& p) const
{
    return base() == p.base();
}


inline bool Rgb16Pixel::equals(const Rgb16ConstPixel& p) const
{
    return base() == p.base();
}


inline Pt::uint8_t Rgb16Pixel::red() const
{
    const Pt::uint16_t pixel = *reinterpret_cast<const Pt::uint16_t*>(base());
    Pt::uint8_t r5 = (pixel & 0xF800) >> 11;
    return (r5 << 3) | (r5 >> 2);
}


inline Pt::uint8_t Rgb16Pixel::green() const
{
    const Pt::uint16_t pixel = *reinterpret_cast<const Pt::uint16_t*>(base());
    Pt::uint8_t g6 = (pixel & 0x07E0) >> 5;
    return (g6 << 2) | (g6 >> 4);
}


inline Pt::uint8_t Rgb16Pixel::blue() const
{
    const Pt::uint16_t pixel = *reinterpret_cast<const Pt::uint16_t*>(base());
    Pt::uint8_t b5 = pixel & 0x001F;
    return (b5 << 3) | (b5 >> 2);
}


inline Argb32Color Rgb16Pixel::toColor() const
{
    return Rgb16::getArgb32Color(base());
}

///////////////////////////////////////////////////////////////////////
// Rgb16ConstPixel
///////////////////////////////////////////////////////////////////////

inline Rgb16ConstPixel::Rgb16ConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                                        Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Rgb16::getPixel(view, data, x, y) )
{
}


inline Rgb16ConstPixel::Rgb16ConstPixel(const BasicConstImageView<Rgb16>& view,
                                        Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Rgb16::getPixel(view, view.data(), x, y) )
{
}


inline Rgb16ConstPixel::Rgb16ConstPixel(const BasicImageView<Rgb16>& view,
                                        Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Rgb16::getPixel(view, view.data(), x, y) )
{
}


inline Rgb16ConstPixel::Rgb16ConstPixel(const Rgb16Pixel& p)
: _view(p._view)
, _base(p._base)
{ 
}


inline void Rgb16ConstPixel::reset(const BasicConstImageView<Rgb16>& view, 
                                   Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Rgb16::getPixel(view, view.data(), x, y);
}


inline void Rgb16ConstPixel::reset(const BasicImageView<Rgb16>& view, 
                                   Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Rgb16::getPixel(view, view.data(), x, y);
}


inline void Rgb16ConstPixel::reset(const Rgb16ConstPixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Rgb16ConstPixel::reset(const Rgb16Pixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Rgb16ConstPixel::advance()
{
    _base = Rgb16::advance(*_view, _base);
}


inline void Rgb16ConstPixel::skipPadding()
{
    _base = Rgb16::skipPadding(*_view, _base);
}


inline void Rgb16ConstPixel::advance(Pt::ssize_t n)
{
    _base = Rgb16::advance(*_view, _base, n);
}


inline void Rgb16ConstPixel::advanceLines(Pt::ssize_t n)
{
    _base = Rgb16::advanceLines(*_view, _base, n);
}


inline Pt::uint8_t Rgb16ConstPixel::red() const
{
    const Pt::uint16_t pixel = *reinterpret_cast<const Pt::uint16_t*>(base());
    Pt::uint8_t r5 = (pixel & 0xF800) >> 11;
    return (r5 << 3) | (r5 >> 2);
}


inline Pt::uint8_t Rgb16ConstPixel::green() const
{
    const Pt::uint16_t pixel = *reinterpret_cast<const Pt::uint16_t*>(base());
    Pt::uint8_t g6 = (pixel & 0x07E0) >> 5;
    return (g6 << 2) | (g6 >> 4);
}


inline Pt::uint8_t Rgb16ConstPixel::blue() const
{
    const Pt::uint16_t pixel = *reinterpret_cast<const Pt::uint16_t*>(base());
    Pt::uint8_t b5 = pixel & 0x001F;
    return (b5 << 3) | (b5 >> 2);
}


inline Argb32Color Rgb16ConstPixel::toColor() const
{
    return Rgb16::getArgb32Color(base());
}


inline void Rgb16ConstPixel::getColors(Argb32Color* colors, std::size_t length) const
{
    Rgb16::getColors(base(), colors, length);
}


inline bool Rgb16ConstPixel::equals(const Rgb16ConstPixel& p) const
{
    return base() == p.base();
}


inline bool Rgb16ConstPixel::equals(const Rgb16Pixel& p) const
{
    return base() == p.base();
}

} // namespace

} // namespace

#endif
