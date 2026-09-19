/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_API_CONNECTIONS_H
#define PT_DB_API_CONNECTIONS_H

/** @addtogroup Pt-Db-Connections

    @brief Open a database and run SQL on it.

    %Connection is the database handle. Construct it with a registered
    driver name. %open() establishes the session with a driver-specific
    connection string; the string does not include the driver name. %close() ends the
    session. %isOpen() reports whether a session is established, and
    %ping() tests whether the backend still answers.

    The same object runs SQL directly and prepares statements.
    %execute() runs a statement that does not return rows and reports
    how many rows changed. %select() runs a query and returns a fully
    buffered %Result. %prepare() compiles a %Statement that can bind
    host variables and be executed more than once. %prepareCached()
    compiles the same way and keeps the statement in a cache keyed by
    the SQL text, so a later call with the same text reuses it.
    %clearStatementCache() drops that cache. %lastInsertId() is the
    generated row id of the last insert on this connection.

    Without an %EventLoop every method above is synchronous.
    %setActive() attaches an %EventLoop so the begin/end forms can
    run. %beginOpen() / %endOpen() / %openFinished() are the async
    open; close, ping, execute, select, prepare and prepareCached
    follow the same pattern. %isIdle() is true when no async
    operation is pending. %cancel() aborts a pending operation.
    The loop does not own the connection.

    C++20 awaitables wrap those pairs: %openAsync(), %closeAsync(),
    %executeAsync(), %selectAsync() and %pingAsync() are used with
    co_await and complete through the same signals.

    Only one async operation may be in flight on a connection, a
    statement, or a cursor that uses it. Starting another while one
    is pending throws %InvalidConnection or %ConnectionError. An
    unknown driver name throws %InvalidConnection from the
    constructor.

    %hasTransaction() is true while a %Transaction is active on this
    connection. Begin, commit and rollback are operations of
    %Transaction, not of %Connection.

    @code
    Pt::Db::Connection conn("sqlite");
    conn.open("file:app.db");
    Pt::Db::Result users = conn.select("SELECT id, name FROM users");
    conn.close();
    @endcode
*/

#endif
