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
#include "SqliteCursor.h"

#include "../RowImpl.h"
#include "../ValueImpl.h"
#include "../ResultImpl.h"
#include "SqliteError.h"

#include <Pt/Db/Exception.h>
#include <Pt/Db/Result.h>
#include <Pt/Db/Row.h>
#include <Pt/Db/Value.h>
#include <Pt/Db/Statement.h>
#include <Pt/System/Thread.h>
#include <Pt/System/Mutex.h>
#include <Pt/Method.h>

#include <stdlib.h>
#include <stdexcept>

namespace {

// Build a Result by running a SELECT via sqlite3_exec callback
int selectCallback(void* pArg, int argc, char** argv, char** /*columnNames*/)
{
    Pt::Db::ResultImpl* res = static_cast<Pt::Db::ResultImpl*>(pArg);

    Pt::Db::RowImpl::data_type data;
    for(int i = 0; i < argc; ++i)
    {
        Pt::Db::Value v;
        if(argv[i])
            v = Pt::Db::Value( new Pt::Db::ValueImpl(argv[i]) );
        data.push_back(v);
    }

    res->add( Pt::Db::Row( new Pt::Db::RowImpl(data) ) );
    return SQLITE_OK;
}

// Retry on SQLITE_BUSY
int busyHandler(void* pArg, int priorCalls)
{
    if(priorCalls < 500)
    {
        if(pArg == 0)
            Pt::System::Thread::sleep( (rand() % 500) + 400 );
        else
            Pt::System::Thread::sleep(500);
        return 1;
    }
    return 0;
}

} // namespace


