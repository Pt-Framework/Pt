/* Copyright (C) 2016 Laurentiu-Gheorghe Crisan
   Copyright (C) 2016 Marc Boris Duerner
 
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

#include "PictureImpl.h"
#include <Pt/Hmi/Picture.h>

namespace Pt {

namespace Hmi {

Picture::Picture()
: _impl( new PictureImpl )
{
}


Picture::Picture(const Gfx::Image& image)
: _impl( new PictureImpl )
{
    _impl->set(image);
}


Picture::Picture(const Gfx::Image& image, Pt::uint16_t alphaThreshold)
: _impl( new PictureImpl )
{
    _impl->set(image, alphaThreshold);
}


Picture::Picture(const Picture& p)
: _impl( new PictureImpl( *p.impl() ) )
{
}


Picture::~Picture()
{
    delete _impl;
}


Picture& Picture::operator =(const Picture& p)
{
    *_impl = *p._impl;
    return *this;
}


bool Picture::empty() const
{
    return _impl->empty();
}


void Picture::set(const Gfx::Image& image)
{
    _impl->set(image);
}

void Picture::set(const Gfx::Image& image,  Pt::uint16_t alphaThreshold)
{
  _impl->set(image, alphaThreshold);
}


size_t Picture::width() const
{
    return _impl->width();
}


size_t  Picture::height() const
{
    return _impl->height();
}


PictureImpl* Picture::impl()
{
    return _impl;
}


const PictureImpl* Picture::impl() const
{
    return _impl;
}

} // namespace

} // namespace
