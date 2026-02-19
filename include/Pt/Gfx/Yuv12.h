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
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/Location.h>
#include <Pt/Gfx/Color.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

class Yuv12PixelBase;
class Yuv12ConstPixelBase;

class Yuv12;
class Yuv12Pixel;
class Yuv12ConstPixel;

template <typename T>
class BasicView;

template <typename T>
class BasicConstView;

/** @brief YV-12 pixel.
*/
class PT_GFX_API Yuv12Pixel
{
    friend class Yuv12;
    friend class Yuv12ConstPixel;
    
    public:
        typedef Yuv12 Format;

    protected:
        Yuv12Pixel(Pt::uint8_t* data, ViewBase& view, 
                   Pt::ssize_t x, Pt::ssize_t y);

    public:
        Yuv12Pixel(BasicView<Yuv12>& view, 
                   Pt::ssize_t xpos, Pt::ssize_t ypos);

        Yuv12Pixel(const Yuv12Pixel& p)
        : _view(p._view)
        , _xpos(p._xpos)
        , _ypos(p._ypos)
        , _subStride(p._subStride)
        , _y(p._y)
        , _u(p._u)
        , _v(p._v)
        { }

        Yuv12Pixel& operator=(const Color& color);

        Yuv12Pixel& operator=(const Argb32Color& color);

        ViewBase& view()
        { return _view; }

        const ViewBase& view() const
        { return _view; }

        Pt::ssize_t xpos() const
        { return _xpos; }

        Pt::ssize_t ypos() const
        { return _ypos; }

        Pt::uint8_t* ybase() const
        { return _y; }

        Pt::uint8_t* ubase() const
        { return _u; }

        Pt::uint8_t* vbase() const
        { return _v; }

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

        Color toColor() const;

        void advance();

        void advance(Pt::ssize_t n);

        bool equals(const Yuv12Pixel& p) const;

        bool equals(const Yuv12ConstPixel& p) const;

    private:
        ViewBase&    _view;
        Pt::ssize_t  _xpos;
        Pt::ssize_t  _ypos;
        Pt::ssize_t  _subStride;
        Pt::uint8_t* _y;
        Pt::uint8_t* _u;
        Pt::uint8_t* _v;
};

/** @brief YV12 const pixel.
*/
class Yuv12ConstPixel
{
    friend class Yuv12;
    friend class Yuv12Pixel;
    
    public:
        typedef Yuv12 Format;
    
    protected:
        Yuv12ConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                        Pt::ssize_t xpos, Pt::ssize_t ypos);
    
    public:
        Yuv12ConstPixel(const BasicConstView<Yuv12>& view,
                        Pt::ssize_t xpos, Pt::ssize_t ypos);

        Yuv12ConstPixel(const BasicView<Yuv12>& view, 
                        Pt::ssize_t xpos, Pt::ssize_t ypos);

        Yuv12ConstPixel(const Yuv12ConstPixel& p)
        : _view(p._view)
        , _xpos(p._xpos)
        , _ypos(p._ypos)
        , _subStride(p._subStride)
        , _y(p._y)
        , _u(p._u)
        , _v(p._v)
        { }

        explicit Yuv12ConstPixel(const Yuv12Pixel& p)
        : _view(p._view)
        , _xpos(p._xpos)
        , _ypos(p._ypos)
        , _subStride(p._subStride)
        , _y(p._y)
        , _u(p._u)
        , _v(p._v)
        { }
        
        const ViewBase& view() const
        { return _view; }

        Pt::ssize_t xpos() const
        { return _xpos; }

        Pt::ssize_t ypos() const
        { return _ypos; }

        const Pt::uint8_t* ybase() const
        { return _y; }

        const Pt::uint8_t* ubase() const
        { return _u; }

        const Pt::uint8_t* vbase() const
        { return _v; }

        Pt::uint8_t y() const
        { return *_y; }

        Pt::uint8_t u() const
        { return *_u; }

        Pt::uint8_t v() const
        { return *_v; }

        Color toColor() const;

        void advance();

        void advance(Pt::ssize_t n);

        bool equals(const Yuv12ConstPixel& p) const
        { return _y == p._y; }

        bool equals(const Yuv12Pixel& p) const
        { return _y == p._y; }

    private:
        const ViewBase&     _view;
        Pt::ssize_t         _xpos;
        Pt::ssize_t         _ypos;
        Pt::ssize_t         _subStride;
        const Pt::uint8_t*  _y;
        const Pt::uint8_t*  _u;
        const Pt::uint8_t*  _v;
};

