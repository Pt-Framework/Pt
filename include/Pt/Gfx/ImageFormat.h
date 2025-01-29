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
class PixelIndex;
class ImageFormat;

class ConstPixel;

/** @brief Pixel in an image.
*/
class Pixel
{
    friend class ConstPixel;

    public:
        typedef BasicView<ImageFormat> View;

    public:
        Pixel(View& view, Pt::ssize_t x, Pt::ssize_t y);

        Pixel(const Pixel& p)
        : _view(p._view)
        , _base(p._base)
        , _x(p._x)
        , _y(p._y)
        {  }

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

        void reset(View& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const Pixel& p)
        {
             _view = p._view;
             _base = p._base;
             _x = p._x;
             _y = p._y;
        }

        void advance();

        void advance( Pt::ssize_t n );

        void assign(const Color& color, CompositionMode mode);

        void assign(const Pixel& p, CompositionMode mode);

        void assign(const ConstPixel& p, CompositionMode mode);

        Color getColor() const;

        const View& view() const
        { return *_view; }

        View& view()
        { return *_view; }

        Pt::ssize_t x() const
        { return _x; }

        Pt::ssize_t y() const
        { return _y; }

        Pt::uint8_t* base()
        { return _base; }

        const Pt::uint8_t* base() const
        { return _base; }

        bool operator!=(const Pixel& p) const
        {  return _base != p._base; }

        bool operator==(const Pixel& p) const
        { return _base == p._base; }

    private:
        View*            _view;
        Pt::uint8_t*     _base;
        Pt::ssize_t      _x;
        Pt::ssize_t      _y;
};

/** @brief Const pixel in an image.
*/
class ConstPixel
{
    friend class Pixel;

    public:
        typedef BasicView<ImageFormat> View;

    public:
        ConstPixel(const View& view, Pt::ssize_t x, Pt::ssize_t y);

        ConstPixel(const ConstPixel& p)
        : _view(p._view)
        , _base(p._base)
        , _x(p._x)
        , _y(p._y)
        {  }

        ConstPixel(const Pixel& p);

        void reset(const View& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const ConstPixel& p)
        {
             _view = p._view;
             _base = p._base;
             _x = p._x;
             _y = p._y;
        }

        void reset(const Pixel& p)
        {
             _view = p._view;
             _base = p._base;
             _x = p._x;
             _y = p._y;
        }

        void advance();

        void advance( Pt::ssize_t n );

        Color getColor() const;

        const View& view() const
        { return *_view; }

        Pt::ssize_t x() const
        { return _x; }

        Pt::ssize_t y() const
        { return _y; }

        const Pt::uint8_t* base() const
        { return _base; }

        bool operator!=(const ConstPixel& p) const
        {  return _base != p._base; }

        bool operator==(const ConstPixel& p) const
        { return _base == p._base; }

    private:
        const View*        _view;
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

        PT_GFX_API static const ImageFormat& instance();

    public:
        typedef BasicView<ImageFormat> View;
        typedef Pt::Gfx::Pixel Pixel;
        typedef Pt::Gfx::ConstPixel ConstPixel;

        //typedef BasicPixel<ImageFormat>      Pixel;
        //typedef BasicConstPixel<ImageFormat> ConstPixel;

    public:
        explicit ImageFormat(size_t pixelStride)
        : _pixelStride(pixelStride)
        { }

        virtual ~ImageFormat()
        {}

        /** @brief Returns distance in bytes between two pixel base pointers.
        */
        std::size_t pixelStride() const
        {
            return _pixelStride;
        }

        bool operator==(const ImageFormat& a) const
        {
          return _pixelStride == a._pixelStride;
        }

        bool operator!=(const ImageFormat& a) const
        {
          return _pixelStride != a._pixelStride;
        }

    public:
        /** @brief Returns the size in bytes for a given image size.
        */
        std::size_t imageSize(Pt::ssize_t width, Pt::ssize_t height,
                              std::size_t padding) const
        { 
            return onImageSize(width, height, padding); 
        }

    public:
        /** @brief Gets the pixel color.
        */
        Color getColor(const View& view, const Pt::uint8_t* base, 
                       Pt::ssize_t x, Pt::ssize_t y) const
        { 
            return onGetColor(view, base, x, y); 
        }
        
        /** @brief Assign.
        */
        void sourceCopy(View& to, PixelIndex& pos, const Color& c) const
        { 
            onSourceCopy(to, pos, c); 
        }

        /** @brief Assign.
        */
        void sourceOver(View& to, PixelIndex& pos, const Color& c) const
        { 
            onSourceOver(to, pos, c); 
        }

