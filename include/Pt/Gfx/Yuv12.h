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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef PT_GFX_YUV12_H
#define PT_GFX_YUV12_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/BasicView.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

class Yuv12;
class Yuv12Pixel;
class Yuv12ConstPixel;

/** @brief YV-12 pixel reference.
*/
class Yuv12Pixel
{
    friend class Yuv12;
    friend class Yuv12ConstPixel;

    public:
        typedef BasicView<Yuv12> View;

    public:
        Yuv12Pixel(View& view, Pt::ssize_t xpos, Pt::ssize_t ypos);

        Yuv12Pixel(const Yuv12Pixel& p)
        : _xpos(p._xpos)
        , _ypos(p._ypos)
        , _subStride(p._subStride)
        , _y(p._y)
        , _u(p._u)
        , _v(p._v)
        { }

        Pt::uint8_t* ybase() const
        { return _v; }

        Pt::uint8_t* ubase() const
        { return _v; }

        Pt::uint8_t* vbase() const
        { return _v; }

        Pt::ssize_t xpos() const
        { return _xpos; }

        Pt::ssize_t ypos() const
        { return _ypos; }

        Pt::uint8_t y() const
        { return *_y; }

        void setY(Pt::uint8_t y) const
        { *_y = y; }

        Pt::uint8_t u() const
        { return *_u; }

        void setU(Pt::uint8_t u) const
        { *_u = u; }

        Pt::uint8_t v() const
        { return *_v; }

        void setV(Pt::uint8_t v) const
        { *_v = v; }

        bool operator==(const Yuv12Pixel& p) const;

        bool operator==(const Yuv12ConstPixel& p) const;

    private:
        Pt::ssize_t  _xpos;
        Pt::ssize_t  _ypos;
        Pt::ssize_t  _subStride;
        Pt::uint8_t* _y;
        Pt::uint8_t* _u;
        Pt::uint8_t* _v;
};

/** @brief YV12 const pixel reference.
*/
class Yuv12ConstPixel
{
    friend class Yuv12;
    friend class Yuv12Pixel;

    public:
        typedef BasicView<Yuv12> View;

    public:
        Yuv12ConstPixel(const View& view, Pt::ssize_t xpos, Pt::ssize_t ypos);

        Yuv12ConstPixel(const Yuv12ConstPixel& p)
        : _xpos(p._xpos)
        , _ypos(p._ypos)
        , _subStride(p._subStride)
        , _y(p._y)
        , _u(p._u)
        , _v(p._v)
        { }

        explicit Yuv12ConstPixel(const Yuv12Pixel& p)
        : _xpos(p._xpos)
        , _ypos(p._ypos)
        , _subStride(p._subStride)
        , _y(p._y)
        , _u(p._u)
        , _v(p._v)
        { }
        
        Pt::ssize_t xpos() const
        { return _xpos; }

        Pt::ssize_t ypos() const
        { return _ypos; }

        Pt::uint8_t y() const
        { return *_y; }

        Pt::uint8_t u() const
        { return *_u; }

        Pt::uint8_t v() const
        { return *_v; }

        bool operator==(const Yuv12ConstPixel& p) const
        { return _y == p._y; }

        bool operator==(const Yuv12Pixel& p) const
        { return _y == p._y; }

    private:
        Pt::ssize_t        _xpos;
        Pt::ssize_t        _ypos;
        Pt::ssize_t        _subStride;
        const Pt::uint8_t* _y;
        const Pt::uint8_t* _u;
        const Pt::uint8_t* _v;
};

/** @brief YV-12 image format.
*/
class Yuv12
{
    public:
        typedef BasicView<Yuv12>  View;

        typedef Yuv12Pixel       Pixel;
        typedef Yuv12ConstPixel ConstPixel;

        static Pt::ssize_t pixelStride()
        {
            return 1;
        }

        static std::size_t imageSize(std::size_t width, std::size_t height,
                                     std::size_t padding)
        {
            Pt::ssize_t stride = width + padding;
            Pt::ssize_t planeSize = stride * height;

            return planeSize + planeSize / 2;
        }

        bool operator==(const Yuv12& ) const
        {
            return true;
        }

        bool operator!=(const Yuv12& ) const
        {
            return false;
        }

        // planes(), planeStride()

    public:
        void advance(View& view, Pixel& p) const
        {
            Yuv12::advance(p._y, p._u, p._v, p._xpos, p._ypos,
                           view.width(), view.padding(), p._subStride);

        }

