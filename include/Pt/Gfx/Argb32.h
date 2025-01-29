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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_GFX_ARGB32_H
#define PT_GFX_ARGB32_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/BasicView.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

template <typename FormatT>
class BasicView;

/** @brief ARGB-32 image format.
*/
class Argb32
{
    public:
        typedef BasicView<Argb32> View;

    public:
        class Pixel;
        class ConstPixel;

        Pt::ssize_t pixelStride() const
        {
            return 4;
        }

        // planes(), planeStride()

        static std::size_t imageSize(std::size_t width, std::size_t height,
                                     std::size_t padding);

        Color getColor(const ConstPixel& pixel) const;
        
        Color getColor(const Pixel& pixel) const;

        
        void sourceCopy(Pixel& to, const Color& c) const;

        void sourceOver(Pixel& to, const Color& c) const;


        void sourceCopy(View& to, Pt::ssize_t x, Pt::ssize_t y, const Color& c) const;

        
        void sourceCopy(Pixel& to, const ConstPixel& p) const;

        void sourceOver(Pixel& to, const ConstPixel& p) const;


        void sourceCopy(Pixel& to, std::size_t n, const Color& c) const;

        void sourceOver(Pixel& to, std::size_t n, const Color& c) const;

        
        void sourceCopy(Pixel& to, std::size_t n, const ConstPixel& p) const;

        void sourceOver(Pixel& to, std::size_t n, const ConstPixel& p) const;


        static void sourceCopy(Pixel& to, const ConstPixel& p, std::size_t n);

        static void sourceOver(Pixel& to, const ConstPixel& p, std::size_t n);

        
        static void sourceCopy(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                               const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                               Pt::ssize_t width, Pt::ssize_t height);

        static void sourceOver(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                               const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                               Pt::ssize_t width, Pt::ssize_t height);

        bool operator==(const Argb32& a) const
        {
            return true;
        }

        bool operator!=(const Argb32& a) const
        {
            return false;
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

        //
        // SourceOver
        //
        static void sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from);
        
        static void sourceOver(Pt::uint8_t* to, const Pt::Gfx::Color& from);

        static void sourceOver(Pt::uint8_t* to, size_t length, const Color& c);

        static void sourceOver(Pt::uint8_t* to, size_t length, const Pt::uint8_t* from);

        static void sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from, size_t length);

    public:
        static void assign(Pt::uint8_t* to, const Color& c,
                           CompositionMode mode, Pt::uint8_t blendingAlpha)
        {
            switch(mode) {
                default:
                case CompositionMode::SourceCopy: 
                {
                    const Pt::uint32_t blendAlphaSrc = blendingAlpha;
                    const Pt::uint32_t blendAlphaInv = 255 - blendingAlpha;
                    to[0] = (blendAlphaSrc * (c.blue () >> 8) + blendAlphaInv * to[0]) >> 8;
                    to[1] = (blendAlphaSrc * (c.green() >> 8) + blendAlphaInv * to[1]) >> 8;
                    to[2] = (blendAlphaSrc * (c.red  () >> 8) + blendAlphaInv * to[2]) >> 8;
                    to[3] = (blendAlphaSrc * (c.alpha() >> 8) + blendAlphaInv * to[3]) >> 8;
                    break;
                }

                case CompositionMode::SourceOver:
                {
                    const Pt::uint32_t colorAlpha    = c.alpha() >> 8;
                    const Pt::uint32_t blendAlphaSrc = colorAlpha * blendingAlpha / 255;
                    const Pt::uint32_t blendAlphaInv = 255 - blendAlphaSrc;
                    to[0] = (blendAlphaSrc * (c.blue () >> 8) + blendAlphaInv * to[0]) >> 8;
                    to[1] = (blendAlphaSrc * (c.green() >> 8) + blendAlphaInv * to[1]) >> 8;
                    to[2] = (blendAlphaSrc * (c.red  () >> 8) + blendAlphaInv * to[2]) >> 8;
                    to[3] = (blendAlphaSrc *  colorAlpha      + blendAlphaInv * to[3]) >> 8;
                    break;
                }
            }
        }

        static void assign(Pt::uint8_t* to, const Pt::uint8_t* from,
                           CompositionMode mode, Pt::uint8_t blendingAlpha)
        {
            switch(mode) {
                default:
                case CompositionMode::SourceCopy: 
                {
                    const Pt::uint32_t blendAlphaSrc = blendingAlpha;
                    const Pt::uint32_t blendAlphaInv = 255 - blendingAlpha;
                    to[0] = (blendAlphaSrc * from[0] + blendAlphaInv * to[0]) >> 8;
                    to[1] = (blendAlphaSrc * from[1] + blendAlphaInv * to[1]) >> 8;
                    to[2] = (blendAlphaSrc * from[2] + blendAlphaInv * to[2]) >> 8;
                    to[3] = (blendAlphaSrc * from[3] + blendAlphaInv * to[3]) >> 8;
                    break;
                }

                case CompositionMode::SourceOver:
                {
                    const Pt::uint32_t colorAlpha    = from[3];
                    const Pt::uint32_t blendAlphaSrc = colorAlpha * blendingAlpha / 255;
                    const Pt::uint32_t blendAlphaInv = 255 - blendAlphaSrc;
                    to[0] = (blendAlphaSrc * from[0]    + blendAlphaInv * to[0]) >> 8;
                    to[1] = (blendAlphaSrc * from[1]    + blendAlphaInv * to[1]) >> 8;
                    to[2] = (blendAlphaSrc * from[2]    + blendAlphaInv * to[2]) >> 8;
                    to[3] = (blendAlphaSrc * colorAlpha + blendAlphaInv * to[3]) >> 8;
                    break;
                }
            }
        }
};


