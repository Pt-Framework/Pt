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

#include "Statement.h"
#include "Cursor.h"
#include "Connection.h"

#include "../ResultImpl.h"
#include "../RowImpl.h"
#include "../ValueImpl.h"

#include "Error.h"

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

    Statement::Statement(Connection* conn, const std::string& query)
        : _stmt(0)
        , _stmtInUse(0)
        , _conn(conn)
        , _query(query)
        , _needReset(false)
    {
    }

    Statement::~Statement()
    {
        if (_stmt)
        {
            //log_debug("sqlite3_finalize(" << stmt << ')');
            ::sqlite3_finalize(_stmt);
        }

        if (_stmtInUse && _stmtInUse != _stmt)
        {
            //log_debug("sqlite3_finalize(" << _stmtInUse << ')');
            ::sqlite3_finalize(_stmtInUse);
        }
    }



    sqlite3_stmt* Statement::getBindStmt()
    {
        if (_stmt == 0)
        {
            // hostvars don't need to be parsed, because sqlite accepts the hostvar-
            // syntax of tntDb (:vvv)

            // prepare statement
            const char* tzTail;
            //log_debug("sqlite3_prepare(" << _conn->getSqlite3() << ", \"" << _query
            //  << "\", " << &_stmt << ", " << &tzTail << ')');
            int ret = ::sqlite3_prepare(_conn->getSqlite3(), _query.data(), _query.size(), &_stmt, &tzTail);
            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
            }

            //log_debug("sqlite3_stmt = " << _stmt);

            if (_stmtInUse)
            {
                // get bindings from _stmtInUse
                //log_debug("sqlite3_transfer_bindings(" << _stmtInUse << ", " << _stmt << ')');
                ret = ::sqlite3_transfer_bindings(_stmtInUse, _stmt);
                if (ret != SQLITE_OK)
                {
                    //log_debug("sqlite3_finalize(" << _stmt << ')');
                    ::sqlite3_finalize(_stmt);
                    _stmt = 0;
                        Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
                }
            }
        }

        return _stmt;
    }

    void Statement::putback(sqlite3_stmt* stmt)
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
            //log_debug("sqlite3_finalize(" << stmt_ << ')');
            ::sqlite3_finalize(stmt);
        }
    }

    int Statement::getBindIndex(const std::string& col)
    {
        sqlite3_stmt* _stmt = getBindStmt();

        //log_debug("sqlite3_bind_parameter_index(" << _stmt << ", :" << col  << ')');
        int idx = ::sqlite3_bind_parameter_index(_stmt, (':' + col).c_str());
        //if (idx == 0)
        //{
        //  log_warn("hostvariable :" << col << " not found");
        //}
        return idx;
    }

    void Statement::reset()
    {
        if (_stmt)
        {
            if (_needReset)
            {
                //log_debug("sqlite3_reset(" << _stmt << ')');
                int ret = ::sqlite3_reset(_stmt);
                if(ret != SQLITE_OK)
                {
                    Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
                }

                _needReset = false;
            }
        }
        else
        {
            getBindStmt();
        }
    }

    void Statement::clear()
    {
        sqlite3_stmt* stmt = getBindStmt();

        int count = ::sqlite3_bind_parameter_count(stmt);

        for (int i = 0; i < count; ++i)
        {
            int ret = ::sqlite3_bind_null(stmt, i + 1);
            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
            }
        }
    }

    void Statement::setNull(const std::string& col)
    {
        int idx = getBindIndex(col);
        sqlite3_stmt* stmt = getBindStmt();

        if (idx != 0)
        {
            reset();

            //log_debug("sqlite3_bind_null(" << stmt << ", " << idx << ')');
            int ret = ::sqlite3_bind_null(stmt, idx);
            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
            }
        }
    }

    void Statement::setBool(const std::string& col, bool data)
    {
        setInt(col, data ? 1 : 0);
    }

    void Statement::setInt(const std::string& col, int data)
    {
        int idx = getBindIndex(col);
        sqlite3_stmt* stmt = getBindStmt();

        if (idx != 0)
        {
            reset();

            //log_debug("sqlite3_bind_int(" << stmt << ", " << idx << ')');
            int ret = ::sqlite3_bind_int(stmt, idx, data);
            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
            }
        }
    }

    void Statement::setUnsigned(const std::string& col, unsigned data)
    {
        if (data > static_cast<unsigned>(std::numeric_limits<int>::max()))
        {
            //log_warn("possible loss of precision while converting large unsigned " << data
            //  << " to double");
            setDouble(col, static_cast<double>(data));
        }
        else
        {
            setInt(col, static_cast<int>(data));
        }
    }

    void Statement::setFloat(const std::string& col, float data)
    {
        setDouble(col, static_cast<double>(data));
    }

    void Statement::setDouble(const std::string& col, double data)
    {
        int idx = getBindIndex(col);
        sqlite3_stmt* stmt = getBindStmt();

        if (idx != 0)
        {
            reset();

            //log_debug("sqlite3_bind_double(" << stmt << ", " << idx << ')');
            int ret = ::sqlite3_bind_double(stmt, idx, data);
            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
            }
        }
    }

    void Statement::setChar(const std::string& col, char data)
    {
        int idx = getBindIndex(col);
        sqlite3_stmt* stmt = getBindStmt();

        if (idx != 0)
        {
            reset();

            //log_debug("sqlite3_bind_text(" << stmt << ", " << idx << ", " << data
            //<< ", 1, SQLITE_TRANSIENT)");
            int ret = ::sqlite3_bind_text(stmt, idx, &data, 1, SQLITE_TRANSIENT);
            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
            }
        }
    }

    void Statement::setString(const std::string& col, const std::string& data)
    {
        int idx = getBindIndex(col);
        sqlite3_stmt* stmt = getBindStmt();

        if (idx != 0)
        {
            reset();

            //log_debug("sqlite3_bind_text(" << stmt << ", " << idx << ", " << data
            //<< ", " << data.size() << ", SQLITE_TRANSIENT)");
            
            int ret = ::sqlite3_bind_text(stmt, idx, data.c_str(), data.size(), SQLITE_TRANSIENT);

            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
            }
        }
    }

    void Statement::setBlob(const std::string& col, const Blob& data)
    {
        int idx = getBindIndex(col);
        sqlite3_stmt* stmt = getBindStmt();

        if (idx != 0)
        {
            reset();

            //log_debug("sqlite3_bind_text(" << stmt << ", " << idx << ", " << data
            //<< ", " << data.size() << ", SQLITE_TRANSIENT)");
            
            int ret = ::sqlite3_bind_blob(stmt, idx, data.data(), data.size(), SQLITE_TRANSIENT);

            if(ret != SQLITE_OK)
            {
                Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
            }
        }
    }

    void Statement::setDate(const std::string& col, const Date& data)
    {
        setString(col, data.toIsoString());
    }

    void Statement::setTime(const std::string& col, const Time& data)
    {
        setString(col, data.toIsoString());
    }

    void Statement::setDatetime(const std::string& col, const DateTime& data)
    {
        setString(col, data.toIsoString());
    }

    Statement::size_type Statement::execute()
    {
        reset();
        _needReset = true;

        //log_debug("sqlite3_step(" << stmt << ')');
        int ret = sqlite3_step(_stmt);
        if (ret != SQLITE_DONE)
        {
            Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
        }

        return ::sqlite3_changes(::sqlite3_db_handle(_stmt));
    }

    Result Statement::select()
    {
        reset();
        _needReset = true;

        ResultImpl* r = new ResultImpl();
        Result result(r);

        int ret;

        do
        {
            //log_debug("sqlite3_step(" << _stmt << ')');
            ret = sqlite3_step(_stmt);

            if (ret == SQLITE_ROW)
            {
                //log_debug("sqlite3_column_count(" << _stmt << ')');
                int count = ::sqlite3_column_count(_stmt);
                RowImpl* row = new RowImpl();
                r->add(Row(row));

                for (int i = 0; i < count; ++i)
                {
                    //log_debug("sqlite3_column_text(" << _stmt << ", " << i << ')');
                    size_t bytes = ::sqlite3_column_bytes(_stmt, i);
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
                Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
            }

        } while (ret == SQLITE_ROW);

        return result;
    }

    Row Statement::selectRow()
    {
        reset();
        _needReset = true;

        //log_debug("sqlite3_step(" << _stmt << ')');
        int ret = sqlite3_step(_stmt);

        if (ret == SQLITE_ROW)
        {
            //log_debug("sqlite3_column_count(" << _stmt << ')');
            int count = ::sqlite3_column_count(_stmt);
            RowImpl* r = new RowImpl();
            Row row(r);

            for (int i = 0; i < count; ++i)
            {
                //log_debug("sqlite3_column_text(" << _stmt << ", " << i << ')');
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
            throw std::logic_error("sqlite3_step reached 'SQLITE_DONE', but more data was expected" + PT_SOURCEINFO);
        }

        Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
        return Row();
    }

    Value Statement::selectValue()
    {
        reset();
        _needReset = true;

        //log_debug("sqlite3_step(" << _stmt << ')');
        int ret = sqlite3_step(_stmt);
        if (ret == SQLITE_ROW)
        {
            //log_debug("sqlite3_column_count(" << _stmt << ')');
            int count = ::sqlite3_column_count(_stmt);

            if (count == 0)
            {
                throw std::logic_error("Invalid query" + PT_SOURCEINFO);
            }

            //log_debug("sqlite3_column_text(" << _stmt << ", 0)");
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
            throw std::logic_error("sqlite3_step reached 'SQLITE_DONE', but more data was expected" + PT_SOURCEINFO);
        }

        Pt::Db::sqlite::Error(ret, PT_SOURCEINFO);
        return Value();
    }

    ICursor* Statement::createCursor()
    {
        _stmtInUse = getBindStmt();
        _stmt = 0;
        return new Cursor(this, _stmtInUse);
    }

} //namespace sqlite

} //namespace Db

} //namespace Pt