        /** @brief Assign.
        */
        void sourceCopy(View& to, const PixelIndex& pos,
                        const View& from, const Pt::uint8_t* base,
                        Pt::ssize_t x, Pt::ssize_t y) const
        { 
            onSourceCopy(to, pos, from, base, x, y);
        }

        /** @brief Assign.
        */
        void sourceOver(View& to, const PixelIndex& pos,
                        const View& from, const Pt::uint8_t* base,
                        Pt::ssize_t x, Pt::ssize_t y) const
        { 
            onSourceOver(to, pos, from, base, x, y); 
        }

    protected:
        virtual std::size_t onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                        std::size_t padding) const = 0;

    protected:
        virtual Color onGetColor(const View& view, const Pt::uint8_t* base, 
                                 Pt::ssize_t x, Pt::ssize_t y) const
        { throw 99; }

        /** @brief Assign.
        */
        void onSourceCopy(View& view, PixelIndex& to, const Color& c) const
        {
            throw 99;
        }
        
        /** @brief Assign.
        */
        void onSourceOver(View& view, PixelIndex& to, const Color& c) const
        {
            throw 99;
        }

        /** @brief Assign.
        */
        virtual void onSourceCopy(View& to, const PixelIndex& pos,
                                  const View& from, const Pt::uint8_t* base,
                                  Pt::ssize_t x, Pt::ssize_t y) const
        {
            throw 99;
        }

        /** @brief Assign.
        */
        virtual void onSourceOver(View& to, const PixelIndex& pos,
                                  const View& from, const Pt::uint8_t* base,
                                  Pt::ssize_t x, Pt::ssize_t y) const
        {
            throw 99;
        }

    public:

        /** @brief Gets the pixel color.
        */
        Color getColor(const ConstPixel& pixel) const
        { 
            return onGetColor(pixel); 
        }

        Color getColor(const Pixel& pixel) const
        { 
            //return onGetColor(pixel);
            return onGetColor( ConstPixel(pixel) );
            //return onGetColor( pixel.view(), pixel.base(), pixel.x(), pixel.y() ); 
        }

        /** @brief Assign.
        */
        void sourceCopy(Pixel& to, const Color& c) const
        { 
            onSourceCopy(to, c); 
        }

        /** @brief Assign.
        */
        void sourceOver(Pixel& to, const Color& c) const
        { 
            onSourceOver(to, c); 
        }

        /** @brief Assign.
        */
        void sourceCopy(Pixel& to, const ConstPixel& p) const
        { 
            onSourceCopy(to, p);
        }

        /** @brief Assign.
        */
        void sourceOver(Pixel& to, const ConstPixel& p) const
        { 
            onSourceOver(to, p); 
        }

        /** @brief Fill.
        */
        void sourceCopy(Pixel& to, std::size_t n, const Color& c) const
        { 
            onSourceCopy(to, n, c); 
        }

        /** @brief Fill.
        */
        void sourceOver(Pixel& to, std::size_t n, const Color& c) const
        { 
            onSourceOver(to, n, c); 
        }

        /** @brief Fill.
        */
        void sourceCopy(Pixel& to, std::size_t n, const ConstPixel& p) const
        { 
            onSourceCopy(to, n, p);
        }

        /** @brief Fill.
        */
        void sourceOver(Pixel& to, std::size_t n, const ConstPixel& p) const
        { 
            onSourceOver(to, n, p); 
        }

        /** @brief Copy.
        */
        void sourceCopy(Pixel& to, const ConstPixel& p, std::size_t n) const
        { 
            onSourceCopy(to, p, n);
        }

        /** @brief Copy.
        */
        void sourceOver(Pixel& to, const ConstPixel& p, std::size_t n) const
        { 
            onSourceOver(to, p, n); 
        }

        /** @brief Copy.
        */
        void sourceCopy(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                        const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                        Pt::ssize_t width, Pt::ssize_t height) const
        {
            onSourceCopy(to, toX, toY, from, fromX, fromY, width, height);
        }

        /** @brief Copy.
        */
        void sourceOver(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                        const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                        Pt::ssize_t width, Pt::ssize_t height) const
        {
            onSourceOver(to, toX, toY, from, fromX, fromY, width, height);
        }

    protected:
        virtual Color onGetColor(const Pixel& pixel) const = 0;

        virtual Color onGetColor(ConstPixel pixel) const
        { throw 42; return Color(); }

        
        virtual void onSourceCopy(Pixel& pixel, const Color& c) const = 0;

        virtual void onSourceOver(Pixel& pixel, const Color& c) const = 0;

        // experimental
        virtual void onSourceCopy(View& to, Pt::ssize_t toX, Pt::ssize_t toY, 
                                  const Color& c) const
        { throw 42; }


        virtual void onSourceCopy(Pixel& to, const ConstPixel& pixel) const
        { throw 42; }

        virtual void onSourceOver(Pixel& to, const ConstPixel& pixel) const
        {  throw 42; }


        virtual void onSourceCopy(Pixel& pixel, std::size_t n, const Color& c) const
        { throw 42; }

        virtual void onSourceOver(Pixel& pixel, std::size_t n, const Color& c) const
        { throw 42; }

        
        virtual void onSourceCopy(Pixel& to, std::size_t n, const ConstPixel& pixel) const
        { throw 42; }

        virtual void onSourceOver(Pixel& t, std::size_t no, const ConstPixel& pixel) const
        {  throw 42; }


        virtual void onSourceCopy(Pixel& to, const ConstPixel& p, size_t length) const
        {  throw 42; }

        virtual void onSourceOver(Pixel& to, const ConstPixel& p, size_t length) const
        {  throw 42; }


        virtual void onSourceCopy(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                                  const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                                  Pt::ssize_t width, Pt::ssize_t height) const
        {  throw 42; }

        virtual void onSourceOver(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                                  const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                                  Pt::ssize_t width, Pt::ssize_t height) const
        {  throw 42; }




        //virtual void onSetPixels(const PixelIndex& to, Pt::uint8_t* base, 
        //                         const Color& c, std::size_t n, 
        //                         CompositionMode  mode) const 
        //                         {}

        //virtual void onCopySpan(PixelIndex& to, Pt::uint8_t* toBase, 
        //                        const PixelIndex& from, const Pt::uint8_t* fromBase,
        //                        std::size_t n, CompositionMode mode) const
        //                        {}

        //virtual void onCopyArea(PixelIndex& to, Pt::uint8_t* toBase, 
        //                        const PixelIndex& from, const Pt::uint8_t* fromBase,
        //                        std::size_t w, std::size_t h, CompositionMode mode) const
        //                        {}



    public:
        /** @brief Sets the pixel color with additional blending alhpa.
        */
        void setPixel(Pixel& to, const Pixel& from,
                      CompositionMode mode, Pt::uint8_t blendingAlpha) const
        { onSetPixel(to, from, mode, blendingAlpha); }

        /** @brief Sets the pixel color with additional blending alhpa.
        */
        void setPixel(Pixel& to, const ConstPixel& from,
                      CompositionMode mode, Pt::uint8_t blendingAlpha) const
        { onSetPixel(to, from, mode, blendingAlpha); }

        /** @brief Sets the pixel color with additional blending alhpa.
        */
        void setPixel(Pixel& to, const Color& c,
                      CompositionMode mode, Pt::uint8_t blendingAlpha) const
        { onSetPixel(to, c, mode, blendingAlpha); }

    protected:
        virtual void onSetPixel(Pixel& to, const Pixel& from,
                                CompositionMode mode, Pt::uint8_t blendingAlpha) const = 0;

        virtual void onSetPixel(Pixel& to, const ConstPixel& from,
                                CompositionMode mode, Pt::uint8_t blendingAlpha) const = 0;

        virtual void onSetPixel(Pixel& pixel, const Color& c,
                                CompositionMode mode, Pt::uint8_t blendingAlpha) const = 0;

    private:
        std::size_t _pixelStride;
};


