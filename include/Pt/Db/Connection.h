/* Copyright (C) 2006-2026 by Tommi Maekitalo
   Copyright (C) 2006-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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
#include <Pt/Slot.h>
#include <Pt/Coroutine.h>
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
class AsyncClose;
class AsyncExecute;
class AsyncSelect;
class AsyncPing;
class ConnectionAwaiter;
#endif

/** @brief Database session for a registered driver.

    %Connection is the database handle the group described: a driver
    name allocates a backend, and %open() establishes the session.
    The object is not copyable. Destroying it cancels a pending async
    operation and releases the backend.

    The driver constructor takes a registered name such as "sqlite".
    The connection is not open after that call. The %IConnection
    constructor takes ownership of an existing backend and is meant
    for tests and custom backends.

    %open() and %close() are the synchronous session. The connection
    string is driver-specific and has no driver prefix. %isOpen()
    reports the session. %operator!() is true when the session is not
    open. %ping() tests whether the backend still answers.
    %lastInsertId() is the generated row id of the last insert; pass
    a sequence name on backends that use named sequences, or an empty
    string otherwise.

    %execute() runs SQL that does not return rows. %select() runs a
    query and returns a buffered %Result. %prepare() compiles a
    %Statement. %prepareCached() compiles and caches by SQL text.
    %clearStatementCache() drops that cache.

    %setActive() attaches an %EventLoop for asynchronous work. The
    loop does not own the connection. Each async operation is a
    begin/end pair and a finished signal; the slot calls the matching
    end method. %isIdle() is true when none is pending. %cancel()
    aborts a pending operation. Starting another while one is pending
    throws %InvalidConnection or %ConnectionError.

    When C++20 is available, %openAsync(), %closeAsync(),
    %executeAsync(), %selectAsync() and %pingAsync() wrap those pairs
    for co_await.

    %hasTransaction() is true while a %Transaction is active. Begin,
    commit and rollback are operations of that type.

    @code
    Pt::Db::Connection conn("sqlite");
    conn.open("file:app.db");
    conn.execute("CREATE TABLE IF NOT EXISTS t(id INTEGER)");
    Pt::Db::Result r = conn.select("SELECT id FROM t");
    conn.close();
    @endcode

    @ingroup Pt-Db-Connections
*/
class PT_DB_API Connection
{
    public:
        typedef std::size_t size_type;

        /** @brief Creates a connection for @a driver.

            Call %open() to establish a session.

            @throw %InvalidConnection if @a driver is not registered.
        */
        explicit Connection(const std::string& driver);

        /** @brief Takes ownership of the backend @a conn.
        */
        explicit Connection(IConnection* conn);

        /** @brief Cancels a pending async operation and destroys the connection.
        */
        ~Connection();

        /** @brief Attaches @a loop for asynchronous operations.
        */
        void setActive(Pt::System::EventLoop& loop);

        /** @brief Cancels any pending async operation.
        */
        void cancel();

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
        Pt::Signal<>& pingFinished();

        /** @brief Returns the last generated row id.

            Pass a sequence name on backends that use named sequences,
            or an empty string otherwise.
        */
        long long lastInsertId(const std::string& name = std::string());

        /** @brief Returns true if no asynchronous operation is pending.
        */
        bool isIdle() const;

        /** @brief Returns true if a transaction is active.
        */
        bool hasTransaction() const;

    public:
        /** @brief Returns true if the database is open.
        */
        bool isOpen() const;

        /** @brief Returns true if the database is not open.
        */
        bool operator!() const;

        /** @brief Opens the database with @a connStr.
        */
        void open(const std::string& connStr);

        /** @brief Closes the database connection.
        */
        void close();

        /** @brief Starts an asynchronous close.

            Cancels any pending operation, then closes asynchronously.
        */
        void beginClose();

        /** @brief Completes an asynchronous close.
        */
        void endClose();

        /** @brief Signal emitted when an asynchronous close completes.
        */
        Pt::Signal<>& closeFinished();

        /** @brief Starts an asynchronous open with @a connStr.
        */
        void beginOpen(const std::string& connStr);

        /** @brief Completes an asynchronous open.
        */
        void endOpen();

        /** @brief Signal emitted when an asynchronous open completes.
        */
        Pt::Signal<>& openFinished();

    public:
        /** @brief Executes @a query and returns the number of rows changed.
        */
        size_type execute(const std::string& query);

        /** @brief Starts asynchronous execution of @a sql.
        */
        void beginExecute(const std::string& sql);

        /** @brief Completes asynchronous execution.

            @return Number of rows changed.
        */
        size_type endExecute();

        /** @brief Signal emitted when asynchronous execution completes.
        */
        Pt::Signal<>& executeFinished();

    public:
        /** @brief Executes @a query and returns a buffered result.
        */
        Result select(const std::string& query);

        /** @brief Starts an asynchronous select of @a sql.
        */
        void beginSelect(const std::string& sql);

        /** @brief Completes an asynchronous select.

            @return Buffered result of the query.
        */
        Result endSelect();

        /** @brief Signal emitted when an asynchronous select completes.
        */
        Pt::Signal<>& selectFinished();

    public:
        /** @brief Compiles @a query into a prepared statement.
        */
        Statement prepare(const std::string& query);

        /** @brief Compiles @a query and caches the prepared statement.
        */
        Statement prepareCached(const std::string& query);

        /** @brief Clears the prepared-statement cache.
        */
        void clearStatementCache();

    public:
        /** @brief Starts asynchronous compile of @a query.
        */
        void beginPrepare(const std::string& query);

        /** @brief Completes asynchronous compile.

            @return Compiled statement.
        */
        Statement endPrepare();

        /** @brief Signal emitted when asynchronous prepare completes.
        */
        Pt::Signal<>& prepareFinished();

