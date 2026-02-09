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
#include <Pt/Gfx/BasicView.h>
#include <Pt/Gfx/Color.h>
#include <Pt/TypeInfo.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// PixelBase
///////////////////////////////////////////////////////////////////////

class PixelBase;
class ConstPixelBase;


class PixelBase
{
    public:
        PixelBase(Pt::uint8_t* base, Pt::ssize_t x, Pt::ssize_t y)
        : _base(base)
        , _x(x)
        , _y(y)
        { }

        virtual ~PixelBase()
        { }

        Pt::uint8_t* base() const
        { return _base; }

        Pt::ssize_t xpos() const
        { return _x; }

        Pt::ssize_t ypos() const
        { return _y; }

        Pt::uint8_t* advance()
        { 
            _base =  onAdvance(_x, _y);
            return _base;
        }

        Pt::uint8_t* advance(Pt::ssize_t n)
        { 
            _base = onAdvance(_x, _y, n); 
            return _base;
        }

        template <typename ColorT>
        ColorT getColor() const;

        void assign(const Color& color)
        { 
            onSetColor(color);
        }

        void assign(const Argb32Color& color)
        { 
            onSetColor(color);
        }
        
        bool assign(const ConstPixelBase& p, std::size_t length)
        {
            return onAssignPixels(p, length);
        }

        void fill(std::size_t n, const Color& color)
        {
            onFillColor(n, color);
        }

    protected:
        virtual Pt::uint8_t* onAdvance(Pt::ssize_t& xpos, Pt::ssize_t& ypos) = 0;

        virtual Pt::uint8_t* onAdvance(Pt::ssize_t& xpos, Pt::ssize_t& ypos,
                                       Pt::ssize_t n) = 0;

        virtual Color onGetColor() const = 0;

        virtual Argb32Color onGetArgb32Color() const
        { return Argb32Color(); }

        virtual void onSetColor(const Color& color) = 0;

        virtual void onSetColor(const Argb32Color& color)
        { }

        virtual void onFillColor(std::size_t n, const Color& color) = 0;

        virtual bool onAssignPixels(const ConstPixelBase& p, std::size_t length)
        { return false; }

    private:
        Pt::uint8_t* _base;
        Pt::ssize_t  _x;
        Pt::ssize_t  _y;
};


template <>
inline Color PixelBase::getColor<Color>() const
{
    return this->onGetColor();
}


template <>
inline Argb32Color PixelBase::getColor<Argb32Color>() const
{
    return this->onGetArgb32Color();
}

///////////////////////////////////////////////////////////////////////
// ConstPixelBase
///////////////////////////////////////////////////////////////////////

class ConstPixelBase
{
    public:
        ConstPixelBase(const Pt::uint8_t* base, Pt::ssize_t x, Pt::ssize_t y)
        : _base(base)
        , _x(x)
        , _y(y)
        { }

        virtual ~ConstPixelBase()
        {}

        const Pt::uint8_t* base() const
        { return _base; }

        Pt::ssize_t xpos() const
        { return _x; }

        Pt::ssize_t ypos() const
        { return _y; }

        const Pt::uint8_t* advance()
        { 
            _base =  onAdvance(_x, _y);
            return _base;
        }

        const Pt::uint8_t* advance(Pt::ssize_t n)
        { 
            _base = onAdvance(_x, _y, n); 
            return _base;
        }

        template <typename ColorT>
        ColorT getColor() const;

        bool copy(PixelBase& p, std::size_t length) const
        {
            return onCopyPixels(p, length);
        }

    protected:
        virtual const Pt::uint8_t* onAdvance(Pt::ssize_t& xpos, Pt::ssize_t& ypos) = 0;

        virtual const Pt::uint8_t* onAdvance(Pt::ssize_t& xpos, Pt::ssize_t& ypos,
                                             Pt::ssize_t n) = 0;

        virtual Color onGetColor() const = 0;

        virtual Argb32Color onGetArgb32Color() const
        { return Argb32Color(); }

        virtual bool onCopyPixels(PixelBase& p, std::size_t length) const
        { return false; }

    private:
        const Pt::uint8_t*   _base;
        Pt::ssize_t          _x;
        Pt::ssize_t          _y;
};


template <>
inline Color ConstPixelBase::getColor<Color>() const
{
    return this->onGetColor();
}


template <>
inline Argb32Color ConstPixelBase::getColor<Argb32Color>() const
{
    return this->onGetArgb32Color();
}

