/*
 * Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
 * 
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

#ifndef PT_SYSTEM_API_PLUGINS_H
#define PT_SYSTEM_API_PLUGINS_H

/** @defgroup Plugins Dynamic Libraries and Plugins

    @brief Loading dynamic libraries and creating plugin instances at runtime.

    A process can load a shared library after it has started and resolve
    symbols from it. The same libraries can export classes that implement
    a known interface. Symbol lookup and instance creation are two layers
    of that mechanism.

    %Library is the portable loader. It opens a library image and
    resolves a named symbol.

    @code
    typedef int (*MyFunc)();

    Pt::System::Path libPath = "MyLib";
    Pt::System::Library library(libPath);

    Pt::System::Symbol symbol = library.getSymbol("myFunction");

    MyFunc func = reinterpret_cast<MyFunc>(symbol.sym());
    int result = func();
    @endcode

    The path "MyLib" is a basename. %Library finds the platform image.
    getSymbol() returns a %Symbol for the name "myFunction".

    The address is a void pointer. Calling it as a function requires a
    cast to a function pointer type. Standard C++ does not allow that
    conversion. Nearly all runtimes implement it as an extension.

    A plugin is a shared library. The application keeps the interface
    type. The concrete class is compiled into the library. No extra
    base class is required beyond that interface.

    The library exports a null-terminated array of %PluginId named
    PluginList. The export uses C linkage so the loader can resolve a
    stable symbol. %PluginManager looks up that name through a %Library.

    Each entry is a %Plugin that creates and destroys instances of one
    interface. %BasicPlugin is the usual plugin. The first template
    argument is the concrete class. The second is the interface it
    implements. It constructs with new and destroys with delete.

    The constructor takes a feature string that names the instance for
    later construction. The address of the %BasicPlugin is placed in
    PluginList.

    Several plugins may share one PluginList. A second concrete class
    for the same interface is another %BasicPlugin in the same array.

    A plugin that needs another allocator derives from %Plugin and
    overrides create and destroy.

    @code
    class Greeter
    {
        public:
            virtual ~Greeter() {}
            virtual void sayHello() const = 0;
    };

    class EnglishGreeter : public Greeter
    {
        public:
            void sayHello() const
            { std::cout << "Hello World\n"; }
    };

    class OtherGreeter : public Greeter
    {
        public:
            void sayHello() const
            { std::cout << "Hi\n"; }
    };

    static Pt::System::BasicPlugin<EnglishGreeter, Greeter> _enGreeter("en");
    static Pt::System::BasicPlugin<OtherGreeter, Greeter> _otherGreeter("other");

    extern "C"
    {
        Pt::System::PluginId* PluginList[] = { &_enGreeter, &_otherGreeter, 0 };
    }
    @endcode

    %PluginManager loads a PluginList through a %Library and creates
    instances by feature. The application loads and unloads plugins
    through the manager.

    @code
    Pt::System::PluginManager<Greeter> manager;
    manager.loadPlugin("PluginList", "/path/to/plugin.so");

    Greeter* greeter = manager.create("en");
    if(greeter)
    {
        greeter->sayHello();
        manager.destroy(greeter);
    }
    @endcode

    The instance is created and destroyed through the manager, not with
    delete. Loaded libraries unload when the manager is destroyed.

    Path search, platform naming, and symbol lookup belong to %Library.
*/

#endif
