/* Copyright (C) 2006-2026 by Tommi Maekitalo
   Copyright (C) 2006-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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

/** @brief Database connection backend.

    Public non-virtual methods own open state and the pending
    operation, and call the protected %onXxx() hooks a backend
    overrides.
*/
class PT_DB_API IConnection : public RefCounted
{
    public:
        typedef std::size_t size_type;


        /** @brief Attaches @a loop for asynchronous operations, or 0 to detach.
        */
        void setActive(Pt::System::EventLoop* loop);

        /** @brief Returns true if the backend still answers.
        */
        bool ping();

        /** @brief Starts an asynchronous ping.
        */
        void beginPing();

        /** @brief Completes an asynchronous ping.

            @return True if the backend still answers.
        */
        bool endPing();

        /** @brief Signal emitted when an asynchronous ping completes.
        */
        Pt::Signal<>& pingFinished()
        { return _pingFinished; }

        /** @brief Returns the last generated row id.

            Pass a sequence name on backends that use named sequences,
            or an empty string otherwise.
        */
        long long lastInsertId(const std::string& name);

        /** @brief Returns true if no asynchronous operation is pending.
        */
        bool isIdle() const
        { return _pendingOp == nullptr; }

        /** @brief Returns true if a transaction is active.
        */
        bool hasTransaction() const
        { return _inTransaction; }

    public:
        /** @brief Opens the database with @a connStr.
        */
        void open(const std::string& connStr);

        /** @brief Closes the database and cancels any pending operation.
        */
        void close();

        /** @brief Starts an asynchronous close.
        */
        void beginClose();

        /** @brief Completes an asynchronous close.
        */
        void endClose();

        /** @brief Signal emitted when an asynchronous close completes.
        */
        Pt::Signal<>& closeFinished()
        { return _closeFinished; }

        /** @brief Returns true if the database is open.
        */
        bool isOpen() const
        { return _isOpen; }

        /** @brief Starts an asynchronous open with @a connStr.
        */
        void beginOpen(const std::string& connStr);

        /** @brief Completes an asynchronous open.
        */
        void endOpen();

        /** @brief Signal emitted when an asynchronous open completes.
        */
        Pt::Signal<>& openFinished()
        { return _openFinished; }

    public:
        /** @brief Begins a transaction with optional @a sql.
        */
        void startTransaction(const char* sql = nullptr);

        /** @brief Commits the active transaction with optional @a sql.
        */
        void commitTransaction(const char* sql = nullptr);

        /** @brief Rolls back the active transaction with optional @a sql.
        */
        void rollbackTransaction(const char* sql = nullptr);

        /** @brief Starts an asynchronous begin-transaction.
        */
        void beginStartTransaction(Transaction& txn, const char* sql);

        /** @brief Completes an asynchronous begin-transaction.
        */
        void endStartTransaction();

        /** @brief Starts an asynchronous commit.
        */
        void beginCommitTransaction(Transaction& txn, const char* sql);

        /** @brief Completes an asynchronous commit.
        */
        void endCommitTransaction();

        /** @brief Starts an asynchronous rollback.
        */
        void beginRollbackTransaction(Transaction& txn, const char* sql);

        /** @brief Completes an asynchronous rollback.
        */
        void endRollbackTransaction();

    public:
        /** @brief Executes @a query and returns the number of rows changed.
        */
        size_type execute(const std::string& query);

        /** @brief Starts asynchronous execution of @a sql.
        */
        void beginExecute(const std::string& sql);

        /** @brief Completes asynchronous execution.
        */
        size_type endExecute();

        /** @brief Signal emitted when asynchronous execution completes.
        */
        Pt::Signal<>& executeFinished()
        { return _executeFinished; }

        /** @brief Executes @a stmt and returns the number of rows changed.
        */
        size_type execute(IStatement& stmt);

        /** @brief Starts asynchronous execution of @a stmt.
        */
        void beginExecute(IStatement& stmt);

        /** @brief Completes asynchronous execution of @a stmt.
        */
        size_type endExecute(IStatement& stmt);

    public:
        /** @brief Compiles @a query into a prepared statement.
        */
        Statement prepare(const std::string& query);

        /** @brief Compiles @a query and caches the prepared statement.
        */
        virtual Statement prepareCached(const std::string& query) = 0;

        /** @brief Clears the prepared-statement cache.
        */
        virtual void clearStatementCache() = 0;

        /** @brief Starts asynchronous prepare with cache lookup.
        */
        virtual void beginPrepareCached(const std::string& query) = 0;

        /** @brief Completes asynchronous prepareCached.
        */
        virtual Statement endPrepareCached() = 0;

        /** @brief Signal emitted when asynchronous prepareCached completes.
        */
        Pt::Signal<>& prepareCachedFinished()
        { return _prepareCachedFinished; }

        /** @brief Starts asynchronous compile of @a query.
        */
        void beginPrepare(const std::string& query);

        /** @brief Completes asynchronous compile.
        */
        Statement endPrepare();

        /** @brief Signal emitted when asynchronous prepare completes.
        */
        Pt::Signal<>& prepareFinished()
        { return _prepareFinished; }

