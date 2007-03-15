/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 by PTV AG                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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


SharedLib* PluginManagerBase::openPlugin(const std::string& path)
{
    SharedLib* shlib = new SharedLib(Pt::System::File(path));
    return shlib;
}


PluginId** PluginManagerBase::resolvePlugin(SharedLib& shlib)
{
    void* symbol = shlib.resolve( "PluginList" );
    if( !symbol ) {
        return 0;
    }

    return (PluginId**)(symbol);
}

}

}