namespace Pt {

namespace Db {

namespace sqlite {

Connection::Connection()
: _db(0)
, _conninfo()
, _worker( Pt::Method<void, Connection>(*this, &Connection::workerRun) )
, _shutdown(false)
, _cancelFlag(false)
, _completedOp(WkNone)
, _pendingOp(WkNone)
, _pendingStmt(0)
, _pendingBatchSize(0)
, _batchCursor(0)
, _batchSize(100)
, _batchDone(false)
, _rowCount(0)
, _opFailed(false)
{
}


Connection::~Connection()
{
    onClose();

    {
        Pt::System::MutexLock lock(_mutex);
        _shutdown = true;
        _workReady.signal();
    }

    if(_worker.isJoinable())
        _worker.join();
}


// --- Worker thread ---

void Connection::ensureWorker()
{
    if(_worker.state() == Pt::System::Thread::Ready)
        _worker.start();
}


void Connection::enqueue(OpType op)
{
    ensureWorker();

    Pt::System::MutexLock lock(_mutex);
    _pendingOp = op;
    _workReady.signal();
}


void Connection::workerRun()
{
    while(true)
    {
        OpType op;
        {
            Pt::System::MutexLock lock(_mutex);

            while(_pendingOp == WkNone && ! _shutdown)
                _workReady.wait(_mutex);

            if(_shutdown)
                break;

            op = _pendingOp;
        }

        // Execute the operation outside the lock
        bool failed = false;
        std::string errorMsg;
        Result resultSet;
        size_type rowCount = 0;
        Pt::Db::Statement preparedStmt;

        try
        {
            switch(op)
            {
                case WkOpen:
                {
                    if(_db)
                    {
                        clearStatementCache();
                        ::sqlite3_close(_db);
                        _db = 0;
                    }
                    int ret = ::sqlite3_open(_conninfo.c_str(), &_db);
                    if(ret != SQLITE_OK)
                        throw InvalidConnection(std::string("sqlite3_open failed: ") + ::sqlite3_errmsg(_db));
                    ::sqlite3_enable_load_extension(_db, 1);
                    ::sqlite3_busy_handler(_db, busyHandler, _db);
                    break;
                }

                case WkExec:
                {
                    char* errmsg = 0;
                    ResultImpl* res = new ResultImpl();
                    resultSet = Result(res);

                    int ret = ::sqlite3_exec(_db, _pendingSql.c_str(), selectCallback, res, &errmsg);

                    if(ret != SQLITE_OK && ret != SQLITE_INTERRUPT)
                    {
                        std::string msg = errmsg ? errmsg : "sqlite3_exec failed";
                        ::sqlite3_free(errmsg);
                        throw QueryFailed(msg, _pendingSql);
                    }
                    if(errmsg)
                        ::sqlite3_free(errmsg);

                    rowCount = static_cast<size_type>( ::sqlite3_changes(_db) );
                    break;
                }

                case WkSelect:
                {
                    char* errmsg = 0;
                    ResultImpl* res = new ResultImpl();
                    resultSet = Result(res);

                    int ret = ::sqlite3_exec(_db, _pendingSql.c_str(), selectCallback, res, &errmsg);

                    if(ret != SQLITE_OK && ret != SQLITE_INTERRUPT)
                    {
                        std::string msg = errmsg ? errmsg : "sqlite3_exec failed";
                        ::sqlite3_free(errmsg);
                        throw QueryFailed(msg, _pendingSql);
                    }
                    if(errmsg)
                        ::sqlite3_free(errmsg);
                    break;
                }

                case WkStmtExec:
                {
                    rowCount = _pendingStmt->execute();
                    break;
                }

                case WkStmtSelect:
                {
                    resultSet = _pendingStmt->select();
                    break;
                }

                case WkBatchFetch:
                {
                    if( ! _batchCursor || _batchDone)
                    {
                        resultSet = Result( new ResultImpl() );
                        break;
                    }

                    ResultImpl* res = new ResultImpl();
                    resultSet = Result(res);

                    for(size_type i = 0; i < _batchSize; ++i)
                    {
                        if(_cancelFlag.load())
                            break;

                        Pt::Db::Row row = _batchCursor->fetch();

                        if(row.empty())
                        {
                            _batchDone = true;
                            break;
                        }

                        res->add(row);
                    }
                    break;
                }

                case WkPrepare:
                {
                    preparedStmt = Pt::Db::Statement( new Pt::Db::sqlite::Statement(this, _pendingSql) );
                    break;
                }

                case WkBeginTxn:
                {
                    char* errmsg = nullptr;
                    int ret = ::sqlite3_exec(_db, "BEGIN TRANSACTION", nullptr, nullptr, &errmsg);
                    if(ret != SQLITE_OK)
                    {
                        std::string msg = errmsg ? errmsg : "BEGIN TRANSACTION failed";
                        ::sqlite3_free(errmsg);
                        throw QueryFailed(msg, "BEGIN TRANSACTION");
                    }
                    break;
                }

                case WkCommitTxn:
                {
                    char* errmsg = nullptr;
                    int ret = ::sqlite3_exec(_db, "COMMIT TRANSACTION", nullptr, nullptr, &errmsg);
                    if(ret != SQLITE_OK)
                    {
                        std::string msg = errmsg ? errmsg : "COMMIT TRANSACTION failed";
                        ::sqlite3_free(errmsg);
                        throw QueryFailed(msg, "COMMIT TRANSACTION");
                    }
                    break;
                }

                case WkRollbackTxn:
                {
                    char* errmsg = nullptr;
                    int ret = ::sqlite3_exec(_db, "ROLLBACK TRANSACTION", nullptr, nullptr, &errmsg);
                    if(ret != SQLITE_OK)
                    {
                        std::string msg = errmsg ? errmsg : "ROLLBACK TRANSACTION failed";
                        ::sqlite3_free(errmsg);
                        throw QueryFailed(msg, "ROLLBACK TRANSACTION");
                    }
                    break;
                }

                default:
                    break;
            }
        }
        catch(const std::exception& e)
        {
            failed   = true;
            errorMsg = e.what();
        }

        // Store result and notify — all under the same lock so that
        // cancel() cannot return until setReady() has already been called.
        {
            Pt::System::MutexLock lock(_mutex);
            _completedOp = op;
            _pendingOp   = WkNone;
            _resultSet   = resultSet;
            _rowCount    = rowCount;
            _preparedStmt = preparedStmt;
            _opFailed    = failed;
            _opError     = errorMsg;

            // Only notify the EventLoop if the operation was not cancelled.
            // After cancelOp() returns, no post() must be pending.
            if(!_cancelFlag.load())
                post();

            _workDone.signal();
        }
    }
}


// --- Synchronous operations ---

void Connection::startTransaction()
{
    execute("BEGIN TRANSACTION");
}


void Connection::commitTransaction()
{
    clearStatementCache();
    execute("COMMIT TRANSACTION");
}


void Connection::rollbackTransaction()
{
    clearStatementCache();
    execute("ROLLBACK TRANSACTION");
}


Connection::size_type Connection::execute(const std::string& query)
{
    char* errmsg = 0;
    int ret = ::sqlite3_exec(_db, query.c_str(), 0, 0, &errmsg);

    if(ret != SQLITE_OK)
        Pt::Db::sqlite::SqliteError(ret, query.c_str());

    return static_cast<size_type>( ::sqlite3_changes(_db) );
}


Result Connection::select(const std::string& query)
{
    return prepare(query).select();
}


Pt::Db::Statement Connection::prepare(const std::string& query)
{
    return Pt::Db::Statement( new Pt::Db::sqlite::Statement(this, query) );
}


long long Connection::insertId()
{
    return ::sqlite3_last_insert_rowid(_db);
}


// --- Async operation hooks ---

void Connection::onOpen(const std::string& connStr)
{
    if(_db)
    {
        clearStatementCache();
        ::sqlite3_close(_db);
        _db = 0;
    }

    int ret = ::sqlite3_open(connStr.c_str(), &_db);
    if(ret != SQLITE_OK)
        throw InvalidConnection(std::string("sqlite3_open failed: ") + ::sqlite3_errmsg(_db));

    ::sqlite3_enable_load_extension(_db, 1);
    ::sqlite3_busy_handler(_db, busyHandler, _db);
}


void Connection::onClose()
{
    onCancelOp();

    if(_db)
    {
        clearStatementCache();
        ::sqlite3_close(_db);
        _db = 0;
    }
}


void Connection::onBeginExec(const std::string& sql)
{
    _pendingSql  = sql;
    _pendingStmt = 0;
    enqueue(WkExec);
}


void Connection::onBeginSelect(const std::string& sql)
{
    _pendingSql  = sql;
    _pendingStmt = 0;
    enqueue(WkSelect);
}


void Connection::beginExec(Statement& stmt)
{
    if(_state != Idle)
        throw InvalidConnection("Operation pending");
    _state       = PendingExec;
    _pendingStmt = &stmt;
    enqueue(WkStmtExec);
}


Connection::size_type Connection::endExec(Statement& /*stmt*/)
{
    _state = Idle;
    Pt::System::MutexLock lock(_mutex);
    if(_opFailed)
        throw QueryFailed(_opError, "");
    return _rowCount;
}


void Connection::beginSelect(Statement& stmt)
{
    if(_state != Idle)
        throw InvalidConnection("Operation pending");
    _state       = PendingSelect;
    _pendingStmt = &stmt;
    enqueue(WkStmtSelect);
}


Result Connection::endSelect(Statement& /*stmt*/)
{
    _state = Idle;
    Pt::System::MutexLock lock(_mutex);
    if(_opFailed)
        throw QueryFailed(_opError, "");
    return _resultSet;
}


Connection::size_type Connection::onEndExec()
{
    Pt::System::MutexLock lock(_mutex);
    if(_opFailed)
        throw QueryFailed(_opError, "");
    return _rowCount;
}


Result Connection::onEndSelect()
{
    Pt::System::MutexLock lock(_mutex);
    if(_opFailed)
        throw QueryFailed(_opError, "");
    return _resultSet;
}


void Connection::onBeginOpen(const std::string& connStr)
{
    _conninfo = connStr;
    enqueue(WkOpen);
}


void Connection::onEndOpen()
{
    Pt::System::MutexLock lock(_mutex);
    if(_opFailed)
        throw InvalidConnection(_opError);
}


void Connection::beginBatchFetch(ICursor& cursor, size_type batchSize)
{
    if(_state != Idle)
        throw InvalidConnection("Operation pending");
    _state       = PendingBatchFetch;
    _batchCursor = static_cast<SqliteCursor*>(&cursor);
    _batchSize   = batchSize;
    enqueue(WkBatchFetch);
}


Result Connection::endBatchFetch()
{
    _state = Idle;
    Pt::System::MutexLock lock(_mutex);
    if(_opFailed)
        throw QueryFailed(_opError, "");
    return _resultSet;
}


void Connection::closeBatchFetch()
{
    if(_state == PendingBatchFetch)
    {
        _state = Idle;
        onCancelOp();
    }
    _batchCursor = nullptr;
    _batchDone   = false;
}


void Connection::onSetActive(Pt::System::EventLoop* loop)
{
    if(loop)
        Selectable::setActive(*loop);
    else
        Selectable::detach();
}


void Connection::onCancelOp()
{
    // Ask SQLite to interrupt any running query (thread-safe per SQLite docs)
    if(_db)
        ::sqlite3_interrupt(_db);

    _cancelFlag.store(true);

    // Wait until worker finishes the current op
    {
        Pt::System::MutexLock lock(_mutex);
        while(_pendingOp != WkNone)
            _workDone.wait(_mutex);
    }

    _cancelFlag.store(false);
}


void Connection::onCancel()
{
    onCancelOp();
}


bool Connection::onRun()
{
    switch(_completedOp)
    {
        case WkOpen:
            _finished.send();
            break;
        case WkExec:
            _executeFinished.send();
            break;
        case WkSelect:
            _selectFinished.send();
            break;
        case WkStmtExec:
            if(_pendingStmt)
                _pendingStmt->executeFinished().send();
            break;
        case WkStmtSelect:
            if(_pendingStmt)
                _pendingStmt->selectFinished().send();
            break;
        case WkBatchFetch:
            if(_batchCursor)
                _batchCursor->fetched().send();
            break;
        case WkPrepare:
            _prepareFinished.send();
            break;
        case WkBeginTxn:
        case WkCommitTxn:
        case WkRollbackTxn:
            _transactionFinished.send();
            break;
        default:
            _finished.send();
            break;
    }
    return true;
}

void Connection::onBeginPrepare(const std::string& query)
{
    _pendingSql  = query;
    _pendingStmt = nullptr;
    enqueue(WkPrepare);
}


Pt::Db::Statement Connection::onEndPrepare()
{
    Pt::System::MutexLock lock(_mutex);
    if(_opFailed)
        throw QueryFailed(_opError, "");
    return _preparedStmt;
}


void Connection::onBeginStartTransaction()
{
    enqueue(WkBeginTxn);
}


void Connection::onEndStartTransaction()
{
    Pt::System::MutexLock lock(_mutex);
    if(_opFailed)
        throw QueryFailed(_opError, "");
}


void Connection::onBeginCommitTransaction()
{
    clearStatementCache();
    enqueue(WkCommitTxn);
}


void Connection::onEndCommitTransaction()
{
    Pt::System::MutexLock lock(_mutex);
    if(_opFailed)
        throw QueryFailed(_opError, "");
}


void Connection::onBeginRollbackTransaction()
{
    clearStatementCache();
    enqueue(WkRollbackTxn);
}


void Connection::onEndRollbackTransaction()
{
    Pt::System::MutexLock lock(_mutex);
    if(_opFailed)
        throw QueryFailed(_opError, "");
}

} // namespace sqlite

} // namespace Db

} // namespace Pt