/** @brief YV-12 image format.
*/
class PT_GFX_API Yuv12 final : public ImageFormat
{
    public:    
        typedef Yuv12Pixel PixelType;
        typedef Yuv12ConstPixel ConstPixelType;
        typedef Color ColorType;

    public:
        Yuv12();
        
        Pt::ssize_t pixelStride() const
        {
            return 1;
        }

        std::size_t imageSize(std::size_t width, std::size_t height,
                              std::size_t padding) const
        {
            Pt::ssize_t stride = width + padding;
            Pt::ssize_t planeSize = stride * height;

            return planeSize + planeSize / 2;
        }

    protected:
        virtual std::unique_ptr<ImageFormat> onClone() const override
        {
            return std::unique_ptr<ImageFormat>(new Yuv12);
        }

        virtual const std::type_info& onGetType() const override
        { 
            return typeid(*this); 
        }

        virtual std::size_t onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                        std::size_t padding) const override;

        virtual PixelBase* onCreatePixel(Pt::uint8_t* data, ViewBase& view, 
                                         Pt::ssize_t x, Pt::ssize_t y, 
                                         PixelStorage& store) const override;
    
        virtual ConstPixelBase* onCreateConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                                                   Pt::ssize_t x, Pt::ssize_t y, 
                                                   PixelStorage& store) const override;

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

            Pt::ssize_t uvstride = stride / 2;
            Pt::ssize_t subPlaneSize = planeSize / 4;

            Pt::ssize_t subXPos = xpos / 2;
            Pt::ssize_t subYPos = ypos / 2;
            Pt::ssize_t subOffset = uvstride * subYPos + subXPos;

            Pt::ssize_t uOffset = planeSize + subOffset;
            u = data + uOffset;

            Pt::ssize_t vOffset = uOffset + subPlaneSize;
            v = data + vOffset;

            return uvstride;
        }

        template <typename T>
        static void advance(T*& y, T*& u, T*& v, 
                            Pt::ssize_t& xpos, Pt::ssize_t& ypos,
                            Pt::ssize_t width, Pt::ssize_t padding,
                            Pt::ssize_t uvstride)
        {
            ++y;

            if( ++xpos >= width )
            {
                ++u;
                ++v;

                if(ypos % 2 == 0)
                {
                  u -= uvstride;
                  v -= uvstride;
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
        static void advanceBy(T*& y, T*& u, T*& v, ssize_t n,
                              ssize_t& xpos, ssize_t& ypos, ssize_t width,
                              ssize_t ystride, ssize_t uvstride)
        {
            Pt::ssize_t new_abs_pos  = (ypos * width) + xpos + n;
            Pt::ssize_t new_ypos = new_abs_pos / width;
            Pt::ssize_t new_xpos = new_abs_pos % width;

            y += (new_ypos - ypos) * ystride + (new_xpos - xpos);
   
            Pt::ssize_t old_u_row = ypos / 2;
            Pt::ssize_t new_u_row = new_ypos / 2;
    
            Pt::ssize_t old_u_col = xpos / 2;
            Pt::ssize_t new_u_col = new_xpos / 2;

            Pt::ssize_t uv_offset = (new_u_row - old_u_row) * uvstride + (new_u_col - old_u_col);
    
            u += uv_offset;
            v += uv_offset;

            xpos = new_xpos;
            ypos = new_ypos;
        }

        static void sourceCopy(Pt::uint8_t* y, Pt::uint8_t* u, Pt::uint8_t* v,
                               const Pt::uint8_t* cy, const Pt::uint8_t* cu, const Pt::uint8_t* cv, 
                               std::size_t length)
        {
        }
};

} // namespace

} // namespace


#include <Pt/Gfx/BasicView.h>
#include <Pt/Gfx/Argb32.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// Yuv12Pixel
///////////////////////////////////////////////////////////////////////

