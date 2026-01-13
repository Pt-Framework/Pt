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

#ifndef PT_GFX_IMAGEFORMAT_H
#define PT_GFX_IMAGEFORMAT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/BasicView.h>
#include <Pt/Gfx/CompositionMode.h>

namespace Pt {

namespace Gfx {

class ImageFormat;
class PixelBase;
class ConstPixelBase;

/** @brief Pixel reference.
*/
class PixelBase
{
    friend class ImageFormat;
    friend class ConstPixelBase;

    public:
        typedef BasicView<ImageFormat> View;

    public:
        PixelBase(View& view, Pt::ssize_t x, Pt::ssize_t y);

        PixelBase(const PixelBase& p)
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

        bool operator!=(const PixelBase& p) const;

        bool operator!=(const ConstPixelBase& p) const;

        bool operator==(const PixelBase& p) const;

        bool operator==(const ConstPixelBase& p) const;

    private:
        Pt::uint8_t*  _base;
        Pt::ssize_t   _x;
        Pt::ssize_t   _y;
};

/** @brief Pixel const reference.
*/
class ConstPixelBase
{
    friend class ImageFormat;
    friend class PixelBase;

    public:
        typedef BasicView<ImageFormat> View;

    public:
        ConstPixelBase(const View& view, Pt::ssize_t x, Pt::ssize_t y);

        ConstPixelBase(const ConstPixelBase& p)
        : _base(p._base)
        , _x(p._x)
        , _y(p._y)
        {  }

        ConstPixelBase(const PixelBase& p)
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

        bool operator!=(const PixelBase& p) const;

        bool operator!=(const ConstPixelBase& p) const;

        bool operator==(const PixelBase& p) const;

        bool operator==(const ConstPixelBase& p) const;

    private:
        const Pt::uint8_t* _base;
        Pt::ssize_t        _x;
        Pt::ssize_t        _y;
};


/** @brief %Image format.
*/
class ImageFormat
{
    public:
        PT_GFX_API static const ImageFormat& rgb16();

        PT_GFX_API static const ImageFormat& rgb32();

        PT_GFX_API static const ImageFormat& argb32();

    public:
        typedef PixelBase        Pixel;
        typedef ConstPixelBase   ConstPixel;

        typedef BasicView<ImageFormat>       View;

    public:
        explicit /*constexpr*/ ImageFormat(size_t pixelStride)
        : _pixelStride(pixelStride)
        { }

        virtual ~ImageFormat() 
        {}

        /** @brief Returns the distance between two pixel base pointers in bytes.
        */
        std::size_t pixelStride() const
        {
            return _pixelStride;
        }

        bool operator==(const ImageFormat& a) const
        {
          return typeid(*this) == typeid(a);
        }

        bool operator!=(const ImageFormat& a) const
        {
          return typeid(*this) != typeid(a);
        }

    public:
        /** @brief Returns the size in bytes for a given image size.
        */
        std::size_t imageSize(Pt::ssize_t width, Pt::ssize_t height,
                              std::size_t padding) const
        { 
            return onImageSize(width, height, padding); 
        }

        bool hasAlpha() const
        {
            return onHasAlpha();
        }

        void getRect(const ViewBase& view, Pt::ssize_t x, Pt::ssize_t y,
                     Pt::ssize_t width, Pt::ssize_t height,
                     Pt::uint32_t* to, Pt::ssize_t toStride) const
        {
            onGetRect(view, x, y, width, height, to, toStride);
        }

        void setRect(ViewBase& view, Pt::ssize_t x, Pt::ssize_t y,
                     Pt::ssize_t width, Pt::ssize_t height,
                     const Pt::uint32_t* from, Pt::ssize_t fromStride) const
        {
            onSetRect(view, x, y, width, height, from, fromStride);
        }

    public:
        void advance(const View& view, PixelBase& p) const
        {
            if( ++p._x >= view.width() )
            {
                p._x = 0;
                ++p._y;

                p._base += view.padding();
            }

            p._base += view.pixelStride();
        }

        void advance(const View& view, ConstPixelBase& p) const
        {
            if( ++p._x >= view.width() )
            {
                p._x = 0;
                ++p._y;

                p._base += view.padding();
            }

            p._base += view.pixelStride();
        }

