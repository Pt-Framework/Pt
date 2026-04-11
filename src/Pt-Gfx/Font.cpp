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
#include <Pt/Gfx/FontRegistry.h>

namespace Pt {

namespace Gfx {

Font::Font()
: _fontData( new FontData() )
{
}


Font::Font(const std::string& family, std::size_t size,
           const std::string& styleName, Weight weight, Slant slant,
           Stretch stretch)
: _fontData(new FontData(family, size, weight, slant, stretch, styleName))
{
}


Font::Font(const std::string& family, std::size_t size,
           Weight weight, Slant slant, Stretch stretch)
: _fontData(new FontData(family, size, weight, slant, stretch))
{
}


const std::string& Font::family() const
{
    return _fontData->family();
}


size_t Font::size() const
{
    return _fontData->size();
}


const std::string& Font::styleName() const
{
    return _fontData->styleName();
}


bool Font::hasStyleName() const
{
    return _fontData->hasStyleName();
}


Font::Weight Font::weight() const
{
    return _fontData->weight();
}


Font::Slant Font::slant() const
{
    return _fontData->slant();
}


Font::Stretch Font::stretch() const
{
    return _fontData->stretch();
}


void Font::addFonts(const System::Path& path)
{
    FontRegistry::instance().addFonts(path);
}


bool Font::addFont(const System::Path& path)
{
    return FontRegistry::instance().addFont(path);
}


bool Font::removeFont(const System::Path& path)
{
    return FontRegistry::instance().removeFont(path);
}


const std::vector<System::Path>& Font::fontFiles()
{
    return FontRegistry::instance().fontFiles();
}

} // namespace

} // namespace
