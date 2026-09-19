/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_API_DB_H
#define PT_DB_API_DB_H

/** @defgroup Pt-Db Database Access

    @brief Portable SQL connections, statements and results.

    This module is the portable SQL layer, so a caller opens a
    database, runs statements, and reads rows through the same types
    on every supported platform. The unit of work is a connection.
    Construct a %Connection with a registered driver name, such as
    "sqlite". That allocates a backend; it does not open the database.
    %open() takes a driver-specific connection string with no driver
    prefix.

    %Connection is not copyable. %Statement, %Result, %Row, %Value,
    %Cursor and %Blob are shared values: copying is cheap, and the
    backend lives until the last wrapper is destroyed.

    Without an %EventLoop every operation is synchronous and completes
    on the calling thread. %setActive() attaches an %EventLoop for
    asynchronous work. The loop does not own the connection: the code
    that creates it keeps it alive while an operation is still waiting
    on the loop. Asynchronous operations use a begin/end pair and a
    finished signal. The slot calls the matching end method. C++20
    awaitables wrap the same pairs for co_await.

    A %Statement is a prepared query with named host variables.
    %execute() runs a statement that does not return rows.
    %select() returns a fully buffered %Result. For a result that is
    too large to hold at once, %getCursor() returns a %Cursor that
    fetches batches.

    A %Transaction begins on a connection, commits or rolls back, and
    rolls back from the destructor if it is still active.

    A failure that is specific to this module is a %DbError. What each
    operation throws is documented on that member.

    The rest of this chapter is the connection, then statements, then
    buffered results, then cursors, then transactions.
*/

/** @defgroup Pt-Db-Connections Connections

    @ingroup Pt-Db
*/

/** @defgroup Pt-Db-Statements Statements

    @ingroup Pt-Db
*/

/** @defgroup Pt-Db-Results Results

    @ingroup Pt-Db
*/

/** @defgroup Pt-Db-Cursors Cursors

    @ingroup Pt-Db
*/

/** @defgroup Pt-Db-Transactions Transactions

    @ingroup Pt-Db
*/

#endif
