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
#ifndef PT_SQLITE_ERROR_H
#define PT_SQLITE_ERROR_H

#include "Error.h"
#include "Pt/Db/Exception.h"
#include "Pt/System/IOError.h"
#include <stdexcept>
#include "sqlite3.h"


namespace Pt {

namespace Db {

namespace sqlite {

	void Error(int errorCode, const Pt::SourceInfo& srcInfo)
	{
		switch(errorCode) {

			case SQLITE_ERROR :
				throw std::runtime_error("SQL error or missing database" + srcInfo);

			case SQLITE_INTERNAL :
				throw std::logic_error("NOT USED. Internal logic error in SQLite" + srcInfo);

			case SQLITE_PERM :
				throw AccessError("Access permission denied", srcInfo);

			case SQLITE_ABORT :
				throw std::runtime_error("Callback routine requested an abort" + srcInfo);

			case SQLITE_BUSY :
				throw AccessError("The database file is locked", srcInfo);

			case SQLITE_LOCKED :
				throw AccessError("A table in the database is locked", srcInfo);

			case SQLITE_NOMEM :
				throw std::bad_alloc();

			case SQLITE_READONLY :
				throw AccessError("Attempt to write a readonly database", srcInfo);

			case SQLITE_INTERRUPT :
				throw std::runtime_error("Operation terminated by sqlite3_interrupt" + srcInfo);

			case SQLITE_IOERR :
				throw System::IOError("Some kind of disk I/O error occurred", srcInfo);

			case SQLITE_CORRUPT :
				throw std::runtime_error("The database disk image is malformed" + srcInfo);

			case SQLITE_NOTFOUND :
				throw std::logic_error("Table or record not found" + srcInfo);

			case SQLITE_FULL :
				throw std::runtime_error("Insertion failed because database is full" + srcInfo);

			case SQLITE_CANTOPEN :  // TODO
				throw std::runtime_error("Unable to open the database file" + srcInfo);

			case SQLITE_PROTOCOL :
				throw std::runtime_error("Database lock protocol error" + srcInfo);

			case SQLITE_SCHEMA :
				throw std::logic_error("The database schema changed" + srcInfo);

			case SQLITE_TOOBIG :
				throw std::logic_error("Too much data for one row" + srcInfo);

			case SQLITE_CONSTRAINT :
				throw std::runtime_error("Abort due to constraint violation" + srcInfo);

			case SQLITE_MISMATCH :
				throw std::logic_error("Data type mismatch" + srcInfo);

			case SQLITE_MISUSE :
				throw std::runtime_error("Library used incorrectly" + srcInfo);

			case SQLITE_NOLFS :
				throw std::runtime_error("Uses OS features not supported on host" + srcInfo);

			case SQLITE_AUTH :
				throw AccessError("Authorization denied", srcInfo);

			case SQLITE_FORMAT :
				throw std::runtime_error("Auxiliary database format error" + srcInfo);

			case SQLITE_RANGE :
				throw std::range_error("2nd parameter to sqlite3_bind out of range" + srcInfo);

			case SQLITE_NOTADB : // TODO
				throw std::logic_error("File opened that is not a database file" + srcInfo);
		}

		throw std::runtime_error("Unknown error in sqlite." + srcInfo);
	}

} // namespace sqlite

} // namespace Db

} // namespace Pt

#endif
