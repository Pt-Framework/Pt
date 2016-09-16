/* Copyright (C) 2016 Marc Boris Duerner 
  
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

#ifndef PT_GFX_RGB32IMAGE_H
#define PT_GFX_RGB32IMAGE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Argb8888Format.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

class Rgb32Base
{
    public:
        static Color toColor(const Pt::uint8_t* p)
        {
            Pt::uint16_t a = *p++ * 257;
            Pt::uint16_t r = *p++ * 257;
            Pt::uint16_t g = *p++ * 257;
            Pt::uint16_t b = *p * 257;
            
            return Color(a, r, g, b);
        }

        static void fromColor(const Color& c, Pt::uint8_t* p)
        {
            p[0] = (Pt::uint8_t) (c.alpha() / 257);
            p[1] = (Pt::uint8_t) (c.red() / 257);
            p[2] = (Pt::uint8_t) (c.green() / 257);
            p[3] = (Pt::uint8_t) (c.blue() / 257);
        }
        
        template <typename V, typename T>
        static void advance(V& view, Pt::ssize_t& xpos, Pt::ssize_t& ypos, T*& p)
        {
            if( ++xpos >= view.width() )
            {
                xpos = 0;
                ++ypos;

                p += view.padding();
            }

            p += 4;
        }

        template <typename V, typename T>
        static void advance(Pt::ssize_t n, V& view, 
                     Pt::ssize_t& xpos, Pt::ssize_t& ypos, T*& p)
        {
            Pt::ssize_t off = xpos + n;
            ypos += off / view.width();
            xpos += off % view.width();

            p = view.data() + view.stride() * ypos + xpos * 4;
        }
};

/** @brief Const pixel in a RGB-32 Image.
*/
class ConstRgb32Pixel
{
    public:
        ConstRgb32Pixel(const ImageView& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        : _view(0)
        , _xpos(0)
        , _ypos(0)
        , _p(0)
        { 
            reset(view, xpos, ypos);
        }

        ConstRgb32Pixel(const ConstRgb32Pixel& p)
        : _view(p._view)
        , _xpos(p._xpos)
        , _ypos(p._ypos)
        , _p(p._p)
        { }

        void reset(const ImageView& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        {
            _view = &view;
            _xpos = xpos;
            _ypos = ypos;

            Pt::ssize_t off = view.stride() * ypos + (xpos * 4);
            _p = view.data() + off;
        }

        void reset(const ConstRgb32Pixel& p)
        {
            _view = p._view;            
            _xpos = p._xpos;
            _ypos = p._ypos;

            _p = p._p;
        }

        void advance()
        {
            Rgb32Base::advance(*_view, _xpos, _ypos, _p);
        }

        void advance( Pt::ssize_t n )
        {
            Rgb32Base::advance(n, *_view, _xpos, _ypos, _p);
        }

        Color toColor() const
        {
            return Rgb32Base::toColor(_p);
        }

        bool operator!=(const ConstRgb32Pixel& p) const
        { 
            return _p != p._p; 
        }
        
        Pt::uint8_t alpha() const
        { return _p[0]; }

        Pt::uint8_t red() const
        { return _p[1]; }

        Pt::uint8_t green() const
        { return _p[2]; }

        Pt::uint8_t blue() const
        { return _p[3]; }

        bool operator==(const ConstRgb32Pixel& p) const
        { 
            return _p == p._p; 
        }

    private:
        ConstRgb32Pixel& operator=(const ConstRgb32Pixel&);

    private:
        const ImageView*   _view;
        Pt::ssize_t        _xpos;
        Pt::ssize_t        _ypos;
        const Pt::uint8_t* _p;
};

/** @brief Const pixel in a RGB-32 Image.
*/
class Rgb32Pixel
{
    public:
        Rgb32Pixel(ImageView& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        : _view(0)
        , _xpos(0)
        , _ypos(0)
        , _p(0)
        { 
            reset(view, xpos, ypos);
        }

        Rgb32Pixel(const Rgb32Pixel& p)
        : _view(p._view)
        , _xpos(p._xpos)
        , _ypos(p._ypos)
        , _p(p._p)
        { }

        Rgb32Pixel& operator=(const Rgb32Pixel& p)
        {
            assign(p, CompositionMode::SourceCopy);
            return *this;
        }

        Rgb32Pixel& operator=(const ConstRgb32Pixel& p)
        {
            assign(p, CompositionMode::SourceCopy);
            return *this;
        }

        Rgb32Pixel& operator=(const Color& color)
        {
            assign(color, CompositionMode::SourceCopy);
            return *this;
        }

        void reset(ImageView& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        {
            _view = &view;
            _xpos = xpos;
            _ypos = ypos;

            Pt::ssize_t off = view.stride() * ypos + (xpos * 4);
            _p = view.data() + off;
        }

        void reset(const Rgb32Pixel& p)
        {
            _view = p._view;            
            _xpos = p._xpos;
            _ypos = p._ypos;

            _p = p._p;
        }

        void advance()
        {
            Rgb32Base::advance(*_view, _xpos, _ypos, _p);
        }

        void advance( Pt::ssize_t n )
        {
            Rgb32Base::advance(n, *_view, _xpos, _ypos, _p);
        }

        void assign(const Color& color, CompositionMode)
        {
            Rgb32Base::fromColor(color, _p);
        }

        void assign(const Rgb32Pixel& p, CompositionMode)
        {
            _p[0] = p.alpha();
            _p[1] = p.red();
            _p[2] = p.green();
            _p[3] = p.blue();
        }

        void assign(const ConstRgb32Pixel& p, CompositionMode)
        {
            _p[0] = p.alpha();
            _p[1] = p.red();
            _p[2] = p.green();
            _p[3] = p.blue();
        }

        Color toColor() const
        {
            return Rgb32Base::toColor(_p);
        }
        
        Pt::uint8_t alpha() const
        { return _p[0]; }

        Pt::uint8_t red() const
        { return _p[1]; }

        Pt::uint8_t green() const
        { return _p[2]; }

        Pt::uint8_t blue() const
        { return _p[3]; }

        bool operator!=(const Rgb32Pixel& p) const
        { return _p != p._p; }
        
        bool operator==(const Rgb32Pixel& p) const
        { return _p == p._p; }

    private:
        ImageView*   _view;
        Pt::ssize_t  _xpos;
        Pt::ssize_t  _ypos;
        Pt::uint8_t* _p;
};

/** @brief RGB-32 image model.
*/
class Rgb32Model
{
    public:
        typedef Rgb32Pixel      PixelType;
        typedef ConstRgb32Pixel ConstPixelType;
        typedef Argb8888Format  FormatType;
};

/** @brief RGB-32 image.
*/
class Rgb32Image : public BasicImage<Rgb32Model>
{
    public:
        /** @brief Constructor.
        */
        Rgb32Image(const Size& size, size_t padding = 0)
        : BasicImage(size, padding)
        { }
        
        /** @brief Construct from external buffer.
        */
        Rgb32Image(Pt::uint8_t* data, const Size& size, size_t padding = 0)
        : BasicImage(data, size, padding)
        { }
};

} // namespace

} // namespace

#endif
