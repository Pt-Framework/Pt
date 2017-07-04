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

#ifndef PT_GFX_ARGB32IMAGE_H
#define PT_GFX_ARGB32IMAGE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/BasicImage.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {


/** @brief ARGB-32 image model.
*/
class Argb32Model
{
    public:
        class Pixel;
        class ConstPixel;

        static std::size_t imageSize(const Size& size, Pt::ssize_t padding)
        {
            std::size_t l = (size.width() * 4) + padding;
            std::size_t n = l * size.height();
            return n;
        }

        static Pt::ssize_t pixelStride()
        {
            return 4;
        }

    public:
        static Color toColor(const Pt::uint8_t* p)
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

        static void fromColor(Pt::uint8_t* p, const Color& c)
        {
            Pt::uint32_t* pixel = reinterpret_cast<Pt::uint32_t*>(p);

            *pixel = ( Pt::uint32_t(c.alpha() & 0xFF00) << 16 ) |
                     ( Pt::uint32_t(c.red  () & 0xFF00) <<  8 ) |
                       Pt::uint32_t(c.green() & 0xFF00)         |
                     ( Pt::uint32_t(c.blue ()         ) >>  8 );
        }

        static void sourceOver(Pt::uint8_t* to, const Pt::uint8_t* from)
        {
            const Pt::uint32_t alphaSrc = from[3];
            const Pt::uint32_t alphaInv = 255 - alphaSrc;

            to[0] = (Pt::uint8_t) ( (alphaSrc * from[0]  + alphaInv * to[0]) >> 8 );
            to[1] = (Pt::uint8_t) ( (alphaSrc * from[1]  + alphaInv * to[1]) >> 8 );
            to[2] = (Pt::uint8_t) ( (alphaSrc * from[2]  + alphaInv * to[2]) >> 8 );
            to[3] = (Pt::uint8_t) ( (alphaSrc * alphaSrc + alphaInv * to[3]) >> 8 );
        }

        static void sourceOver(Pt::uint8_t* to, const Pt::Gfx::Color& from)
        {
            const Pt::uint32_t alpha    = from.alpha() >> 8;
            const Pt::uint32_t alphaSrc = alpha;
            const Pt::uint32_t alphaInv = 255 - alpha;

            to[0] = (Pt::uint8_t) ( (alphaSrc * (from.blue () >> 8) + alphaInv * to[0]) >> 8 );
            to[1] = (Pt::uint8_t) ( (alphaSrc * (from.green() >> 8) + alphaInv * to[1]) >> 8 );
            to[2] = (Pt::uint8_t) ( (alphaSrc * (from.red  () >> 8) + alphaInv * to[2]) >> 8 );
            to[3] = (Pt::uint8_t) ( (alphaSrc *  alpha              + alphaInv * to[3]) >> 8 );
        }

        static void assign(Pt::uint8_t* to, const Color& c,
                           CompositionMode mode)
        {
            switch(mode) {
                default:
                case CompositionMode::SourceCopy:
                    Argb32Model::fromColor(to, c);
                    break;

                case CompositionMode::SourceOver:
                    Argb32Model::sourceOver(to, c);
                    break;
            }
        }

        static void assign(Pt::uint8_t* to, const Pt::uint8_t* from,
                           CompositionMode mode)
        {
            switch(mode) {
                default:
                case CompositionMode::SourceCopy:
                    *((Pt::uint32_t*) to) = *((const Pt::uint32_t*) from);
                    break;

                case CompositionMode::SourceOver:
                    Argb32Model::sourceOver(to, from);
                    break;
            }
        }

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

        static void assign(Pt::uint8_t* to, const Color& c, size_t length,
                           CompositionMode mode);

        static void assign(Pt::uint8_t* to, const Pt::uint8_t* from, size_t length,
                           CompositionMode mode);

        template <typename T>
        static void advance(T*& p, Pt::ssize_t& xpos, Pt::ssize_t& ypos,
                            const BasicView<Argb32Model>& view)
        {
            if( ++xpos >= view.width() )
            {
                xpos = 0;
                ++ypos;

                p += view.padding();
            }

            p += 4;
        }

        template <typename T>
        static void advance(T*& p, Pt::ssize_t n, Pt::ssize_t& xpos, Pt::ssize_t& ypos,
                            const BasicView<Argb32Model>& view, T* data)
        {
            Pt::ssize_t off = xpos + n;
            ypos += off / view.width();
            xpos  = off % view.width();

            p = data + view.stride() * ypos + xpos * 4;
        }
};


/** @brief Const pixel in a ARGB-32 Image.
*/
class Argb32Model::ConstPixel
{
    friend class Pixel;

    public:
        ConstPixel(const BasicView<Argb32Model>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
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

        void reset(const BasicView<Argb32Model>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
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

        void advance()
        {
            Argb32Model::advance(_p, _xpos, _ypos, *_view);
        }

        void advance( Pt::ssize_t n )
        {
            Argb32Model::advance(_p, n, _xpos, _ypos, *_view, _view->data());
        }

        Color toColor() const
        {
            return Argb32Model::toColor(_p);
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

    private:
        ConstPixel& operator=(const ConstPixel&);

    private:
        const BasicView<Argb32Model>* _view;
        Pt::ssize_t                   _xpos;
        Pt::ssize_t                   _ypos;
        const Pt::uint8_t*            _p;
};


/** @brief Const pixel in a ARGB-32 Image.
*/
class Argb32Model::Pixel
{
    public:
        Pixel(BasicView<Argb32Model>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
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

        void reset(BasicView<Argb32Model>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
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
            Argb32Model::advance(_p, _xpos, _ypos, *_view);
        }

        void advance( Pt::ssize_t n )
        {
            Argb32Model::advance(_p, n, _xpos, _ypos, *_view, _view->data());
        }

        void assign(const Color& c, CompositionMode mode)
        {
            Argb32Model::assign(_p, c, mode);
        }

        void assign(const Pixel& p, CompositionMode mode)
        {
            Argb32Model::assign(_p, p._p, mode);
        }

        void assign(const ConstPixel& p, CompositionMode mode)
        {
            Argb32Model::assign(_p, p._p, mode);
        }

        Color toColor() const
        {
            return Argb32Model::toColor(_p);
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

    private:
        BasicView<Argb32Model>* _view;
        Pt::ssize_t             _xpos;
        Pt::ssize_t             _ypos;
        Pt::uint8_t*            _p;
};


/** @brief ARGB-32 image.
*/
class Argb32Image : public BasicImage<Argb32Model>
{
    public:
        /** @brief Constructor.
        */
        Argb32Image(const Size& size, size_t padding = 0)
        : BasicImage(size, padding)
        { }

        /** @brief Construct from external buffer.
        */
        Argb32Image(Pt::uint8_t* data, const Size& size, size_t padding = 0)
        : BasicImage(data, size, padding)
        { }
};


} // namespace
} // namespace


#endif
