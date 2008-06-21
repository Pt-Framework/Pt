#ifndef Pt_Plugin_h
#define Pt_Plugin_h

#include <Pt/System/Api.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/Directory.h>
#include <Pt/System/SharedLib.h>
#include <typeinfo>
#include <list>
#include <map>
#include <string>
#include <iostream>

namespace Pt {

namespace System {

    class PluginId {
        public:
            PluginId(const std::type_info& iface, const std::string& feature, const std::string& info = "")
            : _iface(iface),
              _feature(feature),
              _info(info)
            { }

            virtual ~PluginId()
            { }

            const std::type_info& iface() const
            { return _iface; }

            const std::string& feature() const
            { return _feature; }

            const std::string& info() const
            { return _info; }

        private:
            const std::type_info& _iface;
            std::string _feature;
            std::string _info;
    };


    template <typename Iface>
    class Plugin : public PluginId {
        public:
            Plugin(const std::type_info& iface, const std::string& feature, const std::string& info)
            : PluginId( iface, feature, info)
            { }

            virtual Iface* create() = 0;

            virtual void destroy(Iface* instance) = 0;
    };


    //! @brief A plugin class that supports transparent, named object construction.
    /**
       In the plugin shared object global BasicPlugins have to be arranged
       in a null teminated array with C linkage. The PluginManager can be
       set up to resolve the symbol of this array and use the plugins.

       <code>
       static Pt::BasicPlugin<SomeClass, MyIface> plugin0("some-feature");
       static Pt::BasicPlugin<OtherClass, MyIface> plugin1("other-feature");
       extern "C" { \
           PT_API Pt::PluginId* PluginList[] = { &plugin0, &plugin1, 0 }; \
       }
       </code>
    */
    template <typename Class, typename Iface>
    class BasicPlugin : public Plugin<Iface> {
        public:
            BasicPlugin(const std::string& feature, const std::string& info = "")
            : Plugin<Iface>( typeid(Iface), feature, info)
            { }

            Iface* create()
            { return new Class; }

            void destroy(Iface* instance)
            { delete instance; }
    };


    template < typename IfaceT, typename PluginT = Plugin<IfaceT> >
    class PluginManager
    {
        public:
            typedef typename std::multimap< std::string, PluginT* > PluginMap;
            typedef typename std::multimap< IfaceT*, PluginT* > InstanceMap;

			class Iterator
			{
				public:
					Iterator()
					{}

					Iterator(typename PluginMap::const_iterator it)
					: _it( it)
					{}

					Iterator& operator++()
					{ ++_it; return *this; }
	
					const PluginId& operator*() const
					{ return *(_it->second); }

					const PluginId* operator->() const
					{ return _it->second; }

					bool operator==(const Iterator& it) const
					{ return _it == it._it; }

					bool operator!=(const Iterator& it) const
					{ return _it != it._it; }

					typename PluginMap::const_iterator _it;
			};

        public:
            PluginManager()
            : _iface( typeid(IfaceT) )
            { }

            ~PluginManager();

            void loadPlugin(const std::string& path);

            void registerPlugin(PluginT& plugin);

            void unregisterPlugin(PluginT& plugin);

            IfaceT* create(const std::string& feature);

            IfaceT* create(const Iterator& feature);

            void destroy(IfaceT* inst);

			Iterator begin() const
			{ return Iterator( _plugins.begin() ); }
			
			Iterator end() const
			{ return Iterator( _plugins.end() ); }

        protected:
            PluginMap& plugins()
            { return _plugins; }

            InstanceMap& instances()
            { return _instances; }

        private:
            /// A string representation of the interface id
            const std::type_info& _iface;

            /// A list of all loaded libraries
			std::list<SharedLib> _libs;

            /// A map of a feature string and the Plugin* which handles it.
            PluginMap _plugins;

            /// A map of the created Iface* and the Plugin* it was created by.
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
    void PluginManager<IfaceT, PluginT>::loadPlugin(const std::string& path)
    {       
        SharedLib shlib(path);

    	void* symbol = shlib.resolve( "PluginList" );
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
        _plugins.insert( std::make_pair(plugin.feature(), &plugin) );
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
            _instances.insert( std::make_pair(iface, plugin) );
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
            throw SystemError("Could not destroy object.", PT_SOURCEINFO);
        }

        it->second->destroy(inst);
        _instances.erase(it);
        return;
    }

} // namespace System

} // namespace Pt

#endif
