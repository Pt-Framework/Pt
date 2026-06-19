/*
  Copyright (C) 2006-2026 by Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include "ConnectionManager.h"
#include <Pt/Db/DbError.h>
#include <Pt/System/Path.h>

namespace Pt {

namespace Db {

ConnectionManager::ConnectionManager()
{
}


ConnectionManager::~ConnectionManager()
{
}


void ConnectionManager::registerPlugin(Pt::System::Plugin<IConnector>& plugin)
{
    Pt::System::RecursiveLock lock(_mutex);
    _pluginManager.registerPlugin(plugin);
}


void ConnectionManager::loadPlugin(const std::string& sym, const Pt::System::Path& path)
{
    Pt::System::RecursiveLock lock(_mutex);
    _pluginManager.loadPlugin(sym, path);
}


IConnector* ConnectionManager::findConnector(const std::string& name)
{
    Pt::System::RecursiveLock lock(_mutex);

    std::map<std::string, IConnector*>::iterator it = _cache.find(name);
    if(it != _cache.end())
        return it->second;

    IConnector* connector = _pluginManager.create(name);
    if(connector)
        _cache[name] = connector;

    return connector;
}


ConnectionManager& ConnectionManager::instance()
{
    static ConnectionManager mgr;
    return mgr;
}

} // namespace Db

} // namespace Pt
