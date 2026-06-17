/*
 * Copyright (C) 2006 by Tommi Maekitalo
 * Copyright (C) 2006 by Marc Boris Duerner
 * Copyright (C) 2006 by Stefan Bueder
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PT_DB_TRANSACTION_H
#define PT_DB_TRANSACTION_H

#include <Pt/Db/Api.h>
#include <Pt/Db/Connection.h>
#include <Pt/NonCopyable.h>
#include <Pt/Signal.h>

namespace Pt {

namespace Db {

/** The class Transaction monitors the state of a transaction on a database-conection.

    The constructor starts by default a transaction on the database. The transactionstate
    is hold it the class. The destructor rolls the transaction back, when not explicitely
    commited or rolled back.
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
        /** Creates a transaction

            Creates a new transaction from a connection and parameter
            whether the transaction should start immediately.
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

        /** \brief Destructor

            If active the current transaction will be rolled back.
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

        /** Returns connection.

            Returns the current connection object.

            \return Connection reference.
        */
        const Connection& getConnection() const
        { return _connection; }

        /** \brief Begin transaction.

            Starts a new deferred transaction. If there is an active transaction it will be rolled back
            before beginning this transaction.
        */
        void begin()
        {
            if (_active)
                rollback();
            _connection.startTransaction(onGetBeginSql());
            _active = true;
        }

        /** \brief Commit a transaction

            Commits the current transaction. If there is no active transaction
            nothing happens. The transaction state is reset.
        */
        void commit()
        {
            if (_active)
            {
                _connection.commitTransaction(onGetCommitSql());
                _active = false;
            }
        }

        /** \brief Roll back a transaction.

            Rolls back the current transaction. If there is no active
            transaction nothing is done. The transaction state is reset.
        */
        void rollback()
        {
            if (_active)
            {
                _connection.rollbackTransaction(onGetRollbackSql());
                _active = false;
            }
        }

        // --- Async variants (require Connection::setActive(loop)) ---

        /** \brief Begin async BEGIN TRANSACTION.

            Starts the async transaction. Call endStart() in the
            transactionFinished() signal handler to complete.
        */
        void beginStart()
        {
            if(_active)
                rollback();
            _connection.beginStartTransaction(*this, onGetBeginSql());
        }

        /** \brief Complete async BEGIN TRANSACTION.
        */
        void endStart()
        {
            _connection.endStartTransaction();
            _active = true;
        }

        Pt::Signal<>& startFinished()
        { return _startFinished; }

        /** \brief Begin async COMMIT TRANSACTION.
        */
        void beginCommit()
        {
            _active = false;
            _connection.beginCommitTransaction(*this, onGetCommitSql());
        }

        /** \brief Complete async COMMIT TRANSACTION.
        */
        void endCommit()
        {
            _connection.endCommitTransaction();
        }

        Pt::Signal<>& commitFinished()
        { return _commitFinished; }


        /** \brief Begin async ROLLBACK TRANSACTION.
        */
        void beginRollback()
        {
            _active = false;
            _connection.beginRollbackTransaction(*this, onGetRollbackSql());
        }

        /** \brief Complete async ROLLBACK TRANSACTION.
        */
        void endRollback()
        {
            _connection.endRollbackTransaction();
        }

        Pt::Signal<>& rollbackFinished()
        { return _rollbackFinished; }

    protected:
        virtual const char* onGetBeginSql()
        { return nullptr; }

        virtual const char* onGetCommitSql()
        { return nullptr; }

        virtual const char* onGetRollbackSql()
        { return nullptr; }
};


class SqliteTransaction : public Transaction
{
    public:
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

