/* Copyright (C) 2024 Marc Boris Duerner

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

#include <Pt/Gfx/FontRegistry.h>
#include <Pt/Gfx/FontProvider.h>
#include <Pt/System/Directory.h>
#include <Pt/System/FileInfo.h>

#include <algorithm>

namespace Pt {

namespace Gfx {

FontRegistry& FontRegistry::instance()
{
    static FontRegistry registry;
    return registry;
}

FontRegistry::FontRegistry()
{
}


FontRegistry::~FontRegistry()
{
    while( ! _providers.empty() )
    {
        FontProvider* provider = _providers.back();
        _providers.pop_back();
        provider->unregisterFontRegistry();
    }
}


void FontRegistry::addFonts(const System::Path& path)
{
    if( ! System::FileInfo::exists(path) )
        return;

    System::DirectoryIterator it(path);
    System::DirectoryIterator end;

    for(; it != end; ++it)
    {
        if(it->path().fileName() == "." || it->path().fileName() == "..")
            continue;

        addFont(path / it->path());
    }
}


bool FontRegistry::addFont(const System::Path& path)
{
    if( ! System::FileInfo::exists(path) )
        return false;

    if(std::find(_fontFiles.begin(), _fontFiles.end(), path) != _fontFiles.end())
        return true;

    _fontFiles.push_back(path);

    for(std::vector<FontProvider*>::iterator it = _providers.begin(); it != _providers.end(); ++it)
        (*it)->addFont(path);

    return true;
}


bool FontRegistry::removeFont(const System::Path& path)
{
    std::vector<System::Path>::iterator pos = std::find(_fontFiles.begin(), _fontFiles.end(), path);
    if(pos == _fontFiles.end())
        return false;

    for(std::vector<FontProvider*>::iterator it = _providers.begin(); it != _providers.end(); ++it)
        (*it)->removeFont(path);

    _fontFiles.erase(pos);
    return true;
}


const std::vector<System::Path>& FontRegistry::fontFiles() const
{
    return _fontFiles;
}


void FontRegistry::registerProvider(FontProvider& provider)
{
    if(provider._fontRegistry == this)
        return;

    if(provider._fontRegistry)
        provider._fontRegistry->unregisterProvider(provider);

    if(std::find(_providers.begin(), _providers.end(), &provider) != _providers.end())
        return;

    _providers.push_back(&provider);
    provider.registerFontRegistry(*this);
}


void FontRegistry::unregisterProvider(FontProvider& provider)
{
    std::vector<FontProvider*>::iterator it = std::find(_providers.begin(), _providers.end(), &provider);
    if(it != _providers.end())
        _providers.erase(it);

    if(provider._fontRegistry == this)
        provider.unregisterFontRegistry();
}

} // namespace

} // namespace
