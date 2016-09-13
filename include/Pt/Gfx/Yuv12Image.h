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
#include <Pt/Gfx/BasicImage.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

class Yuv12Format : public ImageFormat
{   
    public:
        Yuv12Format();

        virtual std::size_t imageSize(const Size& size, Pt::ssize_t padding) const
        { return 0; }
        
        virtual void setPixel(Pixel& to, const Pixel& from,
                              CompositionMode mode) const
        {}

        virtual void setPixel(Pixel& pixel, const Color& c,
                              CompositionMode mode) const
        {}
        
        virtual Color getColor(const Pixel& pixel) const
        { return Color(0, 0, 0); }

        virtual void copy(Pixel& dst, const Pixel& src, size_t length, 
                          CompositionMode mode) const
        {}

    protected:
        virtual void onCopy(const ImageView& to, const Point& toPoint,
                            const ImageView& from, const Rect& fromRect,
                            CompositionMode mode) const
        {}
};


class Yuv12Pixel
{
    public:
        Yuv12Pixel(const ImageView& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        : _view(&view)
        , _xpos(xpos)
        , _ypos(ypos)
        { 
            std::size_t off = (_view->size().width() + _view->padding()) * ypos + xpos;
            _y = _view->data() + off;
        }

        Yuv12Pixel(const Yuv12Pixel& p)
        : _view(p._view)
        , _xpos(p._xpos)
        , _ypos(p._ypos)
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

        void reset(const Yuv12Pixel& p)
        {
             _view = p._view;            
             _xpos = p._xpos;
             _ypos = p._ypos;

            _y = p._y;
            _u = p._u;
            _v = p._v;
        }

        void advance()
        {
            ++_y;

            size_t uvOffset = _xpos % 2;
            _u += uvOffset;
            _v += uvOffset;

            if( ++_xpos == _view->size().width() )
            {
                _xpos = 0;
                ++_ypos;

                _y += (_view->size().width() / 2) + _view->padding();
                _u += (_view->size().width() / 2) + _view->padding();
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
        Pt::uint8_t* _y;
        Pt::uint8_t* _u;
        Pt::uint8_t* _v;
};


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
