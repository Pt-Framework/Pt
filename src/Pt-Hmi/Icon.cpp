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

#include <map>
#include <algorithm>
#include <cstddef>

namespace Pt {

namespace Hmi {

/////////////////////////////////////////////////////////////////////////////
// DefaultIconProvider
/////////////////////////////////////////////////////////////////////////////

class DefaultIconProvider : public IconProvider
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
        DefaultIconProvider()
        {
        }

        virtual ~DefaultIconProvider()
        {
        }
        
        bool empty() const
        {
          return _images.empty();
        }

        void clear()
        {
            _images.clear();
        }

        void addImage(const Gfx::SizeF& size, const Gfx::Image& image)
        {
            _images[size] = Entry(image);
        }

        void addImage(const Gfx::SizeF& size, const System::Path& path)
        {
            _images[size] = Entry(path);
        }

        virtual const Gfx::Image& getImage(const Gfx::SizeF& sizeF)
        {
            const Gfx::Size size = round(sizeF);

            std::map<Gfx::SizeF, Entry>::iterator match;
            match = _images.begin();

            std::map<Gfx::SizeF, Entry>::iterator it;
            for(it = _images.begin(); it != _images.end(); ++it)
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

            if( match == _images.end() )
                return _missing;

            Entry& entry = match->second;
            if( entry.image.empty() )
                Application::instance().loadImage(entry.path, entry.image);

            return entry.image;
        }

        const System::Path* getPath(const Gfx::SizeF& sizeF) const
        {
            return 0;;
        }

        Gfx::SizeF minimumSize() const
        {
            return empty() ? Gfx::SizeF() : _images.begin()->first;
        }


        Gfx::SizeF maximumSize() const
        {
            return empty() ? Gfx::SizeF() : _images.rbegin()->first;
        }

    private:
        std::map<Gfx::SizeF, Entry> _images;
        Gfx::Image                  _missing;
};

/////////////////////////////////////////////////////////////////////////////
// IconImpl
/////////////////////////////////////////////////////////////////////////////

class IconImpl
{
    public:
        IconImpl()
        : _refCount(1)
        , _provider(0)
        , _defaultProvider(0)
        { 
            _defaultProvider = new DefaultIconProvider;
            _provider = _defaultProvider;
        }

        IconImpl(const IconImpl& other)
        : _refCount(1)
        , _provider(0)
        , _defaultProvider(0)
        { 
            if(other._defaultProvider)
            {
                _defaultProvider = new DefaultIconProvider(*other._defaultProvider);
                _provider = _defaultProvider;
            }
            else
            {
                setProvider(other._provider);
            }
        }

        IconImpl(IconProvider& provider)
        : _refCount(1)
        , _provider(0)
        , _defaultProvider(0)
        {
            setProvider(&provider);
        }

        ~IconImpl()
        {
            if(_defaultProvider)
            {
                delete _provider;
            }
            else
            {
                setProvider(0);
            }
        }

        void setProvider(IconProvider* provider)
        {
            if(_provider)
                _provider->detachIcon(this);

            _provider = provider;

            if(_provider)
                _provider->attachIcon(this);
        }

        bool empty() const
        {
            return _provider ? _provider->empty() : true;
        }

        void clear()
        {
            if(_provider)
                _provider->clear();
        }

        void addImage(const Gfx::SizeF& size, const Gfx::Image& image)
        {
            if(_provider)
                _provider->addImage(size, image);
        }

        void addImage(const Gfx::SizeF& size, const System::Path& path)
        {
            if(_provider)
                _provider->addImage(size, path);
        }

        const Gfx::Image& getImage(const Gfx::SizeF& sizeF)
        {
            if( ! _provider )
                throw std::logic_error("invalid provider");
            
            return _provider->getImage(sizeF);
        }

        Gfx::SizeF minimumSize() const
        {
            return _provider ? _provider->minimumSize() : Gfx::SizeF();
        }

        Gfx::SizeF maximumSize() const
        {
            return _provider ? _provider->maximumSize() : Gfx::SizeF();
        }

    public:
        size_t refs() const
        {
            return _refCount;
        }

        void ref()
        {
            _refCount++;
        }

        size_t unref()
        {
            return --_refCount;
        }

    private:
        size_t               _refCount;
        IconProvider*        _provider;
        DefaultIconProvider* _defaultProvider;
};


/////////////////////////////////////////////////////////////////////////////
// IconProvider
/////////////////////////////////////////////////////////////////////////////

IconProvider::IconProvider()
{
}


IconProvider::~IconProvider()
{
    while( ! _icons.empty() )
      _icons.back()->setProvider(0);
}


void IconProvider::attachIcon(IconImpl* icon)
{
    _icons.push_back(icon);
}


void IconProvider::detachIcon(IconImpl* icon)
{
    _icons.erase( std::remove(_icons.begin(), _icons.end(), icon),
                  _icons.end() );
}

/////////////////////////////////////////////////////////////////////////////
// Icon
/////////////////////////////////////////////////////////////////////////////

Icon::Icon()
: _impl(0)
{
    _impl = new IconImpl();
}


Icon::Icon(IconProvider& provider)
: _impl(0)
{
  _impl = new IconImpl(provider);
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
    Gfx::SizeF size( image.width(), image.height() );
    addImage(size, image);
}


void Icon::addImage(const Gfx::SizeF& size, const Gfx::Image& image)
{
    detach();
    _impl->addImage(size, image);
}


void Icon::addImage(const Gfx::SizeF& size, const System::Path& path)
{
    detach();
    _impl->addImage(size, path);
}


void Icon::addImage(double width, double height, const System::Path& path)
{
    Gfx::SizeF size(width, height);
    addImage(size, path);
}


const Gfx::Image& Icon::getImage(const Gfx::SizeF& sizeF) const
{
    return _impl->getImage(sizeF);
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
