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

#ifndef PT_DB_STATEMENT_H
#define PT_DB_STATEMENT_H

#include <Pt/SmartPtr.h>
#include <Pt/Signal.h>
#include <Pt/Date.h>
#include <Pt/Time.h>
#include <Pt/DateTime.h>
#include <Pt/Db/Api.h>
#include <Pt/Db/Cursor.h>
#include <Pt/Db/IStatement.h>
#include <Pt/Db/IConnection.h>
#include <Pt/Db/Row.h>
#include <string>


namespace Pt {

namespace Db {

  class Connection;
  class Result;
  class Row;

    /** /brief This class represents a sql-statement

        A statement can have parameters, which are referenced by name, called
        hostvariables. They are prefixed with a colon followed by a name. A
        name starts with a letter followed by alphanumeric characters or
        underscore. Hostvariables are not searched in strings (between
        apostrophes, quotation marks or backticks). The backslash prevents
        the interpretation of a special meaning of the following character.
    */
    class PT_DB_API Statement
    {
        public:
            //! \brief The size-type for this Statement
            typedef IStatement::size_type size_type;

        private:
            //! \brief Shared Implementation
            SmartPtr<IStatement, InternalRefCounted<IStatement> > _stmt;

        public:
            /** \brief Construct a statement from a specific implementation

                The Statement class will manage the passed implementation,
                thus it needs to be created on the heap.

                \param stmt Statement implementation
            */
            Statement(IStatement* stmt = 0)
            : _stmt(stmt)
            { }

            ~Statement();

            /** \brief Sets all hostvariables to NULL.

                Sets all hostvariables to NULL.

                \return Self reference
            */
            Statement& clear()
            { _stmt->clear(); return *this; }


            /** \brief Set a hostvariable to NULL.

                Sets the hostvariable with the given name to NULL.

                \param col Column name
                \return Self reference
            */
            Statement& setNull(const std::string& col)
            { _stmt->setNull(col); return *this; }

            /** Set a host-variable to a boolean value

                Sets the hostvariable with the given name to a boolean value.

                \param col Column name
                \param data New variable value
                \return Self reference
            */
            Statement& set(const std::string& col, bool data)
            { _stmt->setBool(col, data); return *this; }

            /** Set a host-variable to an integer value

                Sets the hostvariable with the given name to a int value.

                \param col Column name
                \param data New variable value
                \return Self reference
            */
            Statement& set(const std::string& col, int data)
            { _stmt->setInt(col, data); return *this; }

            /** Set a host-variable to an unsigned integer value

                Sets the hostvariable with the given name to an unsigned int value.

                \param col Column name
                \param data New variable value
                \return Self reference
            */
            Statement& set(const std::string& col, unsigned data)
            { _stmt->setUnsigned(col, data); return *this; }

            /** Set a host-variable to a float value

                Sets the hostvariable with the given name to a float value.

                \param col Column name
                \param data New variable value
                \return Self reference
            */
            Statement& set(const std::string& col, float data)
            { _stmt->setFloat(col, data); return *this; }

            /** Set a host-variable to a double value

                Sets the hostvariable with the given name to a double value.

                \param col Column name
                \param data New variable value
                \return Self reference
            */
            Statement& set(const std::string& col, double data)
            { _stmt->setDouble(col, data); return *this; }

            /** Set a host-variable to a char

                Sets the hostvariable with the given name to a char.

                \param col Column name
                \param data New variable value
                \return Self reference
            */
            Statement& set(const std::string& col, char data)
            { _stmt->setChar(col, data); return *this; }

            /** Set a host-variable to a string value

                Sets the hostvariable with the given name to a string value.

                \param col Column name
                \param data New variable value
                \return Self reference
            */
            Statement& set(const std::string& col, const std::string& data)
            { _stmt->setString(col, data); return *this; }

            /** Set a host-variable to a Blob value

                Sets the hostvariable with the given name to a blob value.

                \param col Column name
                \param data New variable value
                \return Self reference
            */
            Statement& set(const std::string& col, const Blob& data)
            { _stmt->setBlob(col, data); return *this; }

