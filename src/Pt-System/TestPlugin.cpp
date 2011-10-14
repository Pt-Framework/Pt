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

#include "Pt/Any.h"
#include "Pt/StringStream.h"

#include "Pt/System/Api.h"
#include "Pt/System/Plugin.h"

#include "Pt/TextStream.h"
#include "Pt/Utf8Codec.h"

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
		 	Pt::TextStream ts(ss, new Pt::Utf8Codec);
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
