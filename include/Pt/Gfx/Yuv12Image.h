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
/**
*/
class Yuv12Pixel
{
    public:
        Yuv12Pixel(const ImageView& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        : _view(&view)
        , _xpos(xpos)
        , _ypos(ypos)
        { 
            Pt::ssize_t stride = _view->size().width() + _view->padding();
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

        Pt::uint8_t y() const
        { return *_y; }

        Pt::uint8_t u() const
        { return *_u; }

        Pt::uint8_t v() const
        { return *_v; }

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

        bool operator!=(const Yuv12Pixel& p) const
        { return _y != p._y; }
        
        bool operator==(const Yuv12Pixel& p) const
        { return _y == p._y; }

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
