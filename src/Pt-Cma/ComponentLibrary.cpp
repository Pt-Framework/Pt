#include "Pt/Cma/ComponentLibrary.h"
#include "Pt/Cma/IComponentBuilder.h"
#include <iostream>

namespace Pt {

namespace Cma {

ComponentLibrary::ComponentLibrary(const std::string& file)
: SharedLib(file)
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
    return SharedLib::path();
}

}

}