        void advance(const View& view, ConstPixel& p) const
        {
            Yuv12::advance(p._y, p._u, p._v, p._xpos, p._ypos,
                           view.width(), view.padding(), p._subStride);
        }

        void advance(View& view, Pixel& p, Pt::ssize_t n) const
        {
            Yuv12::advance(p._y, p._u, p._v, n, p._xpos, p._ypos, 
                           view.data(), view.stride(), 
                           view.width(), view.height());
        }

        void advance(const View& view, ConstPixel& p, Pt::ssize_t n) const
        {
            Yuv12::advance(p._y, p._u, p._v, n, p._xpos, p._ypos, 
                           view.data(), view.stride(), 
                           view.width(), view.height());
        }

    public:
        /** @brief Get pixel color.
        */
        Color getColor(const View& view, const Pixel& p) const
        {
            return Yuv12::getColor(p.y(), p.u(), p.v());
        }

        Color getColor(const View& view, const ConstPixel& p) const
        {
            return Yuv12::getColor(p.y(), p.u(), p.v());
        }

        /** @brief Assign pixels.
        */
        void sourceCopy(View& view, Pixel& to, 
                        const View& from, const Pixel& p) const
        {
            *to.ybase() = p.y(); 
            *to.ubase() = p.u();
            *to.vbase() = p.v();
        }

        void sourceCopy(View& view, Pixel& to, 
                        const View& from, const ConstPixel& p) const
        {
            *to.ybase() = p.y(); 
            *to.ubase() = p.u();
            *to.vbase() = p.v();        
        }

        void sourceOver(View& view, Pixel& to, 
                        const View& from, const Pixel& p) const
        {
            sourceCopy(view, to, from, p);
        }

        void sourceOver(View& view, Pixel& to, 
                        const View& from, const ConstPixel& p) const
        {
            sourceCopy(view, to, from, p);
        }

        /** @brief Assign pixels.
        */
        void sourceCopy(View& view, Pixel& to, const Color& c) const
        {
            Yuv12::fromColor(to.ybase(), to.ubase(), to.vbase(), c);
        }
        
        void sourceOver(View& view, Pixel& to, const Color& c) const
        {
            Yuv12::fromColor(to.ybase(), to.ubase(), to.vbase(), c);
        }

        /** @brief Fill pixels.
        */
        void sourceCopy(View& view, Pixel& to, std::size_t n, const Color& c) const
        {
        }

        void sourceOver(View& view, Pixel& to, std::size_t n, const Color& c) const
        {
        }
    
        /** @brief Fill pixels.
        */
        void sourceCopy(View& view, Pixel& to, std::size_t n, 
                        const View& from, const ConstPixel& p) const
        {
        }

        void sourceOver(View& view, Pixel& to, std::size_t n, 
                        const View& from, const ConstPixel& p) const
        {
        }

        /** @brief Copy pixels.
        */
        void sourceCopy(View& view, Pixel& to, 
                        const View& from, const ConstPixel& p, std::size_t n) const
        { 
        }

        void sourceOver(View& view, Pixel& to, 
                        const View& from, const ConstPixel& p, std::size_t n) const
        { 
        }

        void sourceCopy(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                        const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                        Pt::ssize_t width, Pt::ssize_t height)
        {
        }


        void sourceOver(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                        const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                        Pt::ssize_t width, Pt::ssize_t height)
        {
        }

    public:
        static Color getColor(Pt::uint8_t y, Pt::uint8_t u, Pt::uint8_t v)
        {
            Pt::uint32_t rv = 298 * (y - 16)                   + 409 * (v - 128) + 128;
            Pt::uint32_t gv = 298 * (y - 16) - 100 * (u - 128) - 208 * (v - 128) + 128;
            Pt::uint32_t bv = 298 * (y - 16) + 516 * (u - 128)                   + 128;

            Pt::uint16_t r = rv > 65535 ? 65535 : static_cast<Pt::uint16_t>(rv);
            Pt::uint16_t g = gv > 65535 ? 65535 : static_cast<Pt::uint16_t>(gv);
            Pt::uint16_t b = bv > 65535 ? 65535 : static_cast<Pt::uint16_t>(bv);

            return Color(r, g, b);
        }