        void advance(const View& view, PixelBase& p, Pt::ssize_t n) const
        {
            Pt::ssize_t off = p._x + n;

            std::size_t dy = off / view.width();
            std::size_t dx = off % view.width() - p._x;

            p._x += dx;
            p._y += dy;
            p._base += dy * view.stride() + dx * view.pixelStride();
        }

        void advance(const View& view, ConstPixelBase& p, Pt::ssize_t n) const
        {
            Pt::ssize_t off = p._x + n;

            std::size_t dy = off / view.width();
            std::size_t dx = off % view.width() - p._x;

            p._x += dx;
            p._y += dy;
            p._base += dy * view.stride() + dx * view.pixelStride();
        }

        /** @brief Get pixel color.
        */
        Color getColor(const View& view, const PixelBase& p) const
        {
            return onGetColor( view, p.base(), p.x(), p.y() ); 
        }

        Color getColor(const View& view, const ConstPixelBase& p) const
        {
            return onGetColor( view, p.base(), p.x(), p.y() ); 
        }

    public:
        /** @brief Assign pixels.
        */
        void sourceCopy(View& view, PixelBase& to, 
                        const View& from, const PixelBase& p) const
        {
            onSourceCopy(view, to, from, p.base(), p.x(), p.y());
        }

        void sourceCopy(View& view, PixelBase& to, 
                        const View& from, const ConstPixelBase& p) const
        {
            onSourceCopy(view, to, from, p.base(), p.x(), p.y());
        }

        void sourceOver(View& view, PixelBase& to, 
                        const View& from, const PixelBase& p) const
        {
            onSourceOver(view, to, from, p.base(), p.x(), p.y());
        }
        
        void sourceOver(View& view, PixelBase& to, 
                        const View& from, const ConstPixelBase& p) const
        {
            onSourceOver(view, to, from, p.base(), p.x(), p.y());
        }
        
        /** @brief Assign pixels.
        */
        void sourceCopy(View& view, PixelBase& to, const Color& c) const
        {
            onSourceCopy(view, to, c);
        }

        void sourceOver(View& view, PixelBase& to, const Color& c) const
        {
            onSourceOver(view, to, c);
        }

        /** @brief Fill pixels.
        */
        void sourceCopy(View& view, PixelBase& to, std::size_t n, const Color& c) const
        { 
            onSourceCopy(view, to, n, c); 
        }

        void sourceOver(View& view, PixelBase& to, std::size_t n, const Color& c) const
        { 
            onSourceOver(view, to, n, c); 
        }

        /** @brief Fill pixels.
        */
        void sourceCopy(View& view, PixelBase& to, std::size_t n, 
                        const View& from, const ConstPixelBase& p) const
        { 
            onSourceCopy(view, to, n, from, p.base(), p.x(), p.y());
        }

        void sourceOver(View& view, PixelBase& to, std::size_t n, 
                        const View& from, const ConstPixelBase& p) const
        { 
            onSourceOver(view, to, n, from, p.base(), p.x(), p.y()); 
        }

        /** @brief Copy pixels.
        */
        void sourceCopy(View& view, PixelBase& to, 
                        const View& from, const ConstPixelBase& p, std::size_t n) const
        { 
            onSourceCopy(view, to, from, p.base(), p.x(), p.y(), n);
        }

        void sourceOver(View& view, PixelBase& to, 
                        const View& from, const ConstPixelBase& p, std::size_t n) const
        { 
            onSourceOver(view, to, from, p.base(), p.x(), p.y(), n); 
        }

        void sourceCopy(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                        const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                        Pt::ssize_t width, Pt::ssize_t height) const
        {
            onSourceCopy(to, toX, toY, from, fromX, fromY, width, height);
        }

        void sourceOver(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                        const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                        Pt::ssize_t width, Pt::ssize_t height) const
        {
            onSourceOver(to, toX, toY, from, fromX, fromY, width, height);
        }

    protected:
        virtual std::size_t onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                        std::size_t padding) const = 0;

        virtual bool onHasAlpha() const
        {
            return false;
        }

        virtual void onGetRect(const ViewBase& view, Pt::ssize_t x, Pt::ssize_t y,
                               Pt::ssize_t width, Pt::ssize_t height,
                               Pt::uint32_t* to, Pt::ssize_t toStride) const
        { }


