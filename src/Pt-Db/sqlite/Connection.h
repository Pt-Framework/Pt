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

#ifndef PT_DB_SQLITE_CONNECTION_H
#define PT_DB_SQLITE_CONNECTION_H

#include <Pt/Db/IConnection.h>
#include "sqlite3.h"

namespace Pt {

namespace Db {

namespace sqlite {

    class Connection : public IStmtCacheConnection
    {
        sqlite3* _Db;

        public:
            explicit Connection(const char* conninfo);

            ~Connection();

            void beginTransaction();

            void commitTransaction();

            void rollbackTransaction();

            size_type execute(const std::string& query);

            Result select(const std::string& query);

            Row selectRow(const std::string& query);

            Value selectValue(const std::string& query);

            Pt::Db::Statement prepare(const std::string& query);

            long long insertId();

            sqlite3* getSqlite3() const
            { return _Db; }

    };

} //namespace sqlite

} //namespace Db

} //namespace PT

#endif // PTV_DB_SQLITE_CONNECTION_H
