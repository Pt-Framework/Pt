#include "Pt/System/Plugin.h"

#include <list>
#include <iostream>
using namespace std;

namespace Pt {

namespace System {

class PluginManagerBaseImpl {
	public:
		PluginManagerBaseImpl()
		{}

		~PluginManagerBaseImpl()
		{
			// close all shared libraries.
			for(std::list<SharedLib*>::iterator it = _libs.begin(); it != _libs.end(); ++it) {
				delete *it;
			}
		}

		std::list<SharedLib*>& libs()
		{ return _libs; }

	private:
		std::list<SharedLib*> _libs;
};


PluginManagerBase::PluginManagerBase()
: _impl(0)
{
	_impl = new PluginManagerBaseImpl;
}


PluginManagerBase::~PluginManagerBase()
{
	delete _impl;
}


std::list<SharedLib*>& PluginManagerBase::sharedLibs()
{
	return _impl->libs();
}


SharedLib* PluginManagerBase::openPlugin(const std::string& path) throw(SystemError)
{
	SharedLib* shlib = new SharedLib( path.c_str() );
	return shlib;
}


PluginId** PluginManagerBase::resolvePlugin(SharedLib& shlib) throw(SystemError)
{
	void* symbol = shlib.resolve( "PluginList" );
	if( !symbol ) {
		return 0;
	}

	return (PluginId**)(symbol);
}

}

}
