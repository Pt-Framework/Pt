/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2010 Aloysius Indrayanto

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

#include <Pt/Gfx/Font.h>

namespace Pt {

namespace Gfx {

Font::Font()
: _name()
, _size(0)
, _style(Normal)
, _angle(0)
{
}


Font::Font(const std::string& name, std::size_t size, Style style, Pt::ssize_t angle)
: _name(name)
,  _size(size)
,  _style(style)
,  _angle(angle)
{
}


Font::Font( const std::string& name, const Font& font)
: _name(name)
, _size(font._size)
, _style(font._style)
, _angle(font._angle)
{
}

const std::string& Font::name() const
{
    return _name;
}


size_t Font::size() const
{
    return _size;
}


Font::Style Font::style() const
{
    return _style;
}


ssize_t Font::angle() const
{
    return _angle;
}


bool Font::isNull() const
{
    return size() == 0;
}

} // namespace

} // namespace