class PixelIndex
{
    friend class PixelTraits;

    public:
        typedef ImageFormat       Format;
        typedef BasicView<Format> View;

    public:
        PixelIndex(View& view, Pt::ssize_t x, Pt::ssize_t y)
        : _base(0)
        , _x(x)
        , _y(y)
        {
            _base = view.data() + view.stride() * y + x * view.pixelStride();
        }

        PixelIndex(const PixelIndex& p)
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

    private:
        Pt::uint8_t*  _base;
        Pt::ssize_t   _x;
        Pt::ssize_t   _y;
};


class ConstPixelIndex
{
    friend class PixelTraits;

    public:
        typedef ImageFormat       Format;
        typedef BasicView<Format> View;

    public:
        ConstPixelIndex(const View& view, Pt::ssize_t x, Pt::ssize_t y)
        : _base(0)
        , _x(x)
        , _y(y)
        {
            _base = view.data() + view.stride() * y + x * view.pixelStride();
        }

        ConstPixelIndex(const ConstPixelIndex& p)
        : _base(p._base)
        , _x(p._x)
        , _y(p._y)
        {  }

        ConstPixelIndex(const PixelIndex& p)
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

    private:
        const Pt::uint8_t* _base;
        Pt::ssize_t        _x;
        Pt::ssize_t        _y;
};