///////////////////////////////////////////////////////////////////////
// PixelStorage
///////////////////////////////////////////////////////////////////////

class PixelStorage
{
    public:
        static const std::size_t MaxSize = 128;
    
    public:
        template <typename T>
        T* create(Pt::uint8_t* data, ViewBase& view, Pt::ssize_t x, Pt::ssize_t y)
        {
            static_assert(sizeof(T) <= PixelStorage::MaxSize,
                          "insufficient pixel storage");

            return new (_data.mem) T(data, view, x, y);
        }

        template <typename T>
        T* create(const Pt::uint8_t* data, const ViewBase& view, Pt::ssize_t x, Pt::ssize_t y)
        {
            static_assert(sizeof(T) <= PixelStorage::MaxSize,
                          "insufficient pixel storage");

            return new (_data.mem) T(data, view, x, y);
        }

        template <typename T, typename P>
        T* create(P& p)
        {
            static_assert(sizeof(T) <= PixelStorage::MaxSize,
                          "insufficient pixel storage");

            return new (_data.mem) T(p);
        }

    private:
        union Data
        {
            Data()
            : ptr(0)
            { }

            char  mem[PixelStorage::MaxSize];
            void* ptr;
            std::size_t _align1;
            long double _align2;
        } _data;
};

///////////////////////////////////////////////////////////////////////
// Pixel
///////////////////////////////////////////////////////////////////////

class ImageFormat;

template <typename ColorT>
class Pixel;

template <typename ColorT>
class ConstPixel;


template <typename ColorT>
class Pixel
{
    template <typename C>
    friend class ConstPixel;

    public:
        Pixel(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

        Pixel(const Pixel& p);

        ~Pixel()
        {
            if(_pixel)
                _pixel->~PixelBase();
        }

        void reset(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const Pixel& p);

        const ImageFormat& format() const
        { return *_format; }

        Pt::uint8_t* base()
        { return _pixel->base(); }

        const Pt::uint8_t* base() const
        { return _pixel->base(); }

        Pt::ssize_t xpos() const
        { return _pixel->xpos(); }

        Pt::ssize_t ypos() const
        { return _pixel->ypos(); }

        void advance()
        {
            _pixel->advance();
        }

        void advance(Pt::ssize_t n)
        {
            _pixel->advance(n);
        }

        ColorT color() const;

        Pixel& operator=(const Argb32Color& color);

        Pixel& operator=(const Color& color);

        Pixel& operator=(const Pixel& p);

        Pixel& operator=(const ConstPixel<ColorT>& p);

        void fill(std::size_t n, const ColorT& color)
        {   
            _pixel->fill(n, color);
        }

        void assign(const ConstPixel<ColorT>& p, std::size_t length);

        bool equals(const ConstPixel<ColorT>& p) const;

        bool equals(const Pixel& p) const
        {
            return _pixel->base() == p.base();
        }

    private:
        ViewBase*           _view;
        const ImageFormat*  _format;
        PixelStorage        _storage;
        PixelBase*          _pixel;
        Pt::uint8_t*        _data;
};

///////////////////////////////////////////////////////////////////////
// Pixel
///////////////////////////////////////////////////////////////////////

template <typename ColorT>
class ConstPixel
{
    template <typename C>
    friend class Pixel;

