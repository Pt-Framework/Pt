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
        std::string  shlibName = "TestPlugin";

        #ifndef NDEBUG
            shlibName += "d";
        #endif

        std::cerr << "Loading " << shlibName << std::endl;

        Pt::System::PluginManager<TestPlugin> pm;
        pm.loadPlugin( shlibName );

		Pt::System::PluginManager<TestPlugin>::Iterator it;
		for(it = pm.begin(); it != pm.end(); ++it)
		{
			std::cerr << "FEATURE  : " << it->feature() << std::endl;
			std::cerr << "INFO     : " << it->info() << std::endl;

			TestPlugin*  p2 = pm.create( it );
			if( p2 )
			{
				p2->test();
				pm.destroy(p2);
			}
		}

        TestPlugin* pl = pm.create("MyPlugin");
        if(pl == 0)
            throw std::runtime_error(PT_SOURCEINFO + "No such plugin");

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
