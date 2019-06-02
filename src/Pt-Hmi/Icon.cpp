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
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/PngReader.h>

namespace Pt {

namespace Hmi {

class IconImpl
{
    public:
        IconImpl();

        struct Entry
        {
            Entry(Gfx::Image i)
                : image(i)
            {
            }

            Entry(const System::Path& p)
                :path(p)
            {
            }

            Entry()
            {
            }

            Gfx::Image   image;
            System::Path path;
        };

        std::map<Gfx::SizeF, Entry> images;
        size_t refCount;
};


IconImpl::IconImpl()
: refCount(1)
{

}

Icon::Icon()
: _data(new IconImpl())
{
}


Icon::~Icon()
{
    _data->refCount--;

    if (_data->refCount == 0)
        delete _data;
}


Icon::Icon(const Icon& icon)
: _data(icon._data)
{
    _data->refCount++;
}


Icon& Icon::operator=(const Icon& icon)
{
    if (icon._data == this->_data)
        return *this;

    _data->refCount--;

    if (_data->refCount == 0)
        delete _data;

    _data = icon._data;
    _data->refCount++;
    return *this;
}


bool Icon::empty() const
{ 
    return _data->images.empty();
}


void Icon::clear()
{
    if (_data->refCount > 1)
    {
        _data->refCount--;
        _data = new IconImpl();
    }
}


void Icon::addImage(const Gfx::SizeF& size, const Gfx::Image& image)
{
    if (_data->refCount > 1)
    {
        _data->refCount--;
        _data =  new IconImpl(*_data);
    }

    _data->images[size] = IconImpl::Entry(image);
}

void Icon::addImage(const Gfx::SizeF& size, const System::Path& path)
{
    if (_data->refCount > 1)
    {
        _data->refCount--;
        _data = new IconImpl(*_data);
    }

    _data->images[size] = IconImpl::Entry(path);
}


const Gfx::Image& Icon::getImage(const Gfx::SizeF& sizeF) const
{
    const Gfx::Size size = round(sizeF);

    std::map<Gfx::SizeF, IconImpl::Entry>::iterator match = _data->images.end();

    std::map<Gfx::SizeF, IconImpl::Entry>::iterator it;

    for(it = _data->images.begin(); it != _data->images.end(); ++it)
    {
        const Gfx::SizeF& imageSize = it->first;

        if( imageSize.width() <= size.width() &&
            imageSize.height() <= size.height() )
        {
            if( match == _data->images.end() )
            {
                match = it;
            }
            else
            {
                const Gfx::SizeF& matchSize = match->first;
                double m = matchSize.width() * matchSize.height();
                double n = imageSize.width() * imageSize.height();
                if (n > m)
                    match = it;
            }
        }
    }

    if( match == _data->images.end() )
    {
        if( empty() )
            throw std::logic_error("invalid icon");

        match = _data->images.begin();
    }

    if (match->second.image.empty())
        Application::instance().loadImage(match->second.path, match->second.image);

    return match->second.image;
}


Gfx::SizeF Icon::minimumSize() const
{
    return empty() ? Gfx::SizeF() : _data->images.begin()->first;
}


Gfx::SizeF Icon::maximumSize() const
{
    return empty() ? Gfx::SizeF() : _data->images.rbegin()->first;
}

} // namespace

} // namespace
