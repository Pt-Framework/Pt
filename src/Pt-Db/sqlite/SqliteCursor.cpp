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

#include "SqliteCursor.h"
#include "StmtRow.h"
#include "SqliteError.h"
#include "SqliteConnection.h"
#include "../ResultImpl.h"

#include <Pt/Db/Row.h>
#include <Pt/System/Thread.h>

namespace Pt {

namespace Db {

namespace sqlite {

    SqliteCursor::SqliteCursor(SqliteStatement* statement, sqlite3_stmt* stmt)
        : ICursor(statement->connection())
        , _statement(statement)
        , _stmt(stmt)
        , _done(false)
    { }

    SqliteCursor::~SqliteCursor()
    {
        _conn->cancelCursor(*this);
        _statement->putback(_stmt);
    }

    Row SqliteCursor::fetchRow()
    {
        int ret = 0;
        int n = 0;

        //when sqlite3_step() returns SQLITE_LOCKED be aware
        //that sqlite3_reset() will most likely also return
        //SQLITE_LOCKED and thus it is necessary to repeat calling
        //it until it returns SQLITE_OK.
        do
        {
            ret = sqlite3_step(_stmt);

            if( (ret == SQLITE_BUSY) || (ret == SQLITE_LOCKED) )
            {
                Pt::System::Thread::sleep(200);
                n++;
            }
        }
        while((n < 50) && ((ret == SQLITE_BUSY) || (ret == SQLITE_LOCKED)));

        if (ret == SQLITE_DONE)
        {
            return Row();
        }
        else if (ret != SQLITE_ROW)
        {
            Pt::Db::sqlite::SqliteError(ret);
        }

        return Row(new StmtRow(getStmt()));
    }

    Result SqliteCursor::onFetchBatch(size_type batchSize)
    {
        ResultImpl* res = new ResultImpl();
        Result result(res);

        for(size_type i = 0; i < batchSize && ! _done; ++i)
        {
            Row row = fetchRow();
            if(row.empty())
            {
                _done = true;
                break;
            }
            res->add(row);
        }

        return result;
    }


    void SqliteCursor::onBeginBatchFetch(size_type batchSize)
    {
        _open = true;
        _statement->getConnection()->enqueueBatchFetch(*this, batchSize);
    }

    Result SqliteCursor::onEndBatchFetch()
    {
        return _statement->getConnection()->completeBatchFetch(_done);
    }

    void SqliteCursor::onClose()
    {
        _open = false;
        _done = false;
    }

} //namespace sqlite

} //namespace Db

} //namespace Pt
