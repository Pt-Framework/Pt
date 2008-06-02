#include "Pt/Cma/ComponentFactory.h"
#include "Pt/Cma/ComponentLibrary.h"
#include "Pt/Cma/IComponentBuilder.h"
#include "Pt/Cma/IPrefs.h"
#include "Pt/Cma/IUnknown.h"
#include "Pt/Cma/TypeId.h"
#include "Pt/SourceInfo.h"
#include "Pt/System/SharedLib.h"
#include "Pt/System/File.h"
#include <stdexcept>

namespace Pt {

namespace Cma {

ComponentFactory::ComponentFactory()
{
}


ComponentFactory::~ComponentFactory()
{
}


void ComponentFactory::loadLibrary(const std::string& file)
{
    this->_loadLibrary(file);
}


bool ComponentFactory::unload(const std::string& file)
{
    std::string path = System::SharedLib::find(file);
    if( path.empty() )
    {
        return false;
    }

    LibraryList::iterator iter;
    for (iter = _libraries.begin(); iter != _libraries.end(); iter++)
    {
        if (path != (*iter)->path())
        {
            continue;
        }

        if( (*iter)->isUsed() )
        {
            continue;
        }

        delete (*iter);
        _libraries.erase(iter);

        return true;
    }

    return false;
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


IUnknown* ComponentFactory::loadComponent(const std::string& file, bool loadConfig)
{
    ComponentLibrary* lib = this->library(file);
    if (lib == 0)
    {
        lib = this->_loadLibrary(file);

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


bool ComponentFactory::isLoaded(const std::string& file)
{
    return this->library(file) != 0;
}


ComponentLibrary* ComponentFactory::_loadLibrary(const std::string& file)
{
    ComponentLibrary* lib = this->library(file);
    if(lib)
    {
        return lib;  // We already have loaded this library before.
    }

    lib = new ComponentLibrary(file);

    if ( ! lib->size() )
    {
        // Library has no builders. Return 0.
        delete lib;
        return 0;
    }

    _libraries.push_back(lib);
    return lib;
}


ComponentLibrary* ComponentFactory::library(const std::string& file)
{
    std::string path = System::SharedLib::find(file);
    if( path.empty() )
    {
        return 0;
    }

    LibraryList::iterator iter;
    for (iter = _libraries.begin(); iter != _libraries.end(); iter++)
    {
        if( path == (*iter)->path() )
        {
            return *iter;
        }
    }

    return 0;
}


void ComponentFactory::loadConfiguration(const ComponentLibrary& library, IUnknown& component,
                                         const IComponentBuilder& builder)
{
    IPrefs* prefs = component.queryInterface<IPrefs>();

    if(prefs == 0)
    {
        return; // Component does not provide IPrefs interface, so no configuration is loaded.
    }

    // TODO Later we want to support not only .properties files for component
    // based configuration storage.
    Pt::System::File libFile( library.path() );
    std::string configFile = libFile.dirName()
                             + std::string( builder.typeId().name() )
                             + ".properties";

    // Load configuration if config file exists.
    if( Pt::System::File::exists( configFile.c_str() ) )
    {
        prefs->loadPrefs(configFile);
    }

    // TODO We also have to call release when loadPrefs() fails with an exception.
    // TODO See Jira bug #GNPPR-5
    prefs->release();
}

} // namespace Cma

} // namespace Pt
