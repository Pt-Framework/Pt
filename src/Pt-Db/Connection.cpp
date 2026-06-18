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

#include "ConnectionManager.h"

#include <Pt/Db/Connection.h>
#include <Pt/Db/IConnector.h>
#include <Pt/Db/DbError.h>
#include <Pt/Db/Transaction.h>

namespace Pt {

namespace Db {

Connection::Connection(const std::string& driver)
{
    IConnector* connector = ConnectionManager::instance().findConnector(driver);
    if( ! connector )
        throw InvalidConnection("No DB driver: " + driver);
    _connection = ConnectionImplPtr( connector->get() );
}


Connection::Connection(IConnection* conn)
: _connection(conn)
{
}


Connection::~Connection()
{
}


bool Connection::isOpen() const
{
    return _connection->isOpen();
}


bool Connection::operator!() const
{
    return ! _connection->isOpen();
}


void Connection::setActive(Pt::System::EventLoop& loop)
{
    _connection->setActive(&loop);
}


void Connection::cancel()
{
    _connection->cancelOp();
}


void Connection::close()
{
    _connection->close();
}


void Connection::beginClose()
{
    _connection->beginClose();
}


void Connection::endClose()
{
    _connection->endClose();
}


Pt::Signal<>& Connection::closeFinished()
{
    return _connection->closeFinished();
}


void Connection::open(const std::string& connStr)
{
    _connection->open(connStr);
}


void Connection::beginOpen(const std::string& connStr)
{
    _connection->beginOpen(connStr);
}


void Connection::endOpen()
{
    _connection->endOpen();
}


void Connection::startTransaction(const char* sql)
{
    _connection->startTransaction(sql);
}


void Connection::commitTransaction(const char* sql)
{
    _connection->commitTransaction(sql);
}


void Connection::rollbackTransaction(const char* sql)
{
    _connection->rollbackTransaction(sql);
}


Connection::size_type Connection::execute(const std::string& query)
{
    return _connection->execute(query);
}


Result Connection::select(const std::string& query)
{
    return _connection->select(query);
}


Statement Connection::prepare(const std::string& query)
{
    return _connection->prepare(query);
}


Statement Connection::prepareCached(const std::string& query)
{
    return _connection->prepareCached(query);
}


void Connection::clearStatementCache()
{
    _connection->clearStatementCache();
}


void Connection::beginPrepareCached(const std::string& query)
{
    _connection->beginPrepareCached(query);
}


Statement Connection::endPrepareCached()
{
    return _connection->endPrepareCached();
}


Pt::Signal<>& Connection::prepareCachedFinished()
{
    return _connection->prepareCachedFinished();
}


long long Connection::insertId()
{
    return _connection->insertId();
}


void Connection::beginExecute(const std::string& sql)
{
    _connection->beginExecute(sql);
}


Connection::size_type Connection::endExecute()
{
    return _connection->endExecute();
}


void Connection::beginSelect(const std::string& sql)
{
    _connection->beginSelect(sql);
}


Result Connection::endSelect()
{
    return _connection->endSelect();
}


Pt::Signal<>& Connection::openFinished()
{
    return _connection->openFinished();
}


Pt::Signal<>& Connection::executeFinished()
{
    return _connection->executeFinished();
}


Pt::Signal<>& Connection::selectFinished()
{
    return _connection->selectFinished();
}


void Connection::beginPrepare(const std::string& query)
{
    _connection->beginPrepare(query);
}


Statement Connection::endPrepare()
{
    return _connection->endPrepare();
}


Pt::Signal<>& Connection::prepareFinished()
{
    return _connection->prepareFinished();
}


void Connection::beginStartTransaction(Transaction& txn, const char* sql)
{
    _connection->beginStartTransaction(txn, sql);
}


void Connection::endStartTransaction()
{
    _connection->endStartTransaction();
}


void Connection::beginCommitTransaction(Transaction& txn, const char* sql)
{
    _connection->beginCommitTransaction(txn, sql);
}


void Connection::endCommitTransaction()
{
    _connection->endCommitTransaction();
}


void Connection::beginRollbackTransaction(Transaction& txn, const char* sql)
{
    _connection->beginRollbackTransaction(txn, sql);
}


void Connection::endRollbackTransaction()
{
    _connection->endRollbackTransaction();
}


IConnection* Connection::impl()
{
    return _connection.get();
}


const IConnection* Connection::impl() const
{
    return _connection.get();
}


#if __cplusplus >= 202002L

AsyncOpen Connection::openAsync(const std::string& connStr)
{
    return AsyncOpen(*this, connStr);
}

#endif // __cplusplus >= 202002L

} // namespace Db

} // namespace Pt