        /** @brief Starts asynchronous compile and cache of @a query.
        */
        void beginPrepareCached(const std::string& query);

        /** @brief Completes asynchronous prepareCached.

            @return Compiled statement.
        */
        Statement endPrepareCached();

        /** @brief Signal emitted when asynchronous prepareCached completes.
        */
        Pt::Signal<>& prepareCachedFinished();

    private:
        friend class Transaction;

#if __cplusplus >= 202002L
        friend class ConnectionAwaiter;

        void attachAwaiter(ConnectionAwaiter& awaiter);

        void detachAwaiter(ConnectionAwaiter& awaiter);
#endif

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
        /** @brief Returns an awaitable that opens the database with @a connStr.
        */
        AsyncOpen openAsync(const std::string& connStr);

        /** @brief Returns an awaitable that closes the database.
        */
        AsyncClose closeAsync();

        /** @brief Returns an awaitable that executes @a sql.

            The awaitable yields the number of rows changed.
        */
        AsyncExecute executeAsync(const std::string& sql);

        /** @brief Returns an awaitable that selects @a sql.

            The awaitable yields the buffered result.
        */
        AsyncSelect selectAsync(const std::string& sql);

        /** @brief Returns an awaitable that pings the database.

            The awaitable yields true if the connection is alive.
        */
        AsyncPing pingAsync();
#endif

    public:
        /** @brief Returns the backend implementation.
        */
        IConnection* impl();

        /** @brief Returns the backend implementation.
        */
        const IConnection* impl() const;

    private:
        Connection(const Connection&) = delete;

        Connection& operator=(const Connection&) = delete;

        typedef SmartPtr<IConnection,
                         InternalRefCounted<IConnection> > ConnectionImplPtr;

        ConnectionImplPtr _connection;

#if __cplusplus >= 202002L
        ConnectionAwaiter* _awaiter = nullptr;
#endif
};

#if __cplusplus >= 202002L

/** @brief Base awaiter for a pending connection operation.

    @ingroup Pt-Db-Connections
*/
class ConnectionAwaiter : public Pt::Awaiter
                        , public Pt::Connectable
{
    public:
        explicit ConnectionAwaiter(Connection& conn)
        : _conn(&conn)
        {
            _conn->attachAwaiter(*this); ;
        }

        ~ConnectionAwaiter()
        {
            if(_conn)
            {
                _conn->cancel();
                _conn->detachAwaiter(*this);
                _conn = nullptr;
            }
        }

        void onDetach()
        {
            _conn = nullptr;
            _handle = nullptr;
        }

    protected:
        Connection& connection()
        {
            if( ! _conn )
                throw std::logic_error("invalid connection");

            return *_conn;
        }

        void onCancel() override
        {
            if(_conn)
                _conn->cancel();
        }

    private:
        Connection* _conn;
};

/** @brief Awaitable for asynchronous open.

    @ingroup Pt-Db-Connections
*/
class AsyncOpen : public ConnectionAwaiter
{
    public:
        AsyncOpen(Connection& conn, const std::string& connStr)
        : ConnectionAwaiter(conn)
        , _connStr(connStr)
        {}

        void await_resume()
        {
            connection().endOpen();
        }

    private:
        void onBegin() override
        {
            Connection& conn = connection();
            conn.openFinished() += slot(*this, &AsyncOpen::setReady);
            conn.beginOpen(_connStr);
        }

        std::string _connStr;
};

/** @brief Awaitable for asynchronous close.

    @ingroup Pt-Db-Connections
*/
class AsyncClose : public ConnectionAwaiter
{
    public:
        AsyncClose(Connection& conn)
        : ConnectionAwaiter(conn)
        {}

        void await_resume()
        { connection().endClose(); }

    private:
        void onBegin() override
        {
            Connection& conn = connection();
            conn.closeFinished() += slot(*this, &AsyncClose::setReady);
            conn.beginClose();
        }
};


/** @brief Awaitable for asynchronous execute.

    @ingroup Pt-Db-Connections
*/
class AsyncExecute : public ConnectionAwaiter
{
    public:
        AsyncExecute(Connection& conn, const std::string& sql)
        : ConnectionAwaiter(conn)
        , _sql(sql)
        {}

        Connection::size_type await_resume()
        { return connection().endExecute(); }

    private:
        void onBegin() override
        {
            Connection& conn = connection();
            conn.executeFinished() += slot(*this, &AsyncExecute::setReady);
            conn.beginExecute(_sql);
        }

        std::string _sql;
};


/** @brief Awaitable for asynchronous select.

    @ingroup Pt-Db-Connections
*/
class AsyncSelect : public ConnectionAwaiter
{
    public:
        AsyncSelect(Connection& conn, const std::string& sql)
        : ConnectionAwaiter(conn)
        , _sql(sql)
        {}

        Result await_resume()
        { return connection().endSelect(); }

    private:
        void onBegin() override
        {
            Connection& conn = connection();
            conn.selectFinished() += slot(*this, &AsyncSelect::setReady);
            conn.beginSelect(_sql);
        }

        std::string _sql;
};


/** @brief Awaitable for asynchronous ping.

    @ingroup Pt-Db-Connections
*/
class AsyncPing : public ConnectionAwaiter
{
    public:
        AsyncPing(Connection& conn)
        : ConnectionAwaiter(conn)
        {}

        bool await_resume()
        {
            return connection().endPing();
        }

    private:
        void onBegin() override
        {
            Connection& conn = connection();
            conn.pingFinished() += slot(*this, &AsyncPing::setReady);
            conn.beginPing();
        }
};

#endif // __cplusplus >= 202002L

} // namespace Db

} // namespace Pt

#endif // PT_DB_CONNECTION_H
