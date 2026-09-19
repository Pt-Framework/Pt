/* Copyright (C) 2006-2026 by Tommi Maekitalo
   Copyright (C) 2006-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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

    /** @brief Prepared SQL statement with named host variables.

        %Statement is the compiled query the group described. Obtain it
        from %Connection::prepare() or %Connection::prepareCached(). It
        is a shared value. An unbound statement is empty;
        %operator!() is true then. The constructor that takes an
        %IStatement takes ownership of that backend.

        Host variables are `:name`. Bind them with %set() before
        execute or select. %setNull() binds SQL NULL. %clear() sets
        every host variable to NULL. A const char pointer that is 0
        is bound as NULL.

        %execute() runs a statement that does not return rows and
        returns how many rows changed. %lastInsertId() is the
        generated id after that execute. %select() returns a buffered
        %Result. %selectRow() returns the first row and discards the
        rest. %selectValue() returns the first column of the first
        row. Both throw %InvalidQuery when the query returns no row.

        %getCursor() opens a batch cursor on this statement.
        %beginExecute() / %endExecute() / %executeFinished() and
        %beginSelect() / %endSelect() / %selectFinished() are the
        async forms. %cancel() aborts a pending operation.

        @code
        Pt::Db::Statement stmt = conn.prepare(
            "SELECT name FROM users WHERE id = :id");
        stmt.set("id", 1);
        Pt::Db::Value name = stmt.selectValue();
        @endcode

        @ingroup Pt-Db-Statements
    */
    class PT_DB_API Statement
    {
        public:
            /** @brief Size type of this statement.
            */
            typedef IStatement::size_type size_type;

        private:
            SmartPtr<IStatement, InternalRefCounted<IStatement> > _stmt;

        public:
            /** @brief Takes ownership of the backend @a stmt.
            */
            Statement(IStatement* stmt = 0)
            : _stmt(stmt)
            { }

            ~Statement();

            /** @brief Sets all host variables to NULL.

                @return This statement.
            */
            Statement& clear()
            { _stmt->clear(); return *this; }


            /** @brief Sets the host variable @a col to NULL.

                @return This statement.
            */
            Statement& setNull(const std::string& col)
            { _stmt->setNull(col); return *this; }

            /** @brief Sets the host variable @a col to @a data.

                @return This statement.
            */
            Statement& set(const std::string& col, bool data)
            { _stmt->setBool(col, data); return *this; }

            /** @brief Sets the host variable @a col to @a data.

                @return This statement.
            */
            Statement& set(const std::string& col, int data)
            { _stmt->setInt(col, data); return *this; }

            /** @brief Sets the host variable @a col to @a data.

                @return This statement.
            */
            Statement& set(const std::string& col, unsigned data)
            { _stmt->setUnsigned(col, data); return *this; }

            /** @brief Sets the host variable @a col to @a data.

                @return This statement.
            */
            Statement& set(const std::string& col, float data)
            { _stmt->setFloat(col, data); return *this; }

            /** @brief Sets the host variable @a col to @a data.

                @return This statement.
            */
            Statement& set(const std::string& col, double data)
            { _stmt->setDouble(col, data); return *this; }

            /** @brief Sets the host variable @a col to @a data.

                @return This statement.
            */
            Statement& set(const std::string& col, char data)
            { _stmt->setChar(col, data); return *this; }

            /** @brief Sets the host variable @a col to @a data.

                @return This statement.
            */
            Statement& set(const std::string& col, const std::string& data)
            { _stmt->setString(col, data); return *this; }

            /** @brief Sets the host variable @a col to @a data.

                @return This statement.
            */
            Statement& set(const std::string& col, const Blob& data)
            { _stmt->setBlob(col, data); return *this; }

            /** @brief Sets the host variable @a col to @a data, or to NULL if @a data is 0.

                @return This statement.
            */
            Statement& set(const std::string& col, const char* data)
            { data == 0 ? _stmt->setNull(col)
                            : _stmt->setString(col, data); return *this; }

            /** @brief Sets the host variable @a col to @a data.

                @return This statement.
            */
            Statement& set(const std::string& col, const Date& data)
            { _stmt->setDate(col, data); return *this; }

            /** @brief Sets the host variable @a col to @a data.

                @return This statement.
            */
            Statement& set(const std::string& col, const Time& data)
            { _stmt->setTime(col, data); return *this; }

            /** @brief Sets the host variable @a col to @a data.

                @return This statement.
            */
            Statement& set(const std::string& col, const DateTime& data)
            {
                _stmt->setDatetime(col, data);
                return *this;
            }

            /** @brief Opens a batch cursor that fetches @a batchSize rows at a time.
            */
            Cursor getCursor(size_type batchSize);

            /** @brief Cancels any pending asynchronous operation.
            */
            void cancel();

        public:
            /** @brief Executes the statement and returns the number of rows changed.
            */
            size_type execute();

            /** @brief Starts asynchronous execution.
            */
            void beginExecute();

            /** @brief Completes asynchronous execution.

                @return Number of rows changed.
            */
            size_type endExecute();

            /** @brief Signal emitted when asynchronous execution completes.
            */
            Signal<>& executeFinished();

            /** @brief Returns the generated row id of the last insert.
            */
            long long lastInsertId() const;

        public:
            /** @brief Executes the statement and returns a buffered result.
            */
            Result select();

            /** @brief Executes the statement and returns the first row.

                Additional rows are discarded.

                @throw %InvalidQuery if the query returns no row.
            */
            Row selectRow();

            /** @brief Executes the statement and returns the first column of the first row.

                @throw %InvalidQuery if the query returns no row.
            */
            Value selectValue();

            /** @brief Starts an asynchronous select.
            */
            void beginSelect();

            /** @brief Completes an asynchronous select.

                @return Buffered result of the query.
            */
            Result endSelect();

            /** @brief Signal emitted when an asynchronous select completes.
            */
            Signal<>& selectFinished();

        public:
            /** @brief Returns true if this object is not bound to a statement.
            */
            bool operator!() const
            { return !_stmt; }

            /** @brief Returns the backend implementation.
            */
            const IStatement* getImpl() const
            { return &*_stmt; }

            /** @brief Returns the backend implementation.
            */
            IStatement* impl()
            { return _stmt.get(); }
    };

} // namespace Db

} // namespace Pt

#endif // PT_DB_STATEMENT_H
