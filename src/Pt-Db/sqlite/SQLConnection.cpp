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
#include <Pt/Db/Transaction.h>
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
: _pendingTask(nullptr)
, _completedTask(nullptr)
, _db(0)
, _conninfo()
, _thread( Pt::Method<void, Connection>(*this, &Connection::processTasks) )
, _shutdown(false)
, _cancelFlag(false)
, _batchCursor(0)
, _batchDone(false)
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

    if(_thread.isJoinable())
        _thread.join();
}


// --- Worker thread ---

void Connection::ensureWorker()
{
    if(_thread.state() == Pt::System::Thread::Ready)
        _thread.start();
}


void Connection::enqueue(Task* task)
{
    ensureWorker();

    Pt::System::MutexLock lock(_mutex);
    _pendingTask = task;
    _workReady.signal();
}


void Connection::processTasks()
{
    while(true)
    {
        Task* task;
        {
            Pt::System::MutexLock lock(_mutex);

            while( ! _pendingTask && ! _shutdown)
                _workReady.wait(_mutex);

            if(_shutdown)
                break;

            task = _pendingTask;
        }

        // Execute outside the lock; results stored in the task
        std::exception_ptr exception;

        try
        {
            task->execute(*this);
        }
        catch(...)
        {
            exception = std::current_exception();
        }

        // Store exception under the lock so that endXxx() mutex
        // establishes happens-before with writes done in execute().
        {
            Pt::System::MutexLock lock(_mutex);
            task->exception = exception;
            _completedTask = task;
            _pendingTask   = nullptr;

            if( ! _cancelFlag.load())
                post();

            _workDone.signal();
        }
    }
}


// --- Task execute() implementations ---

void Connection::OpenTask::execute(Connection& conn)
{
    if(conn._db)
    {
        conn.clearStatementCache();
        ::sqlite3_close(conn._db);
        conn._db = 0;
    }
    int ret = ::sqlite3_open(connStr.c_str(), &conn._db);
    if(ret != SQLITE_OK)
        throw InvalidConnection(std::string("sqlite3_open failed: ") + ::sqlite3_errmsg(conn._db));
    ::sqlite3_enable_load_extension(conn._db, 1);
    ::sqlite3_busy_handler(conn._db, busyHandler, conn._db);
}

void Connection::OpenTask::complete(Connection& conn)
{
    conn._finished.send();
}


void Connection::ExecTask::execute(Connection& conn)
{
    char* errmsg = 0;
    ResultImpl* res = new ResultImpl();
    result = Result(res);

    int ret = ::sqlite3_exec(conn._db, sql.c_str(), selectCallback, res, &errmsg);

    if(ret != SQLITE_OK && ret != SQLITE_INTERRUPT)
    {
        std::string msg = errmsg ? errmsg : "sqlite3_exec failed";
        ::sqlite3_free(errmsg);
        throw QueryFailed(msg, sql);
    }
    if(errmsg)
        ::sqlite3_free(errmsg);

    rowCount = static_cast<size_type>( ::sqlite3_changes(conn._db) );
}

void Connection::ExecTask::complete(Connection& conn)
{
    conn._executeFinished.send();
}


void Connection::SelectTask::execute(Connection& conn)
{
    char* errmsg = 0;
    ResultImpl* res = new ResultImpl();
    result = Result(res);

    int ret = ::sqlite3_exec(conn._db, sql.c_str(), selectCallback, res, &errmsg);

    if(ret != SQLITE_OK && ret != SQLITE_INTERRUPT)
    {
        std::string msg = errmsg ? errmsg : "sqlite3_exec failed";
        ::sqlite3_free(errmsg);
        throw QueryFailed(msg, sql);
    }
    if(errmsg)
        ::sqlite3_free(errmsg);
}

void Connection::SelectTask::complete(Connection& conn)
{
    conn._selectFinished.send();
}


void Connection::PrepareTask::execute(Connection& conn)
{
    result = Pt::Db::Statement( new Pt::Db::sqlite::Statement(&conn, sql) );
}

void Connection::PrepareTask::complete(Connection& conn)
{
    conn._prepareFinished.send();
}


