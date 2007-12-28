/***************************************************************************
 *   Copyright (C) 2006 by Tommi Maekitalo                                 *
 *   Copyright (C) 2006 by Marc Boris Duerner                              *
 *   Copyright (C) 2006 by Stefan Bueder                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "StmtValue.h"
#include "Pt/Db/Blob.h"
#include "Pt/Date.h"
#include "Pt/Time.h"
#include "Pt/DateTime.h"

namespace Pt {

namespace Db {

namespace sqlite {

    bool StmtValue::isNull() const
    {
        //log_debug("sqlite3_column_type(" << getStmt() << ", " << iCol << ')');
        return ::sqlite3_column_type(getStmt(), _iCol) == SQLITE_NULL;
    }

    bool StmtValue::getBool() const
    {
        return getInt() != 0;
    }

    int StmtValue::getInt() const
    {
        //log_debug("sqlite3_column_int(" << getStmt() << ", " << iCol << ')');
        return ::sqlite3_column_int(getStmt(), _iCol);
    }

    unsigned StmtValue::getUnsigned() const
    {
        //log_debug("possible loss of data in conversion from int to unsigned");
        return static_cast<unsigned>(getInt());
    }

    float StmtValue::getFloat() const
    {
        return static_cast<float>(getInt());
    }

    double StmtValue::getDouble() const
    {
        //log_debug("sqlite3_column_double(" << getStmt() << ", " << iCol << ')');
        return ::sqlite3_column_double(getStmt(), _iCol);
    }

    char StmtValue::getChar() const
    {
        //log_debug("sqlite3_column_text(" << getStmt() << ", " << iCol << ')');
        const unsigned char* ret = ::sqlite3_column_text(getStmt(), _iCol);
        return static_cast<char>(*ret);
    }

    void StmtValue::getString(std::string& stringdata) const
    {
        //log_debug("sqlite3_column_bytes(" << getStmt() << ", " << iCol << ')');
        int bytes = ::sqlite3_column_bytes(getStmt(), _iCol);
        //log_debug("sqlite3_column_text(" << getStmt() << ", " << iCol << ')');
        const unsigned char* ret = ::sqlite3_column_text(getStmt(), _iCol);
        stringdata = std::string(reinterpret_cast<const char*>(ret), bytes);
    }

    Date StmtValue::getDate() const
    {
        std::string str;
        getString(str);
        return Date::fromIsoString(str);
    }

    Time StmtValue::getTime() const
    {
        std::string str;
        getString(str);
        return Time::fromIsoString(str);
    }

    DateTime StmtValue::getDateTime() const
    {
        std::string str;
        getString(str);
        return DateTime::fromIsoString(str);
    }

    /*void StmtValue::getData(Pt::Variant& data) const
    {
        int bytes = ::sqlite3_column_bytes( getStmt(), _iCol );
        const void* ret = ::sqlite3_column_blob( getStmt(), _iCol );
        data.assign( (const char*)ret, bytes );
    }*/

    void StmtValue::getBlob(Pt::Blob& blob) const
    {
        int bytes = ::sqlite3_column_bytes(getStmt(), _iCol);
        const void* ret = ::sqlite3_column_blob(getStmt(), _iCol);

        blob.assign((const char *)ret, bytes);
    }

} // namespace sqlite

} // namespace Db

} // namespace Pt
