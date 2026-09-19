/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef Pt_Db_DbError_h
#define Pt_Db_DbError_h

#include <Pt/Db/Api.h>
#include <string>
#include <stdexcept>

namespace Pt {

namespace Db {

/** @brief Base class for database errors.

    @ingroup Pt-Db
*/
class PT_DB_API DbError : public std::runtime_error
{
    public:
        /** @brief Constructs the error with message @a what.
        */
        explicit DbError(const std::string& what);

        /** @brief Destructor.
        */
        ~DbError() throw();
};


/** @brief Base class for connection errors.

    @ingroup Pt-Db
*/
class PT_DB_API ConnectionError : public DbError
{
    public:
        /** @brief Constructs the error with message @a what.
        */
        explicit ConnectionError(const std::string& what);

        /** @brief Destructor.
        */
        ~ConnectionError() throw();
};


/** @brief Thrown when access to the database is denied.

    @ingroup Pt-Db
*/
class PT_DB_API AccessDenied : public ConnectionError
{
    public:
        /** @brief Constructs the error with message @a what.
        */
        explicit AccessDenied(const std::string& what);

        /** @brief Destructor.
        */
        ~AccessDenied() throw();
};


/** @brief Thrown when the connection cannot be used.

    @ingroup Pt-Db
*/
class PT_DB_API InvalidConnection : public ConnectionError
{
    public:
        /** @brief Constructs the error with message @a what.
        */
        explicit InvalidConnection(const std::string& what);

        /** @brief Destructor.
        */
        ~InvalidConnection() throw();
};


/** @brief Base class for query errors.

    @ingroup Pt-Db
*/
class PT_DB_API QueryFailed : public DbError
{
    public:
        /** @brief Constructs the error with @a what and the failing @a statement.
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


/** @brief Thrown when a query is invalid.

    @ingroup Pt-Db
*/
class PT_DB_API InvalidQuery : public QueryFailed
{
    public:
        /** @brief Constructs the error with @a what and the failing @a statement.
        */
        InvalidQuery(const std::string& what, const std::string& statement);

        /** @brief Destructor.
        */
        ~InvalidQuery() throw();
};


/** @brief Thrown when a database constraint is violated.

    @ingroup Pt-Db
*/
class PT_DB_API ConstraintMismatch : public QueryFailed
{
    public:
        /** @brief Constructs the error with @a what and the failing @a statement.
        */
        ConstraintMismatch(const std::string& what, const std::string& statement);

        /** @brief Destructor.
        */
        ~ConstraintMismatch() throw();
};


/** @brief Thrown on type mismatch, null access, or out-of-range values.

    @ingroup Pt-Db
*/
class PT_DB_API TypeMismatch : public QueryFailed
{
    public:
        /** @brief Constructs the error with @a what and the failing @a statement.
        */
        TypeMismatch(const std::string& what, const std::string& statement);

        /** @brief Destructor.
        */
        ~TypeMismatch() throw();
};

} // namespace Db

} // namespace Pt

#endif