        virtual void onSetRect(ViewBase& view, Pt::ssize_t x, Pt::ssize_t y,
                               Pt::ssize_t width, Pt::ssize_t height,
                               const Pt::uint32_t* from, Pt::ssize_t fromStride) const
        { }

    protected:
        virtual Color onGetColor(const View& view, const Pt::uint8_t* base, 
                                 Pt::ssize_t x, Pt::ssize_t y) const = 0;

    protected:
        /** @brief Assign pixels.
        */
        virtual void onSourceCopy(View& view, PixelBase& to, const Color& c) const = 0;
        
        virtual void onSourceOver(View& view, PixelBase& to, const Color& c) const = 0;
        
        /** @brief Assign pixels.
        */
        virtual void onSourceCopy(View& to, PixelBase& pos,
                                  const View& from, const Pt::uint8_t* base,
                                  Pt::ssize_t x, Pt::ssize_t y) const = 0;

        virtual void onSourceOver(View& to, PixelBase& pos,
                                  const View& from, const Pt::uint8_t* base,
                                  Pt::ssize_t x, Pt::ssize_t y) const = 0;
        /** @brief Fill pixels.
        */
        virtual void onSourceCopy(View& view, PixelBase& to, 
                                  std::size_t n, const Color& c) const = 0;

        virtual void onSourceOver(View& view, PixelBase& to, 
                                  std::size_t n, const Color& c) const = 0;
        
        /** @brief Fill pixels.
        */
        virtual void onSourceCopy(View& view, PixelBase& to, std::size_t n, 
                                  const View& from, const Pt::uint8_t* base,
                                  Pt::ssize_t x, Pt::ssize_t y) const = 0;

        virtual void onSourceOver(View& view, PixelBase& to, std::size_t n, 
                                  const View& from, const Pt::uint8_t* base,
                                  Pt::ssize_t x, Pt::ssize_t y) const = 0;

        /** @brief Copy pixels.
        */
        virtual void onSourceCopy(View& view, PixelBase& to, 
                                  const View& from, const Pt::uint8_t* base,
                                  Pt::ssize_t x, Pt::ssize_t y, std::size_t n) const = 0;

        virtual void onSourceOver(View& view, PixelBase& to, 
                                  const View& from, const Pt::uint8_t* base,
                                  Pt::ssize_t x, Pt::ssize_t y, std::size_t n) const = 0;

        virtual void onSourceCopy(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                                  const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                                  Pt::ssize_t width, Pt::ssize_t height) const = 0;

        virtual void onSourceOver(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                                  const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                                  Pt::ssize_t width, Pt::ssize_t height) const = 0;
    private:
        const std::size_t _pixelStride;
};

///////////////////////////////////////////////////////////////////////
// PixelBase
///////////////////////////////////////////////////////////////////////

inline PixelBase::PixelBase(View& view, Pt::ssize_t x, Pt::ssize_t y)
: _base(0)
, _x(x)
, _y(y)
{
    _base = view.data() + view.stride() * y + x * view.pixelStride();
}


inline bool PixelBase::operator!=(const PixelBase& p) const
{ 
    return _base != p._base; 
}


inline bool PixelBase::operator!=(const ConstPixelBase& p) const
{ 
    return _base != p._base; 
}


inline bool PixelBase::operator==(const PixelBase& p) const
{ 
    return _base == p._base; 
}


inline bool PixelBase::operator==(const ConstPixelBase& p) const
{ 
    return _base == p._base; 
}

///////////////////////////////////////////////////////////////////////
// ConstPixelBase
///////////////////////////////////////////////////////////////////////

inline ConstPixelBase::ConstPixelBase(const View& view, Pt::ssize_t x, Pt::ssize_t y)
: _base(0)
, _x(x)
, _y(y)
{
    _base = view.data() + view.stride() * y + x * view.pixelStride();
}


inline bool ConstPixelBase::operator!=(const PixelBase& p) const
{ 
    return _base != p._base; 
}


inline bool ConstPixelBase::operator!=(const ConstPixelBase& p) const
{ 
    return _base != p._base; 
}


inline bool ConstPixelBase::operator==(const PixelBase& p) const
{ 
    return _base == p._base; 
}


inline bool ConstPixelBase::operator==(const ConstPixelBase& p) const
{ 
    return _base == p._base; 
}

} // namespace

} // namespace

#endif
