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
#include <Pt/Db/Statement.h>
#include <Pt/Db/Transaction.h>

namespace Pt {

namespace Db {

///////////////////////////////////////////////////////////////////////
// IConnection
///////////////////////////////////////////////////////////////////////

IConnection::IConnection()
: _isOpen(false)
, _state(Idle)
, _loop(nullptr)
{
}


Pt::Signal<>& IConnection::openFinished()
{
    return onOpenFinished();
}


Pt::Signal<>& IConnection::executeFinished()
{
    return onExecuteFinished();
}


Pt::Signal<>& IConnection::selectFinished()
{
    return onSelectFinished();
}


Pt::Signal<>& IConnection::prepareFinished()
{
    return onPrepareFinished();
}


void IConnection::setActive(Pt::System::EventLoop* loop)
{
    _loop = loop;
    onSetActive(loop);
}


void IConnection::open(const std::string& connStr)
{
    onOpen(connStr);
    _isOpen = true;
}


void IConnection::close()
{
    if(_state != Idle)
    {
        _state = Idle;
        onCancelOp();
    }
    _isOpen = false;
    onClose();
}


void IConnection::cancelOp()
{
    if(_state == Idle)
        return;
    _state = Idle;
    onCancelOp();
}


void IConnection::beginOpen(const std::string& connStr)
{
    if(_state != Idle)
        throw InvalidConnection("Operation pending");
    if(_isOpen)
        throw InvalidConnection("Already open");
    _state = PendingOpen;
    onBeginOpen(connStr);
}


void IConnection::endOpen()
{
    _state = Idle;
    onEndOpen();
    _isOpen = true;
}


void IConnection::beginExec(const std::string& sql)
{
    if(_state != Idle)
        throw InvalidConnection("Operation pending");
    _state = PendingExec;
    onBeginExec(sql);
}


IConnection::size_type IConnection::endExec()
{
    _state = Idle;
    return onEndExec();
}


void IConnection::beginSelect(const std::string& sql)
{
    if(_state != Idle)
        throw InvalidConnection("Operation pending");
    _state = PendingSelect;
    onBeginSelect(sql);
}


Result IConnection::endSelect()
{
    _state = Idle;
    return onEndSelect();
}


void IConnection::beginPrepare(const std::string& query)
{
    if(_state != Idle)
        throw InvalidConnection("Operation pending");
    _state = PendingPrepare;
    onBeginPrepare(query);
}


Statement IConnection::endPrepare()
{
    _state = Idle;
    return onEndPrepare();
}


void IConnection::beginStartTransaction(Transaction& txn, const char* sql)
{
    if(_state != Idle)
        throw InvalidConnection("Operation pending");
    _state = PendingBeginTxn;
    onBeginStartTransaction(txn, sql);
}


void IConnection::endStartTransaction()
{
    _state = Idle;
    onEndStartTransaction();
}


void IConnection::beginCommitTransaction(Transaction& txn, const char* sql)
{
    if(_state != Idle)
        throw InvalidConnection("Operation pending");
    _state = PendingCommitTxn;
    onBeginCommitTransaction(txn, sql);
}


void IConnection::endCommitTransaction()
{
    _state = Idle;
    onEndCommitTransaction();
}


void IConnection::beginRollbackTransaction(Transaction& txn, const char* sql)
{
    if(_state != Idle)
        throw InvalidConnection("Operation pending");
    _state = PendingRollbackTxn;
    onBeginRollbackTransaction(txn, sql);
}


void IConnection::endRollbackTransaction()
{
    _state = Idle;
    onEndRollbackTransaction();
}


///////////////////////////////////////////////////////////////////////
// IStmtCacheConnection
///////////////////////////////////////////////////////////////////////

Statement IStmtCacheConnection::prepareCached(const std::string& query)
{
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

} // namespace Db

} // namespace Pt
