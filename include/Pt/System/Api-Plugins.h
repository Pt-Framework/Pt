/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_SYSTEM_API_PLUGINS_H
#define PT_SYSTEM_API_PLUGINS_H

/** @addtogroup Pt-System-Plugins

    @brief Creating plugin instances from shared libraries at runtime.

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
*/

#endif
