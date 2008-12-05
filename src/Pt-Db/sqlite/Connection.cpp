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
 
#include "Connection.h"
#include "Statement.h"

#include "../RowImpl.h"
#include "../ValueImpl.h"
#include "../ResultImpl.h"
#include "Error.h"

#include <Pt/Db/Result.h>
#include <Pt/Db/Row.h>
#include <Pt/Db/Value.h>
#include <Pt/Db/Statement.h>


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
}


namespace Pt {

namespace Db {

namespace sqlite {

    Connection::Connection(const char* conninfo)
    {
        int ret = ::sqlite3_open(conninfo, &_Db);
        if(ret != SQLITE_OK)
        {
            Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
        }
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
            sqlite3_free(errmsg);
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
