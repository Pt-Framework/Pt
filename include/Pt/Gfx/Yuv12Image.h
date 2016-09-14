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
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Yuv12Format.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

class Yuv12Pixel
{
    public:
        Yuv12Pixel(const ImageView& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        : _view(&view)
        , _xpos(xpos)
        , _ypos(ypos)
        { 
            Pt::ssize_t stride = _view->stride();
            Pt::ssize_t planeSize = stride * _view->size().height();
            
            _subStride = stride / 2;
            Pt::ssize_t subPlaneSize = planeSize / 4;

            Pt::ssize_t yOffset = stride * ypos + xpos;
            _y = _view->data() + yOffset;

            Pt::ssize_t subXPos = xpos / 2;
            Pt::ssize_t subYPos = ypos / 2;
            Pt::ssize_t subOffset = _subStride * subYPos + subXPos;

            Pt::ssize_t uOffset = planeSize + subOffset;
            _u = _view->data() + uOffset;

            Pt::ssize_t vOffset = uOffset + subPlaneSize;
            _v = _view->data() + vOffset;
        }

        Yuv12Pixel(const Yuv12Pixel& p)
        : _view(p._view)
        , _xpos(p._xpos)
        , _ypos(p._ypos)
        , _subStride(p._subStride)
        , _y(p._y)
        , _u(p._u)
        , _v(p._v)
        { }

        Yuv12Pixel& operator=(const Yuv12Pixel& p)
        {
            *_y = *(p._y);
            *_u = *(p._u);
            *_v = *(p._v);
            return *this;
        }

        Yuv12Pixel& operator=(const Color& color)
        {
            assign(color, CompositionMode::SourceCopy);
            return *this;
        }

        void reset(const Yuv12Pixel& p)
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
            ++_y;

            if( ++_xpos >= _view->width() )
            {
                ++_u;
                ++_v;
                
                if(_ypos % 2 == 0)
                {
                  _u -= _subStride;
                  _v -= _subStride;
                }

                _xpos = 0;
                ++_ypos;
            }
            else  if(_xpos % 2 == 0)
            {
                ++_u;
                ++_v;
            }
        }

        //void advance( Pt::ssize_t n )
        //{
        //}

        void assign(const Color& color, CompositionMode)
        {
            Pt::int32_t r = color.red();
            Pt::int32_t g = color.green();
            Pt::int32_t b = color.blue();

            Pt::int32_t Y = (( 66 * r + 129 * g +  25 * b + 128) >> 16) +  16;
            Pt::int32_t U = ((-38 * r -  74 * g + 112 * b + 128) >> 16) + 128;
            Pt::int32_t V = ((112 * r -  94 * g -  18 * b + 128) >> 16) + 128;

            *_y = Y > 255 ? 255 : static_cast<Pt::uint8_t>(Y);
            *_u = U > 255 ? 255 : static_cast<Pt::uint8_t>(U);
            *_v = V > 255 ? 255 : static_cast<Pt::uint8_t>(V);
        }

        void assign(const Yuv12Pixel& p, CompositionMode)
        {
            *_y = *(p._y);
            *_u = *(p._u);
            *_v = *(p._v);
        }

        Color toColor() const
        {
            Pt::uint32_t R = ( 298 * (*_y - 16)                     + 409 * (*_v - 128) + 128);
            Pt::uint32_t G = ( 298 * (*_y - 16) - 100 * (*_u - 128) - 208 * (*_v - 128) + 128);
            Pt::uint32_t B = ( 298 * (*_y - 16) + 516 * (*_u - 128)                     + 128);

            Pt::uint16_t r = R > 65535 ? 65535 : static_cast<Pt::uint16_t>(R);
            Pt::uint16_t g = G > 65535 ? 65535 : static_cast<Pt::uint16_t>(G);
            Pt::uint16_t b = B > 65535 ? 65535 : static_cast<Pt::uint16_t>(B);
            
            return Color(r, g, b);
        }

        bool operator!=(const Yuv12Pixel& p) const
        { return _y != p._y; }
        
        bool operator==(const Yuv12Pixel& p) const
        { return _y == p._y; }

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

    private:
        const ImageView*  _view;
        Pt::ssize_t _xpos;
        Pt::ssize_t _ypos;
        Pt::ssize_t _subStride;
        Pt::uint8_t* _y;
        Pt::uint8_t* _u;
        Pt::uint8_t* _v;
};

/** @brief YV-12 Image.

    If the Y plane has pad bytes after each row, then the U and V planes have
    half as many pad bytes after their rows. In other words, two U/V rows 
    (including padding) is exactly as long as one Y row (including padding).
*/
class Yuv12Image : public BasicImage<Yuv12Pixel, Yuv12Format>
{
    public:
        Yuv12Image(const Size& size, size_t padding = 0)
        : BasicImage(size, padding)
        { }
        
        Yuv12Image(Pt::uint8_t* data, const Size& size, size_t padding = 0)
        : BasicImage(data, size, padding)
        { }
};

} // namespace

} // namespace

#endif
