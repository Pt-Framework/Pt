#include "TestPlugin.h"
#include "Pt/System/Api.h"
#include "Pt/System/Plugin.h"

#include <list>
#include <iostream>


class PT_EXPORT MyPlugin : public TestPlugin
{
	public:
		virtual void test()
		{ std::cerr << "MyPlugin::test\n"; }
};


static Pt::System::BasicPlugin<MyPlugin, TestPlugin> plugin0("MyPlugin", "0.0.1");


extern "C"
{
   PT_EXPORT Pt::System::PluginId* PluginList[] = { &plugin0, 0 };
}