        static void fromColor(Pt::uint8_t* y, Pt::uint8_t* u, Pt::uint8_t* v,
                              const Color& color)
        {
            Pt::int32_t r = color.red();
            Pt::int32_t g = color.green();
            Pt::int32_t b = color.blue();

            Pt::int32_t yy = (( 66 * r + 129 * g +  25 * b + 128) >> 16) +  16;
            Pt::int32_t uu = ((-38 * r -  74 * g + 112 * b + 128) >> 16) + 128;
            Pt::int32_t vv = ((112 * r -  94 * g -  18 * b + 128) >> 16) + 128;

            *y = yy > 255 ? 255 : static_cast<Pt::uint8_t>(yy);
            *u = uu > 255 ? 255 : static_cast<Pt::uint8_t>(uu);
            *v = vv > 255 ? 255 : static_cast<Pt::uint8_t>(vv);
        }

        template <typename T>
        static Pt::ssize_t init(T* data, Pt::ssize_t stride, 
                                Pt::ssize_t width, Pt::ssize_t height,
                                Pt::ssize_t xpos, Pt::ssize_t ypos,
                                T*& y, T*& u, T*& v)
        {
            Pt::ssize_t yOffset = stride * ypos + xpos;
            y = data + yOffset;

            return initUV(data, stride, width, height, xpos, ypos, u, v);
        }

        template <typename T>
        static Pt::ssize_t initUV(T* data, Pt::ssize_t stride, 
                                  Pt::ssize_t width, Pt::ssize_t height,
                                  Pt::ssize_t xpos, Pt::ssize_t ypos,
                                  T*& u, T*& v)
        {
            Pt::ssize_t planeSize = stride * height;

            Pt::ssize_t subStride = stride / 2;
            Pt::ssize_t subPlaneSize = planeSize / 4;

            Pt::ssize_t subXPos = xpos / 2;
            Pt::ssize_t subYPos = ypos / 2;
            Pt::ssize_t subOffset = subStride * subYPos + subXPos;

            Pt::ssize_t uOffset = planeSize + subOffset;
            u = data + uOffset;

            Pt::ssize_t vOffset = uOffset + subPlaneSize;
            v = data + vOffset;

            return subStride;
        }

        template <typename T>
        static void advance(T*& y, T*& u, T*& v, 
                            Pt::ssize_t& xpos, Pt::ssize_t& ypos,
                            Pt::ssize_t width, Pt::ssize_t padding,
                            Pt::ssize_t subStride)
        {
            ++y;

            if( ++xpos >= width )
            {
                ++u;
                ++v;

                if(ypos % 2 == 0)
                {
                  u -= subStride;
                  v -= subStride;
                }

                xpos = 0;
                ++ypos;

                y += padding;
            }
            else if(xpos % 2 == 0)
            {
                ++u;
                ++v;
            }
        }

        template <typename T>
        static void advance(T*& y, T*& u, T*& v, Pt::ssize_t n,
                            Pt::ssize_t& xpos, Pt::ssize_t& ypos, T* data, 
                            Pt::ssize_t stride, Pt::ssize_t width, Pt::ssize_t height)
        {
            Pt::ssize_t off = xpos + n;
            ypos += off / width;
            xpos  = off % width;

            init(data, stride, width, height, xpos, ypos, y, u, v);
        }
};

///////////////////////////////////////////////////////////////////////
// Yuv12Pixel
///////////////////////////////////////////////////////////////////////

inline Yuv12Pixel::Yuv12Pixel(View& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
: _xpos(xpos)
, _ypos(ypos)
, _subStride(0)
, _y(0)
, _u(0)
, _v(0)
{
    _subStride = Yuv12::init(view.data(), view.stride(), 
                             view.width(), view.height(),
                             xpos,  ypos, _y, _u, _v);
}


inline bool Yuv12Pixel::operator==(const Yuv12Pixel& p) const
{ 
    return _y == p._y; 
}


inline bool Yuv12Pixel::operator==(const Yuv12ConstPixel& p) const
{ 
    return _y == p._y; 
}

///////////////////////////////////////////////////////////////////////
// Yuv12ConstPixel
///////////////////////////////////////////////////////////////////////

inline Yuv12ConstPixel::Yuv12ConstPixel(const View& view, 
                                        Pt::ssize_t xpos, Pt::ssize_t ypos)
: _xpos(xpos)
, _ypos(ypos)
, _subStride(0)
, _y(0)
, _u(0)
, _v(0)
{
    _subStride = Yuv12::init(view.data(), view.stride(), 
                             view.width(), view.height(), 
                             xpos,  ypos, _y, _u, _v);
}

} // namespace

} // namespace

#endif
