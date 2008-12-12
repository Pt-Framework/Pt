/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "Pt/Cma/ComponentLibrary.h"
#include "Pt/Cma/IComponentBuilder.h"
#include <iostream>

namespace Pt {

namespace Cma {

ComponentLibrary::ComponentLibrary(const std::string& file)
: Library(file)
{
    IComponentBuilder** builderList = (IComponentBuilder**) this->resolve("Ptv_ComponentList");

    if(!builderList)
       return;

    if( !(*builderList) )
        return;

    while(*builderList)
    {
        _builders[(*builderList)->typeId()]  = *builderList;
        builderList++;
    }
}


bool ComponentLibrary::isUsed()
{
    BuilderMap::iterator iter;

    for(iter = _builders.begin(); iter !=  _builders.end(); iter++)
    {
        if(iter->second->instances() != 0)
            return true;
    }

    return false;
}


IComponentBuilder* ComponentLibrary::getBuilder(const TypeId& typeId)
{
    BuilderMap::iterator iter;

    iter = _builders.find(typeId);

    if(iter == _builders.end())
        return 0;

    return iter->second;
}


IComponentBuilder* ComponentLibrary::getBuilder()
{
    if(_builders.empty())
        return 0;

    return _builders.begin()->second;
}


size_t ComponentLibrary::size() const
{
    return _builders.size();
}

const std::string& ComponentLibrary::path() const
{
    return Library::path();
}

}

}
