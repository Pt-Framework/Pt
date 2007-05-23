#include "TestPlugin.h"

#include "Pt/Any.h"
#include "Pt/StringStream.h"

#include "Pt/System/Api.h"
#include "Pt/System/Plugin.h"

#include "Pt/Text/TextStream.h"
#include "Pt/Text/Utf8Codec.h"

#include <list>
#include <iostream>
#include <fstream>


class PT_EXPORT MyPlugin : public TestPlugin
{
	public:
		virtual void test()
		{ 
			/*Pt::Any a( int(7) );
			std::stringstream anyStream("5");
			anyStream >> a;
			if(a != 5)
				throw std::runtime_error("a != 5");*/
				
			std::fstream fs;

			int n = 0;
			std::stringstream ss("1");
		 	Pt::Text::TextStream ts(ss, new Pt::Text::Utf8Codec);
			std::basic_iostream<Pt::Char>& bs = ts;
			bs >> n;

			
			float f = 0;
			Pt::String s(L"5.0");
			Pt::StringStream ss2(s);
			ss2 >> f;
			
			std::cerr << "MyPlugin::test " << f << "\n"; 
		}
};


static Pt::System::BasicPlugin<MyPlugin, TestPlugin> plugin0("MyPlugin", "0.0.1");


extern "C"
{
   PT_EXPORT Pt::System::PluginId* PluginList[] = { &plugin0, 0 };
}
