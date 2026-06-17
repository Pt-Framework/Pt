/*
  Copyright (C) 2006 by Tommi Maekitalo
  Copyright (C) 2006 by Marc Boris Duerner

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

#ifndef PT_DB_CONNECTION_H
#define PT_DB_CONNECTION_H

#include <Pt/Db/Api.h>
#include <Pt/SmartPtr.h>
#include <Pt/Db/IConnection.h>
#include <Pt/Db/Statement.h>
#include <Pt/Db/Result.h>
#include <Pt/Db/Row.h>
#include <Pt/Db/Value.h>
#include <Pt/Signal.h>
#include <string>

#if __cplusplus >= 202002L
#include <coroutine>
#endif

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

#if __cplusplus >= 202002L
class AsyncOpen;
#endif

/** \brief Smart-pointer wrapper around a database connection backend.

    Construct with a driver name; the backend is allocated via the
    ConnectionManager and stays valid for the lifetime of the object.
    Call open() or beginOpen() to establish the actual connection.

    Without an EventLoop all operations are synchronous.
    Call setActive() with an EventLoop to enable async mode, then use
    beginExec() / beginSelect() etc. and connect to openFinished() for results.

    @ingroup Pt-Db
*/
class PT_DB_API Connection
{
    public:
        typedef std::size_t size_type;

        /** \brief Allocate a backend for the named driver.

            The connection is not yet open. Call open() or beginOpen() to
            establish the database connection.

            \param driver Registered driver name (e.g. "sqlite").
        */
        explicit Connection(const std::string& driver);

        /** \brief Construct from an existing backend (takes ownership).

            Intended for unit tests and custom backends.
        */
        explicit Connection(IConnection* conn);

        /** \brief Cancels any pending async operation and destroys the object.
        */
        ~Connection();

        /** \brief Attach to an EventLoop for async operations.
        */
        void setActive(Pt::System::EventLoop& loop);

        /** \brief Cancel any pending async operation.
        */
        void cancel();

        /** \brief Return the last auto-increment row id.
        */
        long long insertId();

    public:
        /** \brief Returns true if the database is open.
        */
        bool isOpen() const;

        /** \brief Returns true if the database is not open.
        */
        bool operator!() const;

        /** \brief Synchronously open the database.

            \param connStr Driver-specific connection string (no driver prefix).
        */
        void open(const std::string& connStr);

        /** \brief Close the database connection.
        */
        void close();

        /** \brief Begin async open of the database.

            \param connStr Driver-specific connection string (no driver prefix).
        */
        void beginOpen(const std::string& connStr);

        /** \brief Complete async open. Throws on failure.
        */
        void endOpen();

        /** \brief Signal emitted when an async operation completes.

            Connect to this signal and call the corresponding endXxx()
            in the handler to retrieve results.
        */
        Pt::Signal<>& openFinished();

    public:
        /** \brief Execute a DML/DDL statement synchronously.

            \return Number of rows affected.
        */
        size_type execute(const std::string& query);

        /** \brief Begin async execution statement.
        */
        void beginExec(const std::string& sql);

        /** \brief Begin async execution statement.
        */
        size_type endExec();

        /** \brief Signal emitted when an async raw exec completes.
        */
        Pt::Signal<>& executeFinished();

    public:
        /** \brief Execute a SELECT query synchronously.
        */
        Result select(const std::string& query);

        /** \brief Begin async SELECT for a SQL string.
        */
        void beginSelect(const std::string& sql);

        /** \brief Ends async SELECT and returns result.
        */
        Result endSelect();

        /** \brief Signal emitted when an async raw select completes.
        */
        Pt::Signal<>& selectFinished();

    public:
        /** \brief Compile a prepared statement.
        */
        Statement prepare(const std::string& query);

        /** \brief Compile and cache a prepared statement.
        */
        Statement prepareCached(const std::string& query);

        /** \brief Clear the statement cache.
        */
        void clearStatementCache();

    public:
        /** \brief Compile a prepared statement asynchronously.
        */
        void beginPrepare(const std::string& query);

        /** \brief Complete async prepare. Returns the compiled statement.
        */
        Statement endPrepare();

        /** \brief Signal emitted when an async prepare completes.
        */
        Pt::Signal<>& prepareFinished();

    private:
        friend class Transaction;

        void startTransaction(const char* sql = nullptr);

        void commitTransaction(const char* sql = nullptr);

        void rollbackTransaction(const char* sql = nullptr);

        void beginStartTransaction(Transaction& txn, const char* sql);

        void endStartTransaction();

        void beginCommitTransaction(Transaction& txn, const char* sql);

        void endCommitTransaction();

        void beginRollbackTransaction(Transaction& txn, const char* sql);

        void endRollbackTransaction();

#if __cplusplus >= 202002L
    public:
        /** \brief Asynchronously open the database as a C++20 awaitable.

            Returns an awaitable that can be used with co_await.
            Wraps beginOpen() / endOpen() / openFinished().

            \param connStr Driver-specific connection string (no driver prefix).
        */
        AsyncOpen openAsync(const std::string& connStr);
#endif

    public:
        /** \brief Returns the underlying backend implementation.
        */
        IConnection* impl();

        /** \brief Returns the underlying backend implementation (const).
        */
        const IConnection* impl() const;

    private:
        Connection(const Connection&) = delete;

        Connection& operator=(const Connection&) = delete;

        typedef SmartPtr<IConnection,
                         InternalRefCounted<IConnection> > ConnectionImplPtr;

        ConnectionImplPtr _connection;
};

#if __cplusplus >= 202002L

/** @brief Awaitable for async open of a database connection.

    Wraps the beginOpen() / endOpen() / openFinished() async pattern
    into a C++20 awaitable for use with co_await.

    @ingroup Pt-Db
*/
class AsyncOpen : public Connectable
{
    public:
        /** @brief Construct an awaitable for opening a connection.

            \param conn The database connection to open.
            \param connStr Driver-specific connection string.
        */
        AsyncOpen(Connection& conn, const std::string& connStr)
        : _conn(conn)
        , _connStr(connStr)
        {}

        bool await_ready() const
        { return false; }

        void await_suspend(std::coroutine_handle<> h)
        {
            _handle = h;
            _conn.openFinished() += slot(*this, &AsyncOpen::onReady);
            _conn.beginOpen(_connStr);
        }

        void await_resume()
        { _conn.endOpen(); }

    private:
        void onReady()
        { _handle.resume(); }

        Connection& _conn;
        const std::string& _connStr;
        std::coroutine_handle<> _handle;
};

#endif // __cplusplus >= 202002L

} // namespace Db

} // namespace Pt

#endif // PT_DB_CONNECTION_H
