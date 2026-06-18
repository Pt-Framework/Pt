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

#ifndef PT_DB_ICONNECTION_H
#define PT_DB_ICONNECTION_H

#include <Pt/Db/Api.h>
#include <Pt/RefCounted.h>
#include <Pt/SmartPtr.h>
#include <Pt/Signal.h>
#include <Pt/Db/ICursor.h>
#include <Pt/Db/Cursor.h>
#include <Pt/Db/IStatement.h>
#include <Pt/Db/Result.h>
#include <Pt/Db/Row.h>
#include <Pt/Db/Value.h>

#include <string>
#include <map>

namespace Pt {

namespace System {
class EventLoop;
}

namespace Db {

class Result;
class Row;
class Value;
class Statement;
class Transaction;

/** \brief Base class for database connection backends.

    Implements the NVI pattern: the public non-virtual methods manage
    shared state (_isOpen, _state) and delegate to protected virtual
    onXxx() hooks that backends override.

    \see Db::Connection
*/
class PT_DB_API IConnection : public RefCounted
{
    public:
        typedef std::size_t size_type;


        /** \brief Attach to an EventLoop for async operations.

            Pass nullptr to detach.
        */
        void setActive(Pt::System::EventLoop* loop);

        /** \brief Cancel any pending async operation.

            Blocks until the backend has acknowledged cancellation.
            Sets state to Idle. Never throws.
        */
        void cancelOp() noexcept;

        long long insertId();

    public:
        /** \brief Synchronously open the database.
        */
        void open(const std::string& connStr);

        /** \brief Close the database and cancel any pending operation.
        */
        void close();
        /** \brief Begin async close of the database.

            Cancels any pending operation, then closes asynchronously.
            Fires closeFinished() when done.
        */
        void beginClose();

        /** \brief Complete async close. Throws on failure.
        */
        void endClose();

        /** \brief Signal emitted when an async close completes.
        */
        Pt::Signal<>& closeFinished()
        { return _closeFinished; }
        /** \brief Returns true if the database is open.
        */
        bool isOpen() const
        { return _isOpen; }

        /** \brief Begin async open of the database.
        */
        void beginOpen(const std::string& connStr);

        /** \brief Complete async open. Throws on failure.
        */
        void endOpen();

        /** \brief Signal emitted when an async open completes.

            Fires on the EventLoop thread. Call endOpen() to finalize.
        */
        Pt::Signal<>& openFinished()
        { return _openFinished; }

    public:
        void startTransaction(const char* sql = nullptr);

        void commitTransaction(const char* sql = nullptr);

        void rollbackTransaction(const char* sql = nullptr);

        /** \brief Begin async BEGIN TRANSACTION.
        */
        void beginStartTransaction(Transaction& txn, const char* sql);

        void endStartTransaction();

        /** \brief Begin async COMMIT TRANSACTION.
        */
        void beginCommitTransaction(Transaction& txn, const char* sql);

        void endCommitTransaction();

        /** \brief Begin async ROLLBACK TRANSACTION.
        */
        void beginRollbackTransaction(Transaction& txn, const char* sql);

        void endRollbackTransaction();

    public:
        size_type execute(const std::string& query);

        void beginExecute(const std::string& sql);

        size_type endExecute();

        Pt::Signal<>& executeFinished()
        { return _executeFinished; }

        size_type execute(IStatement& stmt);

        void beginExecute(IStatement& stmt);

        size_type endExecute(IStatement& stmt);

    public:
        Statement prepare(const std::string& query);

        virtual Statement prepareCached(const std::string& query) = 0;

        virtual void clearStatementCache() = 0;

        /** \brief Begin async prepare with cache lookup.

            On cache hit fires prepareCachedFinished() via the EventLoop on
            the next iteration. On miss, behaves like beginPrepare() and
            stores the result in the cache when endPrepareCached() is called.
        */
        virtual void beginPrepareCached(const std::string& query) = 0;

        /** \brief Complete async prepareCached. Returns the compiled statement.
        */
        virtual Statement endPrepareCached() = 0;

        /** \brief Signal emitted when an async prepareCached completes.
        */
        Pt::Signal<>& prepareCachedFinished()
        { return _prepareCachedFinished; }

        /** \brief Begin async prepare of a statement.
        */
        void beginPrepare(const std::string& query);

        Statement endPrepare();

        Pt::Signal<>& prepareFinished()
        { return _prepareFinished; }

    public:
        Result select(const std::string& query);

        /** \brief Begin async SELECT query.
        */
        void beginSelect(const std::string& sql);

