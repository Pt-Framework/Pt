/*
  Copyright (C) 2026 by Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
 */

#ifndef Pt_Db_Exception_h
#define Pt_Db_Exception_h

#include <Pt/Db/Api.h>
#include <string>
#include <stdexcept>

namespace Pt {

namespace Db {

/** @brief Base class for all %Pt::Db exceptions.

    @ingroup Pt-Db-Exceptions
*/
class PT_DB_API DbError : public std::runtime_error
{
    public:
        /** @brief Constructor.

            @param what Description of the error.
        */
        explicit DbError(const std::string& what);

        /** @brief Destructor.
        */
        ~DbError() throw();
};


/** @brief Base class for connection-related errors.

    @ingroup Pt-Db-Exceptions
*/
class PT_DB_API ConnectFailed : public DbError
{
    public:
        /** @brief Constructor.

            @param what Description of the error.
        */
        explicit ConnectFailed(const std::string& what);

        /** @brief Destructor.
        */
        ~ConnectFailed() throw();
};


/** @brief Thrown when access to the database is denied.

    Indicates a failed authorization or insufficient access rights.

    @ingroup Pt-Db-Exceptions
*/
class PT_DB_API AccessDenied : public ConnectFailed
{
    public:
        /** @brief Constructor.

            @param what Description of the error.
        */
        explicit AccessDenied(const std::string& what);

        /** @brief Destructor.
        */
        ~AccessDenied() throw();
};


/** @brief Thrown when the connection cannot be established.

    Indicates an invalid URL, missing driver, or a connection that is
    not in the required state for the requested operation.

    @ingroup Pt-Db-Exceptions
*/
class PT_DB_API InvalidConnection : public ConnectFailed
{
    public:
        /** @brief Constructor.

            @param what Description of the error.
        */
        explicit InvalidConnection(const std::string& what);

        /** @brief Destructor.
        */
        ~InvalidConnection() throw();
};


/** @brief Base class for query-related errors.

    Holds the SQL statement that caused the error.

    @ingroup Pt-Db-Exceptions
*/
class PT_DB_API QueryFailed : public DbError
{
    public:
        /** @brief Constructor.

            @param what Description of the error.
            @param statement The SQL statement that failed.
        */
        QueryFailed(const std::string& what, const std::string& statement);

        /** @brief Destructor.
        */
        ~QueryFailed() throw();

        /** @brief Returns the SQL statement that caused the error.
        */
        const std::string& statement() const;

    private:
        std::string _statement;
};


/** @brief Thrown when a query is syntactically or semantically invalid.

    Indicates invalid SQL, a missing table, or a changed schema.

    @ingroup Pt-Db-Exceptions
*/
class PT_DB_API InvalidQuery : public QueryFailed
{
    public:
        /** @brief Constructor.

            @param what Description of the error.
            @param statement The SQL statement that failed.
        */
        InvalidQuery(const std::string& what, const std::string& statement);

        /** @brief Destructor.
        */
        ~InvalidQuery() throw();
};


/** @brief Thrown when a database constraint is violated.

    Indicates a violation of a unique, foreign key, check, or not-null
    constraint.

    @ingroup Pt-Db-Exceptions
*/
class PT_DB_API ConstraintMismatch : public QueryFailed
{
    public:
        /** @brief Constructor.

            @param what Description of the error.
            @param statement The SQL statement that failed.
        */
        ConstraintMismatch(const std::string& what, const std::string& statement);

        /** @brief Destructor.
        */
        ~ConstraintMismatch() throw();
};


/** @brief Thrown on type mismatch, null access, or out-of-range values.

    Indicates that a column value is null when not expected, has an
    incompatible type, or falls outside the representable range.

    @ingroup Pt-Db-Exceptions
*/
class PT_DB_API TypeMismatch : public QueryFailed
{
    public:
        /** @brief Constructor.

            @param what Description of the error.
            @param statement The SQL statement that failed.
        */
        TypeMismatch(const std::string& what, const std::string& statement);

        /** @brief Destructor.
        */
        ~TypeMismatch() throw();
};

} // namespace Db

} // namespace Pt

#endif

