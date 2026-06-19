/*
  Copyright (C) 2006 by Tommi Maekitalo
  Copyright (C) 2006-2026 by Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include <Pt/Db/IConnection.h>
#include <Pt/Db/DbError.h>
#include <Pt/Db/Result.h>
#include <Pt/Db/Row.h>
#include <Pt/Db/Value.h>
#include <Pt/Db/Statement.h>
#include <Pt/Db/Transaction.h>

namespace Pt {

namespace Db {

///////////////////////////////////////////////////////////////////////
// IConnection
///////////////////////////////////////////////////////////////////////

IConnection::IConnection()
: _isOpen(false)
, _inTransaction(false)
, _loop(nullptr)
, _pendingOp(nullptr)
, _prepareCachedHit(false)
{
}


void IConnection::setActive(Pt::System::EventLoop* loop)
{
    _loop = loop;
    onSetActive(loop);
}


void IConnection::open(const std::string& connStr)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");
    onOpen(connStr);
    _isOpen = true;
}


void IConnection::close()
{
    if(_pendingOp)
    {
        _pendingOp = nullptr;
        onCancelOp();
    }
    _inTransaction = false;
    _isOpen = false;
    onClose();
}


void IConnection::beginClose()
{
    // Cancel any pending operation first (sync, deterministic)
    if(_pendingOp)
    {
        _pendingOp = nullptr;
        onCancelOp();
    }
    _pendingOp = this;
    _isOpen = false;
    onBeginClose();
}


void IConnection::endClose()
{
    _pendingOp = nullptr;
    onEndClose();
}


void IConnection::cancelOp() noexcept
{
    if(_pendingOp == nullptr)
        return;
    _pendingOp = nullptr;
    onCancelOp();
}


void IConnection::cancelConnection() noexcept
{
    if(_pendingOp != this)
        return;
    _pendingOp = nullptr;
    onCancelOp();
}


void IConnection::beginOpen(const std::string& connStr)
{
    if(_pendingOp)
        throw ConnectionError("Operation pending");
    if(_isOpen)
        throw ConnectionError("Already open");
    _pendingOp = this;
    onBeginOpen(connStr);
}


void IConnection::endOpen()
{
    _pendingOp = nullptr;
    onEndOpen();
    _isOpen = true;
}


void IConnection::beginSelect(const std::string& sql)
{
    if(_pendingOp)
        throw ConnectionError("Operation pending");
    _pendingOp = this;
    onBeginSelect(sql);
}


Result IConnection::endSelect()
{
    _pendingOp = nullptr;
    return onEndSelect();
}


void IConnection::beginPrepare(const std::string& query)
{
    if(_pendingOp)
        throw ConnectionError("Operation pending");
    _pendingOp = this;
    onBeginPrepare(query);
}


Statement IConnection::endPrepare()
{
    _pendingOp = nullptr;
    return onEndPrepare();
}


void IConnection::beginStartTransaction(Transaction& txn, const char* sql)
{
    if(_pendingOp)
        throw ConnectionError("Operation pending");
    _pendingOp = &txn;
    onBeginStartTransaction(txn, sql);
}


void IConnection::endStartTransaction()
{
    _pendingOp = nullptr;
    onEndStartTransaction();
    _inTransaction = true;
}


void IConnection::beginCommitTransaction(Transaction& txn, const char* sql)
{
    if(_pendingOp)
        throw ConnectionError("Operation pending");
    _pendingOp = &txn;
    onBeginCommitTransaction(txn, sql);
}


void IConnection::endCommitTransaction()
{
    _pendingOp = nullptr;
    onEndCommitTransaction();
    _inTransaction = false;
}


void IConnection::beginRollbackTransaction(Transaction& txn, const char* sql)
{
    if(_pendingOp)
        throw ConnectionError("Operation pending");
    _pendingOp = &txn;
    onBeginRollbackTransaction(txn, sql);
}


void IConnection::endRollbackTransaction()
{
    _pendingOp = nullptr;
    onEndRollbackTransaction();
    _inTransaction = false;
}


Result IConnection::select(const std::string& query)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");
    return onSelect(query);
}


Statement IConnection::prepare(const std::string& query)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");
    return onPrepare(query);
}


bool IConnection::ping()
{
    if(_pendingOp != nullptr)
        throw InvalidConnection("Operation pending");
    return onPing();
}


void IConnection::beginPing()
{
    if(_pendingOp != nullptr)
        throw InvalidConnection("Operation pending");
    _pendingOp = this;
    onBeginPing();
}


bool IConnection::endPing()
{
    _pendingOp = nullptr;
    return onEndPing();
}


long long IConnection::lastInsertId(const std::string& name)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");
    return onLastInsertId(name);
}


void IConnection::startTransaction(const char* sql)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");
    onStartTransaction(sql);
    _inTransaction = true;
}


void IConnection::commitTransaction(const char* sql)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");
    onCommitTransaction(sql);
    _inTransaction = false;
}


void IConnection::rollbackTransaction(const char* sql)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");
    onRollbackTransaction(sql);
    _inTransaction = false;
}


Result IConnection::select(IStatement& stmt)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");
    return stmt.onSelect();
}


Row IConnection::selectRow(IStatement& stmt)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");
    return stmt.onSelectRow();
}


Value IConnection::selectValue(IStatement& stmt)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");
    return stmt.onSelectValue();
}


Result IConnection::fetchBatch(ICursor& cursor, size_type batchSize)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");
    return cursor.onFetchBatch(batchSize);
}


Cursor IConnection::getCursor(IStatement& stmt, size_type batchSize)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");
    return Cursor(stmt.onCreateCursor(), batchSize);
}

//
// exceute SQL
//

IConnection::size_type IConnection::execute(const std::string& query)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");

    return onExecute(query);
}


void IConnection::beginExecute(const std::string& sql)
{
    if(_pendingOp)
        throw ConnectionError("Operation pending");

    _pendingOp = this;
    onBeginExec(sql);
}


IConnection::size_type IConnection::endExecute()
{
    _pendingOp = nullptr;
    return onEndExec();
}


IConnection::size_type IConnection::execute(IStatement& stmt)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");

    return stmt.onExecute();
}


void IConnection::beginExecute(IStatement& stmt)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");

    _pendingOp = &stmt;
    stmt.onBeginExec();
}


IConnection::size_type IConnection::endExecute(IStatement& stmt)
{
    _pendingOp = nullptr;
    return stmt.onEndExec();
}

//
// select SQL
//

void IConnection::beginSelect(IStatement& stmt)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");
    _pendingOp = &stmt;
    stmt.onBeginSelect();
}


Result IConnection::endSelect(IStatement& stmt)
{
    _pendingOp = nullptr;
    return stmt.onEndSelect();
}


void IConnection::beginBatchFetch(ICursor& cursor, size_type batchSize)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");
    _pendingOp = &cursor;
    cursor.onBeginBatchFetch(batchSize);
}


Result IConnection::endBatchFetch(ICursor& cursor)
{
    _pendingOp = nullptr;
    return cursor.onEndBatchFetch();
}


void IConnection::cancelCursor(ICursor& cursor)
{
    if(_pendingOp == &cursor)
    {
        _pendingOp = nullptr;
        onCancelOp();
    }
    cursor.onClose();
}


void IConnection::cancelStatement(IStatement& stmt)
{
    if(_pendingOp == &stmt)
    {
        _pendingOp = nullptr;
        onCancelOp();
    }
}


///////////////////////////////////////////////////////////////////////
// IStmtCacheConnection
///////////////////////////////////////////////////////////////////////

Statement IStmtCacheConnection::prepareCached(const std::string& query)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");

    StatementCache::iterator it = _stmtCache.find(query);

    if(it == _stmtCache.end())
    {
        Statement stmt = prepare(query);
        IStatement* istmt = stmt.impl();
        _stmtCache.insert( StatementCache::value_type(query, StatementPtr(istmt)) );
        return stmt;
    }

    return Statement( it->second.get() );
}


void IStmtCacheConnection::clearStatementCache()
{
    _stmtCache.clear();
}


void IStmtCacheConnection::beginPrepareCached(const std::string& query)
{
    if(_pendingOp)
        throw InvalidConnection("Operation pending");

    StatementCache::iterator it = _stmtCache.find(query);

    if(it != _stmtCache.end())
    {
        // Cache hit: store the raw pointer and signal via post() on next EventLoop tick
        _cachedHitStmt = it->second.get();
        _prepareCachedHit = true;
        _pendingOp = this;
        onNotifyPreparedCached();
        return;
    }

    // Cache miss: delegate to dedicated async prepare-cached path
    _cachedHitStmt = nullptr;
    _prepareCachedHit = false;
    _pendingOp = this;
    onBeginPrepareCachedMiss(query);
    _pendingPrepareCachedQuery = query;
}


Statement IStmtCacheConnection::endPrepareCached()
{
    if(_pendingOp == nullptr)
        throw InvalidConnection("No prepare-cached operation pending");

    _pendingOp = nullptr;

    if(_prepareCachedHit)
    {
        _prepareCachedHit = false;
        IStatement* raw = _cachedHitStmt;
        _cachedHitStmt = nullptr;
        return Statement(raw);
    }

    // Cache miss path: get the result from the async prepare
    Statement stmt = onEndPrepareCachedMiss();

    if( ! _pendingPrepareCachedQuery.empty() )
    {
        IStatement* istmt = stmt.impl();
        _stmtCache.insert( StatementCache::value_type(_pendingPrepareCachedQuery, StatementPtr(istmt)) );
        _pendingPrepareCachedQuery.clear();
    }

    return stmt;
}

} // namespace Db

} // namespace Pt