void Connection::StmtExecTask::execute(Connection& /*conn*/)
{
    rowCount = stmt->execute();
}

void Connection::StmtExecTask::complete(Connection& /*conn*/)
{
    stmt->executeFinished().send();
}


void Connection::StmtSelectTask::execute(Connection& /*conn*/)
{
    result = stmt->select();
}

void Connection::StmtSelectTask::complete(Connection& /*conn*/)
{
    stmt->selectFinished().send();
}


void Connection::BeginTxnTask::execute(Connection& conn)
{
    char* errmsg = nullptr;
    int ret = ::sqlite3_exec(conn._db, sql.c_str(), nullptr, nullptr, &errmsg);
    if(ret != SQLITE_OK)
    {
        std::string msg = errmsg ? errmsg : "BEGIN TRANSACTION failed";
        ::sqlite3_free(errmsg);
        throw QueryFailed(msg, sql);
    }
}

void Connection::BeginTxnTask::complete(Connection& /*conn*/)
{
    txn->startFinished().send();
}


void Connection::CommitTxnTask::execute(Connection& conn)
{
    char* errmsg = nullptr;
    int ret = ::sqlite3_exec(conn._db, sql.c_str(), nullptr, nullptr, &errmsg);
    if(ret != SQLITE_OK)
    {
        std::string msg = errmsg ? errmsg : "COMMIT TRANSACTION failed";
        ::sqlite3_free(errmsg);
        throw QueryFailed(msg, sql);
    }
}

void Connection::CommitTxnTask::complete(Connection& /*conn*/)
{
    txn->commitFinished().send();
}


void Connection::RollbackTxnTask::execute(Connection& conn)
{
    char* errmsg = nullptr;
    int ret = ::sqlite3_exec(conn._db, sql.c_str(), nullptr, nullptr, &errmsg);
    if(ret != SQLITE_OK)
    {
        std::string msg = errmsg ? errmsg : "ROLLBACK TRANSACTION failed";
        ::sqlite3_free(errmsg);
        throw QueryFailed(msg, sql);
    }
}

void Connection::RollbackTxnTask::complete(Connection& /*conn*/)
{
    txn->rollbackFinished().send();
}


void Connection::BatchFetchTask::execute(Connection& /*conn*/)
{
    if( ! cursor || done)
    {
        result = Result( new ResultImpl() );
        return;
    }

    ResultImpl* res = new ResultImpl();
    result = Result(res);

    for(size_type i = 0; i < batchSize; ++i)
    {
        Pt::Db::Row row = cursor->fetch();

        if(row.empty())
        {
            done = true;
            break;
        }

        res->add(row);
    }
}

void Connection::BatchFetchTask::complete(Connection& /*conn*/)
{
    cursor->fetched().send();
}


// --- Synchronous operations ---

void Connection::startTransaction(const char* sql)
{
    execute(sql ? sql : "BEGIN TRANSACTION");
}


void Connection::commitTransaction(const char* sql)
{
    clearStatementCache();
    execute(sql ? sql : "COMMIT TRANSACTION");
}


void Connection::rollbackTransaction(const char* sql)
{
    clearStatementCache();
    execute(sql ? sql : "ROLLBACK TRANSACTION");
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
    _execTask.sql = sql;
    enqueue(&_execTask);
}


void Connection::onBeginSelect(const std::string& sql)
{
    _selectTask.sql = sql;
    enqueue(&_selectTask);
}


void Connection::beginExec(Statement& stmt)
{
    if(_state != Idle)
        throw InvalidConnection("Operation pending");
    _state             = PendingExec;
    _stmtExecTask.stmt = &stmt;
    enqueue(&_stmtExecTask);
}


Connection::size_type Connection::endExec(Statement& /*stmt*/)
{
    _state = Idle;
    Pt::System::MutexLock lock(_mutex);
    if(_stmtExecTask.exception)
        std::rethrow_exception(_stmtExecTask.exception);
    return _stmtExecTask.rowCount;
}


void Connection::beginSelect(Statement& stmt)
{
    if(_state != Idle)
        throw InvalidConnection("Operation pending");
    _state               = PendingSelect;
    _stmtSelectTask.stmt = &stmt;
    enqueue(&_stmtSelectTask);
}


