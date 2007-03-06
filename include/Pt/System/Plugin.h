#ifndef Pt_Plugin_h
#define Pt_Plugin_h

#include <Pt/System/Api.h>
#include <Pt/TypeInfo.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/Directory.h>
#include <Pt/System/SharedLib.h>

#include <list>
#include <map>
#include <string>
#include <iostream>

namespace Pt {

namespace System {

    class PluginId {
        public:
            PluginId(const std::string& iface, const std::string& feature, const std::string& info = "")
            : _iface(iface),
              _feature(feature),
              _info(info)
            { }

            virtual ~PluginId()
            { }

            const std::string& iface() const
            { return _iface; }

            const std::string& feature() const
            { return _feature; }

            const std::string& info() const
            { return _info; }

        private:
            std::string _iface;
            std::string _feature;
            std::string _info;
    };


    template <typename Iface>
    class Plugin : public PluginId {
        public:
            Plugin(const std::string& iface, const std::string& feature, const std::string& info)
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
            : Plugin<Iface>( TypeInfo<Iface>::typeName(), feature, info)
            { }

            BasicPlugin(const std::string& iface, const std::string& feature, const std::string& info)
            : Plugin<Iface>( iface, feature, info)
            { }

            Iface* create()
            { return new Class; }

            void destroy(Iface* instance)
            { delete instance; }
    };


    class PT_SYSTEM_API PluginManagerBase {
        public:
            PluginManagerBase();

            virtual ~PluginManagerBase();

        protected:
            //! Override to change how a SharedLib is opened.
            virtual SharedLib* openPlugin(const std::string& path);

            //! Override to change how plugins are extracted from a shared library.
            virtual PluginId** resolvePlugin(SharedLib& shlib);

            std::list<SharedLib*>& sharedLibs();

        private:
            class PluginManagerBaseImpl* _impl;
    };


    template < typename IfaceT, typename PluginT = Plugin<IfaceT> >
    class PluginManager : PluginManagerBase {
        public:
            typedef typename std::map< std::string, PluginT* > PluginMap;
            typedef typename std::multimap< IfaceT*, PluginT* > InstanceMap;

        public:
            PluginManager( const std::string& iface = TypeInfo<IfaceT>::typeName() )
            : _iface(iface)
            { }

            ~PluginManager();

            void openDir(const std::string& path);

            void loadPlugin(const std::string& path);

            void registerPlugin(PluginT& plugin);

            void unregisterPlugin(PluginT& plugin);

            virtual IfaceT* create(const std::string& feature);

            virtual void destroy(IfaceT* inst);

        protected:
            PluginMap& plugins()
            { return _plugins; }

            InstanceMap& instances()
            { return _instances; }

        private:
            /// A string representation of the interface id
            std::string _iface;

            /// A map of a feature string and the Plugin* which handles it.
            PluginMap _plugins;

            /// A map of the created Iface* and the Plugin* it was created by.
            InstanceMap _instances;
    };


    template <class IfaceT, typename PluginT >
    PluginManager<IfaceT, PluginT>::~PluginManager()
    {
        // Destroy all instances. If any are left its actually a bug.
        for(typename InstanceMap::iterator it = _instances.begin(); it != _instances.end(); ++it) {
            it->second->destroy( it->first );
        }
        _instances.clear();
    }


    template <class IfaceT, typename PluginT >
    void PluginManager<IfaceT, PluginT>::openDir(const std::string& dirPath)
    {
        throw std::runtime_error("PluginManager::openDir not implemented." + PT_SOURCEINFO);
        /*try {
            Directory dir(dirPath);
            for(Directory::Iterator it = dir.begin(); it != dir.end(); ++it) {
                std::string path = dir.path() + Directory::separator() + *it;
                try {
                    this->loadPlugin(path);
                } catch(...) { }
            }
        } catch(...) {}*/
    }


    template <class IfaceT, typename PluginT >
    void PluginManager<IfaceT, PluginT>::loadPlugin(const std::string& path)
    {
        SharedLib* shlib = this->openPlugin(path);
        if(!shlib) {
            return;
        }

        PluginId** plugins = this->resolvePlugin(*shlib);
        if( !plugins ) {
            delete shlib;
            return;
        }

        for(; *plugins != 0; ++plugins) {
            if( (*plugins)->iface() == _iface ) {
                PluginT* p = (PluginT*)(*plugins);
                this->registerPlugin(*p);
            }
        }

        this->sharedLibs().push_back(shlib);
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
        if( it != _plugins.end() ) {
            _plugins.erase(it);
        }
    }


    template <class IfaceT, typename PluginT >
    IfaceT* PluginManager<IfaceT, PluginT>::create(const std::string& feature)
    {
        typename PluginMap::iterator it = _plugins.find(feature);
        if( it == _plugins.end() ) {
            return 0;
        }

        PluginT* plugin = it->second;
        IfaceT* iface = plugin->create();
        if(iface) {
            _instances.insert( std::make_pair(iface, plugin) );
        }

        return iface;
    }


    template <class IfaceT, typename PluginT >
    void PluginManager<IfaceT, PluginT>::destroy(IfaceT* inst)
    {
        typename InstanceMap::iterator it = _instances.find(inst);
        if( it == _instances.end() ) {
            throw SystemError("Could not destroy object.", PT_SOURCEINFO);
        }

        it->second->destroy(inst);
        _instances.erase(it);
        return;
    }

} // namespace System

} // namespace Pt

#endif
