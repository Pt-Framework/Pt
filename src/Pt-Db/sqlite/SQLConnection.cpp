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
 
#include "SQLConnection.h"
#include "SQLStatement.h"

#include "../RowImpl.h"
#include "../ValueImpl.h"
#include "../ResultImpl.h"
#include "Error.h"

#include <Pt/Db/Result.h>
#include <Pt/Db/Row.h>
#include <Pt/Db/Value.h>
#include <Pt/Db/Statement.h>
#include <Pt/System/Thread.h>

namespace {

    int select_callback(void *pArg, int argc, char ** argv, char **columnNames)
    {
        Pt::Db::ResultImpl* res = static_cast<Pt::Db::ResultImpl*>(pArg);

        Pt::Db::RowImpl::data_type data;

        for (int i = 0; i < argc; ++i)
        {
            Pt::Db::Value v;
            if (argv[i])
            {
                v = Pt::Db::Value( new Pt::Db::ValueImpl(argv[i]) );
            }
            data.push_back(v);
        }

        res->add( Pt::Db::Row( new Pt::Db::RowImpl(data) ) );

        return SQLITE_OK;
    }

    /**
     * @brief Callback method, which is used to handle the busy state of the database.
     *
     * The presence of a busy handler does not guarantee that it will be invoked when there is lock contention. 
     * If SQLite determines that invoking the busy handler could result in a deadlock, it will go ahead and return 
     * SQLITE_BUSY or SQLITE_IOERR_BLOCKED instead of invoking the busy handler. Consider a scenario where one process 
     * is holding a read lock that it is trying to promote to a reserved lock and a second process is holding a reserved 
     * lock that it is trying to promote to an exclusive lock. The first process cannot proceed because it is blocked by 
     * the second and the second process cannot proceed because it is blocked by the first. If both processes invoke 
     * the busy handlers, neither will make any progress. Therefore, SQLite returns SQLITE_BUSY for the first process, 
     * hoping that this will induce the first process to release its read lock and allow the second process to proceed.
     * 
     * @param pArg Copy of the third argument of the method sqlite3_busy_handler(...).
     * @param priorCalls The number of times that the busy handler has been invoked for this locking event.
     *
     * @return 1 when additional attempts are made to access the database, otherwise 0.
     */
    int busyHandler(void* pArg, int priorCalls)
    {
        // sleep if handler has been called less than threshold value
        if (priorCalls < 500)
        {
            // adding a random value here greatly reduces locking
            if (pArg < 0)
            {
                // mostly impossible, because the third argument is the database connection. So if the connection
                // is lost, then no possibility to access the busy handler.
                Pt::System::Thread::sleep((rand() % 500) + 400);
            }
            else 
            {
                Pt::System::Thread::sleep(500);
            }
            return 1;
        }

        // sqlite3_exec will immediately return SQLITE_BUSY.
        return 0;
    }
}


namespace Pt {

namespace Db {

namespace sqlite {

    Connection::Connection(const char* conninfo)
    {
        int ret = ::sqlite3_open(conninfo, &_Db);
        ret = ::sqlite3_enable_load_extension(_Db, 1);
        if(ret != SQLITE_OK)
        {
            Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
        }
        // use busy handler to be called if database is locked.
        ::sqlite3_busy_handler(_Db, busyHandler, _Db);        
    }

    Connection::~Connection()
    {
        if (_Db)
        {
            this->clearStatementCache();
            ::sqlite3_close(_Db);
        }
    }

    void Connection::beginTransaction()
    {
        this->execute("BEGIN TRANSACTION");
    }

    void Connection::beginImmediateTransaction()
    {
        this->execute("BEGIN IMMEDIATE TRANSACTION");
    }

    void Connection::commitTransaction()
    {
        this->execute("COMMIT TRANSACTION");
    }

    void Connection::rollbackTransaction()
    {
        this->execute("ROLLBACK TRANSACTION");
    }

    Connection::size_type Connection::execute(const std::string& query)
    {
        char* errmsg;

        //log_debug("sqlite_exec(" << Db << ", \"" << query << "\", 0, 0, " << &errmsg << ')');

        int ret = ::sqlite3_exec(_Db, query.c_str(), 0, 0, &errmsg);
        if(ret != SQLITE_OK)
        {
            Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
        }
        
        //log_debug("sqlite_exec ret=" << ret);
        
        return ::sqlite3_changes(_Db);
    }

    Result Connection::select(const std::string& query)
    {
        return prepare(query).select();
    }

    Row Connection::selectRow(const std::string& query)
    {
        return prepare(query).selectRow();
    }

    Value Connection::selectValue(const std::string& query)
    {
        return prepare(query).selectValue();
    }

    Pt::Db::Statement Connection::prepare(const std::string& query)
    {
        //log_debug("prepare(\"" << query << "\")");
        return Pt::Db::Statement( new Pt::Db::sqlite::Statement(this, query) );
    }

    long long Connection::insertId()
    {
        return sqlite3_last_insert_rowid( this->_Db );
    }


} //namespace sqlite

} //namespace Db

} //namespace Pt