    public:
        ConstPixel(const BasicConstView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

        ConstPixel(const BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

        ConstPixel(const ConstPixel& p);

        explicit ConstPixel(const Pixel<ColorT>& p);

        ~ConstPixel()
        {
            if(_pixel)
                _pixel->~ConstPixelBase();
        }

        void reset(const BasicConstView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y);

        void reset(const ConstPixel& p);

        const ImageFormat& format() const
        {
            return *_format;
        }

        const Pt::uint8_t* base() const
        { return _pixel->base(); }
        
        Pt::ssize_t xpos() const
        { return _pixel->xpos(); }

        Pt::ssize_t ypos() const
        { return _pixel->ypos(); }
        
        void advance()
        {
            _pixel->advance();
        }

        void advance(Pt::ssize_t n)
        {
            _pixel->advance(n);
        }

        ColorT color() const;

        void copy(Pixel<ColorT>& p, std::size_t length) const;

        bool equals(const ConstPixel& p) const
        {
            return _pixel->base() == p.base();
        }

        bool equals(const Pixel<ColorT>& p) const
        { 
            return _pixel->base() == p.base();
        }

    private:
        const ViewBase*     _view;
        const ImageFormat*  _format;
        PixelStorage        _storage;
        ConstPixelBase*     _pixel;
        const Pt::uint8_t*  _data;
};

///////////////////////////////////////////////////////////////////////
// ImageFormat
///////////////////////////////////////////////////////////////////////

/** @brief %Image format.
*/
class ImageFormat
{ 
  friend std::size_t pixelStride(const ImageFormat& format);

  friend std::size_t imageSize(const ImageFormat& format, Pt::ssize_t width, 
                               Pt::ssize_t height, std::size_t padding);
     
  friend ImageFormat* clone(const ImageFormat& format);

  friend void release(const ImageFormat* format);

    public:
        PT_GFX_API static const ImageFormat& rgb16();

        PT_GFX_API static const ImageFormat& rgb32();

        PT_GFX_API static const ImageFormat& argb32();

    public:
        explicit ImageFormat(size_t pixelStride)
        : _pixelStride(pixelStride)
        { }

        virtual ~ImageFormat() 
        { }

        PixelBase* createPixel(Pt::uint8_t* data, ViewBase& view, 
                               Pt::ssize_t x, Pt::ssize_t y, 
                               PixelStorage& store) const
        {
            return onCreatePixel(data, view, x, y, store);
        }

        ConstPixelBase* createPixel(const Pt::uint8_t* data, const ViewBase& view, 
                                    Pt::ssize_t x, Pt::ssize_t y, 
                                    PixelStorage& store) const
        {
            return onCreateConstPixel(data, view, x, y, store);
        }

        bool operator==(const ImageFormat& a) const
        {
            return onGetType() == a.onGetType();
        }

        bool operator!=(const ImageFormat& a) const
        {
            return ! (*this == a);
        }

        ImageFormat* clone() const
        {
            return onClone();
        }

        void release() const
        {
            onRelease();
        }

    protected:
        virtual ImageFormat* onClone() const
        {
            return 0;
        }

        virtual void onRelease() const
        {
        }

        virtual const std::type_info& onGetType() const = 0;

        virtual std::size_t onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                        std::size_t padding) const = 0;

        virtual PixelBase* onCreatePixel(Pt::uint8_t* data, ViewBase& view, 
                                         Pt::ssize_t x, Pt::ssize_t y, 
                                         PixelStorage& store) const = 0;

        virtual ConstPixelBase* onCreateConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                                                   Pt::ssize_t x, Pt::ssize_t y, 
                                                   PixelStorage& store) const = 0;

        inline const void* r0() const
        { return _r0.ptr; }
    
        inline const void* r1() const
        { return _r1.ptr; }

        inline const void* r2() const
        { return _r2.ptr; }

