/* Copyright (C) 2016 Marc Boris Duerner 
   Copyright (C) 2016 Laurentiu-Gheorghe Crisan
  
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

#ifndef PT_GFX_ARGB8888FORMAT_H
#define PT_GFX_ARGB8888FORMAT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ImageFormat.h>

namespace Pt {

namespace Gfx {

class PT_GFX_API Argb8888Format : public ImageFormat
{
    public:
        Argb8888Format();

        virtual std::size_t imageSize(const ImageInfo& image) const;
        
        virtual void setPixel(Pixel& to, const Pixel& from,
                              CompositionMode mode) const;

        virtual void setPixel(Pixel& pixel, const Color& c,
                              CompositionMode mode) const;
        
        virtual Color getColor(const Pixel& pixel) const;

        virtual void copy(Pixel& dst, const Pixel& src, size_t length, 
                          CompositionMode mode) const;

    protected:
        virtual void onCopy(const ImageInfo& to, const Point& toPoint,
                            const ImageInfo& from, const Rect& fromRect,
                            CompositionMode mode) const;
};


class Yuv12View
{
    public:
        Yuv12View(Pt::uint8_t* data, const Size& size, Pt::ssize_t padding)
        : _data(data)
        , _size(size)
        , _padding(padding)
        { }

        const Size& size() const
        { return _size; }

        Pt::uint8_t* data() const
        { return _data; }

        Pt::ssize_t padding() const
        { return _padding; }

    private:
        Pt::uint8_t* _data;
        Size         _size;
        Pt::ssize_t  _padding;
};


class Yuv12Image
{
};


class Yuv12Pixel
{
    public:
        Yuv12Pixel(const Yuv12View& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
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
        const Yuv12View*  _view;
        Pt::ssize_t _xpos;
        Pt::ssize_t _ypos;
        Pt::uint8_t* _y;
        Pt::uint8_t* _u;
        Pt::uint8_t* _v;
};


class Yuv12Iterator
{
    public:
        Yuv12Iterator(const Yuv12View& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
        : _pixel(view, xpos, ypos)
        {
        }

        Yuv12Iterator& operator=(const Yuv12Iterator& it)
        {
            _pixel.reset(it._pixel);
            return *this;
        }

        Yuv12Pixel operator*()
        { return _pixel; }

        Yuv12Iterator& operator++()
        {
            _pixel.advance();
            return *this; 
        }

        bool operator!=(const Yuv12Iterator& it) const
        { return _pixel != it._pixel; }
        
        bool operator==(const Yuv12Iterator& it) const
        { return _pixel == it._pixel; }

    private:
        Yuv12Pixel _pixel;
};


} // namespace

} // namespace

#endif
