/*
 * Copyright (C) 2006-2013 Marc Boris Duerner
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

#ifndef Pt_Plugin_h
#define Pt_Plugin_h

#include <Pt/System/Api.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/Library.h>
#include <typeinfo>
#include <list>
#include <map>
#include <string>

namespace Pt {

namespace System {

/** @brief ID for plugin exports.
*/
class PluginId 
{
    public:
        /** @brief Construct with type info of plugin interface.
        */
        PluginId(const std::type_info& iface)
        : _iface(&iface)
        { }

        /** @brief Destructor.
        */
        virtual ~PluginId()
        { }

        /** @brief Returns the type of the plugin interface.
        */
        const std::type_info& iface() const
        { return *_iface; }

        /** @brief Returns the plugin feature string.
        */
        virtual const char* feature() const = 0;

        /** @brief Returns the plugin info string.
        */
        virtual const char* info() const = 0;

    private:
        const std::type_info* _iface;
};

/** @brief Interface for plugins.
*/
template <typename Iface>
class Plugin : public PluginId 
{
    public:
        /** @brief Default Constructor.
        */
        Plugin()
        : PluginId( typeid(Iface) )
        { }

        /** @brief Creates an instance.
        */
        virtual Iface* create() = 0;

        /** @brief Destroys an instance.
        */
        virtual void destroy(Iface* instance) = 0;
};


/** @brief A plugin implementation.
    
    In the plugin library, shared object global %BasicPlugins have to be
    arranged in a null teminated array with C linkage. The PluginManager
    can be set up to resolve the symbol of this array and use the plugins.

    @code
    static Pt::BasicPlugin<SomeClass, MyIface> plugin0("some-feature");
    static Pt::BasicPlugin<OtherClass, MyIface> plugin1("other-feature");
    
    extern "C" { \
        PT_API Pt::PluginId* PluginList[] = { &plugin0, &plugin1, 0 }; \
    }
    @endcode
*/
template <typename Class, typename Iface>
class BasicPlugin : public Plugin<Iface> {
    public:
        /** @brief Constructs with feature string and info.
        */
        BasicPlugin(const std::string& feature, const std::string& info = std::string())
        : Plugin<Iface>()
        , _feature(feature)
        , _info(info)
        { }

        // inherit docs
        Iface* create()
        { return new Class; }

        // inherit docs
        void destroy(Iface* instance)
        { delete instance; }

        // inherit docs
        virtual const char* feature() const
        { return _feature.c_str(); }

        // inherit docs
        virtual const char* info() const
        { return _info.c_str(); }

    private:
        std::string _feature;
        std::string _info;
};

/** @brief Manages loaded plugins.
*/
template < typename IfaceT, typename PluginT = Plugin<IfaceT> >
class PluginManager
{
    public:
        typedef typename std::multimap< std::string, PluginT* > PluginMap;
        typedef typename std::multimap< IfaceT*, PluginT* > InstanceMap;

        /** @brief %Iterator for loaded plugins.
        */
        class Iterator
        {
            friend class PluginManager;

            public:
                /** @brief Default Constructor.
                */
                Iterator()
                {}

                //! @internal
                Iterator(typename PluginMap::const_iterator it)
                : _it( it)
                {}

                //! @brief Advances the iterator
                Iterator& operator++()
                { ++_it; return *this; }
    
                //! @brief Access iterator value
                const PluginId& operator*() const
                { return *(_it->second); }

                //! @brief Access iterator value
                const PluginId* operator->() const
                { return _it->second; }

                //! @brief Comparison operator.
                bool operator==(const Iterator& it) const
                { return _it == it._it; }

                //! @brief Comparison operator.
                bool operator!=(const Iterator& it) const
                { return _it != it._it; }

            private:
                //! @internal
                typename PluginMap::const_iterator _it;
        };

    public:
        /** @brief Default Constructor.
        */
        PluginManager()
        : _iface( typeid(IfaceT) )
        { }

        PluginManager(Pt::System::PluginId** plugins)
        : _iface( typeid(IfaceT) )
        { 
            if( ! plugins)
              return;
            
            for(; *plugins != 0; ++plugins)
            {
                if( (*plugins)->iface() == _iface )
                {
                    PluginT* p = (PluginT*)(*plugins);
                    this->registerPlugin(*p);
                }
            }
        }

