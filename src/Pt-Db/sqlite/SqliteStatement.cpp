/*
 * Copyright (C) 2006 by Tommi Maekitalo
 * Copyright (C) 2006 by Marc Boris Duerner
 * Copyright (C) 2006 by Stefan Bueder
 *
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

#include "SqliteStatement.h"
#include "SqliteCursor.h"
#include "SqliteConnection.h"

#include "../ResultImpl.h"
#include "../RowImpl.h"
#include "../ValueImpl.h"

#include "SqliteError.h"

#include "Pt/Db/DbError.h"
#include "Pt/Date.h"
#include "Pt/Time.h"
#include "Pt/DateTime.h"

#include "Pt/Db/Result.h"
#include "Pt/Db/Row.h"
#include "Pt/Db/Value.h"

#include <limits>
#include <sstream>
#include <stdexcept>


namespace Pt {

namespace Db {

namespace sqlite {

    SqliteStatement::SqliteStatement(SqliteConnection* conn, const std::string& query)
        : IStatement(conn)
        , _stmt(0)
        , _stmtInUse(0)
        , _conn(conn)
        , _query(query)
        , _needReset(false)
    {
    }

    SqliteStatement::~SqliteStatement()
    {
        if(_conn)
            _conn->closeStatement(*this);

        if (_stmt)
        {
            //PT_LOG_DEBUG("sqlite3_finalize(" << stmt << ')');
            ::sqlite3_finalize(_stmt);
        }

        if (_stmtInUse && _stmtInUse != _stmt)
        {
            //PT_LOG_DEBUG("sqlite3_finalize(" << _stmtInUse << ')');
            ::sqlite3_finalize(_stmtInUse);
        }
    }



    sqlite3_stmt* SqliteStatement::getBindStmt()
    {
        if (_stmt == 0)
        {
            // hostvars don't need to be parsed, because sqlite accepts the hostvar-
            // syntax of tntDb (:vvv)

            // prepare statement
            const char* tzTail;

            int n = static_cast<int>( _query.size() );
            sqlite3* db = _conn->getSqlite3();

            int ret = ::sqlite3_prepare(db, _query.data(), n, &_stmt, &tzTail);
            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::SqliteError(ret, _query.c_str());
            }

            //PT_LOG_DEBUG("sqlite3_stmt = " << _stmt);

            if (_stmtInUse)
            {
                // get bindings from _stmtInUse
                //PT_LOG_DEBUG("sqlite3_transfer_bindings(" << _stmtInUse << ", " << _stmt << ')');
                ret = ::sqlite3_transfer_bindings(_stmtInUse, _stmt);
                if (ret != SQLITE_OK)
                {
                    //PT_LOG_DEBUG("sqlite3_finalize(" << _stmt << ')');
                    ::sqlite3_finalize(_stmt);
                    _stmt = 0;
                    Pt::Db::sqlite::SqliteError(ret, _query.c_str());
                }
            }
        }

        return _stmt;
    }

    void SqliteStatement::putback(sqlite3_stmt* stmt)
    {
        if (_stmt == 0)
        {
            _stmt = stmt; // thank you - we can use it

            if (_stmtInUse == stmt)
            {
                _stmtInUse = 0; // it is not in use any more
            }
            _needReset = true;
        }
        else
        {
            // we have already a new statement-handle - destroy the old one
            //PT_LOG_DEBUG("sqlite3_finalize(" << stmt_ << ')');
            ::sqlite3_finalize(stmt);

            if (_stmtInUse == stmt)
            {
                _stmtInUse = 0; // it is not in use any more
            }
        }
    }

    int SqliteStatement::getBindIndex(const std::string& col)
    {
        sqlite3_stmt* stmt = getBindStmt();

        //PT_LOG_DEBUG("sqlite3_bind_parameter_index(" << _stmt << ", :" << col  << ')');
        int idx = ::sqlite3_bind_parameter_index(stmt, (':' + col).c_str());
        //if (idx == 0)
        //{
        //  PT_LOG_WARN("hostvariable :" << col << " not found");
        //}
        return idx;
    }

    void SqliteStatement::reset()
    {
        if (_stmt)
        {
            if (_needReset)
            {
                //PT_LOG_DEBUG("sqlite3_reset(" << _stmt << ')');
                int ret = ::sqlite3_reset(_stmt);
                if(ret != SQLITE_OK)
                {
                    Pt::Db::sqlite::SqliteError(ret, _query.c_str());
                }

                _needReset = false;
            }
        }
        else
        {
            getBindStmt();
        }
    }

    void SqliteStatement::clear()
    {
        sqlite3_stmt* stmt = getBindStmt();

        int count = ::sqlite3_bind_parameter_count(stmt);

        for (int i = 0; i < count; ++i)
        {
            int ret = ::sqlite3_bind_null(stmt, i + 1);
            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::SqliteError(ret, _query.c_str());
            }
        }
    }

    void SqliteStatement::setNull(const std::string& col)
    {
        int idx = getBindIndex(col);
        sqlite3_stmt* stmt = getBindStmt();

        if (idx != 0)
        {
            reset();

            //PT_LOG_DEBUG("sqlite3_bind_null(" << stmt << ", " << idx << ')');
            int ret = ::sqlite3_bind_null(stmt, idx);
            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::SqliteError(ret, _query.c_str());
            }
        }
    }

    void SqliteStatement::setBool(const std::string& col, bool data)
    {
        setInt(col, data ? 1 : 0);
    }

    void SqliteStatement::setInt(const std::string& col, int data)
    {
        int idx = getBindIndex(col);
        sqlite3_stmt* stmt = getBindStmt();

        if (idx != 0)
        {
            reset();

            //PT_LOG_DEBUG("sqlite3_bind_int(" << stmt << ", " << idx << ')');
            int ret = ::sqlite3_bind_int(stmt, idx, data);
            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::SqliteError(ret, _query.c_str());
            }
        }
    }

    void SqliteStatement::setUnsigned(const std::string& col, unsigned data)
    {
        if (data > static_cast<unsigned>(std::numeric_limits<int>::max()))
        {
            //PT_LOG_WARN("possible loss of precision while converting large unsigned " << data
            //  << " to double");
            setDouble(col, static_cast<double>(data));
        }
        else
        {
            setInt(col, static_cast<int>(data));
        }
    }

    void SqliteStatement::setFloat(const std::string& col, float data)
    {
        setDouble(col, static_cast<double>(data));
    }

    void SqliteStatement::setDouble(const std::string& col, double data)
    {
        int idx = getBindIndex(col);
        sqlite3_stmt* stmt = getBindStmt();

        if (idx != 0)
        {
            reset();

            //PT_LOG_DEBUG("sqlite3_bind_double(" << stmt << ", " << idx << ')');
            int ret = ::sqlite3_bind_double(stmt, idx, data);
            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::SqliteError(ret, _query.c_str());
            }
        }
    }

    void SqliteStatement::setChar(const std::string& col, char data)
    {
        int idx = getBindIndex(col);
        sqlite3_stmt* stmt = getBindStmt();

        if (idx != 0)
        {
            reset();

            //PT_LOG_DEBUG("sqlite3_bind_text(" << stmt << ", " << idx << ", " << data
            //<< ", 1, SQLITE_TRANSIENT)");
            int ret = ::sqlite3_bind_text(stmt, idx, &data, 1, SQLITE_TRANSIENT);
            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::SqliteError(ret, _query.c_str());
            }
        }
    }

    void SqliteStatement::setString(const std::string& col, const std::string& data)
    {
        int idx = getBindIndex(col);
        sqlite3_stmt* stmt = getBindStmt();

        if (idx != 0)
        {
            reset();

            //PT_LOG_DEBUG("sqlite3_bind_text(" << stmt << ", " << idx << ", " << data
            //<< ", " << data.size() << ", SQLITE_TRANSIENT)");

            int n = static_cast<int>( data.size() );

            int ret = ::sqlite3_bind_text(stmt, idx, data.c_str(), n, SQLITE_TRANSIENT);

            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::SqliteError(ret, _query.c_str());
            }
        }
    }

    void SqliteStatement::setBlob(const std::string& col, const Blob& data)
    {
        int idx = getBindIndex(col);
        sqlite3_stmt* stmt = getBindStmt();

        if (idx != 0)
        {
            reset();

            //PT_LOG_DEBUG("sqlite3_bind_text(" << stmt << ", " << idx << ", " << data
            //<< ", " << data.size() << ", SQLITE_TRANSIENT)");

            int n = static_cast<int>( data.size() );

            int ret = ::sqlite3_bind_blob(stmt, idx, data.data(), n, SQLITE_TRANSIENT);

            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::SqliteError(ret, _query.c_str());
            }
        }
    }

    void SqliteStatement::setDate(const std::string& col, const Date& data)
    {
        setString(col, data.toIsoString());
    }

    void SqliteStatement::setTime(const std::string& col, const Time& data)
    {
        setString(col, data.toIsoString());
    }

    void SqliteStatement::setDatetime(const std::string& col, const DateTime& data)
    {
        setString(col, data.toIsoString());
    }

    SqliteStatement::size_type SqliteStatement::onExecute()
    {
        reset();
        _needReset = true;

        //PT_LOG_DEBUG("sqlite3_step(" << stmt << ')');
        int ret = sqlite3_step(_stmt);
        if (ret != SQLITE_DONE)
        {
            Pt::Db::sqlite::SqliteError(ret, _query.c_str());
        }

        setLastInsertId( ::sqlite3_last_insert_rowid(::sqlite3_db_handle(_stmt)) );
        return ::sqlite3_changes(::sqlite3_db_handle(_stmt));
    }

    Result SqliteStatement::onSelect()
    {
        reset();
        _needReset = true;

        ResultImpl* r = new ResultImpl();
        Result result(r);

        int ret;

        do
        {
            //PT_LOG_DEBUG("sqlite3_step(" << _stmt << ')');
            ret = sqlite3_step(_stmt);

            if (ret == SQLITE_ROW)
            {
                //PT_LOG_DEBUG("sqlite3_column_count(" << _stmt << ')');
                int count = ::sqlite3_column_count(_stmt);
                RowImpl* row = new RowImpl();
                r->add(Row(row));

                for (int i = 0; i < count; ++i)
                {
                    //PT_LOG_DEBUG("sqlite3_column_text(" << _stmt << ", " << i << ')');
                    int bytes = ::sqlite3_column_bytes(_stmt, i);
                    const unsigned char* txt = sqlite3_column_text(_stmt, i);
                    Value v;
                    if (txt)
                    {
                        v = Value( new ValueImpl(reinterpret_cast<const char*>(txt), bytes) );
                    }
                    row->add(v);
                }
            }
            else if(ret != SQLITE_DONE)
            {
                Pt::Db::sqlite::SqliteError(ret, _query.c_str());
            }

        } while (ret == SQLITE_ROW);

        return result;
    }

    Row SqliteStatement::onSelectRow()
    {
        reset();
        _needReset = true;

        //PT_LOG_DEBUG("sqlite3_step(" << _stmt << ')');
        int ret = sqlite3_step(_stmt);

        if (ret == SQLITE_ROW)
        {
            //PT_LOG_DEBUG("sqlite3_column_count(" << _stmt << ')');
            int count = ::sqlite3_column_count(_stmt);
            RowImpl* r = new RowImpl();
            Row row(r);

            for (int i = 0; i < count; ++i)
            {
                //PT_LOG_DEBUG("sqlite3_column_text(" << _stmt << ", " << i << ')');
                const unsigned char* txt = sqlite3_column_text(_stmt, i);
                Value v;

                if (txt)
                {
                    v = Value(new ValueImpl( reinterpret_cast<const char*>(txt)) );
                }
                r->add(v);
            }
            return row;
        }
        else if(ret == SQLITE_DONE)
        {
            throw InvalidQuery("sqlite3_step reached SQLITE_DONE, but more data was expected", _query);
        }

        Pt::Db::sqlite::SqliteError(ret, _query.c_str());
        return Row();
    }

    Value SqliteStatement::onSelectValue()
    {
        reset();
        _needReset = true;

        //PT_LOG_DEBUG("sqlite3_step(" << _stmt << ')');
        int ret = sqlite3_step(_stmt);
        if (ret == SQLITE_ROW)
        {
            //PT_LOG_DEBUG("sqlite3_column_count(" << _stmt << ')');
            int count = ::sqlite3_column_count(_stmt);

            if (count == 0)
            {
                throw InvalidQuery("Invalid query", _query);
            }

            //PT_LOG_DEBUG("sqlite3_column_text(" << _stmt << ", 0)");
            Value v;
            int bytes = ::sqlite3_column_bytes(_stmt, 0);
            const void* blob = ::sqlite3_column_blob(_stmt, 0);
            if (blob)
            {
                v = Value( new ValueImpl(reinterpret_cast<const char*>(blob), bytes) );
            }
            return v;
        }
        else if (ret == SQLITE_DONE)
        {
            throw InvalidQuery("sqlite3_step reached SQLITE_DONE, but more data was expected", _query);
        }

        Pt::Db::sqlite::SqliteError(ret, _query.c_str());
        return Value();
    }

    ICursor* SqliteStatement::onCreateCursor()
    {
        _stmtInUse = getBindStmt();
        _stmt = 0;
        return new SqliteCursor(this, _stmtInUse);
    }


    void SqliteStatement::onBeginExec()
    {
        _conn->enqueueStmtExec(*this);
    }


    IStatement::size_type SqliteStatement::onEndExec()
    {
        return _conn->completeStmtExec();
    }


    void SqliteStatement::onBeginSelect()
    {
        _conn->enqueueStmtSelect(*this);
    }


    Result SqliteStatement::onEndSelect()
    {
        return _conn->completeStmtSelect();
    }

} //namespace sqlite

} //namespace Db

} //namespace Pt