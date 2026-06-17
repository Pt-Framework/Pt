#ifndef PT_DB_SQLITE_SQLITEERROR_H
#define PT_DB_SQLITE_SQLITEERROR_H

namespace Pt {

namespace Db {

namespace sqlite {

    void SqliteError(int errorCode, const char* statement = "");

} // namespace sqlite

} // namespace Db

} // namespace Pt

#endif // PT_DB_SQLITE_SQLITEERROR_H
