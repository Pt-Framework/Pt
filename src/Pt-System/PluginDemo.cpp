#include "TestPlugin.h"
#include "Pt/System/Plugin.h"
#include "Pt/System/Environment.h"
#include "Pt/Char.h"
#include "Pt/Main.h"
#include "Pt/String.h"
#include <iostream>



int main(int argc, char** argv)
{
	try
	{
	 /*   std::locale::global( std::locale(std::locale(), new std::ctype<Pt::Char>) );
        std::locale::global( std::locale(std::locale(), new std::numpunct<Pt::Char>) );
        std::locale::global( std::locale(std::locale(), new std::num_get<Pt::Char>) );
        std::locale::global( std::locale(std::locale(), new std::num_put<Pt::Char>) );
*/
		std::string  shlibName = "libTestPlugin";

		#ifndef NDEBUG
		  shlibName += "d";
		#endif

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
