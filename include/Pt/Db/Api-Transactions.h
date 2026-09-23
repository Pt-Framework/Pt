/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_API_TRANSACTIONS_H
#define PT_DB_API_TRANSACTIONS_H

/** @addtogroup Pt-Db-Transactions

    @brief Begin, commit and roll back a unit of work.

    %Transaction is a unit of work on one %Connection. The constructor
    begins a deferred transaction unless the start flag is false.
    %begin() starts one; if a transaction is already active it is
    rolled back first. %commit() ends it and keeps the changes.
    %rollback() ends it and discards them. If the object is destroyed
    while still active, the destructor rolls back and swallows any
    error from that rollback.

    The object is not copyable. It does not own the connection.

    Asynchronous begin, commit and rollback follow the connection
    model and need %setActive() on that connection.
    %beginStart() / %endStart() / %startFinished() begin the
    transaction. %beginCommit() / %endCommit() / %commitFinished()
    commit it. %beginRollback() / %endRollback() /
    %rollbackFinished() roll it back. %endStart() is the call that
    marks the transaction active on the async path.

    C++20 awaitables wrap those pairs: %beginAsync(), %commitAsync()
    and %rollbackAsync() are used with co_await and complete through
    the same signals. Only one awaitable may be pending on the
    connection. Destroying the %Transaction while one is in flight
    detaches it; a later resume throws %std::logic_error.

    Override %onGetBeginSql(), %onGetCommitSql() and
    %onGetRollbackSql() to supply backend-specific SQL. A null return
    lets the backend use its default. %SqliteTransaction() uses that
    hook to run `BEGIN IMMEDIATE TRANSACTION` when immediate locking
    is requested.

    @code
    Pt::Db::Transaction txn(conn);
    conn.execute("UPDATE accounts SET balance = balance - 10 WHERE id = 1");
    conn.execute("UPDATE accounts SET balance = balance + 10 WHERE id = 2");
    txn.commit();
    @endcode
*/

#endif
