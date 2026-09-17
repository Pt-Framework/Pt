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

    Dynamic libraries can be loaded at runtime and symbols can be resolved
    using the Pt::System::Library class. Additionally, the plugin API provides
    a more advanced way of creating classes, that implement a common interface,
    from a dynamically loadable library or module. The mechanism is
    non-intrusive and can be used with an existing class hierarchy. Plugins can
    be loaded and unloaded by client application code. The concrete type of the
    created class is opaque to the application that uses the plugin, it only
    needs to know the interface.

    Lets assume the classes you want to create from a plugin are derived from an
    interface class called Greeter. The Greeter class has one abstract function
    called sayHello:

    @code
    class Greeter
    {
        public:
            virtual ~Greeter() {}
            virtual void sayHello() const = 0;
    };
    @endcode

    Now, we want to write a plugin that implements Greeter to say "Hello World" in
    english. This simply means to derive from Greeter and implement the sayHello
    method:

    @code
    class EnglishGreeter : public Greeter
    {
        public:
            void sayHello() const
            { 
              std::cout << "Hello World\n";
            }
    };
    @endcode

    So far this has nothing to do with writing the plugin, it is pretty much the
    situation how object-oriented applications and frameworks are designed. To
    build the plugin, the EnglishGreeter must be build as a shared library and
    export the symbol "PluginList" that we will use later to resolve our plugin.
    PluginList must be a null-terminated array of PluginId* and be exported with
    C-linkage. This array will contain a number of BasicPlugin instances that
    serve as builder for the class we want to load from the plugin in our client
    application.

    @code
    static Pt::System::BasicPlugin<EnglishGreeter, Greeter> _enGreeter("en");

    extern "C"
    {
        Pt::System::PluginId* PluginList[] = { &_enGreeter, 0 };
    }
    @endcode

    Here we create a BasicPlugin statically in the plugin library that is able
    to create an EnglishGreeter which implements the Greeter interface, hence
    the two template parameters. The constructor takes a feature string, in this
    case "en", that can be used later for named construction of objects. The
    address of the BasicPlugin is then placed in the PluginList, so it can be
    resolved by the loader code. This is pretty much all you need to do to write
    a plugin. If we decide to write a GermanGreeter and FrenchGreeter later and
    do not want to compile them into a separate file we can simply add them to
    the PluginList. Instead of using the BasicPlugin template, we can derive from
    Pt::System::Plugin and override the create and destroy methods if we need a special
    way of creating or destroying. BasicPlugin is derived from Pt::System::Plugin and
    simply creates with new and destroys with delete.
*/

#endif