/** @brief Const pixel in a ARGB-32 Image.
*/
class Argb32::ConstPixel
{
    friend class Pixel;

    public:
        ConstPixel(const BasicView<Argb32>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        : _view(0)
        , _xpos(0)
        , _ypos(0)
        , _p(0)
        {
            reset(view, xpos, ypos);
        }

        ConstPixel(const ConstPixel& p)
        : _view(p._view)
        , _xpos(p._xpos)
        , _ypos(p._ypos)
        , _p(p._p)
        { }

        ConstPixel(const Pixel& p);

        void reset(const BasicView<Argb32>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        {
            _view = &view;
            _xpos = xpos;
            _ypos = ypos;

            Pt::ssize_t off = view.stride() * ypos + (xpos * 4);
            _p = view.data() + off;
        }

        void reset(const ConstPixel& p)
        {
            _view = p._view;
            _xpos = p._xpos;
            _ypos = p._ypos;

            _p = p._p;
        }

        const BasicView<Argb32>& view() const
        { return *_view; }

        void advance()
        {
            if( ++_xpos >= _view->width() )
            {
                _xpos = 0;
                ++_ypos;

                _p += _view->padding();
            }

            _p += 4;
        }

        void advance( Pt::ssize_t n )
        {
            Pt::ssize_t off = _xpos + n;
            _ypos += off / _view->width();
            _xpos  = off % _view->width();

            _p = _view->data() + _view->stride() * _ypos + _xpos * 4;
        }

        Color getColor() const
        {
            return _view->format()->getColor(_p);
        }

        Pt::uint8_t alpha() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return *val >> 24;
        }

        Pt::uint8_t red() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return (*val & 0x00FF0000) >> 16;
        }

        Pt::uint8_t green() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return (*val & 0x0000FF00) >> 8;
        }

        Pt::uint8_t blue() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return *val & 0x000000FF;
        }

        bool operator!=(const ConstPixel& p) const
        {
            return _p != p._p;
        }

        bool operator==(const ConstPixel& p) const
        {
            return _p == p._p;
        }

        Pt::ssize_t x() const
        { return _xpos; }

        Pt::ssize_t y() const
        { return _ypos; }

        const Pt::uint8_t* data() const
        {
            return _p;
        }

    private:
        ConstPixel& operator=(const ConstPixel&);

    private:
        const BasicView<Argb32>* _view;
        Pt::ssize_t              _xpos;
        Pt::ssize_t              _ypos;
        const Pt::uint8_t*       _p;
};


