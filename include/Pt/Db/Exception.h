/*
 * Copyright (C) 2004-2006 Marc Boris Duerner
 * Copyright (C) 2005-2006 Aloysius Indrayanto
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

#ifndef Pt_Db_Exception_h
#define Pt_Db_Exception_h

#include <Pt/Db/Api.h>
#include <string>
#include <exception>
#include <stdexcept>
#include <Pt/SourceInfo.h>


namespace Pt {

namespace Db {

    /** @brief This indicates that a resource could not be accessed.

        An exception of class AccessError is used to report failed access
        to a resource due to missing authorization, missing access rights
        or if a resource is in an otherwise inaccessible state. This class
        implements std::logic_error. Use the PT_SOURCEINFO macro to pass
        SourceInfo to Exception.

        TODO: rename AccessDenied
    */
    class PT_DB_API AccessError : public std::logic_error {
        public:
            //! @see Exception()
            AccessError(const std::string& what, const SourceInfo& si) throw();

            //! @brief Destructor.
            ~AccessError() throw();
    };

    /** @brief Basic exception type for database interaction.

        This exception is used to inform about database errors. Commonly databases
        return some kind of error values. To be able to use the benefits of exceptions
        this class encapsulates such error values.
     */
    class PT_DB_API DatabaseException : public std::logic_error
    {
        public:
            /**
             * @brief The possible error values of a DatabaseException.
             *
             *
             */
            enum DatabaseError
            {
                UNSPECIFIED=0,            //!< No error was specified (default value).
                //ACCESS_ERROR,           //!< The database cannot be accessed (e.g. locked table...).
                PERMISSION_DENIED,      //!< The current user has not the permission to access the database.
                BAD_ALLOC,              //!< Required memory cannot be allocated.
                VALUE_OUT_OF_RANGE,     //!< A parameter provided was out of range.
                TABLE_RECORD_NOT_FOUND, //!< The table or record specified was not found.
                TYPE_MISMATCH,          //!< A parameter provided was of wrong type.
                AUTHORIZATION_FAILED    //!< The authorization failed.
            };

            /** @brief Constructs a DataBaseException with no specific error value.

                @param what The message text of the DatabaseException.
                @param statement The statement that failed.
                @param si The SourceInfo where the error occurred.
             */
            DatabaseException( const std::string& what, const std::string& statement, const SourceInfo& si ) throw();

            /** @brief Constructs a DataBaseException with a specific error value.

                The error value allows the catcher to implement specific code for specific error values.

                @param what The message text of the DatabaseException.
                @param statement The statement that failed.
                @param error An error value which concretizes the DataBaseException.
                @param si The SourceInfo where the error occurred.
             */
            DatabaseException( const std::string& what, const std::string& statement, const DatabaseError& error, const SourceInfo& si ) throw();

            //! @brief Destructor.
            ~DatabaseException() throw();

            /** @brief Returns the error value of this DataBaseException.

                @return The error value of this DataBaseException.
             */
            const DatabaseException::DatabaseError& databaseError() const;

            const std::string& statement() const;

        private:
            DatabaseError m_dbError;
            std::string m_statement;
    };

} // namespace Db

} // namespace Pt

#endif

