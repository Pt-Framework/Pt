/* Copyright (C) 2019 Marc Boris Duerner
 
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

#include <Pt/Hmi/Icon.h>
#include <Pt/Gfx/PngReader.h>

namespace Pt {

namespace Hmi {

Icon::Icon()
{
}


Icon::~Icon()
{
}


Icon::Icon(const Icon& icon)
: _images(icon._images)
{
}


Icon& Icon::operator=(const Icon& icon)
{
    _images = icon._images;
    return *this;
}


bool Icon::empty() const
{ 
    return _images.empty(); 
}


void Icon::clear()
{ 
    _images.clear(); 
}


void Icon::addImage(const Gfx::SizeF& size, const Pt::System::Path& path)
{ 
    _images[size] = path; 
}


const Pt::System::Path* Icon::getImage(const Gfx::SizeF& area) const
{
    std::map<Gfx::SizeF, Pt::System::Path>::const_iterator match = _images.end();

    std::map<Gfx::SizeF, Pt::System::Path>::const_iterator it;
    for(it = _images.begin(); it != _images.end(); ++it)
    {
        const Gfx::SizeF& size = it->first;
                
        if( size.width() <= area.width() && 
            size.height() <= area.height() )
        {
            if( match == _images.end() )
            {
                match = it;
            }
            else
            {
                const Gfx::SizeF& matchSize = match->first;
                double m = matchSize.width() * matchSize.height();
                double n = size.width() * size.height();
                if(n > m)
                  match = it;
            }
        }
    }

    return match != _images.end() ? &match->second : 0; 
}

} // namespace

} // namespace
