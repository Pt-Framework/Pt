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

#ifndef PT_DB_CONNECTIONMANAGER_H
#define PT_DB_CONNECTIONMANAGER_H

#include <Pt/Db/IConnector.h>
#include <Pt/System/Plugin.h>
#include <Pt/System/Mutex.h>
#include <Pt/NonCopyable.h>
#include <map>
#include <string>

namespace Pt {

namespace System {
class Path;
}

namespace Db {

class ConnectionManager : private Pt::NonCopyable
{
    public:
        ConnectionManager();

        ~ConnectionManager();

        /** \brief Register a plugin that provides a named driver.

            The plugin must remain valid for the lifetime of this
            ConnectionManager. Ownership of any IConnector instances
            created by the plugin is transferred to the ConnectionManager.
        */
        void registerPlugin(Pt::System::Plugin<IConnector>& plugin);

        /** \brief Load driver plugins from a shared library.

            Resolves the symbol \a sym from the library at \a path and
            registers all IConnector plugins found there.
        */
        void loadPlugin(const std::string& sym, const Pt::System::Path& path);

        /** \brief Find a driver by name. Returns null if not found.

            On the first call for a given name a new IConnector is
            created via the registered plugin and cached for reuse.
        */
        IConnector* findConnector(const std::string& name);

        /** \brief Returns the process-wide singleton instance.
        */
        static ConnectionManager& instance();

    private:
        Pt::System::PluginManager<IConnector> _pluginManager;
        std::map<std::string, IConnector*>    _cache;
        Pt::System::RecursiveMutex            _mutex;
};

} // namespace Db

} // namespace Pt

#endif // PT_DB_CONNECTIONMANAGER_H
