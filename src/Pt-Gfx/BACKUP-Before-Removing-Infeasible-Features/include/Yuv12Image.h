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

#ifndef PT_GFX_YUV12IMAGE_H
#define PT_GFX_YUV12IMAGE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Gfx/CompositionMode.h>
#include <Pt/Gfx/BasicImage.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

/** @brief YV-12 image model.
*/
class Yuv12Model
{
    public:
        class Pixel;
        class ConstPixel;

        static std::size_t imageSize(const Size& size, Pt::ssize_t padding)
        {
            Pt::ssize_t stride = size.width() + padding;
            Pt::ssize_t planeSize = stride * size.height();

            return planeSize + planeSize / 2;
        }

        static Pt::ssize_t pixelStride()
        {
            return 1;
        }

    public:
        static Color toColor(Pt::uint8_t y, Pt::uint8_t u, Pt::uint8_t v)
        {
            Pt::uint32_t rv = 298 * (y - 16)                   + 409 * (v - 128) + 128;
            Pt::uint32_t gv = 298 * (y - 16) - 100 * (u - 128) - 208 * (v - 128) + 128;
            Pt::uint32_t bv = 298 * (y - 16) + 516 * (u - 128)                   + 128;

            Pt::uint16_t r = rv > 65535 ? 65535 : static_cast<Pt::uint16_t>(rv);
            Pt::uint16_t g = gv > 65535 ? 65535 : static_cast<Pt::uint16_t>(gv);
            Pt::uint16_t b = bv > 65535 ? 65535 : static_cast<Pt::uint16_t>(bv);

            return Color(r, g, b);
        }

        static void fromColor(Pt::uint8_t& y, Pt::uint8_t& u, Pt::uint8_t& v,
                              const Color& color)
        {
            Pt::int32_t r = color.red();
            Pt::int32_t g = color.green();
            Pt::int32_t b = color.blue();

            Pt::int32_t yy = (( 66 * r + 129 * g +  25 * b + 128) >> 16) +  16;
            Pt::int32_t uu = ((-38 * r -  74 * g + 112 * b + 128) >> 16) + 128;
            Pt::int32_t vv = ((112 * r -  94 * g -  18 * b + 128) >> 16) + 128;

            y = yy > 255 ? 255 : static_cast<Pt::uint8_t>(yy);
            u = uu > 255 ? 255 : static_cast<Pt::uint8_t>(uu);
            v = vv > 255 ? 255 : static_cast<Pt::uint8_t>(vv);
        }

        template <typename T>
        static Pt::ssize_t init(T* data, Pt::ssize_t stride, const Size& size,
                                Pt::ssize_t xpos, Pt::ssize_t ypos,
                                T*& y, T*& u, T*& v)
        {
            Pt::ssize_t yOffset = stride * ypos + xpos;
            y = data + yOffset;

            return init(data, stride, size, xpos, ypos, u, v);
        }

        template <typename T>
        static Pt::ssize_t init(T* data, Pt::ssize_t stride, const Size& size,
                                Pt::ssize_t xpos, Pt::ssize_t ypos,
                                T*& u, T*& v)
        {
            Pt::ssize_t planeSize = stride * size.height();

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
                            T* data, Pt::ssize_t stride, const Size& size,
                            Pt::ssize_t padding, Pt::ssize_t subStride)
        {
            ++y;

            if( ++xpos >= size.width() )
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
                            Pt::ssize_t& xpos, Pt::ssize_t& ypos,
                            T* data, Pt::ssize_t stride, const Size& size)
        {
            Pt::ssize_t off = xpos + n;
            ypos += off / size.width();
            xpos  = off % size.width();

            init(data, stride, size,xpos, ypos, y, u, v);
        }
};

