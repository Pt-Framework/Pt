/*
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
#include "SqliteError.h"
#include "Pt/Db/DbError.h"
#include "Pt/System/IOError.h"
#include <new>
#include "sqlite3.h"


namespace Pt {

namespace Db {

namespace sqlite {

void SqliteError(int errorCode, const char* statement)
{
    switch(errorCode)
    {
        case SQLITE_ERROR :
            throw InvalidQuery("SQL error or missing database", statement);

        case SQLITE_INTERNAL :
            throw InvalidQuery("Internal logic error in SQLite", statement);

        case SQLITE_PERM :
            throw AccessDenied("Access permission denied");

        case SQLITE_ABORT :
            throw QueryFailed("Callback routine requested an abort", statement);

        case SQLITE_BUSY :
            throw AccessDenied("The database file is locked");

        case SQLITE_LOCKED :
            throw AccessDenied("A table in the database is locked");

        case SQLITE_NOMEM :
            throw std::bad_alloc();

        case SQLITE_READONLY :
            throw AccessDenied("Attempt to write a readonly database");

        case SQLITE_INTERRUPT :
            throw QueryFailed("Operation terminated by sqlite3_interrupt", statement);

        case SQLITE_IOERR :
            throw System::IOError("Some kind of disk I/O error occurred");

        case SQLITE_CORRUPT :
            throw QueryFailed("The database disk image is malformed", statement);

        case SQLITE_NOTFOUND :
            throw InvalidQuery("Table or record not found", statement);

        case SQLITE_FULL :
            throw QueryFailed("Insertion failed because database is full", statement);

        case SQLITE_CANTOPEN :
            throw InvalidConnection("Unable to open the database file");

        case SQLITE_PROTOCOL :
            throw InvalidConnection("Database lock protocol error");

        case SQLITE_SCHEMA :
            throw InvalidQuery("The database schema changed", statement);

        case SQLITE_TOOBIG :
            throw QueryFailed("Too much data for one row", statement);

        case SQLITE_CONSTRAINT :
            throw ConstraintMismatch("Constraint violation", statement);

        case SQLITE_MISMATCH :
            throw TypeMismatch("Data type mismatch", statement);

        case SQLITE_MISUSE :
            throw InvalidConnection("Library used incorrectly");

        case SQLITE_NOLFS :
            throw InvalidConnection("Uses OS features not supported on host");

        case SQLITE_AUTH :
            throw AccessDenied("Authorization denied");

        case SQLITE_FORMAT :
            throw InvalidQuery("Auxiliary database format error", statement);

        case SQLITE_RANGE :
            throw TypeMismatch("Parameter index out of range", statement);

        case SQLITE_NOTADB :
            throw InvalidConnection("File opened that is not a database file");
    }

    throw QueryFailed("Unknown error in sqlite", statement);
}

} // namespace sqlite

} // namespace Db

} // namespace Pt
