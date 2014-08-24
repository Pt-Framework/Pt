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
#ifndef PT_CMA_COMPONENTFACTORY_H
#define PT_CMA_COMPONENTFACTORY_H

#include <Pt/Cma/Api.h>
#include <Pt/Cma/ComponentLibrary.h>
#include <Pt/Cma/IUnknown.h>
#include <Pt/Cma/IComponentBuilder.h>
#include <Pt/System/Path.h>
#include <list>
#include <string>

namespace Pt {

namespace Cma {

/**
  * @brief A factory to load libraries and instantiate Components from them.
  *
  * The preferred way to create a component is by using the createComponent()
  * method. When createComponent() is called, a component is created from a previously
  * loaded library by the given component type id (type id). A library can be
  * loaded by calling loadLibrary().
  * To load and create the first component of a library immediately, the method
  * loadComponent() can be used. It loads the given library and returns a pointer
  * to the first component which is contained in the library, if any.
  *
  * !! Example:
  *
  * \code
  *    ComponentFactory factory;
  *    factory.loadLibrary("libraryPath");
  *    IUnknown* unknown = factory.createComponent("someComponent");
  * \endcode
  */
class PT_CMA_API ComponentFactory
{
    public:
        typedef std::list<ComponentLibrary*> LibraryList;

        /**
        * @brief Constructs this factory.
        */
        ComponentFactory();

        /**
        * @brief Destructor of this factory.
        */
        virtual ~ComponentFactory();

        /**
        * @brief Loads the given library so later components of this library can be
        * created by this factory.
        *
        * The given File object has to specify the file by its platform-specific name, including
        * any library prefix and suffix. The path to the file (including its file name) is passed
        * to the platform's loadLibrary-function as is.
        *
        * If the library was loaded before, nothing happens and the library is not loaded
        * again.
        *
        * @param file A File object referencing the library in the file system.
        */
        void loadLibrary(const System::Path& file);

        /**
        * @brief Unloads the given library.
        *
        * The given File object has to specify the file by its platform-specific name, including
        * any library prefix and suffix.
        *
        * If the library was not loaded before, nothing happens.
        *
        * @param file A File object referencing the library in the file system.
        */
        bool unload(const System::Path& file);

        /**
        * @brief Unloads all libraries which were loaded before.
        */
        void unloadAll();

        /**
        * @brief Creates and returns a component of the given type and optionally loads
        * its configuration.
        *
        * Only components of library, which were previously loaded by calling one of the
        * load() methods or one of the loadComponent() methods can be created.
        *
        * If 'loadParameter' is set to true, the configuration of the component is tried
        * to be loaded. This is done only for components which implement the special
        * interface IPrefs. The configuration file is loaded from the same directory the
        * library of the created component is located in. The name of the file is constructed
        * from the components file, followed by one of the configuration file extensions,
        * for example '.properties'. Loading the component 'MapRenderer' will result in the
        * configuration file 'MapRenderer.properties' to be loaded. If no configuration file
        * can be found, no configuration is loaded.
        *
        * @param componentType The component type of the Component to create.
        * @param loadConfig Optional parameter: When true is passed, the configuration of
        * the component is tried to be loaded; otherwise no configuration is loaded.
        * @return An IUnknown pointer to the interfaces of the component.
        */
        IUnknown* createComponent(const std::string& componentType, bool loadConfig = true);

        /**
        * @brief Loads the given library and returns an IUnknown pointer to the first component
        * of the library, if any.
        *
        * This method basically does the same as calling loadLibrary() followed by a call to
        * createComponent() for the first component. The library is loaded and stored in this
        * factory. After that the first component, which can be found in the library, is created
        * and returned as IUnknown pointer to the caller.
        *
        * The given File object has to specify the file by its platform-specific name, including
        * any library prefix and suffix. The path to the file (including its file name) is passed
        * to the platform's loadLibrary-function as is.
        *
        * If the library was loaded before, the library is not loaded again.
        *
        * @param file A File object referencing the library in the file system.
        * @param loadConfig Optional parameter: When true is passed, the configuration of
        * the component is tried to be loaded; otherwise no configuration is loaded.
        * @return An IUnknown pointer to the interfaces of the component.
        */
        IUnknown* loadComponent(const System::Path& file, bool loadConfig = true);

        /**
        * @brief Returns true if the given library already was loaded.
        *
        * The given File object has to specify the file by its platform-specific name, including
        * any library prefix and suffix.
        *
        * @param file A File object referencing the library in the file system.
        * @return true if the library was already loaded by this factory; false otherwise.
        */
        bool isLoaded(const System::Path& file);

    private:
        /**
        * @brief Loads the given library file and stores in the internal library list
        * and returns a pointer to the corresponding ComponentLibrary object.
        *
        * If the library was already loaded, the library is not loaded again.
        *
        * @param libraryFile This library file is loaded.
        * @return Pointer to the corresponding ComponentLibrary object of the library
        * which was loaded.
        */
        ComponentLibrary* _loadLibrary(const System::Path& file);

        /**
        * @brief Returns a pointer to the ComponentLibrary object for the given library
        * file if it was already loaded in this factory.
        *
        * If the given library file was not yet loaded, 0 is returned.
        *
        * @param libraryFile A pointer to the corresponding ComponentLibrary for this
        * library is returned.
        * @return A pointer to the corresponding ComponentLibrary for the given library
        * file; or 0 if the library was not yet loaded.
        */
        ComponentLibrary* library(const System::Path& file);

        /**
        * @brief Loads the configuration for the given component.
        *
        * The given ComponentLibrary object and IComponentBuilder need to belong
        * to the given component (IUnknown) to work properly.
        *
        * The configuration of the component is tried to be loaded. This is done only for
        * components which implement the special interface IPrefs. The configuration file
        * is loaded from the same directory the library of the created component is located
        * in. The name of the file is constructed from the components file, followed by one
        * of the configuration file extensions, for example '.properties'. Loading the component
        * 'MapRenderer' will result in the configuration file 'MapRenderer.properties' to be
        * loaded. If no configuration file* can be found, no configuration is loaded.
        *
        * @param library ComponentLibrary object from which the given Component ('component')
        * was created from.
        * @param component The component for which the component is going to be loaded.
        * @param builder The builder by which the given Component ('component') was built.
        */
        void loadConfiguration(const ComponentLibrary& library,
                                IUnknown& component,
                                const IComponentBuilder& builder);

    protected:
        LibraryList _libraries;
};
}

}

#endif
