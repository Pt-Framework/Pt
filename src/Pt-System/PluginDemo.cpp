#include "TestPlugin.h"
#include "Pt/System/Plugin.h"
#include "Pt/System/Environment.h"

#include <iostream>


int main(int argc, char** argv)
{
	try
	{
		std::string  shlibName = Pt::System::Environment::sharedLibraryPrefix() + "TestPlugin";

		#ifndef NDEBUG
			shlibName += "d";
		#endif

		shlibName += Pt::System::Environment::sharedLibraryExtension();

		Pt::System::PluginManager<TestPlugin> pm;
		pm.loadPlugin( shlibName );

		TestPlugin* pl = pm.create("MyPlugin");
		if(pl == 0)
			throw std::runtime_error("No such plugin" + PT_SOURCEINFO);

		pl->test();

		pm.destroy(pl);
		return 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 1;
}
