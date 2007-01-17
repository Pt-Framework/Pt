#ifndef PT_SQLITE_ERROR_H
#define PT_SQLITE_ERROR_H

#include "Pt/Exception.h"
#include "Pt/IO/IOError.h"
#include <stdexcept>
#include "sqlite3.h"


namespace Pt {

namespace Db {

namespace sqlite {

	inline void Error(int errorCode, const Pt::SourceInfo& srcInfo)
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
				throw IO::IOError("Some kind of disk I/O error occurred", srcInfo);

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
				throw std::runtime_error("Abort due to contraint violation" + srcInfo);

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

		throw Exception("Unknown error in sqlite.", srcInfo);
	}

} // namespace sqlite

} // namespace Db

} // namespace Pt

#endif
