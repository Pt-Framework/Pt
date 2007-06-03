#include "Pt/Cma/ComponentFactory.h"
#include "Pt/Cma/ComponentLibrary.h"
#include "Pt/Cma/IComponentBuilder.h"
#include "Pt/Cma/IPrefs.h"
#include "Pt/Cma/IUnknown.h"
#include "Pt/Cma/TypeId.h"
#include "Pt/SourceInfo.h"
#include "Pt/System/Directory.h"
#include "Pt/System/Environment.h"
#include "Pt/System/SharedLib.h"

#include <iostream>


namespace Pt {

namespace Cma {


ComponentFactory::ComponentFactory()
{
}


ComponentFactory::~ComponentFactory()
{
}


void ComponentFactory::loadLibrary(const Pt::System::File& libraryFile)
{
    this->_loadLibrary(libraryFile);
}


void ComponentFactory::loadLibrary(const Pt::System::Directory& baseDirectory, const std::string& libraryName)
{
    this->_loadLibrary(System::SharedLib::createLibraryFile(baseDirectory, libraryName));
}


bool ComponentFactory::unload(const Pt::System::File& libraryFile)
{
    LibraryList::iterator iter;

    for (iter = _libraries.begin(); iter != _libraries.end(); iter++)
    {
        if (libraryFile != (*iter)->libraryFile())
        {
            continue;
        }

        if((*iter)->isUsed())
        {
            continue;
        }

        delete (*iter);
        _libraries.erase(iter);

        return true;
    }

    return false;
}

bool ComponentFactory::unload(const Pt::System::Directory& baseDirectory, const std::string& libraryName)
{
    return this->unload(System::SharedLib::createLibraryFile(baseDirectory, libraryName));
}


void ComponentFactory::unloadAll()
{
    LibraryList::iterator iter;

    iter = _libraries.begin();
    while ( iter != _libraries.end() )
    {
        if ((*iter)->isUsed())
        {
            throw std::logic_error("One or more component libaries are in use" + PT_SOURCEINFO);
        }

        delete (*iter);

        iter = _libraries.erase(iter);
    }
}


IUnknown* ComponentFactory::createComponent(const std::string& componentTypeName, bool loadConfig)
{
    TypeId componentTypeId(componentTypeName);

    LibraryList::iterator iter;
    for (iter = _libraries.begin(); iter != _libraries.end(); iter++)
    {
        IComponentBuilder* builder = (*iter)->getBuilder(componentTypeId);

        if (builder)
        {
            IUnknown* component = builder->createComponent();

            if (loadConfig && component)
            {
                this->loadConfiguration(**iter, *component, *builder);
            }

            return component;
        }
    }

    std::string message(componentTypeName);
    message += " component not found.";

    throw std::invalid_argument(message + PT_SOURCEINFO);
    return 0;
}


IUnknown* ComponentFactory::loadComponent(const Pt::System::File& libraryFile, bool loadConfig)
{
    ComponentLibrary* lib = this->library(libraryFile);

    if (lib == 0)
    {
        lib = this->_loadLibrary(libraryFile);

        if (lib == 0)
        {
            // Library has no builders, so we can not return an IUnknown.
            return 0;
        }
    }

    IComponentBuilder* builder   = lib->getBuilder();
    IUnknown*          component = builder->createComponent();

    if (loadConfig && component)
    {
        this->loadConfiguration(*lib, *component, *builder);
    }

    return component;
}


IUnknown* ComponentFactory::loadComponent(const Pt::System::Directory& baseDirectory,
                                          const std::string& libraryName,
                                          bool loadConfig)
{
    return this->loadComponent(System::SharedLib::createLibraryFile(baseDirectory, libraryName), loadConfig);
}


bool ComponentFactory::isLoaded(const Pt::System::File& libraryFile)
{
    return this->library(libraryFile) != 0;
}


bool ComponentFactory::isLoaded(const Pt::System::Directory& baseDirectory, const std::string& libraryName)
{
    return this->library(System::SharedLib::createLibraryFile(baseDirectory, libraryName)) != 0;
}


ComponentLibrary* ComponentFactory::_loadLibrary(const Pt::System::File& libraryFile)
{
    ComponentLibrary* lib = this->library(libraryFile);

    if (lib)
    {
        return lib;  // We already have loaded this library before.
    }


    lib = new ComponentLibrary(libraryFile);

    if (!lib->size())
    {
        // Library has no builders. Return 0.
        delete lib;
        return 0;
    }

    _libraries.push_back(lib);

    return lib;
}


ComponentLibrary* ComponentFactory::library(const Pt::System::File& libraryFile)
{
    LibraryList::iterator iter;

    for (iter = _libraries.begin(); iter != _libraries.end(); iter++)
    {
        if (libraryFile == (*iter)->libraryFile())
        {
            return *iter;
        }
    }

    return 0;
}


void ComponentFactory::loadConfiguration(const ComponentLibrary& library, IUnknown& component, const IComponentBuilder& builder)
{
    IPrefs* prefs = component.queryInterface<IPrefs>();

    if(prefs == 0)
    {
        return; // Component does not provide IPrefs interface, so no configuration is loaded.
    }

    // TODO Later we want to support not only .properties files for component based configuration storage.
    Pt::System::File configFile(library.libraryFile().parentPath()
                                + std::string(builder.typeId().name())
                                + ".properties");

    if( configFile.exists() )
    {
        // Load configuration if config file exists.
        prefs->loadPrefs(configFile);
    }

    // TODO We also have to call release when loadPrefs() fails with an exception.
    // TODO See Jira bug #GNPPR-5
    prefs->release();
}

} // namespace Cma

} // namespace Pt
