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

/////////////////////////////////////////////////////////////////////////////
// IconImpl
/////////////////////////////////////////////////////////////////////////////

class IconImpl
{
    public:
        struct Entry
        {
            Entry()
            { }

            Entry(const Gfx::Image& i)
            : image(i)
            { }

            Entry(const System::Path& p)
            : path(p)
            { }

            Gfx::Image   image;
            System::Path path;
        };

    public:
        IconImpl()
        : refCount(1)
        { }

        bool empty() const
        {
          return images.empty();
        }

        const Entry& front() const
        {
            return images.begin()->second;
        }

        const Entry& back() const
        { 
            return images.rbegin()->second;
        }

        void clear()
        {
            images.clear();
        }

        void insert(const Gfx::SizeF& size, const Gfx::Image& image)
        {
            images[size] = IconImpl::Entry(image);
        }

        void insert(const Gfx::SizeF& size, const System::Path& path)
        {
            images[size] = IconImpl::Entry(path);
        }

        Entry* findBest(const Gfx::SizeF& sizeF)
        {
            const Gfx::Size size = round(sizeF);

            std::map<Gfx::SizeF, IconImpl::Entry>::iterator match;
            match = images.begin();

            std::map<Gfx::SizeF, IconImpl::Entry>::iterator it;
            for(it = images.begin(); it != images.end(); ++it)
            {
                const Gfx::SizeF& imageSize = it->first;
                double imageArea = imageSize.width() * imageSize.height();

                const Gfx::SizeF& matchSize = match->first;
                double matchArea = matchSize.width() * matchSize.height();
                
                if( imageSize.width() <= size.width() &&
                    imageSize.height() <= size.height() && 
                    imageArea > matchArea)
                {
                    match = it;
                }
            }

            return match == images.end() ? 0 : &(match->second);
        }

        Gfx::SizeF minimumSize() const
        {
            return empty() ? Gfx::SizeF() : images.begin()->first;
        }


        Gfx::SizeF maximumSize() const
        {
            return empty() ? Gfx::SizeF() : images.rbegin()->first;
        }

    public:
        size_t refs() const
        {
            return refCount;
        }

        void ref()
        {
            refCount++;
        }

        size_t unref()
        {
            return --refCount;
        }

    private:
        std::map<Gfx::SizeF, Entry> images;
        size_t refCount;
};

/////////////////////////////////////////////////////////////////////////////
// Icon
/////////////////////////////////////////////////////////////////////////////

Icon::Icon()
: _impl(0)
{
    _impl = new IconImpl();
}


Icon::~Icon()
{
  if ( ! _impl->unref() )
      delete _impl;
}


Icon::Icon(const Icon& icon)
: _impl(0)
{
    _impl = icon._impl;
    _impl->ref();
}


Icon& Icon::operator=(const Icon& other)
{
    if(other._impl == this->_impl)
        return *this;

    other._impl->ref();

    if( ! _impl->unref() )
        delete _impl;

    _impl = other._impl;
    return *this;
}


void Icon::detach()
{
    if( _impl->refs() == 1 )
        return;

    IconImpl* other = _impl;
    _impl = new IconImpl(*other);

    if( ! other->unref() )
        delete other;
}


bool Icon::empty() const
{ 
    return _impl->empty();
}


void Icon::clear()
{
    detach();
    _impl->clear();
}


void Icon::addImage(const Gfx::Image& image)
{
    Gfx::SizeF size(image.width(), image.height());
    addImage(size, image);
}


void Icon::addImage(const Gfx::SizeF& size, const Gfx::Image& image)
{
    detach();
    _impl->insert(size, image);
}


void Icon::addImage(const Gfx::SizeF& size, const System::Path& path)
{
    detach();
    _impl->insert(size, path);
}


void Icon::addImage(double width, double height, const System::Path& path)
{
    Gfx::SizeF size(width, height);
    addImage(size, path);
}


const Gfx::Image& Icon::getImage(const Gfx::SizeF& sizeF) const
{
    IconImpl::Entry* match = _impl->findBest(sizeF);
    if( ! match )
        throw std::logic_error("invalid icon");

    if( match->image.empty() )
        Application::instance().loadImage(match->path, match->image);

    return match->image;
}


const System::Path* Icon::getPath(const Gfx::SizeF& sizeF) const
{
    IconImpl::Entry* match = _impl->findBest(sizeF);
    if( ! match )
        return 0;

    return &(match->path);
}


Gfx::SizeF Icon::minimumSize() const
{
    return _impl->minimumSize();
}


Gfx::SizeF Icon::maximumSize() const
{
    return _impl->maximumSize();
}

} // namespace

} // namespace
