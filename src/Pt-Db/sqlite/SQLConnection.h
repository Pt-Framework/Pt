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

            /** \brief Starts a database deferred transaction.

                The default transaction behavior is deferred. Deferred means that no locks are acquired on the database 
                until the database is first accessed. Thus with a deferred transaction, the BEGIN statement itself 
                does nothing. Locks are not acquired until the first read or write operation. The first read operation 
                against a database creates a SHARED lock and the first write operation creates a RESERVED lock. Because 
                the acquisition of locks is deferred until they are needed, it is possible that another thread or 
                process could create a separate transaction and write to the database after the BEGIN on the current 
                thread has executed. 
            */
            void beginTransaction();

            /** \brief Starts a database immediate transaction.

                If the transaction is immediate, then RESERVED locks are acquired on all databases as soon as the 
                BEGIN command is executed, without waiting for the database to be used. After a BEGIN IMMEDIATE, 
                you are guaranteed that no other thread or process will be able to write to the database or do a 
                BEGIN IMMEDIATE or BEGIN EXCLUSIVE. Other processes can continue to read from the database.
            */
            void beginImmediateTransaction();

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
