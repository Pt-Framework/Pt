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
#include <Pt/Gfx/FontFace.h>

namespace Pt {

namespace Gfx {

Font::Font()
: _fontData( new FontData() )
{
}


Font::Font(const std::string& name, std::size_t size, const std::string& style)
: _fontData( new FontData(name, size, style) )
{
}


Font::Font(const FontFace& face, std::size_t size)
: _fontData(new FontData(face.name(), size, face.style()))
{
}


const std::string& Font::name() const
{
    return _fontData->name();
}


size_t Font::size() const
{
    return _fontData->size();
}


const std::string& Font::style() const
{
    return _fontData->style();
}

} // namespace

} // namespace