        /** @brief Destructor.
        */
        ~PluginManager();

        /** @brief Loads plugins from a library.
        */
        void loadPlugin(const std::string& sym, const Path& path);

        /** @brief Registers a plugin.
        */
        void registerPlugin(PluginT& plugin);

        /** @brief Unregisters a plugin.
        */
        void unregisterPlugin(PluginT& plugin);

        /** @brief Creates an instance by name.
        */
        IfaceT* create(const std::string& feature);

        /** @brief Creates an instance.
        */
        IfaceT* create(const Iterator& feature);

        /** @brief Destroys an instance.
        */
        void destroy(IfaceT* inst);

        /** @brief Begin of loaded plugins.
        */
        Iterator begin() const
        { return Iterator( _plugins.begin() ); }
           
        /** @brief End of loaded plugins.
        */
        Iterator end() const
        { return Iterator( _plugins.end() ); }

    protected:
        //! @internal
        PluginMap& plugins()
        { return _plugins; }

        //! @internal
        InstanceMap& instances()
        { return _instances; }

    private:
        //! @internal
        const std::type_info& _iface;

       //! @internal A list of all loaded libraries
        std::list<Library> _libs;

        //! @internal A map of a feature string and the Plugin* which handles it.
        PluginMap _plugins;

        //! @internal A map of the created Iface* and the Plugin* it was created by.
        InstanceMap _instances;
};


template <class IfaceT, typename PluginT >
PluginManager<IfaceT, PluginT>::~PluginManager()
{
    // Destroy all instances. If any are left its actually a bug.
    for(typename InstanceMap::iterator it = _instances.begin(); it != _instances.end(); ++it)
    {
        it->second->destroy( it->first );
    }
    _instances.clear();
}


template <class IfaceT, typename PluginT >
void PluginManager<IfaceT, PluginT>::loadPlugin(const std::string& sym, const Path& path)
{
    Library shlib(path);

    void* symbol = shlib.resolve( sym.c_str() );
    if( ! symbol )
        return;

    PluginId** plugins = (PluginId**) symbol;

    for(; *plugins != 0; ++plugins)
    {
        if( (*plugins)->iface() == _iface )
        {
            PluginT* p = (PluginT*)(*plugins);
            this->registerPlugin(*p);
        }
    }

    _libs.push_back(shlib);
}


template <class IfaceT, typename PluginT >
void PluginManager<IfaceT, PluginT>::registerPlugin(PluginT& plugin)
{
    typename PluginMap::value_type p(plugin.feature(), &plugin);
    _plugins.insert(p);
}


template <class IfaceT, typename PluginT >
void PluginManager<IfaceT, PluginT>::unregisterPlugin(PluginT& plugin)
{
    typename PluginMap::iterator it = _plugins.find( plugin.feature() );
    if( it != _plugins.end() ) 
    {
        _plugins.erase(it);
    }
}


template <class IfaceT, typename PluginT >
IfaceT* PluginManager<IfaceT, PluginT>::create(const std::string& feature)
{
    typename PluginMap::iterator it = _plugins.find(feature);
    if( it == _plugins.end() ) 
    {
        return 0;
    }

    PluginT* plugin = it->second;
    IfaceT* iface = plugin->create();
    if(iface) 
    {
        typename InstanceMap::value_type elem(iface, plugin);
        _instances.insert( elem );
    }

    return iface;
}


template <class IfaceT, typename PluginT >
IfaceT* PluginManager<IfaceT, PluginT>::create(const Iterator& pit)
{
    typename PluginMap::const_iterator it = pit._it;

    PluginT* plugin = it->second;
    IfaceT* iface = plugin->create();
    if(iface) 
    {
        _instances.insert( std::make_pair(iface, plugin) );
    }

    return iface;
}


template <class IfaceT, typename PluginT >
void PluginManager<IfaceT, PluginT>::destroy(IfaceT* inst)
{
    typename InstanceMap::iterator it = _instances.find(inst);
    if( it == _instances.end() ) 
    {
        throw SystemError("plugin destroy");
    }

    it->second->destroy(inst);
    _instances.erase(it);
    return;
}

} // namespace System

} // namespace Pt

#endif // Pt_Plugin_h