/** @brief Const pixel in a YV-12 Image.
*/
class Yuv12Model::ConstPixel
{
    public:
        ConstPixel(const BasicView<Yuv12Model>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        : _view(0)
        , _xpos(0)
        , _ypos(0)
        , _subStride(0)
        , _y(0)
        , _u(0)
        , _v(0)
        {
            reset(view, xpos, ypos);
        }

        ConstPixel(const ConstPixel& p)
        : _view(p._view)
        , _xpos(p._xpos)
        , _ypos(p._ypos)
        , _subStride(p._subStride)
        , _y(p._y)
        , _u(p._u)
        , _v(p._v)
        { }

        void reset(const BasicView<Yuv12Model>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        {
            _view = &view;
            _xpos = xpos;
            _ypos = ypos;

            _subStride = Yuv12Model::init(view.data(), view.stride(), view.size(), xpos,  ypos, _y, _u, _v);
        }

        void reset(const ConstPixel& p)
        {
            _view = p._view;
            _xpos = p._xpos;
            _ypos = p._ypos;
            _subStride = p._subStride;

            _y = p._y;
            _u = p._u;
            _v = p._v;
        }

        void advance()
        {
            Yuv12Model::advance(_y, _u, _v, _xpos, _ypos,
                                _view->data(), _view->stride(),
                                _view->size(), _view->padding(), _subStride);
        }

        void advance( Pt::ssize_t n )
        {
            Yuv12Model::advance(_y, _u, _v, n, _xpos, _ypos,
                               _view->data(), _view->stride(),_view->size());
        }

        Color toColor() const
        {
            return Yuv12Model::toColor(*_y, *_u, *_v);
        }

        Pt::uint8_t y() const
        { return *_y; }

        Pt::uint8_t u() const
        { return *_u; }

        Pt::uint8_t v() const
        { return *_v; }

        bool operator!=(const ConstPixel& p) const
        { return _y != p._y; }

        bool operator==(const ConstPixel& p) const
        { return _y == p._y; }

    private:
        ConstPixel& operator=(const ConstPixel&);

    private:
        const BasicView<Yuv12Model>*   _view;
        Pt::ssize_t        _xpos;
        Pt::ssize_t        _ypos;
        Pt::ssize_t        _subStride;
        const Pt::uint8_t* _y;
        const Pt::uint8_t* _u;
        const Pt::uint8_t* _v;
};

/** @brief Pixel in a YV-12 Image.
*/
class Yuv12Model::Pixel
{
    public:
        Pixel(BasicView<Yuv12Model>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        : _view(&view)
        , _xpos(xpos)
        , _ypos(ypos)
        , _subStride(0)
        , _y(0)
        , _u(0)
        , _v(0)
        {
            _subStride = Yuv12Model::init(view.data(), view.stride(), view.size(), xpos,  ypos, _y, _u, _v);
        }

        Pixel(const Pixel& p)
        : _view(p._view)
        , _xpos(p._xpos)
        , _ypos(p._ypos)
        , _subStride(p._subStride)
        , _y(p._y)
        , _u(p._u)
        , _v(p._v)
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

        void reset(BasicView<Yuv12Model>& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        {
            _view = &view;
            _xpos = xpos;
            _ypos = ypos;

            _subStride = Yuv12Model::init(view.data(), view.stride(), view.size(), xpos,  ypos, _u, _v);
        }

        void reset(const Pixel& p)
        {
            _view = p._view;
            _xpos = p._xpos;
            _ypos = p._ypos;
            _subStride = p._subStride;

            _y = p._y;
            _u = p._u;
            _v = p._v;
        }

        void advance()
        {
            Yuv12Model::advance(_y, _u, _v, _xpos, _ypos,
                                _view->data(), _view->stride(),
                                _view->size(), _view->padding(), _subStride);
        }

        void advance( Pt::ssize_t n )
        {
            Yuv12Model::advance(_y, _u, _v, n, _xpos, _ypos,
                               _view->data(), _view->stride(),_view->size());
        }

        void assign(const Color& color, CompositionMode)
        {
            Yuv12Model::fromColor(*_y, *_u, *_v, color);
        }

        void assign(const Pixel& p, CompositionMode)
        {
            *_y = *(p._y);
            *_u = *(p._u);
            *_v = *(p._v);
        }

        void assign(const ConstPixel& p, CompositionMode)
        {
            *_y = p.y();
            *_u = p.u();
            *_v = p.v();
        }

        Color toColor() const
        {
            return Yuv12Model::toColor(*_y, *_u, *_v);
        }

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

        bool operator!=(const Pixel& p) const
        { return _y != p._y; }

        bool operator==(const Pixel& p) const
        { return _y == p._y; }

    private:
        BasicView<Yuv12Model>*   _view;
        Pt::ssize_t  _xpos;
        Pt::ssize_t  _ypos;
        Pt::ssize_t  _subStride;
        Pt::uint8_t* _y;
        Pt::uint8_t* _u;
        Pt::uint8_t* _v;
};

/** @brief YV-12 image.

    If the Y plane has pad bytes after each row, then the U and V planes have
    half as many pad bytes after their rows. In other words, two U/V rows
    (including padding) is exactly as long as one Y row (including padding).
*/
class Yuv12Image : public BasicImage<Yuv12Model>
{
    public:
        /** @brief Constructor.
        */
        Yuv12Image(const Size& size, size_t padding = 0)
        : BasicImage(size, padding)
        { }

        /** @brief Construct from external buffer.
        */
        Yuv12Image(Pt::uint8_t* data, const Size& size, size_t padding = 0)
        : BasicImage(data, size, padding)
        { }
};

} // namespace

} // namespace

#endif
