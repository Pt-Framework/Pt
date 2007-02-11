/***************************************************************************
 *   Copyright (C) 2006 by Tommi Maekitalo                                 *
 *   Copyright (C) 2006 by Marc Boris Duerner                              *
 *   Copyright (C) 2006 by Stefan Bueder                                   *
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
#ifndef PT_DB_CONNECTOR_H
#define PT_DB_CONNECTOR_H

#include <Pt/Db/Api.h>
#include <Pt/NonCopyable.h>
#include <string>

/*#define TNTDB_CONNECTIONMANAGER_DECLARE(drivername) \
  extern "C" { extern tntDb::drivername::ConnectionManager connectionManager_ ## drivername; }

#define TNTDB_CONNECTIONMANAGER_DEFINE(drivername) \
  extern "C" { tntDb::drivername::ConnectionManager connectionManager_ ## drivername; }
*/

namespace Pt {

namespace Db {

    class Connection;

    class PT_DB_API IConnector : private NonCopyable
    {
        public:
            virtual ~IConnector() { }
            virtual Connection connect(const std::string& url) = 0;
    };

} // namespace Db

} // namespace Pt

#endif // PTV_DB_CONNECTOR_H

