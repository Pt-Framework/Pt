/*
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
#include "TestPlugin.h"
#include "Pt/System/Plugin.h"
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
        pm.loadPlugin( "PluginList", shlibName );

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
