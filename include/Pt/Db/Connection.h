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

#ifndef PT_DB_Connection_H
#define PT_DB_Connection_H

#include <Pt/Db/Api.h>
#include <Pt/SmartPtr.h>
#include <Pt/Db/IConnection.h>
#include <Pt/Db/Statement.h>
#include <string>

namespace Pt {

namespace Db {

    class Result;
    class Row;
    class Value;
    class Statement;

    /** \brief This class holds a connection to a database.

        A Connection object is created with a specific implementation
        of a database connection (SqLite, mySql, ...). The actual
        Connection is referencecounted. You can copy this class as
        you need. The connection will be closed if their are no further
        references to it.
    */
    class PT_DB_API Connection
    {
        public:
            //! \brief The size-type used for the Connection class.
            typedef std::size_t size_type;

        private:
            //! \brief Reference counted implementation.
            SmartPtr<IConnection, InternalRefCounted<IConnection> > _connection;

        public:
            /** \brief Standard constructor.

                Construct a Connection with empty values for parameters.
            */
            Connection() { }
            
            /** \brief Construct a Connection from a special implementation.
                
                \param conn Implementation of a Connection.
            */
            Connection(IConnection* conn)
                : _connection(conn)
            { }

            /** \brief Close the connection to a database.

                Remove the reference to the connected database. If this was the last
                open reference to the database the connection is closed.            
            */
            void close()
            { 
                _connection.reset();
            }
            
            /** \brief Starts a database transaction.            
            */
            void beginTransaction();

            /** \brief Commits a transaction.

                Commits the current transaction. Mostly this function is not
                needed. Better use function of class Pt::Db::Transaction instead.
            */
            void commitTransaction();

            /** \brief Rollback a transaction.

                Rolls back the current transaction. Mostly this function is not
                needed. Better use function of class Pt::Db::Transaction instead.
            */
            void rollbackTransaction();

            /** \brief Executes a static database query.

                Executes a static query, without returning results. The query 
                is normally a kind of INSERT-, UPDATE- or DELETE-statement. 
                When you need to pass parameters you should use 
                class Pt::Db::Statement.

                \param query Query to execute.
                \return Number of changes in database.
            */
            size_type execute(const std::string& query);

            /** \brief Executes a select query.

                Executes a static query, which returns a result. 
                The query is normally a SELECT-statement.

                \param query Query to execute.
                \return Result of query.
            */
            Result select(const std::string& query);

            /** \brief Executes a row selection query.

                Executes a static query and returns the first row of the 
                result. If the result contains no rows an exception is
                thrown. 

                \param query Query to execute.
                \return Row result of query.
            */
            Row selectRow(const std::string& query);

            /**  \brief Execute a value selection query.
            
                Executes a static query and retunrn the first value of the
                first row of the result. If the result contains no rows 
                with values an exception is thrown.

                \param query Query to execute.
                \return Value result of query.
            */
            Value selectValue(const std::string& query);

            /** \brief Create a statement.

                Creates a new Statement object of a given query.

                \param query Query statement.
                \return Statement.
            */
            Statement prepare(const std::string& query);

            /** \brief Create a cached statement.

                Creates a new Statement-object of a given query and stores
                the statement in a cache. When called with the same query
                again the cached statemnt is returned.

                \param query Query statement.
                \return Cached Statement.
            */
            Statement prepareCached(const std::string& query);

            /** \brief Returns an auto_increment value.

                This function returns the value of a auto_increment value of a table
                which is generated by the Db when a new row is inserted.

                \return The new auto_increment value.
            */
            long long insertId();

            /** \brief Clears statement cache.
                
                Clears the statment cache which was created 
                by prepareCache.
            */
            void clearStatementCache()
            { _connection->clearStatementCache(); }

            /** \brief Test if a connection doesn't exists.

                Return true if there is no connection established
                to a database.

                \return True if disconnect.
            */
            bool operator!() const             { return !_connection; }

            //! \brief Returns the actual implementation-class.
            const IConnection* getImpl() const { return &*_connection; }
    };

    /** \brief Establish a connection to a database.
    
        The url is prefixed with a drivername followed by a colon and a driver-
        specific part. If the connection can't be established, an exception is thrown.
        
        Examples:
        
        \code
        tntDb::Connection myConn = tntDb::connect("mysql:Db=DS2;user=web;passwd=web");
        tntDb::Connection pgConn = tntDb::connect("postgresql:Dbname=DS2 user=web passwd=web");
        \endcode
        
        \param url the url of the database to connect to
        \return the established connection
        \throw LogicError
        \throw RuntimeError
    */
    PT_DB_API Connection connect(const std::string& url);

    /** \brief Fetch a connection from a pool or create a new one.
    
        A static pool of connections is kept in memory. The function looks
        in this pool, if there is a connection, which matches the url. If found
        the connection is removed from the pool and returned. When the returned
        connection-object is destroyed (and all copies of it), the actual
        connection is put back into the pool.
        
        When there is no connections in the pool, which match the url, a new
        connection is established.
    */
    //Connection connectCached(const std::string& url);

    /** \brief Releases unused connections
    
        Keeps the given number of connections.
    */
    //void dropCached(unsigned keep = 0);

    /** \brief Releases unused connections.
    
        Releases unused connections, which match the given url. Keeps
        the given number of connections
    */
    //void dropCached(const std::string& url, unsigned keep = 0);

} // namespace Db

} // namespace Pt

#endif // PTB_DB_Connection_H