    public:
        /** @brief Executes @a query and returns a buffered result.
        */
        Result select(const std::string& query);

        /** @brief Starts an asynchronous select of @a sql.
        */
        void beginSelect(const std::string& sql);

        /** @brief Completes an asynchronous select.
        */
        Result endSelect();

        /** @brief Signal emitted when an asynchronous select completes.
        */
        Pt::Signal<>& selectFinished()
        { return _selectFinished; }

        /** @brief Executes @a stmt and returns a buffered result.
        */
        Result select(IStatement& stmt);

        /** @brief Executes @a stmt and returns the first row.
        */
        Row selectRow(IStatement& stmt);

        /** @brief Executes @a stmt and returns the first column of the first row.
        */
        Value selectValue(IStatement& stmt);

        /** @brief Starts an asynchronous select of @a stmt.
        */
        void beginSelect(IStatement& stmt);

        /** @brief Completes an asynchronous select of @a stmt.
        */
        Result endSelect(IStatement& stmt);

    public:
        /** @brief Opens a batch cursor on @a stmt.
        */
        Cursor getCursor(IStatement& stmt, size_type batchSize);

        /** @brief Fetches the next batch from @a cursor.
        */
        Result fetchBatch(ICursor& cursor, size_type batchSize);

        /** @brief Starts an asynchronous batch fetch from @a cursor.
        */
        void beginBatchFetch(ICursor& cursor, size_type batchSize);

        /** @brief Completes an asynchronous batch fetch from @a cursor.
        */
        Result endBatchFetch(ICursor& cursor);

        /** @brief Cancels a pending fetch on @a cursor.
        */
        void cancelCursor(ICursor& cursor);

        /** @brief Cancels a pending operation on @a stmt.
        */
        void cancelStatement(IStatement& stmt);

    protected:
        IConnection();

        /** @brief Cancels any pending asynchronous operation. Never throws.
        */
        void cancelPending() noexcept;

        /** @brief Cancels a pending connection-level asynchronous operation.
        */
        void cancelConnection() noexcept;

        virtual void onSetActive(Pt::System::EventLoop* loop) = 0;

        virtual void onOpen(const std::string& connStr) = 0;

        virtual void onClose() = 0;

        virtual void onCancelPending() noexcept = 0;

        virtual void onBeginClose() = 0;

        virtual void onEndClose() = 0;

        virtual void onNotifyPreparedCached() = 0;

        virtual void onBeginOpen(const std::string& connStr) = 0;

        virtual void onEndOpen() = 0;

        virtual void onBeginExec(const std::string& sql) = 0;

        virtual size_type onEndExec() = 0;

        virtual void onBeginSelect(const std::string& sql) = 0;

        virtual Result onEndSelect() = 0;

        virtual void onBeginPrepare(const std::string& query) = 0;

        virtual Statement onEndPrepare() = 0;

        virtual void onBeginPrepareCachedMiss(const std::string& query) = 0;

        virtual Statement onEndPrepareCachedMiss() = 0;

        virtual void onBeginStartTransaction(Transaction& txn, const char* sql) = 0;

        virtual void onEndStartTransaction() = 0;

        virtual void onBeginCommitTransaction(Transaction& txn, const char* sql) = 0;

        virtual void onEndCommitTransaction() = 0;

        virtual void onBeginRollbackTransaction(Transaction& txn, const char* sql) = 0;

        virtual void onEndRollbackTransaction() = 0;

        virtual size_type onExecute(const std::string& query) = 0;

        virtual Result onSelect(const std::string& query) = 0;

        virtual Statement onPrepare(const std::string& query) = 0;

        virtual bool onPing() = 0;

        virtual void onBeginPing() = 0;

        virtual bool onEndPing() = 0;

        virtual long long onLastInsertId(const std::string& name) = 0;

        virtual void onStartTransaction(const char* sql) = 0;

        virtual void onCommitTransaction(const char* sql) = 0;

        virtual void onRollbackTransaction(const char* sql) = 0;

        Pt::Signal<>            _openFinished;
        Pt::Signal<>            _closeFinished;
        Pt::Signal<>            _executeFinished;
        Pt::Signal<>            _selectFinished;
        Pt::Signal<>            _prepareFinished;
        Pt::Signal<>            _prepareCachedFinished;
        Pt::Signal<>            _pingFinished;
        bool                    _prepareCachedHit;
        bool                    _isOpen;
        bool                    _inTransaction;
        Pt::System::EventLoop*  _loop;
        void*                   _pendingOp;

        friend class Connection;
};


/** @brief Connection backend with a prepared-statement cache.
*/
class PT_DB_API IStmtCacheConnection : public IConnection
{
    public:
        /** @brief Compiles @a query and caches the prepared statement.
        */
        virtual Statement prepareCached(const std::string& query);

        /** @brief Clears the prepared-statement cache.
        */
        virtual void clearStatementCache();

        /** @brief Starts asynchronous prepare with cache lookup.
        */
        virtual void beginPrepareCached(const std::string& query);

        /** @brief Completes asynchronous prepareCached.
        */
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