class PixelTraits
{
    public:
        typedef ImageFormat        Format;
        typedef BasicView<Format>  View;
        
        typedef PixelIndex         Index;
        typedef ConstPixelIndex    ConstIndex;

    public:
        template <typename P>
        static void getColor(const View& view, const P& p)
        {
            view.format()->getColor( view, p.base(), p.x(), p.y( ));
        }

        template <typename P>
        static void advance(const View& view, P& p)
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
        void advance(const View& view, P& p, Pt::ssize_t n)
        {
            Pt::ssize_t off = p._x + n;

            std::size_t dy = off / view.width();
            std::size_t dx = off % view.width() - p._x;

            p._x += dx;
            p._y += dy;
            p._base += dy * view.stride() + dx * view.pixelStride();
        }

        template <typename P>
        static void sourceCopy(View& view, PixelIndex& to, 
                               const View& from, const P& p)
        {
            view.format()->sourceCopy(view, to, from, p.base(), p.x(), p.y());
        }

        template <typename P>
        static void sourceOver(View& view, PixelIndex& to, 
                               const View& from, const P& p)
        {
            view.format()->sourceOver(view, to, from, p.base(), p.x(), p.y());
        }

        static void sourceCopy(View& view, PixelIndex& to, const Color& c)
        {
            view.format()->sourceCopy(view, to, c);
        }
        
        static void sourceOver(View& view, PixelIndex& to, const Color& c)
        {
            view.format()->sourceOver(view, to, c);
        }

        template <typename P>
        static bool equals(const P& p1, const P& p2)
        {
            return p1._base == p2._base;
        }
};

/////////////////////////////////////////////////////////////////////////////
// Pixel Implementation
/////////////////////////////////////////////////////////////////////////////

inline Pixel::Pixel(View& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
{
    _base = view.data() + view.stride() * y + x * view.pixelStride();
}


inline void Pixel::reset(View& view, Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _x = x;
    _y = y;

    _base = view.data() + view.stride() * _y + _x * view.pixelStride();
}


inline void Pixel::advance()
{
    if( ++_x >= _view->width() )
    {
        _x = 0;
        ++_y;

        _base += _view->padding();
    }

    _base += _view->pixelStride();
}


inline void Pixel::advance(Pt::ssize_t n)
{
    Pt::ssize_t off = _x + n;

    std::size_t dy = off / _view->width();
    std::size_t dx = off % _view->width() - _x;

    _x += dx;
    _y += dy;
    _base += dy * _view->stride() + dx * _view->pixelStride();
}


inline void Pixel::assign(const Color& color, CompositionMode mode)
{
    _view->assign(*this, color, mode);
}


inline void Pixel::assign(const Pixel& p, CompositionMode mode)
{
    _view->assign(*this, p, mode);
}


inline void Pixel::assign(const ConstPixel& p, CompositionMode mode)
{
    _view->assign(*this, p, mode);
}


inline Color Pixel::getColor() const
{
    return _view->format()->getColor(*this);
}

/////////////////////////////////////////////////////////////////////////////
// ConstPixel Implementation
/////////////////////////////////////////////////////////////////////////////

inline ConstPixel::ConstPixel(const View& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
{
    _base = view.data() + view.stride() * y + x * view.pixelStride();
}


inline ConstPixel::ConstPixel(const Pixel& p)
: _view(p._view)
, _base(p._base)
, _x(p._x)
, _y(p._y)
{  }


inline void ConstPixel::reset(const View& view, Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _x = x;
    _y = y;

    _base = view.data() + view.stride() * _y + _x * view.pixelStride();
}


inline void ConstPixel::advance()
{
    if( ++_x >= _view->width() )
    {
        _x = 0;
        ++_y;

        _base += _view->padding();
    }

    _base += _view->pixelStride();
}


inline void ConstPixel::advance(Pt::ssize_t n)
{
    Pt::ssize_t off = _x + n;

    std::size_t dy = off / _view->width();
    std::size_t dx = off % _view->width() - _x;

    _x += dx;
    _y += dy;
    _base += dy * _view->stride() + dx * _view->pixelStride();
}


inline Color ConstPixel::getColor() const
{
    return _view->format()->getColor(*this);
}

} // namespace

} // namespace

#endif
