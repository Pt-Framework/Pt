/* Copyright (C) 2006-2026 by Tommi Maekitalo
   Copyright (C) 2006-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_DB_TRANSACTION_H
#define PT_DB_TRANSACTION_H

#include <Pt/Db/Api.h>
#include <Pt/Db/Connection.h>
#include <Pt/NonCopyable.h>
#include <Pt/Signal.h>

namespace Pt {

namespace Db {

/** @brief Unit of work on a database connection.

    %Transaction is the unit of work the group described. The
    constructor begins a deferred transaction unless @a starttransaction
    is false. The object is not copyable and does not own the
    connection.

    %begin() starts a transaction; if one is already active it is
    rolled back first. %commit() keeps the changes. %rollback()
    discards them. If the object is destroyed while still active, the
    destructor rolls back and swallows any error from that rollback.

    Asynchronous begin, commit and rollback need %setActive() on the
    connection. %beginStart() / %endStart() / %startFinished() begin
    the transaction; %endStart() is the call that marks it active.
    %beginCommit() / %endCommit() / %commitFinished() commit.
    %beginRollback() / %endRollback() / %rollbackFinished() roll
    back.

    Override %onGetBeginSql(), %onGetCommitSql() and
    %onGetRollbackSql() to supply backend-specific SQL. A null return
    lets the backend use its default.

    @code
    Pt::Db::Transaction txn(conn);
    conn.execute("UPDATE t SET n = n + 1");
    txn.commit();
    @endcode

    @ingroup Pt-Db-Transactions
*/
class Transaction : private NonCopyable
{
    private:
        Connection&  _connection;
        bool         _active;
        Pt::Signal<> _startFinished;
        Pt::Signal<> _commitFinished;
        Pt::Signal<> _rollbackFinished;

    public:
        /** @brief Creates a transaction on @a conn.

            Begins the transaction unless @a starttransaction is false.
        */
        Transaction(Connection& conn, bool starttransaction = true)
        : _connection(conn)
        , _active(false)
        {
            if (starttransaction)
            {
                begin();
            }
        }

        /** @brief Rolls back an active transaction and destroys the object.
        */
        ~Transaction()
        {
            if (_active)
            {
                try
                {
                    rollback();
                }
                catch (const std::exception&)
                {
                }
            }
        }

        /** @brief Returns the connection this transaction uses.
        */
        const Connection& getConnection() const
        { return _connection; }

        /** @brief Begins a deferred transaction.

            Rolls back an already active transaction first.
        */
        void begin()
        {
            if (_active)
                rollback();
            _connection.startTransaction(onGetBeginSql());
            _active = true;
        }

        /** @brief Commits the active transaction.

            Does nothing if no transaction is active.
        */
        void commit()
        {
            if (_active)
            {
                _connection.commitTransaction(onGetCommitSql());
                _active = false;
            }
        }

        /** @brief Rolls back the active transaction.

            Does nothing if no transaction is active.
        */
        void rollback()
        {
            if (_active)
            {
                _connection.rollbackTransaction(onGetRollbackSql());
                _active = false;
            }
        }

        /** @brief Starts an asynchronous begin-transaction.
        */
        void beginStart()
        {
            if(_active)
                rollback();
            _connection.beginStartTransaction(*this, onGetBeginSql());
        }

        /** @brief Completes an asynchronous begin-transaction.
        */
        void endStart()
        {
            _connection.endStartTransaction();
            _active = true;
        }

        /** @brief Signal emitted when an asynchronous begin completes.
        */
        Pt::Signal<>& startFinished()
        { return _startFinished; }

        /** @brief Starts an asynchronous commit.
        */
        void beginCommit()
        {
            _active = false;
            _connection.beginCommitTransaction(*this, onGetCommitSql());
        }

        /** @brief Completes an asynchronous commit.
        */
        void endCommit()
        {
            _connection.endCommitTransaction();
        }

        /** @brief Signal emitted when an asynchronous commit completes.
        */
        Pt::Signal<>& commitFinished()
        { return _commitFinished; }


        /** @brief Starts an asynchronous rollback.
        */
        void beginRollback()
        {
            _active = false;
            _connection.beginRollbackTransaction(*this, onGetRollbackSql());
        }

        /** @brief Completes an asynchronous rollback.
        */
        void endRollback()
        {
            _connection.endRollbackTransaction();
        }

        /** @brief Signal emitted when an asynchronous rollback completes.
        */
        Pt::Signal<>& rollbackFinished()
        { return _rollbackFinished; }

    protected:
        /** @brief Returns SQL for BEGIN, or a null pointer for the backend default.
        */
        virtual const char* onGetBeginSql()
        { return nullptr; }

        /** @brief Returns SQL for COMMIT, or a null pointer for the backend default.
        */
        virtual const char* onGetCommitSql()
        { return nullptr; }

        /** @brief Returns SQL for ROLLBACK, or a null pointer for the backend default.
        */
        virtual const char* onGetRollbackSql()
        { return nullptr; }
};


/** @brief SQLite transaction with optional immediate locking.

    %SqliteTransaction is a %Transaction that can start with
    `BEGIN IMMEDIATE TRANSACTION` instead of the backend default.
    Pass @a immediate true for that locking. @a start still controls
    whether the constructor begins the transaction.

    @ingroup Pt-Db-Transactions
*/
class SqliteTransaction : public Transaction
{
    public:
        /** @brief Creates a SQLite transaction on @a conn.
        */
        SqliteTransaction(Connection& conn, bool start = true, bool immediate = false)
        : Transaction(conn, false)
        , _immediate(immediate)
        {
            if(start)
                begin();
        }

    protected:
        const char* onGetBeginSql() override
        {
            return _immediate ? "BEGIN IMMEDIATE TRANSACTION" : nullptr;
        }

    private:
        bool _immediate;
};

} // namespace Db

} // namespace Pt

#endif // PT_DB_TRANSACTION_H