Result Connection::endSelect(Statement& /*stmt*/)
{
    _state = Idle;
    Pt::System::MutexLock lock(_mutex);
    if(_stmtSelectTask.exception)
        std::rethrow_exception(_stmtSelectTask.exception);
    return _stmtSelectTask.result;
}


Connection::size_type Connection::onEndExec()
{
    Pt::System::MutexLock lock(_mutex);
    if(_execTask.exception)
        std::rethrow_exception(_execTask.exception);
    return _execTask.rowCount;
}


Result Connection::onEndSelect()
{
    Pt::System::MutexLock lock(_mutex);
    if(_selectTask.exception)
        std::rethrow_exception(_selectTask.exception);
    return _selectTask.result;
}


void Connection::onBeginOpen(const std::string& connStr)
{
    _openTask.connStr = connStr;
    enqueue(&_openTask);
}


void Connection::onEndOpen()
{
    Pt::System::MutexLock lock(_mutex);
    if(_openTask.exception)
        std::rethrow_exception(_openTask.exception);
}


void Connection::beginBatchFetch(ICursor& cursor, size_type batchSize)
{
    if(_state != Idle)
        throw InvalidConnection("Operation pending");
    _state                   = PendingBatchFetch;
    _batchFetchTask.cursor    = static_cast<SqliteCursor*>(&cursor);
    _batchFetchTask.batchSize = batchSize;
    _batchFetchTask.done      = _batchDone;
    _batchCursor              = static_cast<SqliteCursor*>(&cursor);
    enqueue(&_batchFetchTask);
}


Result Connection::endBatchFetch()
{
    _state = Idle;
    Pt::System::MutexLock lock(_mutex);
    if(_batchFetchTask.exception)
        std::rethrow_exception(_batchFetchTask.exception);
    _batchDone = _batchFetchTask.done;
    return _batchFetchTask.result;
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
    if(_db)
        ::sqlite3_interrupt(_db);

    _cancelFlag.store(true);

    {
        Pt::System::MutexLock lock(_mutex);
        while(_pendingTask)
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
    if(_completedTask)
        _completedTask->complete(*this);
    return true;
}


void Connection::onBeginPrepare(const std::string& query)
{
    _prepareTask.sql = query;
    enqueue(&_prepareTask);
}


Pt::Db::Statement Connection::onEndPrepare()
{
    Pt::System::MutexLock lock(_mutex);
    if(_prepareTask.exception)
        std::rethrow_exception(_prepareTask.exception);
    return _prepareTask.result;
}


void Connection::onBeginStartTransaction(Pt::Db::Transaction& txn, const char* sql)
{
    _beginTxnTask.txn = &txn;
    _beginTxnTask.sql = sql ? sql : "BEGIN TRANSACTION";
    enqueue(&_beginTxnTask);
}


void Connection::onEndStartTransaction()
{
    Pt::System::MutexLock lock(_mutex);
    if(_beginTxnTask.exception)
        std::rethrow_exception(_beginTxnTask.exception);
}


void Connection::onBeginCommitTransaction(Pt::Db::Transaction& txn, const char* sql)
{
    clearStatementCache();
    _commitTxnTask.txn = &txn;
    _commitTxnTask.sql = sql ? sql : "COMMIT TRANSACTION";
    enqueue(&_commitTxnTask);
}


void Connection::onEndCommitTransaction()
{
    Pt::System::MutexLock lock(_mutex);
    if(_commitTxnTask.exception)
        std::rethrow_exception(_commitTxnTask.exception);
}


void Connection::onBeginRollbackTransaction(Pt::Db::Transaction& txn, const char* sql)
{
    clearStatementCache();
    _rollbackTxnTask.txn = &txn;
    _rollbackTxnTask.sql = sql ? sql : "ROLLBACK TRANSACTION";
    enqueue(&_rollbackTxnTask);
}


void Connection::onEndRollbackTransaction()
{
    Pt::System::MutexLock lock(_mutex);
    if(_rollbackTxnTask.exception)
        std::rethrow_exception(_rollbackTxnTask.exception);
}

} // namespace sqlite

} // namespace Db

} // namespace Pt