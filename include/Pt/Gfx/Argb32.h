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
        template <typename T>
        Argb32Pixel(T& view, Pt::ssize_t x, Pt::ssize_t y);

        Argb32Pixel(const Argb32Pixel& p) = default;

        ~Argb32Pixel() = default;

        Argb32Pixel& operator=(const Argb32Color& color);

        Argb32Pixel& operator=(const Gfx::ColorF& color);

        template <typename T>
        void reset(T& view, Pt::ssize_t x, Pt::ssize_t y);

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

        void skipPadding();

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
        template <typename T>
        Argb32ConstPixel(const T& view, Pt::ssize_t x, Pt::ssize_t y);

        template <typename T>
        Argb32ConstPixel(T& view, Pt::ssize_t x, Pt::ssize_t y);

        Argb32ConstPixel(const Argb32ConstPixel& p) = default;

        Argb32ConstPixel(const Argb32Pixel& p);

        ~Argb32ConstPixel() = default;

        template <typename T>
        void reset(const T& view, Pt::ssize_t x, Pt::ssize_t y);

        template <typename T>
        void reset(T& view, Pt::ssize_t x, Pt::ssize_t y);

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

        void skipPadding();

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

        static void setColor(Pt::uint8_t* to, const Argb32Color& from);

        static void setColor(Pt::uint8_t* to, const ColorF& c);

        static void setColor(Pt::uint8_t* to, std::size_t length, const Argb32Color& c);

        static void setColor(Pt::uint8_t* to, std::size_t length, const ColorF& c);

        static void setColors(Pt::uint8_t* to, const Argb32Color* colors, std::size_t length);

        static void setColors(Pt::uint8_t* to, const ColorF* colors, std::size_t length);

        static void copy(Pt::uint8_t* to, const Pt::uint8_t* from);

        static void copy(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length);

        //
        // SourceCopy
        //
        static void sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from);

        static void sourceCopy(Pt::uint8_t* to, std::size_t length, const Pt::uint8_t* from);

        static void sourceCopy(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length);

        //
        // SourceOver
        //
        static void sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from);
        
        //static void sourceOver(Pt::uint8_t* to, const Pt::Gfx::ColorF& from);

        //static void sourceOver(Pt::uint8_t* to, std::size_t length, const ColorF& c);

        static void sourceOver(Pt::uint8_t* to, std::size_t length, const Pt::uint8_t* from);

        static void sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length);
};

} // namespace

} // namespace

#include <Pt/Gfx/Argb32.hpp>

#endif