            /** Set a host-variable to a string value

                Sets the hostvariable with the given name to a string value.

                \param col Column name
                \param data New variable value
                \return Self reference
            */
            Statement& set(const std::string& col, const char* data)
            { data == 0 ? _stmt->setNull(col)
                            : _stmt->setString(col, data); return *this; }

            /** Set a host-variable to a date

                Sets the hostvariable with the given name to a date value.

                \param col Column name
                \param data New variable value
                \return Self reference
            */
            Statement& set(const std::string& col, const Date& data)
            { _stmt->setDate(col, data); return *this; }

            /** Set a host-variable to a time

                Sets the hostvariable with the given name to a time value.

                \param col Column name
                \param data New variable value
                \return Self reference
            */
            Statement& set(const std::string& col, const Time& data)
            { _stmt->setTime(col, data); return *this; }

            /** Set a host-variable to a date-time

                Sets the hostvariable with the given name to a date-time value.

                \param col Column name
                \param data New variable value
                \return Self reference
            */
            Statement& set(const std::string& col, const DateTime& data)
            {
                _stmt->setDatetime(col, data);
                return *this;
            }

            /** \brief Open a batch cursor on this statement.

                Returns a %Cursor that manages the cursor lifecycle.

                For synchronous row-by-row iteration use range-for:
                @code
                for(auto& row : stmt.getCursor(100)) { ... }
                @endcode

                For asynchronous batch iteration connect to %Cursor::fetchFinished()
                and call %Cursor::beginFetch().

                \param batchSize Number of rows to fetch per batch.
                \return Cursor positioned before the first row.
            */
            Cursor getCursor(size_type batchSize);

            /** \brief Cancel any pending async operation on this statement.
            */
            void cancel();

        public:
            /** \brief Executes a query with the current parameters

                The query should not return results. This method is normally
                used with INSERT-, UPDATE- or DELETE-statements.

                \return  The number of database rows that were changed
            */
            size_type execute();

            /** \brief Begin async execution (DML).
            */
            void beginExecute();

            /** \brief Retrieve row count after async exec completes.
            */
            size_type endExecute();

            /** \brief Signal emitted when an async exec (DML) completes.
            */
            Signal<>& executeFinished();

            /** \brief Returns the row id of the last inserted row.

                Valid after execute() or endExecute().
            */
            long long lastInsertId() const;

        public:
            /** \brief Execute a query
                Executes a query, which returns a resultset, with the current
                parameters. The query is normally a SELECT-statement.

                \return Result of the query
            */
            Result select();

            /** \brief Execute a query

                Executes a query, which returns a row, with the current
                parameters. If the query returns no rows, a exception of type
                tntDb::NotFound is thrown. When the query returns more than one row,
                additional rows are discarded.

                \return Result-row of the query
                \throw TODO
            */
            Row selectRow();

            /** \brief Execute a query

                Executes a query, which returns a single value, with the current
                parameters. If the query return no rows, a exception of type
                tntDb::NotFound is thrown. Only the first value of the first row is
                returned.

                \return Result-value of the query
                \throw TODO
            */
            Value selectValue();

            /** \brief Begin async SELECT. */
            void beginSelect();

            /** \brief Retrieve Result after async select completes.
            */
            Result endSelect();

            /** \brief Signal emitted when an async select completes.
            */
            Signal<>& selectFinished();

        public:
            /** \brief Test if bound to a statement

                Returns true, if this class is not bound to an actual statement.

                \return True if unbound
            */
            bool operator!() const
            { return !_stmt; }

            //! \brief Returns the actual implementation-class.
            const IStatement* getImpl() const
            { return &*_stmt; }

            //! \brief Returns the actual implementation-class (non-const).
            IStatement* impl()
            { return _stmt.get(); }
    };

} // namespace Db

} // namespace Pt

#endif // PT_DB_STATEMENT_H