/** @brief Const pixel in a ARGB-32 Image.
*/
class Argb32::Pixel
{
    friend class ConstPixel;

    public:
        Pixel(BasicView<Argb32>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        : _view(0)
        , _xpos(0)
        , _ypos(0)
        , _p(0)
        {
            reset(view, xpos, ypos);
        }

        Pixel(const Pixel& p)
        : _view(p._view)
        , _xpos(p._xpos)
        , _ypos(p._ypos)
        , _p(p._p)
        { }

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

        void reset(BasicView<Argb32>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        {
            _view = &view;
            _xpos = xpos;
            _ypos = ypos;

            Pt::ssize_t off = view.stride() * ypos + (xpos * 4);
            _p = view.data() + off;
        }

        void reset(const Pixel& p)
        {
            _view = p._view;
            _xpos = p._xpos;
            _ypos = p._ypos;

            _p = p._p;
        }

        void advance()
        {
            if( ++_xpos >= _view->width() )
            {
                _xpos = 0;
                ++_ypos;

                _p += _view->padding();
            }

            _p += 4;
        }

        void advance( Pt::ssize_t n )
        {
            Pt::ssize_t off = _xpos + n;
            _ypos += off / _view->width();
            _xpos  = off % _view->width();

            _p = _view->data() + _view->stride() * _ypos + _xpos * 4;
        }

        void assign(const Color& c, CompositionMode mode)
        {
            _view->assign(*this, c, mode);
        }

        void assign(const Pixel& p, CompositionMode mode)
        {
            _view->assign(*this, p, mode);
        }

        void assign(const ConstPixel& p, CompositionMode mode)
        {
            _view->assign(*this, p, mode);
        }

        Color getColor() const
        {
            return _view->format()->getColor(_p);
        }

        const BasicView<Argb32>& view() const
        { return *_view; }

        BasicView<Argb32>& view()
        { return *_view; }


        Pt::uint8_t alpha() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return *val >> 24;
        }

        Pt::uint8_t red() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return (*val & 0x00FF0000) >> 16;
        }

        Pt::uint8_t green() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return (*val & 0x0000FF00) >> 8;
        }

        Pt::uint8_t blue() const
        {
            const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(_p);
            return *val & 0x000000FF;
        }

        void setAlpha(Pt::uint8_t a)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(_p);
            *val = (*val & 0x00FFFFFF) | (uint32_t(a) << 24);
        }

        void setRed(Pt::uint8_t r)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(_p);
            *val = (*val & 0xFF00FFFF) | (uint32_t(r) << 16);
        }

        void setGreen(Pt::uint8_t g)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(_p);
            *val = (*val & 0xFFFF00FF) | (uint32_t(g) << 8);
        }

        void setBlue(Pt::uint8_t b)
        {
            Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(_p);
            *val = (*val & 0xFFFFFF00) | uint32_t(b);
        }

        bool operator!=(const Pixel& p) const
        { return _p != p._p; }

        bool operator==(const Pixel& p) const
        { return _p == p._p; }

        Pt::ssize_t x() const
        { return _xpos; }

        Pt::ssize_t y() const
        { return _ypos; }

        Pt::uint8_t* data()
        {
            return _p;
        }

        const Pt::uint8_t* data() const
        {
            return _p;
        }

    private:
        BasicView<Argb32>* _view;
        Pt::ssize_t        _xpos;
        Pt::ssize_t        _ypos;
        Pt::uint8_t*       _p;
};

///////////////////////////////////////////////////////////////////////
// ConstPixel
///////////////////////////////////////////////////////////////////////

inline Argb32::ConstPixel::ConstPixel(const Pixel& p)
: _view(p._view)
, _xpos(p._xpos)
, _ypos(p._ypos)
, _p(p._p)
{ 
}

///////////////////////////////////////////////////////////////////////
// Argb32
///////////////////////////////////////////////////////////////////////

inline std::size_t Argb32::imageSize(std::size_t width, std::size_t height,
                                     std::size_t padding)
{
    std::size_t l = (width * 4) + padding;
    std::size_t n = l * height;
    return n;
}


inline Color Argb32::getColor(const Pixel& pixel) const
{ 
    return getColor( ConstPixel(pixel) ); 
}