inline Yuv12Pixel::Yuv12Pixel(BasicView<Yuv12>& view, 
                              Pt::ssize_t xpos, Pt::ssize_t ypos)
: _view(view)
, _xpos(xpos)
, _ypos(ypos)
, _subStride(0)
, _y(0)
, _u(0)
, _v(0)
{
    _subStride = Yuv12::init(view.data(), _view.stride(), 
                             _view.width(), _view.height(),
                             xpos,  ypos, _y, _u, _v);
}


inline Yuv12Pixel::Yuv12Pixel(Pt::uint8_t* data, ViewBase& view, 
                              Pt::ssize_t xpos, Pt::ssize_t ypos)
: _view(view)
, _xpos(xpos)
, _ypos(ypos)
, _subStride(0)
, _y(0)
, _u(0)
, _v(0)
{
    _subStride = Yuv12::init(data, view.stride(), 
                             view.width(), view.height(),
                             xpos,  ypos, _y, _u, _v);
}


inline bool Yuv12Pixel::equals(const Yuv12Pixel& p) const
{ 
    return _y == p._y; 
}


inline bool Yuv12Pixel::equals(const Yuv12ConstPixel& p) const
{ 
    return _y == p._y; 
}


inline Color Yuv12Pixel::toColor() const
{ 
    return Yuv12::getColor(*_y, *_u, *_v);
}


inline Yuv12Pixel& Yuv12Pixel::operator=(const Color& color)
{
    Yuv12::fromColor(_y, _u, _v, color);
    return *this;
}


inline Yuv12Pixel& Yuv12Pixel::operator=(const Argb32Color& color)
{
    Yuv12::fromColor( _y, _u, _v, Color::fromRgb8(color.red(), color.green(), 
                                                 color.blue(), color.alpha() ) );
    return *this;
}


inline void Yuv12Pixel::advance()
{
    Yuv12::advance(_y, _u, _v, _xpos, _ypos,
                   _view.width(), _view.padding(), _subStride);
}


inline void Yuv12Pixel::advance(Pt::ssize_t n)
{
    Yuv12::advanceBy(_y, _u, _v, n, _xpos, _ypos,
                      _view.width(), _view.stride(), _subStride);
}

///////////////////////////////////////////////////////////////////////
// Yuv12ConstPixel
///////////////////////////////////////////////////////////////////////

inline Yuv12ConstPixel::Yuv12ConstPixel(const BasicConstView<Yuv12>& view, 
                                        Pt::ssize_t xpos, Pt::ssize_t ypos)
: _view( view )
, _xpos(xpos)
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


inline Yuv12ConstPixel::Yuv12ConstPixel(const BasicView<Yuv12>& view, 
                                        Pt::ssize_t xpos, Pt::ssize_t ypos)
: _view( view )
, _xpos(xpos)
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


inline Yuv12ConstPixel::Yuv12ConstPixel(const Pt::uint8_t* data, const ViewBase& view,
                                        Pt::ssize_t xpos, Pt::ssize_t ypos)
: _view(view)
, _xpos(xpos)
, _ypos(ypos)
, _subStride(0)
, _y(0)
, _u(0)
, _v(0)
{
    _subStride = Yuv12::init(data, view.stride(), 
                             view.width(), view.height(), 
                             xpos,  ypos, _y, _u, _v);
}


inline Color Yuv12ConstPixel::toColor() const
{ 
    return Yuv12::getColor(*_y, *_u, *_v);
}


inline void Yuv12ConstPixel::advance()
{
    Yuv12::advance(_y, _u, _v, _xpos, _ypos,
                   _view.width(), _view.padding(), _subStride);
}


inline void Yuv12ConstPixel::advance(Pt::ssize_t n)
{
    Yuv12::advanceBy(_y, _u, _v, n, _xpos, _ypos,
                      _view.width(), _view.stride(), _subStride);
}


//template <typename FormatT1, typename FormatT2,
//         typename TraitsT1, typename TraitsT2>
//BasicPixelIterator<FormatT2, TraitsT2> copy(const BasicConstSpan<FormatT1, TraitsT1>& from, 
//                                            BasicPixelIterator<FormatT2, TraitsT2> to)
//{
//    to->assign(from.front(), from.length());
//    return to += from.length();
//}

} // namespace

} // namespace

#endif