        Result endSelect();

        Pt::Signal<>& selectFinished()
        { return _selectFinished; }

        Result select(IStatement& stmt);

        Row selectRow(IStatement& stmt);

        Value selectValue(IStatement& stmt);

        void beginSelect(IStatement& stmt);

        Result endSelect(IStatement& stmt);

    public:
        Cursor getCursor(IStatement& stmt, size_type batchSize);

        Result fetchBatch(ICursor& cursor, size_type batchSize);

        void beginBatchFetch(ICursor& cursor, size_type batchSize);

        Result endBatchFetch(ICursor& cursor);

        void closeCursor(ICursor& cursor);

        void closeStatement(IStatement& stmt);

    protected:
        IConnection();

        enum State
        {
            Idle                 = 0,
            PendingOpen          = 1,
            PendingExec          = 2,
            PendingSelect        = 3,
            PendingPrepare       = 4,
            PendingBeginTxn      = 5,
            PendingCommitTxn     = 6,
            PendingRollbackTxn   = 7,
            PendingBatchFetch    = 8,
            PendingClose         = 9
        };

        virtual void onSetActive(Pt::System::EventLoop* loop) = 0;

        virtual void onOpen(const std::string& connStr) = 0;

        virtual void onClose() = 0;

        virtual void onCancelOp() noexcept = 0;

        virtual void onBeginClose() = 0;

        virtual void onEndClose() = 0;

        /** \brief Called by IStmtCacheConnection on cache-hit to trigger post().
        */
        virtual void onNotifyPreparedCached() = 0;

        virtual void onBeginOpen(const std::string& connStr) = 0;

        virtual void onEndOpen() = 0;

        virtual void onBeginExec(const std::string& sql) = 0;

        virtual size_type onEndExec() = 0;

        virtual void onBeginSelect(const std::string& sql) = 0;

        virtual Result onEndSelect() = 0;

        virtual void onBeginPrepare(const std::string& query) = 0;

        virtual Statement onEndPrepare() = 0;

        /** \brief Called by IStmtCacheConnection on cache-miss to trigger async prepare.
            Backend enqueues a prepare task that fires prepareCachedFinished() on completion.
        */
        virtual void onBeginPrepareCachedMiss(const std::string& query) = 0;

        virtual Statement onEndPrepareCachedMiss() = 0;

        virtual void onBeginStartTransaction(Transaction& txn, const char* sql) = 0;

        virtual void onEndStartTransaction() = 0;

        virtual void onBeginCommitTransaction(Transaction& txn, const char* sql) = 0;

        virtual void onEndCommitTransaction() = 0;

        virtual void onBeginRollbackTransaction(Transaction& txn, const char* sql) = 0;

        virtual void onEndRollbackTransaction() = 0;

        // Sync operation hooks — implemented by each backend.
        virtual size_type onExecute(const std::string& query) = 0;

        virtual Result onSelect(const std::string& query) = 0;

        virtual Statement onPrepare(const std::string& query) = 0;

        virtual long long onInsertId() = 0;

        virtual void onStartTransaction(const char* sql) = 0;

        virtual void onCommitTransaction(const char* sql) = 0;

        virtual void onRollbackTransaction(const char* sql) = 0;

        Pt::Signal<>            _openFinished;
        Pt::Signal<>            _closeFinished;
        Pt::Signal<>            _executeFinished;
        Pt::Signal<>            _selectFinished;
        Pt::Signal<>            _prepareFinished;
        Pt::Signal<>            _prepareCachedFinished;
        bool                    _prepareCachedHit;
        bool                    _isOpen;
        State                   _state;
        Pt::System::EventLoop*  _loop;
        void*                   _pendingOp;
};


/** \brief Base class for backends that need a statement cache.

    Provides default implementations of prepareCached() and
    clearStatementCache(). Backends inherit this instead of IConnection
    directly.

    \see Db::Connection
*/
class PT_DB_API IStmtCacheConnection : public IConnection
{
    public:
        virtual Statement prepareCached(const std::string& query);

        virtual void clearStatementCache();

        virtual void beginPrepareCached(const std::string& query);

        virtual Statement endPrepareCached();

    private:
        typedef SmartPtr<IStatement, InternalRefCounted<IStatement> > StatementPtr;
        typedef std::map<std::string, StatementPtr> StatementCache;
        StatementCache _stmtCache;
        IStatement*    _cachedHitStmt = nullptr;
        std::string    _pendingPrepareCachedQuery;
};

} // namespace Db

} // namespace Pt

#endif // PT_DB_ICONNECTION_H

