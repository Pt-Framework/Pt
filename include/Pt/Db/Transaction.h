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
            // \brief Actual connection to a database.
            Connection _DbConnection;

            // \brief Parameter whether if exists a current transaction.
            bool _active;

        public:
            /** Creates a transaction

                Creates a new transaction from a connection and parameter
                whether the transaction should start immediately.
            */
            Transaction(const Connection& conn, bool starttransaction = true)
            : _DbConnection(conn)
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
            const Connection& getConnection() const  { return _DbConnection; }

            /** \brief Begin transaction.

                Starts a new deferred transaction. If there is an active transaction it will be rolled back 
                before beginning this transaction.
            */
            void begin()
            {
                if (_active)
                {
                    rollback();
                }
                _DbConnection.beginTransaction();
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
                    _DbConnection.commitTransaction();
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
                    _DbConnection.rollbackTransaction();
                    _active = false;
                }
            }
    };


    /** The class Transaction monitors the state of a transaction on a database-conection.

        The constructor starts by default a transaction on the database. The transactionstate
        is hold it the class. The destructor rolls the transaction back, when not explicitely
        commited or rolled back.
    */
    class SqliteTransaction : private NonCopyable
    {
        private:
            // \brief Actual connection to a database.
            Connection _DbConnection;

            // \brief Parameter whether if exists a current transaction.
            bool _active;

        public:
            /** Creates a transaction

                Creates a new transaction from a connection and parameter
                whether the transaction should start immediately.
            */
            SqliteTransaction(const Connection& conn, bool starttransaction = true, bool immediate = false)
            : _DbConnection(conn)
            , _active(false)
            {
                if (starttransaction)
                {
                    begin(immediate);
                }
            }

            /** \brief Destructor

                If active the current transaction will be rolled back.
            */
            ~SqliteTransaction()
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
            const Connection& getConnection() const  { return _DbConnection; }

            /** \brief Begin a deferred transaction.

                Starts a new deferred transaction. If there is an active
                transaction it will be rolled back before beginning this
                transaction.

                The default transaction behavior is deferred. Deferred means
                that no locks are acquired on the database until the database
                is first accessed. Thus with a deferred transaction, the
                BEGIN statement itself does nothing. Locks are not acquired
                until the first read or write operation. The first read
                operation against a database creates a SHARED lock and the
                first write operation creates a RESERVED lock. Because the
                acquisition of locks is deferred until they are needed, it
                is possible that another thread or process could create a
                separate transaction and write to the database after the
                BEGIN on the current thread has executed.

                If the transaction is immediate, then RESERVED locks are
                acquired on all databases as soon as the BEGIN command is
                executed, without waiting for the database to be used.
                After a BEGIN IMMEDIATE, it is guaranteed that no other
                thread or process will be able to write to the database
                or do a BEGIN IMMEDIATE or BEGIN EXCLUSIVE. Other processes
                can continue to read from the database.
            */
            void begin(bool immediate = false)
            {
                if (_active)
                {
                    rollback();
                }

                if(immediate)
                    _DbConnection.execute("BEGIN IMMEDIATE TRANSACTION");
                else
                    _DbConnection.beginTransaction();

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
                    _DbConnection.commitTransaction();
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
                    _DbConnection.rollbackTransaction();
                    _active = false;
                }
            }
    };

} // namespace Db

} // namespace Pt

#endif // PT_DB_TRANSACTION_H

