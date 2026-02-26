/* Copyright (C) 2015 Marc Boris Duerner

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

#ifndef PT_GFX_VIEWBASE_H
#define PT_GFX_VIEWBASE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

class ViewBase
{
    public:
        typedef Pt::ssize_t pos_t;
        typedef Pt::ssize_t length_t;

    public:
        ViewBase()
        : _xpos(0)
        , _ypos(0)
        , _width(0)
        , _height(0)
        , _padding(0)
        , _stride(0)
        { }

        ViewBase(Pt::ssize_t width, Pt::ssize_t height, 
                 Pt::ssize_t stride, Pt::ssize_t padding = 0)
        : _xpos(0)
        , _ypos(0)
        , _width(width)
        , _height(height)
        , _padding(padding)
        , _stride(stride)
        { }

        ViewBase(Pt::ssize_t xpos, Pt::ssize_t ypos,
                 Pt::ssize_t width, Pt::ssize_t height, 
                 Pt::ssize_t stride, Pt::ssize_t padding = 0)
        : _xpos(xpos)
        , _ypos(ypos)
        , _width(width)
        , _height(height)
        , _padding(padding)
        , _stride(stride)
        { }

        Pt::ssize_t xpos() const
        { return _xpos; }

        Pt::ssize_t ypos() const
        { return _ypos; }

        Pt::ssize_t width() const
        { return _width; }

        Pt::ssize_t height() const
        { return _height; }

        bool empty() const
        { return _width == 0 || _height == 0; }

        Pt::ssize_t stride() const
        { return _stride; }

        Pt::ssize_t padding() const
        { return _padding; }

    private:      
        Pt::ssize_t   _xpos;
        Pt::ssize_t   _ypos;
        Pt::ssize_t   _width;
        Pt::ssize_t   _height;
        Pt::ssize_t   _padding;
        Pt::ssize_t   _stride;
};

} // namespace

} // namespace

#endif