inline Color Argb32::getColor(const ConstPixel& pixel) const
{ 
    return getColor( pixel.data() ); 
}



inline void Argb32::sourceCopy(View& to, Pt::ssize_t x, Pt::ssize_t y, const Color& c) const
{
    Pt::uint8_t* base = to.data() + ( y * to.stride() ) + x * 4;
    sourceCopy(base, c);
}




inline void Argb32::sourceCopy(Pixel& to, const Color& c) const
{
    sourceCopy(to.data(), c);
}


inline void Argb32::sourceOver(Pixel& to, const Color& c) const
{
    sourceOver(to.data(), c);
}


inline void Argb32::sourceCopy(Pixel& to, const ConstPixel& p) const
{
    Argb32::sourceCopy(to.data(), p.data());
}


inline void Argb32::sourceOver(Pixel& to, const ConstPixel& p) const
{
    Argb32::sourceOver(to.data(), p.data());
}


inline void Argb32::sourceCopy(Pixel& to, std::size_t n, const Color& c) const
{
    sourceCopy(to.data(), n, c);
}


inline void Argb32::sourceOver(Pixel& to, std::size_t n, const Color& c) const
{
    sourceOver(to.data(), n, c);
}


inline void Argb32::sourceCopy(Pixel& to, std::size_t n, const ConstPixel& p) const
{
    Argb32::sourceCopy(to.data(), n, p.data());
}


inline void Argb32::sourceOver(Pixel& to, std::size_t n, const ConstPixel& p) const
{
    Argb32::sourceOver(to.data(), n, p.data());
}


inline void Argb32::sourceCopy(Pixel& to, const ConstPixel& p, std::size_t n)
{
    Argb32::sourceCopy(to.data(), p.data(), n);
}


inline void Argb32::sourceOver(Pixel& to, const ConstPixel& p, std::size_t n)
{
    Argb32::sourceOver(to.data(), p.data(), n);
}


inline void Argb32::sourceCopy(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                               const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                               Pt::ssize_t width, Pt::ssize_t height)
{
    // TODO: make ARGB-32 specific version

    Pt::ssize_t pixelSize = 4;

    // TODO: equals to toInfo.pitch()
    Pt::ssize_t toStride = (to.width() * pixelSize) + to.padding();
    Pt::ssize_t fromStride = (from.width() * pixelSize) + from.padding();

    Pt::ssize_t toBegin = (toY * toStride) + (toX * pixelSize);
    Pt::ssize_t fromBegin = (fromY * fromStride) + (fromX * pixelSize);

    Pt::uint8_t* toLine = to.data() + toBegin;
    const Pt::uint8_t* fromLine = from.data() + fromBegin;

    Pt::ssize_t n = width * pixelSize;

    for(Pt::ssize_t y = 0; y < height; ++y)
    {
        memcpy(toLine, fromLine, n);

        toLine += toStride;
        fromLine += fromStride;
    }
}


inline void Argb32::sourceOver(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                               const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                               Pt::ssize_t width, Pt::ssize_t height)
{
    // TODO: make ARGB-32 specific version

    Pt::ssize_t pixelSize = 4;

    // TODO: equals to toInfo.pitch()
    Pt::ssize_t toStride = (to.width() * pixelSize) + to.padding();
    Pt::ssize_t fromStride = (from.width() * pixelSize) + from.padding();

    Pt::ssize_t toBegin = (toY * toStride) + (toX * pixelSize);
    Pt::ssize_t fromBegin = (fromY * fromStride) + (fromX * pixelSize);

    Pt::uint8_t* toLine = to.data() + toBegin;
    const Pt::uint8_t* fromLine = from.data() + fromBegin;

    for(int y = 0; y < height; ++y)
    {
        Pt::uint8_t* to = toLine;
        const Pt::uint8_t* from = fromLine;

        for(int x = 0; x < width ; ++x )
        {
            Argb32::sourceOver(to, from);
            to += 4;
            from += 4;
        }

        toLine += toStride;
        fromLine += fromStride;
    }
}

//
// Implementation
//

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

} // namespace

} // namespace

#endif
