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
#include <Pt/Date.h>
#include <Pt/Time.h>
#include <Pt/DateTime.h>
#include <Pt/Db/Api.h>
#include <Pt/Db/IStatement.h>
#include <Pt/Db/ICursor.h>
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
            class ConstIterator;

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

            /** \brief Executes a query with the current parameters

                The query should not return results. This method is normally
                used with INSERT-, UPDATE- or DELETE-statements.

                \return  The number of database rows that were changed
            */
            size_type execute();

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

            /** \brief Get Iterator to first row

                This methods creates a cursor and fetches the first row.

                \return Iterator to first row
            */
            ConstIterator begin() const;

            /** \brief Get Iterator to end of row

                A empty iterator is returned, which indicates the end of a row.

                \return Iterator to the end of row.
            */
            ConstIterator end() const;

            /** \brief Test if bound to a statement

                Returns true, if this class is not bound to an actual statement.

                \return True if unbound
            */
            bool operator!() const
            { return !_stmt; }

            //! \brief Returns the actual implementation-class.
            const IStatement* getImpl() const
            { return &*_stmt; }
    };

    /** \brief Iterator for statements.

        This iterator can be used to iterate over a Statement like over a sequence.
        It fullfils the requirements for a forward iterator. An empty iterator marks
        the end of the sequence.
    */
    class PT_DB_API Statement::ConstIterator : public std::iterator<std::forward_iterator_tag, Row>
    {
        private:
            Row _current;
            SmartPtr<ICursor, InternalRefCounted<ICursor> > _cursor;

        public:
            /** \brief Construct an iterator from a specific implementation

                The iterator will manage the passed implementation,
                thus it needs to be created on the heap.

                \param cursor Iterator implementation
            */
            ConstIterator(ICursor* cursor = 0);

            /** \brief Test it two iterators are equal.

                Two iterators are equal if they point to the same iteration.

                \param c Other iterator.
                \return True if equal.
            */
            bool operator== (const ConstIterator& c) const
            { return _cursor == c._cursor; }

            /** \brief Test it two iterators are not equal.

                Two iterators are not equal if they do not point to the same
                iteration.

                \param c Other iterator.
                \return True if not equal.
            */
            bool operator!= (const ConstIterator& c) const
            { return _cursor != c._cursor; }

            /** \brief Steps forward.

                Fetches the next row. If no rows are available, the cursor
                is closed and removed.

                \return Iterator to next element.
            */
            ConstIterator& operator++();

            /** \brief Get current row.

                Provides read access to the current row in the iterated
                statement.

                \return Const reference to a row.
            */
            const Row& operator* () const
            { return _current; }

            /** \brief Get pointer to current row.

                Provides read access to the current row in the iterated
                statement.

                \return Const pointer to a row.
            */
            const Row* operator-> () const
            { return &_current; }

            //! \brief Returns the actual implementation-class
            const ICursor* getImpl() const
            { return &*_cursor; }
    };

} // namespace Db

} // namespace Pt

#endif // PT_DB_STATEMENT_H