    private:
        const std::size_t  _pixelStride;
        const varint_t     _r0;
        const varint_t     _r1;
        const varint_t     _r2;
};


inline ImageFormat* clone(const ImageFormat& format)
{
    return format.onClone();
}


inline void release(const ImageFormat* format)
{
     format->onRelease();
}


inline std::size_t pixelStride(const ImageFormat& format)
{
    return format._pixelStride;
}


inline std::size_t imageSize(const ImageFormat& format, Pt::ssize_t width, Pt::ssize_t height,
                             std::size_t padding)
{
    return format.onImageSize(width, height, padding);
}

///////////////////////////////////////////////////////////////////////
// Pixel
///////////////////////////////////////////////////////////////////////

template <typename ColorT>
inline Pixel<ColorT>::Pixel(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _format( &view.format() )
, _pixel(0)
, _data( view.data() )
{ 
    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
}


template <typename ColorT>
inline Pixel<ColorT>::Pixel(const Pixel& p)
: _view(p._view)
, _format(p._format)
, _pixel(0)
, _data(p._data)
{ 
    if(p._pixel)
    {
        _pixel = _format->createPixel(_data, *_view, p.xpos(), p.ypos(), _storage);
    }
}


template <typename ColorT>
inline void Pixel<ColorT>::reset(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
{
    if(_pixel)
    {
        _pixel->~PixelBase();
        _data = 0;
        _view = 0;
        _format = 0;
        _pixel = 0;
    }

    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
    _data = view.data();
    _view = &view;
    _format = &view.format();
}


template <typename ColorT>
inline void Pixel<ColorT>::reset(const Pixel& p)
{
    if(_pixel)
    {
        _pixel->~PixelBase();
        _data = 0;
        _view = 0;
        _format = 0;
        _pixel = 0;
    }

    if(p._pixel)
    {
        _pixel = p._format->createPixel(p._data, *p._view, p.xpos(), p.ypos(), _storage);
        _data = p._data;
        _view = p._view;
        _format = p._format;
    }
}


template <typename ColorT>
inline ColorT Pixel<ColorT>::color() const
{ 
    return _pixel->getColor<ColorT>();
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const ConstPixel<ColorT>& p, std::size_t length)
{
    bool isCompatible = _pixel->assign(*p._pixel, length);
    if( isCompatible )
        return;

    isCompatible = p._pixel->copy(*_pixel, length);
    if( isCompatible )
        return;

    Pixel to = *this;
    ConstPixel<ColorT> from = p;
    for(std::size_t n = 0; n < length; ++n)
    {
        to = from;
        to.advance();
        from.advance();
    }
}


template <typename ColorT>
inline Pixel<ColorT>& Pixel<ColorT>::operator=(const Argb32Color& color)
{
    _pixel->assign(color);
    return *this;
}


template <typename ColorT>
inline Pixel<ColorT>& Pixel<ColorT>::operator=(const Color& color)
{
    _pixel->assign(color);
    return *this;
}


template <typename ColorT>
inline Pixel<ColorT>& Pixel<ColorT>::operator=(const Pixel& p)
{
    _pixel->assign( p.color() );
    return *this;
}


template <typename ColorT>
inline Pixel<ColorT>& Pixel<ColorT>::operator=(const ConstPixel<ColorT>& p)
{
    _pixel->assign( p.color() );
    return *this;
}


template <typename ColorT>
inline bool Pixel<ColorT>::equals(const ConstPixel<ColorT>& p) const
{ 
    return _pixel->base() == p._base();
}

///////////////////////////////////////////////////////////////////////
// ConstPixel
///////////////////////////////////////////////////////////////////////

template <typename ColorT>
inline ConstPixel<ColorT>::ConstPixel(const BasicConstView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _format( &view.format() )
, _pixel(0)
, _data( view.data() )
{ 
    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
}


template <typename ColorT>
inline ConstPixel<ColorT>::ConstPixel(const BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _format( &view.format() )
, _pixel(0)
, _data( view.data() )
{ 
    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
}


template <typename ColorT>
inline ConstPixel<ColorT>::ConstPixel(const ConstPixel& p)
: _view(p._view)
, _format(p._format)
, _pixel(0)
, _data(p._data)
{ 
    if(p._pixel)
    {
        _pixel = _format->createPixel(_data, *_view, p.xpos(), p.ypos(), _storage);
    }
}


template <typename ColorT>
inline ConstPixel<ColorT>::ConstPixel(const Pixel<ColorT>& p)
: _view(p._view)
, _format(p._format)
, _pixel(0)
, _data(p._data)
{ 
    _pixel = _format->createPixel(_data, *_view, p.xpos(), p.ypos(), _storage);
}


template <typename ColorT>
inline void ConstPixel<ColorT>::reset(const BasicConstView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
{
    if(_pixel)
    {
        _pixel->~ConstPixelBase();
        _data = 0;
        _view = 0;
        _format = 0;
        _pixel = 0;
    }

    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
    _data = view.data();
    _view = &view;
    _format = &view.format();
}


template <typename ColorT>
inline void ConstPixel<ColorT>::reset(const ConstPixel& p)
{
    if(_pixel)
    {
        _pixel->~ConstPixelBase();
        
        _pixel = 0;
        _data = 0;
        _view = 0;
        _format = 0;
    }

    if(p._pixel)
    {
        _pixel = p._format->createPixel(p._data, *p._view, p.xpos(), p.ypos(), _storage);
        _data = p._data;
        _view = p._view;
        _format = p._format;
    }
}


template <typename ColorT>
inline ColorT ConstPixel<ColorT>::color() const
{ 
    return _pixel->getColor<ColorT>();
}


template <typename ColorT>
inline void ConstPixel<ColorT>::copy(Pixel<ColorT>& p, std::size_t length) const
{
    bool isCompatible = _pixel->copy(*p._pixel, length);
    if( isCompatible )
        return;

    isCompatible = p._pixel->assign(*_pixel, length);
    if( isCompatible )
        return;

    Pixel<ColorT> to = p;
    ConstPixel from = *this;
    for(std::size_t n = 0; n < length; ++n)
    {
        to = from;
        to.advance();
        from.advance();
    }
}

} // namespace

} // namespace

#endif
